/***************************************************************************
                          audio_stream.h  -  linux audio routines
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include <stdio.h>
#include "sound_stream.h"

class AudioStream : public SoundStream
{
 public:
  int device_no;
  int buffer_size;
  int pcm_vol;

  AudioStream();
  virtual ~AudioStream();

  void close();

  void signal1();
  void print_state();
  void set_params();
  int open(int mode_, int freq_=44100, int channels_=2, int bits_=16, int buffer_size_=2048);
  long read_bytes(void *data, long length);
  long read_frames(void *data, long length);

  long write_bytes(void *data, long length);
  long write_frames(void *data, long length);
  long wait_bytes(long length);
  long wait_frames(long length);

  static QStringList getInputDeviceNames();
  static QStringList getOutputDeviceNames();

};

#endif
