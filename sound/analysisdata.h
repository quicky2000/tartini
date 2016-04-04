/***************************************************************************
                          analysisdata.h  -  description
                             -------------------
    begin                : Fri Dec 17 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ANALYSIS_DAYA_H
#define ANALYSIS_DAYA_H

//#include "useful.h"
#include <functional>
#include <vector>
#include "filter.h"

class AnalysisData
{
public:
  float fundamentalFreq; /*< The fundamental frequency in hertz */
  float note; /*< The note in semi-tones */
  float correlation; /*< How well the fundamental frequency fits the signal (0=no fit, 1=perfet fit) */
  float rms; /*< The Root-mean-square, a measure of intensity/volume in the chunk */
  float maxIntensity;
  int highestCorrelationIndex;
  int chosenCorrelationIndex;
  float volumeValue; /*< A value between 0 and 1 related to volume and correlation */
  std::vector<float> periodEstimates;
  std::vector<float> periodEstimatesAmp;
  std::vector<float> harmonicAmp;
  std::vector<float> harmonicFreq;
  std::vector<float> harmonicNoise;
  FilterState filterState; //the state of the filter at the beginning of the chunk
  int noteIndex;
  bool done;
  //bool isValid();
  AnalysisData() { done = false; }
};


struct lessFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.fundamentalFreq < y.fundamentalFreq; }
};

struct greaterFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.fundamentalFreq > y.fundamentalFreq; }
};

struct lessNote : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.note < y.note; }
};

struct greaterNote : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.note > y.note; }
};

struct lessCorrelation : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.correlation < y.correlation; }
};

struct greaterCorrelation : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.correlation > y.correlation; }
};

struct lessVolumeValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.volumeValue < y.volumeValue; }
};

struct greaterVolumeValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.volumeValue > y.volumeValue; }
};

struct lessRMS : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.rms < y.rms; }
};

struct greaterRMS : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.rms > y.rms; }
};

struct lessMaxIntensity : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.maxIntensity < y.maxIntensity; }
};

struct greaterMaxIntensity : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.maxIntensity > y.maxIntensity; }
};

#endif
