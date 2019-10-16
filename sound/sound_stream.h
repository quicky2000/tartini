/***************************************************************************
                          sound_stream.h  -  description
                             -------------------
    begin                : Sat Nov 27 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SOUND_STREAM_H
#define SOUND_STREAM_H

#define F_NONE         0
#define F_READ         1
#define F_WRITE        2
#define F_RDWR         3

//class FBuffer;
class QStringList;

class SoundStream
{
 public:
  int channels;
  int bits;
  int mode;

  inline SoundStream(void);
  virtual inline ~SoundStream(void);

  inline int sample_size(void) const;
  inline int frame_size(void) const;

  virtual long read_bytes(void *data, long length) = 0;
  virtual long read_frames(void *data, long length) = 0;
  virtual long write_bytes(void *data, long length) = 0;
  virtual long write_frames(void *data, long length) = 0;
  inline virtual long wait_bytes(long /*length*/);
  inline virtual long wait_frames(long /*length*/);

  virtual int writeFloats(float **channelData, int length, int ch);
  virtual int readFloats(float **channelData, int length, int ch);
  virtual int writeReadFloats(float **outChannelData, int outCh, float **inChannelData, int inCh, int length);

    inline int get_frequency() const;

  protected:
    inline void set_frequency(int p_frequency);

  private:
    int freq;
};

#include "sound_stream.hpp"

#endif
