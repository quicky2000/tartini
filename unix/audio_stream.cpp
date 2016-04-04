/***************************************************************************
                          audio_stream.cpp  -  linux audio routines
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
#ifdef __linux__
#include <linux/soundcard.h>
#else
#include <machine/soundcard.h> /* JH20010905 */
#endif

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "audio_stream.h"
#include "gdata.h"

#include <errno.h>
#include <string.h>

/************************************************* Definitions ********/
/*
#ifdef __linux__
 #define DEVICE_NAME_BASE            "/dev/dsp1"
#else
 #define DEVICE_NAME_BASE            "/dev/audio"
#endif
*/

int CalcHigherLogTwo(int n)
{
    int log2 = 0;
    while( (1<<log2) < n ) log2++;
    return log2;
}

AudioStream::AudioStream()
{
  device_no = -1;
  buffer_size = 1024;
}

AudioStream::~AudioStream()
{
    close();
}

void AudioStream::close()
{
    if(device_no == -1) return;

    if(mode == F_READ) {
      if(pcm_vol >= 0) { //restore the output volume
        int mixer_no = ::open("/dev/mixer", O_RDONLY);
  
        if(ioctl(mixer_no, MIXER_WRITE(SOUND_MIXER_PCM), &pcm_vol) == -1)
          fprintf(stderr, "failed to restore output volume\n");
  
        ::close(mixer_no);
      }
    }

    ::close(device_no);
    device_no = -1;
}

void AudioStream::signal1()
{
  //ioctl(device_no, SNDCTL_DSP_POST, 0);
  ioctl(device_no, SNDCTL_DSP_SYNC, 0);
  //printf("sync\n");
}

void AudioStream::print_state()
{
  audio_buf_info info;

  ioctl(device_no, SNDCTL_DSP_GETISPACE, &info);
  printf("full fragments left=%d, num fragments=%d, fragment size=%d, bytes left=%d\n",
	 info.fragments,
	 info.fragstotal,
	 info.fragsize,
	 info.bytes);

  /*
  int frag_size;
  if (ioctl(device_no, SNDCTL_DSP_GETBLKSIZE, &frag_size) == -1) {
    printf("Error reading frag_size\n");
  }
  printf("frag_size = %d\n", frag_size);

  printf("\n");
  */
}

void AudioStream::set_params()
{
  int n_fragments = 4; // number of fragments
  //int fragment_size = CalcHigherLogTwo(buffer_size*sample_size()); // a fragment is 2^fragment_size bytes
  int fragment_size = CalcHigherLogTwo(buffer_size*frame_size()); // a fragment is 2^fragment_size bytes
  //printf("bytes==%d\n", buffer_size*sample_size());

  int value = (n_fragments << 16) | fragment_size;
  fprintf(stderr, "Try to set num fragments=%d, size of each fragment=%d\n", value >> 16, 1 << (value & 0xFF));
  int status = ioctl (device_no, SNDCTL_DSP_SETFRAGMENT, &value);
  if (status == -1)
    fprintf(stderr, "SNDCTL_DSP_SETFRAGMENT ioctl failed\n");

  fprintf(stderr, "Sound device: try: channels=%d, freq=%d, bits=%d\n", channels, freq, bits);

  int arg = bits;      /* sample size */
  status = ioctl(device_no, SOUND_PCM_WRITE_BITS, &arg);
  if (status == -1)
    fprintf(stderr, "SOUND_PCM_WRITE_BITS ioctl failed\n");
  if (arg != bits)
    fprintf(stderr, "Unable to set sample size\n");
  bits = arg;

  arg = channels;  /* mono or stereo */
  status = ioctl(device_no, SOUND_PCM_WRITE_CHANNELS, &arg);
  if (status == -1)
    fprintf(stderr, "SOUND_PCM_WRITE_CHANNELS ioctl failed\n");
  if (arg != channels)
    fprintf(stderr, "Unable to set number of channels\n");
  channels = arg;

  arg = freq;      /* sampling rate */
  status = ioctl(device_no, SOUND_PCM_WRITE_RATE, &arg);
  freq = arg; //set our value to the actual freq set
  if (status == -1)
    fprintf(stderr, "SOUND_PCM_WRITE_WRITE ioctl failed\n");

  fprintf(stderr, "Sound device: got: channels=%d, freq=%d, bits=%d\n", channels, freq, bits);

  /*
  arg = 8;
  status = ioctl(device_no, SOUND_PCM_SUBDIVIDE, &arg);
  if (status == -1)
    cout << "SOUND_PCM_SUBDIVIDE ioctl failed" << endl;
  cerr << "Subdivide = " << arg << endl;
  */
  status = ioctl(device_no, SNDCTL_DSP_GETBLKSIZE, &arg);
  fprintf(stderr, "Auctual fragment size = %d\n", arg);
  //if (status == -1)
      //cout << "SNDCTL_DSP_GETBLKSIZE ioctl failed" << endl;
  //cerr << "Block size = " << arg << endl;

}

int AudioStream::open(int mode_, int freq_, int channels_, int bits_, int buffer_size_)
{
  mode = mode_;
  freq = freq_;
  channels = channels_;
  bits = bits_;
  buffer_size = buffer_size_;

  //char *audio_device = gdata->settings.get_string("unix", "audio_device", "/dev/dsp");
  const char *audioInput = gdata->settings.getString("Sound", "soundInput");
  const char *audioOutput = gdata->settings.getString("Sound", "soundOutput");

  // We can open a device in read/write mode, but this isn't a preference - so, default to opening the input device.
  const char *audioInOut = gdata->settings.getString("Sound", "soundInput");

  // check sound device first by opening in non-blocking mode
  if(mode == F_READ) device_no = ::open(audioInput, O_RDONLY|O_NONBLOCK);
  else if(mode == F_WRITE) device_no = ::open(audioOutput, O_WRONLY|O_NONBLOCK);
  else if(mode == F_RDWR) device_no = ::open(audioInOut, O_RDWR|O_NONBLOCK);
  if(device_no == -1) {
    fprintf(stderr, "Opening of audio device failed.\n");
    return -1; //fail
  }
  ::close(device_no);

  // open sound device
  if(mode == F_READ) device_no = ::open(audioInput, O_RDONLY);
  else if(mode == F_WRITE) device_no = ::open(audioOutput, O_WRONLY);
  else if(mode == F_RDWR) device_no = ::open(audioInOut, O_RDWR);
  if(device_no == -1) {
    fprintf(stderr, "Opening of audio device failed.\n");
    return -1; //fail
  }

  set_params();

  pcm_vol = -1;
  if(mode == F_READ) {
    if(gdata->settings.getBool("Sound", "muteOutput")) { //mute the output sound while recording
      int mixer_no = ::open("/dev/mixer", O_RDONLY);
      if(mixer_no != -1) {
        //store the volume so we can restore it after
        if(ioctl(mixer_no, MIXER_READ(SOUND_MIXER_PCM), &pcm_vol) == -1) {
          fprintf(stderr, "failed to read volume: %s\n", strerror(errno));
          pcm_vol = -1;
        }
        int zero = 0;
        if(ioctl(mixer_no, MIXER_WRITE(SOUND_MIXER_PCM), &zero) == -1) {
          fprintf(stderr, "failed to mute output: %s\n", strerror(errno));
          pcm_vol = -1;
        }
        ::close(mixer_no);
      }
    }
  }


  /*
  if(mode == F_READ) { //start recording immediatly
      int arg = PCM_ENABLE_INPUT;
      ioctl(device_no, SNDCTL_DSP_SETTRIGGER, arg);
  }
  */
  return 0;
}

long AudioStream::read_bytes(void *data, long length)
{
  if(device_no == -1) return 0;

  //print_state();
  //printf("length=%d\n", length);

  return read(device_no, data, length);
}

long AudioStream::read_frames(void *data, long length)
{
  if(device_no == -1) return 0;

  return read(device_no, data, length * frame_size()) / frame_size();
}

long AudioStream::write_bytes(void *data, long length)
{
  if(device_no == -1) return 0;

  // wait for current playback to complete
  //if(ioctl(device_no, SOUND_PCM_SYNC, 0) == -1)
  //fprintf(stderr, "SOUND_PCM_SYNC ioctl failed");
  //print_state();
  //printf("lentgh=%d\n", (int)length);
  return write(device_no, data, length);
}

long AudioStream::write_frames(void *data, long length)
{
  if(device_no == -1) return 0;

  // wait for current playback to complete
  //if(ioctl(device_no, SOUND_PCM_SYNC, 0) == -1)
  //fprintf(stderr, "SOUND_PCM_SYNC ioctl failed");

  return write(device_no, data, length * frame_size()) / frame_size();
}

long AudioStream::wait_bytes(long length)
{
  if(device_no == -1) return 0;
  audio_buf_info info;

  do {
      ioctl(device_no, SNDCTL_DSP_GETISPACE, &info);
      fprintf(stderr, "full fragments left=%d, num fragments=%d, fragment size=%d, bytes left=%d\n",
	     info.fragments,
	     info.fragstotal,
	     info.fragsize,
	     info.bytes);
      sleep(1);
  } while(info.bytes < length);

  return 0;
}

long AudioStream::wait_frames(long length)
{
    return wait_bytes(length * frame_size());
}

QStringList AudioStream::getInputDeviceNames()
{
  QStringList toReturn;
  toReturn += gdata->settings.getString("Sound", "soundInput");
  return toReturn;
}

QStringList AudioStream::getOutputDeviceNames()
{
  QStringList toReturn;
  toReturn += gdata->settings.getString("Sound", "soundOutput");
  return toReturn;
}
