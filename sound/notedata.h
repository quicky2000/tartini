/***************************************************************************
                          notedata.h  -  description
                             -------------------
    begin                : Mon May 23 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/

#ifndef NOTEDATA_H
#define NOTEDATA_H

#include "analysisdata.h"
#include "SmartPtr.h"
#include "array1d.h"
#include <vector>


class Channel;

/**
   @author Philip McLeod
*/
class NoteData
{
 public:
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;

  /**
     keeps the sum of scalers. i.e. The highest possible aggregate value
  */
  double nsdfAggregateRoof;
  float firstNsdfPeriod;
  float currentNsdfPeriod;

  inline NoteData(void);
  NoteData(Channel *channel_);
  NoteData(Channel *channel_, int startChunk_, AnalysisData *analysisData);
  ~NoteData(void);

  SmartPtr<Array1d<int> > maxima;

  void resetData(void);
  inline bool isValid(void) const;
  inline void setStartChunk(int startChunk_);
  inline void setEndChunk(int endChunk_);
  inline int startChunk(void) const;
  inline int endChunk(void) const;
  void addData(AnalysisData *analysisData, float periods);
  inline int numChunks(void) const;

  /**
     @return The length of the note (in seconds)
  */
  double noteLength(void) const;

  inline float numPeriods(void) const;

  /**
     @return in Hertz
   */
  inline double avgFreq(void);

  /**
     @return The average of this note, in fractions of semi-tones.
  */
  float avgPitch(void) const;
  inline void setPeriodOctaveEstimate(float periodOctaveEstimate_);
  inline float periodOctaveEstimate(void) const;
  void addVibratoData(int chunk);
  inline float volume(void) const;
  void recalcAvgPitch(void);

  inline const SmartPtr<Array1d<int> > get_minima(void) const;
 private:
  SmartPtr<Array1d<int> > minima;

  Channel *channel;

  /**
     the chunk at which this note starts on
  */
  int _startChunk;

  /**
     the chunk after the last one in the note
  */
  int _endChunk;

  /**
     The maximum RMS volume during the note
  */
  float maxLogRMS;

  /**
     The maximum intensity volume during the note
  */
  float maxIntensityDB;
  float maxCorrelation;
  float maxPurity;
  float _numPeriods;

  /**
     This is in terms of the periodRatio at the beginning of the note
  */
  float _periodOctaveEstimate;

  /**
     A normalised volume between 0 and 1
  */
  float _volume;

  float _avgPitch;

  /**
     In the vibrato-analysis, search pitchLookupSmoothed with steps of the size of loopStep
  */
  int loopStep;

  /**
     Remembers where the previous vibrato-analysis broke off
  */
  int loopStart;
  int prevExtremumTime;
  float prevExtremumPitch;
  enum PrevExtremum
    {
      NONE,
      FIRST_MAXIMUM,
      FIRST_MINIMUM,
      MAXIMUM,
      MINIMUM
    };
  PrevExtremum prevExtremum;

};

#include "notedata.hpp"

#endif // NOTEDATA_H
// EOF
