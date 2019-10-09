/***************************************************************************
                          wave_stream.h  -  description
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
#ifndef WAVE_STREAM_H
#define WAVE_STREAM_H

#include "sound_file_stream.h"
#include <stdio.h>

class QStringList;

class WaveStream : public SoundFileStream
{
 public:
  WaveStream(void);
  virtual ~WaveStream(void);

  int open_read(const char *p_filename);
  int read_header(void);
  long read_bytes(void *p_data, long p_length);
  long read_frames(void *p_data, long p_length);

  int open_write(const char *p_filename, int p_freq=44100, int p_channels=2, int p_bits=16);
  void write_header(void);
  long write_bytes(void *p_data, long p_length);
  long write_frames(void *p_data, long p_length);

  QStringList getOutputDeviceNames(void);

  void close(void);

  void jump_to_frame(int p_frame);
  void jump_back(int p_frames);
  void jump_forward(int p_frames);

 private:
  FILE *m_file;
  int m_header_length;
};

#endif // WAVE_STREAM_H
// EOF
