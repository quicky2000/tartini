/***************************************************************************
                          mytransforms.cpp  -  description
                             -------------------
    begin                : Sat Dec 11 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
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
#include "filter.h"

#include "useful.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <float.h>

#ifdef WINDOWS
#define isnan _isnan
#endif

#ifdef MACX
#define isnan(x) (x != x)
#endif


MyTransforms::MyTransforms()
{
  beenInit = false;
}
  
MyTransforms::~MyTransforms()
{
  uninit();
}

/** init() Initalises the parameters of a class instance. This must be called before use
  @param n_ The size of the data windows to be processed
  @param k_ The number of outputs wanted (autocorr size = n_ + k_). Set k_ = 0, to get default n_/2
  @param rate_ The sampling rate of the incoming signal to process
  @param threshold_ The ratio of highest peak to the first peak allowed to be chosen
*/
void MyTransforms::init(int n_, int k_, double rate_, /*float threshold_, */bool equalLoudness_, int numHarmonics_)
{
  uninit();
  if(k_ == 0) k_ = (n_ + 1) / 2;
  
  n = n_;
  k = k_;
  size = n + k;
  rate = rate_;
  //_threshold = threshold_;
  equalLoudness = equalLoudness_;
  numHarmonics = numHarmonics_;

  dataTemp = (float*)fftwf_malloc(sizeof(float) * n);
  dataTime = (float*)fftwf_malloc(sizeof(float) * n);
  dataFFT  =  (float*)fftwf_malloc(sizeof(float) * n);
  autocorrTime = (float*)fftwf_malloc(sizeof(float) * size);
  autocorrFFT  = (float*)fftwf_malloc(sizeof(float) * size);
  //equalLoudnessTable = (float*)fftwf_malloc(sizeof(float) * n);

  planAutocorrTime2FFT = fftwf_plan_r2r_1d(size, autocorrTime, autocorrFFT, FFTW_R2HC, FFTW_ESTIMATE);
  planAutocorrFFT2Time = fftwf_plan_r2r_1d(size, autocorrFFT, autocorrTime, FFTW_HC2R, FFTW_ESTIMATE);
  
  planDataTime2FFT = fftwf_plan_r2r_1d(n, dataTime, dataFFT, FFTW_R2HC, FFTW_ESTIMATE);
  planDataFFT2Time = fftwf_plan_r2r_1d(n, dataFFT, dataTime, FFTW_HC2R, FFTW_ESTIMATE);

  harmonicsAmpLeft = (float*)malloc(numHarmonics * sizeof(float));
  harmonicsPhaseLeft = (float*)malloc(numHarmonics * sizeof(float));
  harmonicsAmpCenter = (float*)malloc(numHarmonics * sizeof(float));
  harmonicsPhaseCenter = (float*)malloc(numHarmonics * sizeof(float));
  harmonicsAmpRight = (float*)malloc(numHarmonics * sizeof(float));
  harmonicsPhaseRight = (float*)malloc(numHarmonics * sizeof(float));
  //storeFFTAmp1 = (float*)malloc(n/2 * sizeof(float));
  //storeFFTAmp2 = (float*)malloc(n/2 * sizeof(float));
  
  freqPerBin = rate / double(size);
  //buildEqualLoudnessTable(50.0);
  beenInit = true;
}

void MyTransforms::uninit()
{
  if(beenInit) {
    //free(storeFFTAmp2);
    //free(storeFFTAmp1);
    free(harmonicsAmpLeft);
    free(harmonicsPhaseLeft);
    free(harmonicsAmpCenter);
    free(harmonicsPhaseCenter);
    free(harmonicsAmpRight);
    free(harmonicsPhaseRight);
    fftwf_destroy_plan(planDataFFT2Time);
    fftwf_destroy_plan(planDataTime2FFT);
    fftwf_destroy_plan(planAutocorrFFT2Time);
    fftwf_destroy_plan(planAutocorrTime2FFT);
    //fftwf_free(equalLoudnessTable);
    fftwf_free(autocorrFFT);
    fftwf_free(autocorrTime);
    fftwf_free(dataFFT);
    fftwf_free(dataTime);
    fftwf_free(dataTemp);
    beenInit = false;
  }
}

/** Performs an autocorrelation on the input
  @param input An array of length n, in which the autocorrelation is taken
  @param ouput This should be an array of length k.
               This is the correlation of the signal with itself
               for delays 1 to k (stored in elements 0 to k-1)
  @return The sum of squares of the input. (ie the zero delay correlation)
  Note: Use the init function to set values of n and k before calling this.
*/
double MyTransforms::autocorr(float *input, float *output)
{
  myassert(beenInit);
  float fsize = float(size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+n, autocorrTime);
  std::fill(autocorrTime+n, autocorrTime+size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);
/*
  if(equalLoudness) { //use equal-loudness curves
    double radius, ratio, dB, phons, newRadius;
    double speakerLoudness = 100; //in dB
    //double maxDB = -99999999.0;
    for(int j=1; j<size/2; j++) {
      radius = sqrt(sq(autocorrFFT[j]) + sq(autocorrFFT[size-j])) / fsize;
      if(!(radius > 0.0)) continue; //includes NAN
      dB = 50.0; //20.0 * log10(radius) + speakerLoudness;
      phons = dbToPhons(double(j) * freqPerBin, dB);
      //newRadius = pow10((phons-90.0) / 20.0);
      //ratio = newRadius / radius;
      ratio = pow10((phons - dB) / 20.0);
      autocorrFFT[j] *= ratio;
      autocorrFFT[size-j] *= ratio;
      //if(newRadius > maxDB) maxDB = newRadius;
    }
    autocorrFFT[0] = 0;
    autocorrFFT[size/2] = 0;
    //printf("maxNewRadius = %lf\n", maxDB); 
       
    //Do an inverse FFT
    fftwf_execute(planAutocorrFFT2Time);
    //normalize
    for(int x=0; x<n; x++)
      autocorrTime[x] /= fsize;
    
    //fix up the zero padding again
    std::fill(autocorrTime+n, autocorrTime+size, 0.0f);
    //copy back to the input array so other calculation can be done on this
    std::copy(autocorrTime, autocorrTime+n, input);
    
    //Do a forward FFT
    fftwf_execute(planAutocorrTime2FFT);
  }
*/
  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j=1; j<size/2; j++) {
    autocorrFFT[j] = sq(autocorrFFT[j]) + sq(autocorrFFT[size-j]);
    autocorrFFT[size-j] = 0.0f;
  }
  autocorrFFT[0] = sq(autocorrFFT[0]);
  autocorrFFT[size/2] = sq(autocorrFFT[size/2]);

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  //for(int x=0; x<k; x++)
  //  output[x] = autocorrTime[x+1] / fsize;
  for(float *p1=output, *p2=autocorrTime+1; p1<output+k;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(size);
}

/** The Average Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
*/
double MyTransforms::asdf(float *input, float *output)
{
  myassert(beenInit);
  double sumSq = autocorr(input, output); //Do an autocorrelation and return the sum of squares of the input
  double sumRightSq = sumSq, sumLeftSq = sumSq;
  for(int j=0; j<k; j++) {
    sumLeftSq  -= sq(input[n-1-j]);
    sumRightSq -= sq(input[j]);
    output[j] = sumLeftSq + sumRightSq - 2*output[j];
  }
  return sumSq;
}

/** The Normalised Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
    @return The sum of square
*/
double MyTransforms::nsdf(float *input, float *output)
{
  myassert(beenInit);
  double sumSq = autocorr(input, output); //the sum of squares of the input

  double sumRightSq = sumSq, sumLeftSq = sumSq;
  if(gdata->analysisType() == 0) { //nsdf
    for(int j=0; j<k; j++) {
      sumLeftSq  -= sq(input[n-1-j]);
      sumRightSq -= sq(input[j]);
      //output[j] = 1.0 - (sumLeftSq + sumRightSq - 2*output[j]) / (sumLeftSq + sumRightSq);
      output[j] = 2.0 * output[j] / (sumLeftSq + sumRightSq);
    }
  } else { //autocorr
    for(int j=0; j<k; j++) {
      //output[j] = output[j] * float(n) / float(n-1 - j) / sumSq;
      output[j] = output[j] / sumSq;
    }
  }
  return sumSq;
}

/** Estimate the period or the fundamental frequency.
  Finds the first maximum of NSDF which past the first
  positive zero crossing and is over the threshold.
  If no maxima are over the threshold then the the highest maximum is returned.
  If no positive zero crossing is found, zero is returned.
  @param input. An array of length n.
  @param threshold. A number between 0 and 1 at which maxima above are acceped.
  @return The estimated period (in samples), or zero if none found.
*/
//void MyTransforms::calculateAnalysisData(float *input, AnalysisData &analysisData, Channel *ch, float threshold)
void MyTransforms::calculateAnalysisData(float *input, int chunk, Channel *ch, float threshold)
{
  myassert(ch);
  myassert(ch->dataAtChunk(chunk));
  AnalysisData &analysisData = *ch->dataAtChunk(chunk);
  //Array1d<float> output(k);
  float *output = ch->nsdfData.begin();

  std::vector<int> maxPositions;
  int pos = 0;
  int curMaxPos = 0;
  //float corrError = 0.0f;
  //freqPerBin = rate / 2.0 / double(size);

  analysisData.maxIntensity = fabs(*std::max_element(input, input+n, absoluteLess()));
  
  if(gdata->doingActiveFFT()) {
    //std::copy(dataTime, dataTime+n, dataTemp);
    if(equalLoudness)
      std::copy(ch->filteredInput.begin(), ch->filteredInput.end(), dataTime);
    else
      std::copy(ch->directInput.begin(), ch->directInput.end(), dataTime);
    doChannelDataFFT(ch);
    //std::copy(dataTemp, dataTemp+n, dataTime);
  }
  if(equalLoudness)
    std::copy(ch->filteredInput.begin(), ch->filteredInput.end(), dataTime);
  else
    std::copy(ch->directInput.begin(), ch->directInput.end(), dataTime);
  
  //if(!gdata->doingActiveAnalysis()) return;
  //if(gdata->doingFreqAnalysis()) {
  if(gdata->doingFreqAnalysis() && gdata->doingActiveAnalysis()) {
    //calculate the Normalised Square Difference Function
    //analysisData.rms = nsdf(dataTime, output.begin()) / double(n/*size*/);
    analysisData.rms = nsdf(dataTime, output) / double(n/*size*/);
    
/**
    Find the highest maxima between each pair of positive zero crossings.
    Including the highest maxima between the last +ve zero crossing and the end if any.
    Ignoring the first (which is at zero)
    In this diagram the disired maxima are marked with a *
    
                  *             *
    \      *     /\      *     /\
    _\____/\____/__\/\__/\____/__
      \  /  \  /      \/  \  /
       \/    \/            \/
*/
    
    //while(pos < k-1 && output[pos] > 0.0f) pos++; //find the first negitive zero crossing
    while(pos < (k-1)/3 && output[pos] > 0.0f) pos++; //find the first negitive zero crossing
    while(pos < k-1 && output[pos] <= 0.0f) pos++; //loop over all the values below zero
    if(pos == 0) pos = 1; // can happen if output[0] is NAN
    
    while(pos < k-1) {
      myassert(!(output[pos] < 0)); //don't assert on NAN
      if(output[pos] > output[pos-1] && output[pos] >= output[pos+1]) { //a local maxima
        //if(output[pos] >= threshold) { maxPos = pos; break; } //the first maxima above threshold. Stop there.
        if(curMaxPos == 0) curMaxPos = pos; //the first maxima (between zero crossings)
        else if(output[pos] > output[curMaxPos]) curMaxPos = pos; //a higher maxima (between the zero crossings)
      }
      pos++;
      if(pos < k-1 && output[pos] <= 0.0f) { //a negative zero crossing
        if(curMaxPos > 0) { //if there was a maximum
          maxPositions.push_back(curMaxPos); //add it to the vector of maxima
          curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
        }
        while(pos < k-1 && output[pos] <= 0.0f) pos++; //loop over all the values below zero
      }
    }
  
    if(curMaxPos > 0) { //if there was a maximum in the last part
      maxPositions.push_back(curMaxPos); //add it to the vector of maxima
      curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
    }
    
    //store some of the best period estimates
    analysisData.periodEstimates.clear();
    analysisData.periodEstimatesAmp.clear();
    //float smallThreshold = 0.7f;
    //float smallCutoff = output[overallMaxIndex] * smallThreshold;
    float smallCutoff = 0.5f;
    for(std::vector<int>::iterator iter = maxPositions.begin(); iter < maxPositions.end(); iter++) {
      if(output[*iter] >= smallCutoff) {
        //analysisData.periodEstimates.push_back(double(*iter + 1) + parabolaTurningPoint(output[*iter-1], output[*iter], output[*iter+1]));
        //analysisData.periodEstimatesAmp.push_back(output[*iter]); //TODO: These should be calculated more accurately
        float x, y;
        //do a parabola fit to find the maximum
        parabolaTurningPoint2(output[*iter-1], output[*iter], output[*iter+1], float(*iter + 1), &x, &y);
        y = bound(y, -1.0f, 1.0f);
        analysisData.periodEstimates.push_back(x);
        analysisData.periodEstimatesAmp.push_back(y);
      }
    }
    
    //if(maxPositions.empty()) { //no period found
    if(analysisData.periodEstimates.empty()) { //no period found
      analysisData.correlation = 0.0f;
      analysisData.fundamentalFreq = 0.0f;
      analysisData.note = 0.0f;
      analysisData.volumeValue = 0.0f;
      analysisData.highestCorrelationIndex = -1;
      analysisData.chosenCorrelationIndex = -1;
      analysisData.noteIndex = -1;
      analysisData.done = true;
      return;
    }
    
    
    //find the overall maximum position
    int overallMaxIndex = 0;
    uint iterPos;
    //for(std::vector<float>::iterator iterPos = analysisData.periodEstimatesAmp.begin()+1; iterPos < analysisData.periodEstimatesAmp.end(); iterPos++) {
    for(iterPos = 1; iterPos < analysisData.periodEstimatesAmp.size(); iterPos++) {
      if(analysisData.periodEstimatesAmp[iterPos] > analysisData.periodEstimatesAmp[overallMaxIndex]) overallMaxIndex = iterPos;
    }
    analysisData.highestCorrelationIndex = overallMaxIndex;

    chooseCorrelationIndex(analysisData, threshold);
    
    if(gdata->doingHarmonicAnalysis()) {
      std::copy(dataTime, dataTime+n, dataTemp);
      doHarmonicAnalysis(dataTemp, analysisData, analysisData.periodEstimates[analysisData.chosenCorrelationIndex]/*period*/);
      //doHarmonicAnalysis(input, analysisData, period);
    }
    
    //analysisData.volumeValue = MAX((log(analysisData.rms) / 10.0) + 1.0, 0.0) * analysisData.correlation;
    analysisData.volumeValue = ((log(analysisData.rms) * 20.0) + (analysisData.correlation-1.0f) * 160.0f) * 0.2;
    //analysisData.volumeValue = sqrt(sqrt(sqrt(analysisData.rms * analysisData.correlation)));
  
  
    if(!analysisData.done) {
      if(chunk == 0 || ch->noteData.empty()) {
        myassert(ch->noteData.empty());
        ch->noteData.push_back(NoteData(0, 1, analysisData.rms, analysisData.maxIntensity, analysisData.correlation, analysisData.volumeValue));
        //analysisData.noteIndex = 0;
      } else {
        myassert(ch->isValidChunk(chunk));
        AnalysisData &prevAnalysisData = *ch->dataAtChunk(chunk-1);
        uint x1=0, x2=0;
        std::vector<float> &prevPeriods = prevAnalysisData.periodEstimates;
        std::vector<float> &curPeriods = analysisData.periodEstimates;
        int noteScore = 0;
/*
        int highestScore = MIN(prevPeriods.size(), curPeriods.size());
        while(x1 < prevPeriods.size() && x2 < curPeriods.size()) {
          if(within(0.5, freq2note(rate / prevPeriods[x1]), freq2note(rate / curPeriods[x2]))) { //within 0.5 simi-tones
            noteScore++;
            x1++; x2++;
          } else {
            if(prevPeriods[x1] < curPeriods[x2]) x1++;
            else x2++;
          }
        }
        //printf("%d, %d, %lf\n", highestScore, noteScore, double(noteScore) / double(highestScore));
        if(noteScore > 0 && double(noteScore) / double(highestScore) > 0.6) { //same note
*/
        //try forwards
        for(x2=0; x2<curPeriods.size(); x2++) {
          if(within(0.25, double(prevAnalysisData.note), freq2note(rate / curPeriods[x2]))) { //within 0.75 simi-tones
            noteScore = 1;
            break;
          }
        }
        //try backwards
        if(noteScore == 0) {
          for(x1=0; x1<prevPeriods.size(); x1++) {
            if(within(0.25, double(analysisData.note), freq2note(rate / prevPeriods[x1]))) { //within 0.75 simi-tones
              noteScore = 1;
              break;
            }
          }
        }
        if(noteScore > 0) {
          ch->noteData.back().endChunk = chunk+1;
          ch->noteData.back().addValues(analysisData.rms, analysisData.maxIntensity, analysisData.correlation, analysisData.volumeValue);
        } else { //new note?
          if(ch->noteData.back().size() < 5) {
            //clear all the existing pointers to the note
            //for(AnalysisData *it=ch->dataAtChunk(ch->noteData.back().startChunk); it < ch->dataAtChunk(ch->noteData.back().endChunk); it++) {
            for(large_vector<AnalysisData>::iterator it=ch->dataIteratorAtChunk(ch->noteData.back().startChunk); it < ch->dataIteratorAtChunk(ch->noteData.back().endChunk); it++) {
              //myassert(it);
              it->noteIndex = -2;
            }
            ch->noteData.back().setChunks(chunk, chunk+1);
            ch->noteData.back().addValues(analysisData.rms, analysisData.maxIntensity, analysisData.correlation, analysisData.volumeValue);
          } else {
            ch->noteData.push_back(NoteData(chunk, chunk+1, analysisData.rms, analysisData.maxIntensity, analysisData.correlation, analysisData.volumeValue));
          }
        }
      }
      analysisData.noteIndex = ch->noteData.size()-1;
      analysisData.done = true;
    }
  
  
  } else { //no freq analysis
    int j;
    if(!analysisData.done) {
      //calc rms by hand
      analysisData.rms = 0.0f;
      for(j=0; j<n; j++) {
        analysisData.rms += sq(dataTime[j]);
      }
      //analysisData.rms = sqrt(analysisData.rms);
      analysisData.rms /= float(n);
      analysisData.correlation = 0.0f;
      analysisData.fundamentalFreq = 0.0f;
      analysisData.note = 0.0f;
      analysisData.volumeValue = 0.0;
      analysisData.highestCorrelationIndex = -1;
      analysisData.chosenCorrelationIndex = -1;
      analysisData.noteIndex = -1;
      analysisData.done = true;
    }
  }
}

void MyTransforms::chooseCorrelationIndex(AnalysisData &analysisData, float threshold)
{    
  uint iterPos;
  int choosenMaxIndex = 0;
  if(analysisData.periodEstimates.empty()) return; //no period found
  if(gdata->analysisType() == 0) {
    //choose a cutoff value based on the highest value and a relative threshold
    float cutoff = analysisData.periodEstimatesAmp[analysisData.highestCorrelationIndex] * threshold;
    //find the first of the maxPositions which is above the cutoff
    for(iterPos = 0; iterPos < analysisData.periodEstimatesAmp.size(); iterPos++) {
      if(analysisData.periodEstimatesAmp[iterPos] >= cutoff) { choosenMaxIndex = iterPos; break; }
    }
    //myassert(wantedMaxPos > 0);
  } else {
    choosenMaxIndex = analysisData.highestCorrelationIndex;
  }
  analysisData.chosenCorrelationIndex = choosenMaxIndex;
  analysisData.correlation = analysisData.periodEstimatesAmp[choosenMaxIndex];

  double period = analysisData.periodEstimates[choosenMaxIndex];
  double freq = rate / period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.note = bound(freq2note(freq), 0.0, gdata->topNote());
  if(isnan(analysisData.note)) analysisData.note = 0.0f;
}

/** Builds a lookup table for use in doEqualLoudness
  @param dB The decibel level of at which the human ear filter should be simulated (default 50dB)
*/
/*
void MyTransforms::buildEqualLoudnessTable(double dB)
{
  double phons;
  float maxLoudness = 0.00000001f;
  equalLoudnessTable[0] = 0.0;
  for(int k=1; k<n/2; k++) {
    phons = dbToPhons(double(k) * freqPerBin, dB);
    equalLoudnessTable[k] = pow10((phons - dB) / 20.0);
    if(equalLoudnessTable[k] > maxLoudness) maxLoudness = equalLoudnessTable[k];
  }
  //normalise
  for(int y=1; y<n/2; y++) {
    equalLoudnessTable[y] /= maxLoudness;
    //printf("[%d] = %f\n", j, equalLoudnessTable[j]);
  }
}
*/

/** Modify the coefficients in Fourier space to give approximate the human ear filter.
*/
/*
//void MyTransforms::doEqualLoudness(Channel *ch)
void MyTransforms::doEqualLoudness()
{
  fftwf_execute(planDataTime2FFT);
  
  //double logSize = log10(double(ch->fftData1.size()));
  //Adjust the coefficents, both real and imaginary part by same amount
  for(int j=1; j<n/2; j++) {
    //LOG RULES: log(sqrt(x)) = log(x) / 2.0
    //LOG RULES: log(a * b) = log(a) + log(b)
    //storeFFTAmp1[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData1[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData1[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds);
    //ch->fftData1[j] = log10(sq(dataFFT[j]) + sq(dataFFT[n-j])) / 2.0 - logSize;
    dataFFT[j] *= equalLoudnessTable[j]; //real part
    dataFFT[n-j] *= equalLoudnessTable[j]; //imaginary part
    //storeFFTAmp2[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData2[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData2[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds );
    //ch->fftData2[j] = log10(sq(dataFFT[j]) + sq(dataFFT[n-j])) / 2.0 - logSize;
  }
  //storeFFTAmp1[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData1[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData1[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  //ch->fftData1[0] = log10(sq(dataFFT[0]) + sq(dataFFT[n/2])) / 2.0 - logSize;
  dataFFT[0] = 0; //remove the mean/DC component
  dataFFT[n/2] = 0;
  //storeFFTAmp2[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData2[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData2[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  //ch->fftData2[0] = log10(sq(dataFFT[0]) + sq(dataFFT[n/2])) / 2.0 - logSize;

  fftwf_execute(planDataFFT2Time);

  float fn = float(n);
  //normalize
  //for(int x=0; x<n; x++)
  //  dataTime[x] /= fn;
  for(float *ptr=dataTime; ptr<dataTime+n;)
    *ptr++ /= fn;
}
*/

/** Do an FFT for use in the Active FFT data window
  Note: This uses the values from dataTime and can destroy them
*/
void MyTransforms::doChannelDataFFT(Channel *ch)
{
  fftwf_execute(planDataTime2FFT);
  
  //LOG RULES: log(sqrt(x)) = log(x) / 2.0
  //LOG RULES: log(a * b) = log(a) + log(b)
  double logSize = log10(double(ch->fftData1.size())); //0.0
  //Adjust the coefficents, both real and imaginary part by same amount
  for(int j=1; j<n/2; j++) {
    //ch->fftData1[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds);
    ch->fftData1[j] = log10(sq(dataFFT[j]) + sq(dataFFT[n-j])) / 2.0 - logSize;
  }
  //ch->fftData1[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  ch->fftData1[0] = log10(sq(dataFFT[0]) + sq(dataFFT[n/2])) / 2.0 - logSize;
  if(gdata->doingActiveCepstrum()) {
    for(int j=1; j<n/2; j++) {
      dataFFT[j] = ch->fftData1[j];
      dataFFT[n-j] = 0.0;
    }
    dataFFT[0] = ch->fftData1[0];
    dataFFT[0] = 0.0;
    fftwf_execute(planDataFFT2Time);
    std::copy(dataTime, dataTime+n/2, ch->cepstrumData.begin());
  }
}

void MyTransforms::calcHarmonicAmpPhase(float *harmonicsAmp, float *harmonicsPhase, int binsPerHarmonic)
{
  int harmonic;
  for(int j=0; j<numHarmonics; j++) {
    harmonic = (j+1) * binsPerHarmonic;
    if(harmonic < n) {
      harmonicsAmp[j] = sqrt(sq(dataFFT[harmonic]) + sq(dataFFT[n-harmonic]));
      harmonicsPhase[j] = atan2f(dataFFT[n-harmonic], dataFFT[harmonic]);
    } else {
      harmonicsAmp[j] = 0.0;
      harmonicsPhase[j] = 0.0;
    }
  }
}

void MyTransforms::doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period)
{
  double numPeriodsFit = floor(double(n) / period); //2.0
  double numPeriodsUse = numPeriodsFit - 1.0;
  int iNumPeriodsUse = int(numPeriodsUse);
  double centerX = float(n) / 2.0;
  //do left
  //stretch_array(n, input, n, dataTime, (float(n) / 2.0) - period, period, LINEAR);
  double start = centerX - (numPeriodsFit / 2.0) * period;
  double length = (numPeriodsUse) * period;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpLeft, harmonicsPhaseLeft, iNumPeriodsUse);
  
  //do center
  //float start = (float(n) - period) / 2.0;
  //stretch_array(n, input, n, dataTime, start, period, LINEAR);
  //start = centerX - ((numPeriodsUse) / 2.0) * period;
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpCenter, harmonicsPhaseCenter, iNumPeriodsUse);
  
  //do right
  //stretch_array(n, input, n, dataTime, float(n)/2.0, period, LINEAR);
  //start = centerX - ((numPeriodsFit / 2.0) - 1) * period;
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpRight, harmonicsPhaseRight, iNumPeriodsUse);
  
  double freq = rate / period;
  int harmonic;
  
  analysisData.harmonicAmp.resize(numHarmonics);
  analysisData.harmonicFreq.resize(numHarmonics);
  analysisData.harmonicNoise.resize(numHarmonics);
  for(int j=0; j<numHarmonics; j++) {
    harmonic = (j+1) * iNumPeriodsUse;
    //analysisData.harmonicAmp[j] = float(analysisData.correlation / double(j+1));
    //analysisData.harmonicFreq[j] = float(freq * double(j+1));
    //analysisData.harmonicAmp[j] = log10(sqrt(sq(dataFFT[j+1]) + sq(dataFFT[n-(j+1)]))) / 10.0;
    analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j]) / 5.0;
    if(analysisData.harmonicAmp[j] < 0.0) analysisData.harmonicAmp[j] = 0.0;
    //should be 1 whole period between left and right. i.e. the same freq give 0 phase difference
    double diffAngle = (harmonicsPhaseRight[j] - harmonicsPhaseLeft[j]) / twoPI;
    //if(diffAngle < 0) diffAngle++;
    //if(diffAngle > 0.5) diffAngle--;
    diffAngle = cycle(diffAngle + 0.5, 1.0) - 0.5;
    double diffAngle2 = (harmonicsPhaseCenter[j] - harmonicsPhaseLeft[j]) / twoPI;
    //if an odd harmonic the phase will be 180 degrees out. So correct for this
    if(j % 2 == 0) diffAngle2 += 0.5;
    //if(diffAngle2 < 0) diffAngle2++;
    //while(diffAngle2 > 0.5) diffAngle2--;
    diffAngle2 = cycle(diffAngle2 + 0.5, 1.0) - 0.5;
    analysisData.harmonicNoise[j] = fabs(diffAngle2 - diffAngle);
    analysisData.harmonicFreq[j] = float(freq * double(j+1)) + (freq*diffAngle);
  }
}
/*
void MyTransforms::applyHighPassFilter(float *input, float *output)
{
  int j;
  highPassFilter->clear();
  for(j=0; j<n; j++) output[j] = bound(highPassFilter->apply(input[j]), -1.0, 1.0);
}
*/
