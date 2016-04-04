/***************************************************************************
                          notedata.cpp  -  description
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

#include "notedata.h"

NoteData::NoteData()
{
  maxRms = -150.0f;
}

NoteData::NoteData(int startChunk_, int endChunk_, float rms_, float intensity_, float correlation_, float purity_)
{
  startChunk = startChunk_;
  endChunk = endChunk_;
  maxRms = rms_;
  maxIntensity = intensity_;
  maxCorrelation = correlation_;
  maxPurity = purity_;
}

NoteData::~NoteData()
{
}

void NoteData::addValues(float rms_, float intensity_, float correlation_, float purity_)
{
  if(rms_ > maxRms) maxRms = rms_;
  if(intensity_ > maxIntensity) maxIntensity = intensity_;
  if(correlation_ > maxCorrelation) maxCorrelation = correlation_;
  if(purity_ > maxPurity) maxPurity = purity_;
}
