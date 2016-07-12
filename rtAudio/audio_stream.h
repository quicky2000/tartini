/***************************************************************************
                          audio_stream.h  -  linux audio routines
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#ifndef AUDIO_STREAM_H
#define AUDIO_STREAM_H

#include <stdio.h>
#include "RtAudio.h"
#include "sound_stream.h"
#include "RingBuffer.h"

class AudioStream: public SoundStream
{
 public:

  AudioStream(void);
  virtual ~AudioStream(void);

  int bufferSize(void) { return buffer_size; }
  int numBuffers(void) { return num_buffers; }
  int totalBufferFrames(void) { return buffer_size * num_buffers; }
  int inTotalBufferFrames(void) { return totalBufferFrames(); }
  int outTotalBufferFrames(void) { return totalBufferFrames(); }

  void close(void);

  int open(int mode_, int freq_ = 44100, int channels_ = 2, int bits_ = 16, int buffer_size_ = 2048);

  //note: use the writeFloats or readFloats instead
  long read_bytes(void * /*data*/, long /*length*/) { return 0; } //not implemented
  long read_frames(void * /*data*/, long /*length*/) { return 0; } //not implemented
  long write_bytes(void * /*data*/, long /*length*/) { return 0; } //not implemented
  long write_frames(void * /*data*/, long /*length*/) { return 0; } //not implemented

  int writeFloats(float **channelData, int length, int ch);
  int readFloats(float **channelData, int length, int ch);

  /**
     This requires that inChannelData and outChannelData have the same number of channels
     and the same length, and this length is the same size as the AudioStream buffer
     @param outChannelData The data to write out to the sound card
     @param inChannelData The data read back from the sound card
     @param length The amount of data per channel. This has to be the same for in and out data
     @param ch The number of channels. This has to be the same of in and out data
  */
  int writeReadFloats(float ** outChannelData, int outCh, float ** inChannelData, int inCh, int length);
  bool isSameInOutDevice(void) { return inDevice == outDevice; }

  static QStringList getInputDeviceNames(void);
  static QStringList getOutputDeviceNames(void);
  /**
     @param deviceName The name of a device as given from get*DeviceNames
     @return The device number that matches the name, or -1 if the device name is not found
  */
  static int getDeviceNumber(const char * deviceName);

 private:
  int buffer_size; //in frames
  int num_buffers;
  RtAudio *audio;
  float *buffer;
  RingBuffer<float> flowBuffer;
  int inDevice, outDevice;

};
#endif // AUDIO_STREAM_H
// EOF
