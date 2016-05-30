/***************************************************************************
                          sound_file_stream.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@pmcleod.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SOUND_FILE_STREAM_H
#define SOUND_FILE_STREAM_H

#include "sound_stream.h"
#include <stdio.h>

class SoundFileStream : public SoundStream
{
protected:
  int _total_frames;
  int _pos; //in frames

  inline void setPos(int newPos);
public:

  inline SoundFileStream();
  virtual inline ~SoundFileStream();

  inline int data_length();
  inline int totalFrames();
  inline int pos();
  
  virtual inline int open_read(const char * /*filename*/);
  virtual inline long read_bytes(void * /*data*/, long /*length*/);
  virtual inline long read_frames(void * /*data*/, long /*length*/);

  virtual inline int open_write(const char * /*filename*/, int /*freq_*/=44100, int /*channels_*/=2, int /*bits_*/=16);
  virtual inline long write_bytes(void * /*data*/, long /*length*/);
  virtual inline long write_frames(void * /*data*/, long /*length*/);

  virtual inline void close();

  virtual inline void jump_to_frame(int /*frame*/);
  virtual inline void jump_back(int /*frames*/);
  virtual inline void jump_forward(int /*frames*/);
};

#include "sound_file_stream.hpp"

#endif
