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

class Channel;
class Filter;

class MyTransforms
{
  int n, k, size;
  fftwf_plan planDataTime2FFT, planDataFFT2Time;
  fftwf_plan planAutocorrTime2FFT, planAutocorrFFT2Time;
  float *dataTemp;
  float *dataTime, *dataFFT;
  float *autocorrTime, *autocorrFFT;
  float *harmonicsAmpLeft, *harmonicsPhaseLeft;
  float *harmonicsAmpCenter, *harmonicsPhaseCenter;
  float *harmonicsAmpRight, *harmonicsPhaseRight;
  //float *equalLoudnessTable;
  //float *storeFFTAmp1;
  //float *storeFFTAmp2;
  bool beenInit;
  double freqPerBin;
  //float threshold;
  double rate;
  int numHarmonics;
  
 public:
  bool equalLoudness;
  
  MyTransforms();
  virtual ~MyTransforms();

  void init(int n_, int k_, double rate_, /*float threshold_=0.9, */bool equalLoudness_=false, int numHarmonics_=40);
  void uninit();
  double autocorr(float *input, float *output);
  double asdf(float *input, float *output);
  double nsdf(float *input, float *output);
  //void calculateAnalysisData(float *input, AnalysisData &analysisDatam, Channel *ch, float threshold);
  void calculateAnalysisData(float *input, int chunk, Channel *ch, float threshold);
  //void buildEqualLoudnessTable(double dB=50.0);
  //void doEqualLoudness();
  void doChannelDataFFT(Channel *ch);
  void calcHarmonicAmpPhase(float *harmonicAmp, float *harmonicPhase, int binsPerHarmonic);
  void doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period);
  void chooseCorrelationIndex(AnalysisData &analysisData, float threshold);
  //void applyHighPassFilter(float *input, float *output);
};
  
#endif
