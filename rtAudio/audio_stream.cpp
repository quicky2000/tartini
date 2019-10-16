/***************************************************************************
                          audio_stream.cpp  -  joining to rtAudio audio routines
                             -------------------
    begin                : 2006
    copyright            : (C) 2006-2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "audio_stream.h"
#include "gdata.h"

//------------------------------------------------------------------------------
AudioStream::AudioStream(void)
{
  buffer_size = 1024;
  num_buffers = 0;
  audio = NULL;
  buffer = NULL;
  flowBuffer.setAutoGrow(true);
  inDevice = outDevice = 0;
}

//------------------------------------------------------------------------------
AudioStream::~AudioStream(void)
{
  close();
  if(audio) delete audio;
}

//------------------------------------------------------------------------------
void AudioStream::close(void)
{
  if(audio)
    {
      audio->stopStream();
      audio->closeStream();
    }
}

//------------------------------------------------------------------------------
int AudioStream::open(int mode_, int freq_, int channels_, int /*bits_*/, int buffer_size_)
{
  mode = mode_;
  set_frequency(freq_);
  set_channels(channels_);
  //bits_; //ignored, just use floats and let rtAudio do the conversion
  set_bits(32);
  buffer_size = buffer_size_;
  num_buffers = 4;

  QStringList inNames = getInputDeviceNames();
  const char * audioInput = gdata->getSettingsValue("Sound/soundInput", QString("Default"));
  inDevice = getDeviceNumber(audioInput);

  QStringList outNames = getOutputDeviceNames();
  const char * audioOutput = gdata->getSettingsValue("Sound/soundOutput", QString("Default"));
  outDevice = getDeviceNumber(audioOutput);

  fprintf(stderr, "Input Device %d: %s\n", inDevice, audioInput);
  fprintf(stderr, "Output Device %d: %s\n", outDevice, audioOutput);

  try
    {
      if(mode == F_READ)
	{
	  audio = new RtAudio(0, 0, inDevice, get_channels(), RTAUDIO_FLOAT32, get_frequency(), &buffer_size, num_buffers);
	}
      else if(mode == F_WRITE)
	{
	  audio = new RtAudio(outDevice, get_channels(), 0, 0, RTAUDIO_FLOAT32, get_frequency(), &buffer_size, num_buffers);
	}
      else if(mode == F_RDWR)
	{
	  audio = new RtAudio(outDevice, get_channels(), inDevice, get_channels(), RTAUDIO_FLOAT32, get_frequency(), &buffer_size, num_buffers);
	}
      else
	{
	  fprintf(stderr, "No mode selected\n");
	  return -1;
	}
    }
  catch (RtError &error)
    {
      error.printMessage();
      audio = NULL;
      return -1;
    }

  if(buffer_size != buffer_size_)
    {
      fprintf(stderr, "Warning: Got buffer_size of %d instead of %d\n", buffer_size, buffer_size_);
    }

  try
    {
      // Get a pointer to the stream buffer
      buffer = (float *) audio->getStreamBuffer();
      audio->startStream();
    }
  catch (RtError &error)
    {
      error.printMessage();
      delete audio;
      return -1;
    }
  return 0;
}

//------------------------------------------------------------------------------
int AudioStream::writeFloats(float **channelData, int length, int ch)
{
  float * bufPtr = buffer;
  int c, j;
  if(length == buffer_size)
    {
      for(j = 0; j < length; j++)
	{
	  for(c = 0; c < ch; c++)
	    {
	      *bufPtr++ = channelData[c][j];
	    }
	}

      // Trigger the output of the data buffer
      try
	{
	  audio->tickStream();
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  return 0;
	}
    }
  else
    {
      for(j = 0; j < length; j++)
	{
	  for(c = 0; c < ch; c++)
	    {
	      flowBuffer.put(channelData[c][j]);
	    }
	}
      while(flowBuffer.size() >= buffer_size * ch)
	{
	  int recieved = flowBuffer.get(buffer, buffer_size * ch);
	  if(recieved != buffer_size * ch)
	    {
	      fprintf(stderr, "AudioStream::writeFloats: Error recieved != buffer_size*ch\n");
	    }
	  // Trigger the output of the data buffer
	  try
	    {
	      audio->tickStream();
	    }
	  catch (RtError &error)
	    {
	      error.printMessage();
	      return 0;
	    }
	}
    }
  return length;
}

//------------------------------------------------------------------------------
int AudioStream::readFloats(float **channelData, int length, int ch)
{
  float * bufPtr = buffer;
  int c, j;
  if(length == buffer_size)
    {
      // Trigger the input of the data buffer
      try
	{
	  audio->tickStream();
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  return 0;
	}
    for(j = 0; j < length; j++)
      {
	for(c = 0; c < ch; c++)
	  {
	    channelData[c][j] = *bufPtr++;
	  }
      }
    }
  else
    {
      fprintf(stderr, "Error reading floats\n");
    }
  return length;
}

//------------------------------------------------------------------------------
int AudioStream::writeReadFloats(float ** outChannelData, int outCh, float ** inChannelData, int inCh, int length)
{
  float * bufPtr = buffer;
  int c, j;

  //put the outChannelData into the Audio buffer to be written out
  if(length == buffer_size)
    {
      for(j = 0; j < length; j++)
	{
	  for(c = 0; c < outCh; c++)
	    {
	      *bufPtr++ = outChannelData[c][j];
	    }
	}
      // Trigger the input/output of the data buffer
      try
	{
	  audio->tickStream();
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  return 0;
	}

    //get the new data from the Audio buffer and put it in inChannelData
    bufPtr = buffer;
    for(j = 0; j < length; j++)
      {
	for(c = 0; c < inCh; c++)
	  {
	    inChannelData[c][j] = *bufPtr++;
	  }
      }
    }
  else
    {
      fprintf(stderr, "Error: audio buffer length is a different size than the data chunk buffer\n");
      fprintf(stderr, "Not supported on Read/Write yet. Try changing the buffer size in preferences\n");
    }
  return length;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getInputDeviceNames(void)
{
  QStringList toReturn;
  toReturn << "Default";

  RtAudio * tempAudio = NULL;
  try
    {
      tempAudio = new RtAudio();
    }
  catch (RtError &error)
    {
      error.printMessage();
      return toReturn;
    }

  // Determine the number of devices available
  int numDevices = tempAudio->getDeviceCount();

  // Scan through devices for various capabilities
  RtAudioDeviceInfo info;
  for (int i = 1; i <= numDevices; i++)
    {
      try
	{
	  info = tempAudio->getDeviceInfo(i);
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  break;
	}
      if(info.inputChannels > 0)
	{
	  toReturn << info.name.c_str();
	}
    }

  // Clean up
  delete tempAudio;
  return toReturn;
}

//------------------------------------------------------------------------------
QStringList AudioStream::getOutputDeviceNames(void)
{
  QStringList toReturn;
  toReturn << "Default";

  RtAudio *tempAudio = NULL;
  try
    {
      tempAudio = new RtAudio();
    }
  catch (RtError &error)
    {
      error.printMessage();
      return toReturn;
    }

  // Determine the number of devices available
  int numDevices = tempAudio->getDeviceCount();

  // Scan through devices for various capabilities
  RtAudioDeviceInfo info;
  for (int i = 1; i <= numDevices; i++)
    {
      try
	{
	  info = tempAudio->getDeviceInfo(i);
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  break;
	}

      if(info.outputChannels > 0)
	{
	  toReturn << info.name.c_str();
	}
    }
  // Clean up
  delete tempAudio;
  return toReturn;
}

//------------------------------------------------------------------------------
int AudioStream::getDeviceNumber(const char * deviceName)
{
  int deviceNumber = -1;
  if(strcmp("Default", deviceName) == 0)
    {
      return 0;
    }

  RtAudio * tempAudio = NULL;
  try
    {
      tempAudio = new RtAudio();
    }
  catch (RtError &error)
    {
      error.printMessage();
      return -1;
    }

  // Determine the number of devices available
  int numDevices = tempAudio->getDeviceCount();

  // Scan through devices for various capabilities
  RtAudioDeviceInfo info;
  for (int i = 1; i <= numDevices; i++)
    {
      try
	{
	  info = tempAudio->getDeviceInfo(i);
	}
      catch (RtError &error)
	{
	  error.printMessage();
	  break;
	}
      if(strcmp(info.name.c_str(), deviceName) == 0)
	{
	  deviceNumber = i;
	  break;
	}
  }
  // Clean up
  delete tempAudio;
  return deviceNumber;
}

// EOF
