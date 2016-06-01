/***************************************************************************
                          mytransforms.h  -  description
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

#ifndef MYTRANSFORMS_H
#define MYTRANSFORMS_H

#include <fftw3.h>
#include "analysisdata.h"
#include "fast_smooth.h"

class Channel;
class Filter;

class MyTransforms
{  
 public:
  bool equalLoudness;
  
  MyTransforms(void);
  virtual ~MyTransforms(void);

  /**
     init() Initalises the parameters of a class instance. This must be called before use
     @param n_ The size of the data windows to be processed
     @param k_ The number of outputs wanted (autocorr size = n_ + k_). Set k_ = 0, to get default n_/2
     @param rate_ The sampling rate of the incoming signal to process
     @param threshold_ The ratio of highest peak to the first peak allowed to be chosen
  */
  void init(int n_, int k_, double rate_, /*float threshold_=0.9, */bool equalLoudness_=false, int numHarmonics_=40);

  void uninit(void);

  /**
      Performs an autocorrelation on the input
      @param input An array of length n, in which the autocorrelation is taken
      @param ouput This should be an array of length k.
      This is the correlation of the signal with itself
      for delays 1 to k (stored in elements 0 to k-1)
      @return The sum of squares of the input. (ie the zero delay correlation)
      Note: Use the init function to set values of n and k before calling this.
  */
  double autocorr(float *input, float *output);
  double autoLogCorr(float *input, float *output);

  /**
     The Average Square Difference Function.
     @param input. An array of length n, in which the ASDF is taken
     @param ouput. This should be an array of length k
  */
  double asdf(float *input, float *output);

  /**
     The Normalised Square Difference Function.
     @param input. An array of length n, in which the ASDF is taken
     @param ouput. This should be an array of length k
     @return The sum of square
  */
  double nsdf(float *input, float *output);

  /**
     Find the highest maxima between each pair of positive zero crossings.
     Including the highest maxima between the last +ve zero crossing and the end if any.
     Ignoring the first (which is at zero)
     In this diagram the disired maxima are marked with a *
     
                  *             *
    \      *     /\      *     /	\
    _\____/\____/__\/\__/\____/__
      \  /  \  /      \/  \  /
       \/    \/            \/

     @param input The array to look for maxima in
     @param len Then length of the input array
     @param maxPositions The resulting maxima positions are pushed back to this vector
     @return The index of the overall maximum
  */
  static int findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions);

  /**
     @return The index of the first sub maximum.
     This is now scaled from (threshold * overallMax) to 0.
  */
  static int findNSDFsubMaximum(float *input, int len, float threshold);

  /**
     Estimate the period or the fundamental frequency.
     Finds the first maximum of NSDF which past the first
     positive zero crossing and is over the threshold.
     If no maxima are over the threshold then the the highest maximum is returned.
     If no positive zero crossing is found, zero is returned.
     @param input. An array of length n.
     @param threshold. A number between 0 and 1 at which maxima above are acceped.
     @return The estimated period (in samples), or zero if none found.
  */
  void calculateAnalysisData(/*float *input, */int chunk, Channel *ch/*, float threshold*/);

  /**
     Do a Windowed FFT for use in the Active FFT data window
  */
  void doChannelDataFFT(Channel *ch, float *curInput, int chunk);
  void calcHarmonicAmpPhase(float *harmonicAmp, float *harmonicPhase, int binsPerHarmonic);
  void doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period);

  /**
     using the given fractional period, constructs a smaller subwindow
     which slide through the window. The smallest clarity measure of this
     moving sub window is returned.
  */
  float get_fine_clarity_measure(double period);
  double get_max_note_change(float *input, double period);
  void applyHanningWindow(float *d);

  /**
     @param buffer The input buffer, as outputted from the fftw. ie 1st half real, 2nd half imaginary parts
     @param len The length of the buffer, including real and imaganary parts
     @return The normalised frequency centriod
  */
  static double calcFreqCentroid(float *buffer, int len);

  /**
     @param buffer The input buffer of logarithmic magnitudes
     @param len The length of the buffer
     @return The normalised frequency centriod
  */
  static double calcFreqCentroidFromLogMagnitudes(float *buffer, int len);

 private:
  /**
     n = size of data
  */
  int n;

  /**
     k = size of padding for autocorrelation
   */
  int k;

  /**
     size = n+k
  */
  int size;

  fftwf_plan planDataTime2FFT;
  fftwf_plan planDataFFT2Time;
  fftwf_plan planAutocorrTime2FFT;
  fftwf_plan planAutocorrFFT2Time;
  float *dataTemp;
  float *dataTime;
  float *dataFFT;
  float *autocorrTime;
  float *autocorrFFT;
  float *hanningCoeff;
  float hanningScalar;
  float *harmonicsAmpLeft;
  float *harmonicsPhaseLeft;
  float *harmonicsAmpCenter;
  float *harmonicsPhaseCenter;
  float *harmonicsAmpRight;
  float *harmonicsPhaseRight;
  bool beenInit;
  double freqPerBin;
  double rate;
  int numHarmonics;
  fast_smooth *fastSmooth;

};
  
#endif // MYTRANSFORMS_H
//EOF
