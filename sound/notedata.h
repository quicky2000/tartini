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

/**
@author Philip McLeod
*/
class NoteData
{
public:
  NoteData();
  NoteData(int startChunk_, int endChunk_, float rms_, float intensity_, float correlation_, float purity_);
  ~NoteData();

  int size() { return endChunk - startChunk; }
  void setChunks(int startChunk_, int endChunk_) { startChunk = startChunk_; endChunk = endChunk_; }
  void addValues(float rms_, float intensity_, float correlation_, float purity_);
  int startChunk; //the chunk at which this note starts on
  int endChunk; //the chunk after the last one in the note
  float maxRms; //The maximum RMS volume during the note
  float maxIntensity; //The maximum intensity volume during the note
  float maxCorrelation;
  float maxPurity;
};

#endif
