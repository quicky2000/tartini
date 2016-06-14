/***************************************************************************
                          mytransforms.cpp  -  description
                             -------------------
    begin                : Sat Dec 11 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "mytransforms.h"
#include "myassert.h"
#include "array1d.h"
#include "equalloudness.h"
#include "gdata.h"
#include "bspline.h"
#include "channel.h"
#include "Filter.h"
#include "conversions.h"
#include "musicnotes.h"

#include "useful.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <float.h>

#include <complex>

//------------------------------------------------------------------------------
MyTransforms::MyTransforms(void)
{
  beenInit = false;
}
  
//------------------------------------------------------------------------------
MyTransforms::~MyTransforms(void)
{
  uninit();
}

//------------------------------------------------------------------------------
void MyTransforms::init(int n_, int k_, double rate_, bool equalLoudness_, int numHarmonics_)
{
  const int myFFTMode = FFTW_ESTIMATE;
  uninit();
  if(k_ == 0)
    {
      k_ = (n_ + 1) / 2;
    }
  
  n = n_;
  k = k_;
  size = n + k;
  rate = rate_;
  equalLoudness = equalLoudness_;
  numHarmonics = numHarmonics_;

  dataTemp = (float*)fftwf_malloc(sizeof(float) * n);
  dataTime = (float*)fftwf_malloc(sizeof(float) * n);
  dataFFT  =  (float*)fftwf_malloc(sizeof(float) * n);
  autocorrTime = (float*)fftwf_malloc(sizeof(float) * size);
  autocorrFFT  = (float*)fftwf_malloc(sizeof(float) * size);
  hanningCoeff  = (float*)fftwf_malloc(sizeof(float) * n);

  planAutocorrTime2FFT = fftwf_plan_r2r_1d(size, autocorrTime, autocorrFFT, FFTW_R2HC, myFFTMode);
  planAutocorrFFT2Time = fftwf_plan_r2r_1d(size, autocorrFFT, autocorrTime, FFTW_HC2R, myFFTMode);
  
  planDataTime2FFT = fftwf_plan_r2r_1d(n, dataTime, dataFFT, FFTW_R2HC, myFFTMode);
  planDataFFT2Time = fftwf_plan_r2r_1d(n, dataFFT, dataTime, FFTW_HC2R, myFFTMode); //???

  harmonicsAmpLeft = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseLeft = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsAmpCenter = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseCenter = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsAmpRight = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseRight = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  
  freqPerBin = rate / double(size);
  //init hanningCoeff. The hanning windowing function
  hanningScalar = 0;
  for(int j = 0; j < n; j++)
    {
      hanningCoeff[j] = (1.0 - cos(double(j + 1) / double(n + 1) * twoPI)) / 2.0;
      hanningScalar += hanningCoeff[j];
    }

  //to normalise the FFT coefficients we divide by the sum of the hanning window / 2
  hanningScalar /= 2;

  fastSmooth = new fast_smooth(n / 8);
#ifdef PRINTF_DEBUG
  printf("fast_smooth size = %d\n", n / 8);
#endif // PRINTF_DEBUG
  beenInit = true;
}

//------------------------------------------------------------------------------
void MyTransforms::uninit(void)
{
  if(beenInit)
    {
      fftwf_free(harmonicsAmpLeft);
      fftwf_free(harmonicsPhaseLeft);
      fftwf_free(harmonicsAmpCenter);
      fftwf_free(harmonicsPhaseCenter);
      fftwf_free(harmonicsAmpRight);
      fftwf_free(harmonicsPhaseRight);
      fftwf_destroy_plan(planDataFFT2Time);
      fftwf_destroy_plan(planDataTime2FFT);
      fftwf_destroy_plan(planAutocorrFFT2Time);
      fftwf_destroy_plan(planAutocorrTime2FFT);
      fftwf_free(autocorrFFT);
      fftwf_free(autocorrTime);
      fftwf_free(dataFFT);
      fftwf_free(dataTime);
      fftwf_free(dataTemp);
      delete fastSmooth;
      beenInit = false;
    }
}

//------------------------------------------------------------------------------
double MyTransforms::autocorr(float *input, float *output)
{
  myassert(beenInit);
  float fsize = float(size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input + n, autocorrTime);
  std::fill(autocorrTime + n, autocorrTime + size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j = 1; j < size / 2; j++)
    {
      autocorrFFT[j] = sq(autocorrFFT[j]) + sq(autocorrFFT[size-j]);
      autocorrFFT[size-j] = 0.0f;
    }
  autocorrFFT[0] = sq(autocorrFFT[0]);
  autocorrFFT[size / 2] = sq(autocorrFFT[size / 2]);

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  for(float * p1 = output, *p2 = autocorrTime + 1; p1 < output + k;)
    {
      *p1++ = *p2++ / fsize;
    }

  return double(autocorrTime[0]) / double(size);
}

//------------------------------------------------------------------------------
double MyTransforms::autoLogCorr(float *input, float *output)
{
  myassert(beenInit);
  float fsize = float(size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+n, autocorrTime);
  std::fill(autocorrTime+n, autocorrTime+size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j = 1; j < size / 2; j++)
    {
      autocorrFFT[j] = (sq(autocorrFFT[j]) + sq(autocorrFFT[size - j]));
      autocorrFFT[size - j] = 0.0f;
    }
  autocorrFFT[0] = (sq(autocorrFFT[0]));
  autocorrFFT[size / 2] = (sq(autocorrFFT[size / 2]));

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  for(float *p1=output, *p2=autocorrTime+1; p1<output+k;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(size);
}

//------------------------------------------------------------------------------
double MyTransforms::asdf(float *input, float *output)
{
  myassert(beenInit);
  //Do an autocorrelation and return the sum of squares of the input
  double sumSq = autocorr(input, output);
  double sumRightSq = sumSq;
  double sumLeftSq = sumSq;
  for(int j = 0; j < k; j++)
    {
      sumLeftSq  -= sq(input[n - 1 - j]);
      sumRightSq -= sq(input[j]);
      output[j] = sumLeftSq + sumRightSq - 2 * output[j];
    }
  return sumSq;
}

//------------------------------------------------------------------------------
double MyTransforms::nsdf(float *input, float *output)
{
  myassert(beenInit);

  //the sum of squares of the input
  double sumSq = autocorr(input, output);

  double totalSumSq = sumSq * 2.0;
  if(gdata->analysisType() == MPM || gdata->analysisType() == MPM_MODIFIED_CEPSTRUM)
    {
      //nsdf
      for(int j = 0; j < k; j++)
	{
	  totalSumSq  -= sq(input[n - 1 - j]) + sq(input[j]);
	  //dividing by zero is very slow, so deal with it seperately
	  if(totalSumSq > 0.0)
	    {
	      output[j] *= 2.0 / totalSumSq;
	    }
	  else
	    {
	      output[j] = 0.0;
	    }
	}
    }
  else
    {
      //autocorr
      for(int j = 0; j < k; j++)
	{
	  //dividing by zero is very slow, so deal with it seperately
	  if(totalSumSq > 0.0)
	    {
	      output[j] /= sumSq;
	    }
	  else
	    {
	      output[j] = 0.0;
	    }
	}
    }
  return sumSq;
}

//------------------------------------------------------------------------------
int MyTransforms::findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions)
{
  int pos = 0;
  int curMaxPos = 0;
  int overallMaxIndex = 0;

  //find the first negitive zero crossing
  while(pos < (len-1)/3 && input[pos] > 0.0f)
    {
      pos++;
    }

  //loop over all the values below zero
  while(pos < len-1 && input[pos] <= 0.0f)
    {
      pos++;
    }

  // can happen if output[0] is NAN
  if(pos == 0)
    {
      pos = 1;
    }

  while(pos < len-1)
    {
      //don't assert on NAN
      myassert(!(input[pos] < 0));
      if(input[pos] > input[pos - 1] && input[pos] >= input[pos + 1])
	{
	  //a local maxima
	  if(curMaxPos == 0)
	    {
	      //the first maxima (between zero crossings)
	      curMaxPos = pos;
	    }
	  else if(input[pos] > input[curMaxPos])
	    {
	      //a higher maxima (between the zero crossings)
	      curMaxPos = pos;
	    }
	}
      pos++;
      if(pos < len-1 && input[pos] <= 0.0f)
	{
	  //a negative zero crossing
	  if(curMaxPos > 0)
	    {
	      //if there was a maximum
	      //add it to the vector of maxima
	      maxPositions.push_back(curMaxPos);
	      if(overallMaxIndex == 0)
		{
		  overallMaxIndex = curMaxPos;
		}
	      else if(input[curMaxPos] > input[overallMaxIndex])
		{
		  overallMaxIndex = curMaxPos;
		}
	      curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
	    }
	  //loop over all the values below zero
	  while(pos < len-1 && input[pos] <= 0.0f)
	    {
	      pos++;
	    }
	}
    }

  if(curMaxPos > 0)
    {
      //if there was a maximum in the last part
      //add it to the vector of maxima
      maxPositions.push_back(curMaxPos);
      if(overallMaxIndex == 0)
	{
	  overallMaxIndex = curMaxPos;
	}
      else if(input[curMaxPos] > input[overallMaxIndex])
	{
	  overallMaxIndex = curMaxPos;
	}
      //clear the maximum position, so we start looking for a new ones
      curMaxPos = 0;
    }
  return overallMaxIndex;
}

//------------------------------------------------------------------------------
int MyTransforms::findNSDFsubMaximum(float *input, int len, float threshold)
{
  std::vector<int> indices;
  int overallMaxIndex = findNSDFMaxima(input, len, indices);
  threshold += (1.0 - threshold) * (1.0 - input[overallMaxIndex]);
  float cutoff = input[overallMaxIndex] * threshold;
  for(uint j=0; j<indices.size(); j++) {
    if(input[indices[j]] >= cutoff) return indices[j];
  }
  //should never get here
  myassert(0);

  //stop the compiler warning
  return 0;
}

//------------------------------------------------------------------------------
void MyTransforms::calculateAnalysisData(/*float *input, */int chunk, Channel *ch/*, float threshold*/)
{
  myassert(ch);
  myassert(ch->dataAtChunk(chunk));
  AnalysisData &analysisData = *ch->dataAtChunk(chunk);
  AnalysisData *prevAnalysisData = ch->dataAtChunk(chunk - 1);
  float *output = ch->get_nsdf_data().begin();
  float *curInput = (equalLoudness) ? ch->get_filtered_input().begin() : ch->get_direct_input().begin();

  std::vector<int> nsdfMaxPositions;
  analysisData.setMaxIntensityDB(linear2dB(fabs(*std::max_element(curInput, curInput + n, absoluteLess<float>())),*gdata));
  
  doChannelDataFFT(ch, curInput, chunk);
  std::copy(curInput, curInput + n, dataTime);
  
  if(gdata->doingFreqAnalysis() && (ch->firstTimeThrough() || gdata->doingActiveAnalysis()))
    {
      //calculate the Normalised Square Difference Function
      double logrms = linear2dB(nsdf(dataTime, ch->get_nsdf_data().begin()) / double(n),*gdata); /**< Do the NSDF calculation */
      analysisData.setLogRms(logrms);
      if(gdata->doingAutoNoiseFloor() && !analysisData.isDone())
	{
	  //do it for gdata. this is only here for old code. remove some stage
	  if(chunk == 0)
	    {
	      gdata->rmsFloor() = 0.0;
	      gdata->rmsCeiling() = gdata->dBFloor();
	    }
	  if(logrms + 15 < gdata->rmsFloor())
	    {
	      gdata->rmsFloor() = logrms + 15;
	    }
	  if(logrms > gdata->rmsCeiling())
	    {
	      gdata->rmsCeiling() = logrms;
	    }

	  //do it for the channel
	  if(chunk == 0)
	    {
	      ch->rmsFloor = 0.0; ch->rmsCeiling = gdata->dBFloor();
	    }
	  if(logrms + 15 < ch->rmsFloor)
	    {
	      ch->rmsFloor = logrms + 15;
	    }
	  if(logrms > ch->rmsCeiling)
	    {
	      ch->rmsCeiling = logrms;
	    }
	}

      analysisData.setFreqCentroid(calcFreqCentroidFromLogMagnitudes(ch->get_fft_data1().begin(), ch->get_fft_data1().size()));
      if(prevAnalysisData)
	{
	  analysisData.setDeltaFreqCentroid(bound(fabs(analysisData.getFreqCentroid() - prevAnalysisData->getFreqCentroid()) * 20.0, 0.0, 1.0));
	}
      else
	{ 
	  analysisData.setDeltaFreqCentroid(0.0);
	}
    
      findNSDFMaxima(ch->get_nsdf_data().begin(), k, nsdfMaxPositions);
      if(!analysisData.isDone())
	{
	}

      //store some of the best period estimates
      analysisData.clearPeriodEstimates();
      analysisData.clearPeriodEstimatesAmp();
      float smallCutoff = 0.4f;
      for(std::vector<int>::iterator iter = nsdfMaxPositions.begin(); iter < nsdfMaxPositions.end(); iter++)
	{
	  if(output[*iter] >= smallCutoff)
	    {
	      float x, y;
	      //do a parabola fit to find the maximum
	      parabolaTurningPoint2(output[*iter-1], output[*iter], output[*iter+1], float(*iter + 1), &x, &y);
	      y = bound(y, -1.0f, 1.0f);
	      analysisData.addPeriodEstimates(x);
	      analysisData.addPeriodEstimatesAmp(y);
	    }
	}

      float periodDiff = 0.0f;
      if(analysisData.isPeriodEstimatesEmpty())
	{
	  //no period found
	  analysisData.calcScores();
	  analysisData.setDone(true);
	}
      else
	{
	  //calc the periodDiff
	  if(chunk > 0)
	    {
	      float prevPeriod = prevAnalysisData->getHighestCorrelationIndex() != -1 ? prevAnalysisData->getPeriodEstimatesAt(prevAnalysisData->getHighestCorrelationIndex()) : 0;

	      periodDiff = analysisData.searchClosestPeriodEstimates(prevPeriod) - prevPeriod;
	      if(absolute(periodDiff) > 8.0f)
		{
		  periodDiff = 0.0f;
		}
	    }

	  int nsdfMaxIndex = analysisData.getPeriodEstimatesAmpMaxElementIndex();
	  analysisData.setHighestCorrelationIndex(nsdfMaxIndex);

	  if(!analysisData.isDone())
	    {
	      if(gdata->analysisType() == MPM_MODIFIED_CEPSTRUM)
		{
		  //calculate pitch
		  ch->chooseCorrelationIndex(chunk, float(analysisData.getCepstrumIndex()));
		}
	      else
		{
		  if(ch->isNotePlaying() && chunk > 0)
		    {
		      //calculate pitch
		      ch->chooseCorrelationIndex(chunk, ch->periodOctaveEstimate(chunk - 1));
		    }
		  else
		    {
		      //calculate pitch
		      ch->chooseCorrelationIndex1(chunk);
		    }
		}
	      ch->calcDeviation(chunk);

	      //calculate vibratoPitch, vibratoWidth, vibratoSpeed
	      ch->doPronyFit(chunk);
	    }

	  analysisData.setChangeness(0.0f);

	  if(gdata->doingHarmonicAnalysis())
	    {
	      std::copy(dataTime, dataTime+n, dataTemp);
	      if(analysisData.getChosenCorrelationIndex() >= 0)
		{
		  doHarmonicAnalysis(dataTemp, analysisData, analysisData.getPeriodEstimatesAt(analysisData.getChosenCorrelationIndex())/*period*/);
		}
	    }
	}

      if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && ch->firstTimeThrough())
	{
	  float periodDiff2 = ch->calcDetailedPitch(curInput, analysisData.getPeriod(), chunk);
	  periodDiff = periodDiff2;

	  ch->get_pitch_lookup().push_back(ch->get_detailed_pitch_data().begin(), ch->get_detailed_pitch_data().size());
	  ch->get_pitch_lookup_smoothed().push_back(ch->detailedPitchDataSmoothed.begin(), ch->detailedPitchDataSmoothed.size());
	}

      if(!analysisData.isDone())
	{
	  analysisData.calcScores();
	  ch->processNoteDecisions(chunk, periodDiff);
	  analysisData.setDone(true);
	}

      if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && ch->firstTimeThrough())
	{
	  ch->calcVibratoData(chunk);
	}
    }

  if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && (!ch->firstTimeThrough()))
    {
      ch->get_pitch_lookup().copyTo(ch->get_detailed_pitch_data().begin(), chunk*ch->get_detailed_pitch_data().size(), ch->get_detailed_pitch_data().size());
      ch->get_pitch_lookup_smoothed().copyTo(ch->detailedPitchDataSmoothed.begin(), chunk*ch->detailedPitchDataSmoothed.size(), ch->detailedPitchDataSmoothed.size());
    }

  if(!analysisData.isDone())
    {
      int j;
      //calc rms by hand
      double rms = 0.0;
      for(j=0; j<n; j++)
	{
	  rms += sq(dataTime[j]);
	}
      analysisData.setLogRms(linear2dB(rms / float(n),*gdata));
      analysisData.calcScores();
      analysisData.setDone(true);
    }
}

//------------------------------------------------------------------------------
float MyTransforms::get_fine_clarity_measure(double period)
{
#ifdef PRINTF_DEBUG
  printf("%f, ", analysisData.periodEstimates[choosenMaxIndex]);
  printf("%f, ", analysisData.periodEstimatesAmp[choosenMaxIndex]);
#endif // PRINTF_DEBUG
  int tempN = n - int(ceil(period));
  float *tempData = new float[tempN];
  float bigSum = 0;
  float corrSum = 0;
  float matchVal;
  float matchMin = 1.0;
  //create some data points at the fractional period delay into tempData
  stretch_array(n, dataTime, tempN, tempData, period, tempN, LINEAR);
  //tempN / 4;
  int dN = int(floor(period));
  for(int j = 0; j < dN; j++)
    {
      bigSum += sq(dataTime[j]) + sq(tempData[j]);
      corrSum += dataTime[j] * tempData[j];
    }
  matchMin = (2.0 * corrSum) / bigSum;
#ifdef PRINTF_DEBUG
  printf("%f, ", matchMin);
#endif // PRINTF_DEBUG
  for(int j = 0; j < tempN - dN; j++)
    {
      bigSum -= sq(dataTime[j]) + sq(tempData[j]);
      corrSum -= dataTime[j] * tempData[j];
      bigSum += sq(dataTime[j + dN]) + sq(tempData[j + dN]);
      corrSum += dataTime[j + dN] * tempData[j + dN];
      matchVal = (2.0 * corrSum) / bigSum;
      if(matchVal < matchMin)
	{
	  matchMin = matchVal;
	}
    }
#ifdef PRINTF_DEBUG
    printf("%f\n", matchMin);
#endif // PRINTF_DEBUG
    delete[] tempData;

    return matchMin;
}

//------------------------------------------------------------------------------
double MyTransforms::get_max_note_change(float *input, double period)
{
  //TODO
  /*matlab code
    l = length(s);
    m = floor(l / 4); % m is the maximum size sub-window to use
    % w is the sub-window size
    if p < m
    w = p * floor(m / p);
    else
    w = p;
    end

    n = -4:4;
    ln = length(n);
    left = cell(1, ln);
    right = cell(1, ln);
    left_pow = zeros(1, ln);
    right_pow = zeros(1, ln);
    pow = zeros(1, ln);
    err = zeros(1, ln);

    for i = 1:ln
    left{i} = s(1:(w-n(i))); % a smaller delay period has a larger window size, to ensure only the same data is used
    right{i} = s(1+p+n(i):w+p);
    left_pow(i) = sum(left{i}.^2);
    right_pow(i) = sum(right{i}.^2);
    err(i) = sum((left{i} - right{i}) .^ 2);
    end
  */
  int i, j, j2;
  int max_subwindow = n / 4;
  int subwindow_size;
  if(period < 1.0)
    {
      //period too small
      return 0.0;
    }
  if(period > n/2)
    {
      printf("period = %lf\n", period);
      return 0.0;
    }
  int iPeriod = int(floor(period));
  if(period < double(max_subwindow))
    {
      subwindow_size = int(floor(period * (double(max_subwindow) / period)));
    }
  else
    {
      subwindow_size = int(floor(period));
    }
  int num = n - subwindow_size - iPeriod;

  std::vector<int> offsets;
  for(j = -4; j <= 4; j++)
    {
      //do a total of 9 subwindows at once. 4 either side.
      offsets.push_back(j);
    }
  int ln = offsets.size();

  std::vector<float> left(ln);
  std::vector<float> right(ln);
  std::vector<float> left_pow(ln);
  std::vector<float> right_pow(ln);
  std::vector<float> pow(ln);
  std::vector<float> err(ln);
  std::vector<float> result(ln);
  std::vector<float> unsmoothed(num);
  std::vector<float> smoothed(num);
  std::vector<float> smoothed_diff(num);
  //calc the values of pow and err for the first in each row.
  for(i = 0; i < ln; i++)
    {
      left_pow[i] = right_pow[i] = pow[i] = err[i] = 0;
      for(j = 0, j2 = iPeriod+offsets[i]; j < subwindow_size - offsets[i]; j++, j2++)
	{
	  left_pow[i] += sq(input[j]);
	  right_pow[i] += sq(input[j2]);
	  err[i] += sq(input[j] - input[j2]);
	}
    }
#ifdef PRINTF_DEBUG
  printf("subwindow_size=%d, num=%d, period=%lf\n", subwindow_size, num, period);
#endif // PRINTF_DEBUG
  /*matlab code
    for j = 1:(num-1)
      for i = 1:ln
        pow(i) = (left_pow(i) + right_pow(i));
        resulte(i, j) = err(i);
        resultp(i, j) = pow(i);
        result(i, j) = 1 - (err(i) / pow(i));

        %err = err - (s(j) - s(j+p)).^2 + (s(j+w) - s(j+w+p)).^2;
        err(i) = err(i) - ((s(j) - s(j+p+n(i))).^2) + (s(j+w-n(i)) - s(j+w+p)).^2;
        left_pow(i) = left_pow(i) - s(j).^2 + s(j+w-n(i)).^2;
        right_pow(i) = right_pow(i) - s(j+p+n(i)).^2 + s(j+p+w).^2;
      end
    end

    for i = 1:ln
      pow(i) = (left_pow(i) + right_pow(i));
      result(i, num) = 1 - (err(i) / pow(i));
    end
*/
  //TODO: speed up this for loop
  for(j = 0; j < num - 1; j++)
    {
      for(i = 0; i < ln; i++)
	{
	  pow[i] = left_pow[i] + right_pow[i];
	  result[i] = 1.0 - (err[i] / pow[i]);

	  err[i] += -sq(input[j] - input[j + iPeriod+offsets[i]]) + sq(input[j + subwindow_size - offsets[i]] - input[j + subwindow_size + iPeriod]);
	  left_pow[i] += -sq(input[j]) + sq(input[j + subwindow_size - offsets[i]]);
	  right_pow[i] += -sq(input[j + iPeriod+offsets[i]]) + sq(input[j + iPeriod+subwindow_size]);
	}
      /*matlab code
	for j = 1:num
	  [dud pos] = max(result(:,j));
	  if pos > 1 && pos < ln
            [period(j) dummy] = parabolaPeak(result(pos-1, j), result(pos, j), result(pos+1, j), p+n(pos));
	  else
            period(j) = p+n(pos);
	  end
	  period_int(j) = p+n(pos);
	end*/
      int pos = int(std::max_element(result.begin(), result.begin() + ln) - result.begin());
      if(pos > 0 && pos < ln-1)
	{
	  unsmoothed[j] = double(iPeriod + offsets[pos]) + parabolaTurningPoint(result[pos - 1], result[pos], result[pos+1]);
	}
      else
	{
	  unsmoothed[j] = double(iPeriod + offsets[pos]);
	}
    }
  fastSmooth->fast_smoothB(&(unsmoothed[0]), &(smoothed[0]), num - 1);
  int max_pos = 0;
  for(j = 0; j < num - 2; j++)
    {
      smoothed_diff[j] = fabs(smoothed[j + 1] - smoothed[j]);
#ifdef PRINTF_DEBUG
      printf("%f ", smoothed[j]);
#endif // PRINTF_DEBUG
      if(smoothed_diff[j] > smoothed_diff[max_pos])
	{
	  max_pos = j;
	}
    }
#ifdef PRINTF_DEBUG
  printf("\nsmooted_diff=%f\n", smoothed_diff[max_pos]);
#endif // PRINTF_DEBUG
  double ret = smoothed_diff[max_pos] / period * double(rate) * double(subwindow_size) / 10000.0;
  return ret;
}

/**
   Find the index of the first maxima above the threshold times the overall maximum.
   @param threshold A value between 0.0 and 1.0
   @return The index of the first subMaxima
*/
int findFirstSubMaximum(float *data, int length, float threshold)
{
  float maxValue = *std::max_element(data, data + length);
  float cutoffValue = maxValue * threshold;
  for(int j = 0; j < length; j++)
    {
      if(data[j] >= cutoffValue)
	{
	  return j;
	}
    }
  myassert(0); //shouldn't get here.
  return length;
}

/**
   Given cepstrum input finds the first maxima above the threshold
   @param threshold The fraction of the 'overall maximum value' the 'cepstrum maximum' must be over.
   A value between 0.0 and 1.0.
*/
int findCepstrumMaximum(float *data, int length, float threshold)
{
  int pos = 0;
  //loop until the first negative value in data
  while(pos < length-1 && data[pos] > 0.0f)
    {
      pos++;
    }
  return pos + findFirstSubMaximum(data + pos, length - pos, threshold);
}

//------------------------------------------------------------------------------
void MyTransforms::doChannelDataFFT(Channel *ch, float *curInput, int chunk)
{
  std::copy(curInput, curInput + n, dataTime);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  int nDiv2 = n / 2;
  //LOG RULES: log(sqrt(x)) = log(x) / 2.0
  //LOG RULES: log(a * b) = log(a) + log(b)
  myassert(ch->get_fft_data1().size() == nDiv2);
  double logSize = log10(double(ch->get_fft_data1().size())); //0.0
  //Adjust the coefficents, both real and imaginary part by same amount
  double sqValue;
  const double logBase = 100.0;
  for(int j = 1; j < nDiv2; j++)
    {
      sqValue = sq(dataFFT[j]) + sq(dataFFT[n - j]);
      ch->get_fft_data2()[j] = logBaseN(logBase, 1.0 + 2.0 * sqrt(sqValue) / double(nDiv2) * (logBase - 1.0));
      if(sqValue > 0.0)
	{
	  ch->get_fft_data1()[j] = bound(log10(sqValue) / 2.0 - logSize, gdata->dBFloor(), 0.0);
	}
      else
	{
	  ch->get_fft_data1()[j] = gdata->dBFloor();
	}
    }
  sqValue = sq(dataFFT[0]) + sq(dataFFT[nDiv2]);
  ch->get_fft_data2()[0] = logBaseN(logBase, 1.0 + 2.0 * sqrt(sqValue) / double(nDiv2) * (logBase - 1.0));
  if(sqValue > 0.0)
    {
      ch->get_fft_data1()[0] = bound(log10(sqValue) / 2.0 - logSize, gdata->dBFloor(), 0.0);
    }
  else
    {
      ch->get_fft_data1()[0] = gdata->dBFloor();
    }

#ifdef PRINTF_DEBUG
  printf("n = %d, fff = %f\n", nDiv2, *std::max_element(ch->get_fft_data2().begin(), ch->get_fft_data2().end()));
#endif // PRINTF_DEBUG

  if(gdata->analysisType() == MPM_MODIFIED_CEPSTRUM)
    {
      for(int j = 1; j < nDiv2; j++)
	{
	  dataFFT[j] = ch->get_fft_data2()[j];
	  dataFFT[n-j] = 0.0;
	}
      dataFFT[0] = ch->get_fft_data2()[0];
      dataFFT[nDiv2] = 0.0;
      fftwf_execute(planDataFFT2Time);

      for(int j = 1; j < n; j++)
	{
	  dataTime[j] /= dataTime[0];
	}
      dataTime[0] = 1.0;
      for(int j = 0; j < nDiv2; j++)
	{
	  ch->get_cepstrum_data()[j] = dataTime[j];
	}
      AnalysisData &analysisData = *ch->dataAtChunk(chunk);
      analysisData.setCepstrumIndex(findNSDFsubMaximum(dataTime, nDiv2, 0.6f));
      analysisData.setCepstrumPitch(freq2pitch(double(analysisData.getCepstrumIndex()) / ch->rate()));
    }
}

//------------------------------------------------------------------------------
void MyTransforms::calcHarmonicAmpPhase(float *harmonicsAmp, float *harmonicsPhase, int binsPerHarmonic)
{
  int harmonic;
  for(int j = 0; j < numHarmonics; j++)
    {
      harmonic = (j + 1) * binsPerHarmonic;
      if(harmonic < n)
	{
	  harmonicsAmp[j] = sqrt(sq(dataFFT[harmonic]) + sq(dataFFT[n - harmonic]));
	  harmonicsPhase[j] = atan2f(dataFFT[n - harmonic], dataFFT[harmonic]);
	}
      else
	{
	  harmonicsAmp[j] = 0.0;
	  harmonicsPhase[j] = 0.0;
	}
    }
}

//------------------------------------------------------------------------------
void MyTransforms::doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period)
{
  double numPeriodsFit = floor(double(n) / period); //2.0
  double numPeriodsUse = numPeriodsFit - 1.0;
  int iNumPeriodsUse = int(numPeriodsUse);
  double centerX = float(n) / 2.0;
#ifdef PRINTF_DEBUG
  printf("iNumPeriodsUse = %d\n", iNumPeriodsUse);
#endif // PRINTF_DEBUG
  //do left
  double start = centerX - (numPeriodsFit / 2.0) * period;
  double length = (numPeriodsUse) * period;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpLeft, harmonicsPhaseLeft, iNumPeriodsUse);
#ifdef PRINTF_DEBUG
  printf("left\n");
  for(int jj = 0; jj < 6; jj++)
    {
      printf("[%d %lf %lf]", jj, harmonicsAmpLeft[jj], harmonicsPhaseLeft[jj]);
    }
  printf("\n");
#endif // PRINTF_DEBUG
  
  //do center
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpCenter, harmonicsPhaseCenter, iNumPeriodsUse);
#ifdef PRINTF_DEBUG
  printf("centre\n");
  for(int jj = 0; jj < 6; jj++)
    {
      printf("[%d %lf %lf]", jj, harmonicsAmpCenter[jj], harmonicsPhaseCenter[jj]);
    }
  printf("\n");
#endif // PRINTF_DEBUG
  
  //do right
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpRight, harmonicsPhaseRight, iNumPeriodsUse);
#ifdef PRINTF_DEBUG
  printf("right\n");
  for(int jj = 0; jj < 6; jj++)
    {
      printf("[%d %lf %lf]", jj, harmonicsAmpRight[jj], harmonicsPhaseRight[jj]);
    }
  printf("\n");
#endif // PRINTF_DEBUG
  
  double freq = rate / period;
  
  analysisData.resizeHarmonicAmpNoCutOff(numHarmonics);
  analysisData.resizeHarmonicAmp(numHarmonics);
  analysisData.resizeHarmonicFreq(numHarmonics);
  analysisData.resizeHarmonicNoise(numHarmonics);

  for(int j = 0; j < numHarmonics; j++)
    {
      analysisData.setHarmonicAmpAt(j, log10(harmonicsAmpCenter[j] / hanningScalar) * 20);
      analysisData.setHarmonicAmpNoCutOffAt(j,analysisData.getHarmonicAmpAt(j));
      analysisData.setHarmonicAmpAt(j, 1.0 - (analysisData.getHarmonicAmpAt(j) / gdata->ampThreshold(AMPLITUDE_RMS, 0)));
      if(analysisData.getHarmonicAmpAt(j) < 0.0)
	{
	  analysisData.setHarmonicAmpAt(j,0.0);
	}
      //should be 1 whole period between left and right. i.e. the same freq give 0 phase difference
      double diffAngle = (harmonicsPhaseRight[j] - harmonicsPhaseLeft[j]) / twoPI;
      diffAngle = cycle(diffAngle + 0.5, 1.0) - 0.5;
      double diffAngle2 = (harmonicsPhaseCenter[j] - harmonicsPhaseLeft[j]) / twoPI;
      //if an odd harmonic the phase will be 180 degrees out. So correct for this
      if(j % 2 == 0)
	{
	  diffAngle2 += 0.5;
	}
      diffAngle2 = cycle(diffAngle2 + 0.5, 1.0) - 0.5;
      analysisData.setHarmonicNoiseAt(j, fabs(diffAngle2 - diffAngle));
      analysisData.setHarmonicFreqAt(j, float(freq * double(j+1)) + (freq*diffAngle));
    }
}

//------------------------------------------------------------------------------
void MyTransforms::applyHanningWindow(float *d)
{
  for(int j = 0; j < n; j++)
    {
      d[j] *= hanningCoeff[j];
    }
}

//------------------------------------------------------------------------------
double MyTransforms::calcFreqCentroid(float *buffer, int len)
{
  double centroid = 0.0;
  double totalWeight = 0.0;
  double absValue;
  for(int j = 1; j < len / 2; j++)
    {
      //ignore the end freq bins, ie j=0
      //calculate centroid
      absValue = sqrt(sq(buffer[j]) + sq(buffer[len - j]));
      centroid += double(j) * absValue;
      totalWeight += absValue;
    }
  centroid /= totalWeight * double(len / 2);
  return centroid;
}

//------------------------------------------------------------------------------
double MyTransforms::calcFreqCentroidFromLogMagnitudes(float *buffer, int len)
{
  double centroid = 0.0;
  double totalWeight = 0.0;
  for(int j = 1; j < len; j++)
    {
      //ignore the end freq bins, ie j=0
      //calculate centroid
      centroid += double(j)*buffer[j];
      totalWeight += buffer[j];
    }
  return centroid;
}
//EOF
