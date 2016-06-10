/***************************************************************************
                          audio_thread.cpp  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#include <stdio.h>
#include <math.h>
#include <qrect.h>
//Added by qt3to4:
#include <QCustomEvent>
#include <QMutex>

#include "audio_thread.h"
#include "audio_stream.h"
#include "sound_file_stream.h"
#include "freqpair.h"
#include "main.h"
#include "gdata.h"
#include "Filter.h"
#include "bspline.h"
#include "soundfile.h"
#include "channel.h"

//------------------------------------------------------------------------------
AudioThread::AudioThread(void)
{
  //printf("Warning - audio thread created with no sound file specified.\n");
  _playSoundFile = NULL;
  _recSoundFile = NULL;
  sleepCount = 0;
}

//------------------------------------------------------------------------------
void AudioThread::start(void)
{
  fprintf(stderr, "Warning - audio thread created with no sound file specified.\n");
  _playSoundFile = NULL;
  _recSoundFile = NULL;
  stopping = false;
  QThread::start(QThread::HighPriority);
}

//------------------------------------------------------------------------------
void AudioThread::start(SoundFile *sPlay, SoundFile *sRec)
{
  fprintf(stderr, "Audio thread created for\n");
  if(sPlay)
    {
      fprintf(stderr, "Playing file %s\n", sPlay->getFileName());
      fflush(stdout);
    }
  if(sRec)
    {
      fprintf(stderr, "Recording file %s\n", sRec->getFileName());
      fflush(stdout);
    }
  _playSoundFile = sPlay;
  _recSoundFile = sRec;
  stopping = false;
  QThread::start(QThread::HighPriority/*TimeCriticalPriority*/);
}

//------------------------------------------------------------------------------
void AudioThread::stop(void)
{
  stopping = true;
}

//------------------------------------------------------------------------------
void AudioThread::stopAndWait(void)
{
  stop();
  wait();
}

//------------------------------------------------------------------------------
void AudioThread::run(void)
{
#ifndef WINDOWS
  //setup stuff for multi-threaded profiling
  profiler_ovalue.it_interval.tv_sec = 0;
  profiler_ovalue.it_interval.tv_usec = 0;
  profiler_ovalue.it_value.tv_sec = 0;
  profiler_ovalue.it_value.tv_usec = 0;
  setitimer(ITIMER_PROF, &profiler_value, &profiler_ovalue); //for running multi-threaded profiling
#endif

  first = true;
  fast_update_count = 0;
  slow_update_count = 0;
  frame_num = 0;

  //read to the 1 chunk befor time 0
  if((gdata->getSoundMode() & SOUND_REC))
    {
      gdata->setDoingActiveAnalysis(true);
      myassert(_recSoundFile->isFirstTimeThrough() == true);
      _recSoundFile->recordChunk(_recSoundFile->offset());
    }
  
  QApplication::postEvent(mainWindow, new QCustomEvent(SOUND_STARTED));
  gdata->setRunning(STREAM_FORWARD);

  while(!stopping)
    {
      if(doStuff() == 0)
	{
	  break;
	}
#ifdef WINDOWS
      Sleep(0); //make other threads do some stuff
#else
      sleep(0); //make other threads do some stuff
#endif
    }

  gdata->setRunning(STREAM_STOP);

  if((gdata->getSoundMode() & SOUND_REC))
    {
      gdata->setDoingActiveAnalysis(false);
      _recSoundFile->setFirstTimeThrough(false);
      _recSoundFile->rec2play();
      gdata->setSoundMode(SOUND_PLAY);
    }

  if(gdata->getAudioStream())
    {
      delete gdata->getAudioStream();
      gdata->setAudioStream(NULL);
    }
  _playSoundFile = NULL;
  _recSoundFile = NULL;
  
  QApplication::postEvent(mainWindow, new QCustomEvent(SOUND_STOPPED));
}

//------------------------------------------------------------------------------
int AudioThread::doStuff(void)
{
  int force_update = false;
  if(gdata->getRunning() == STREAM_PAUSE)
    {
      msleep(20);
      //paused
      return 1;
    }
  if(!_playSoundFile && !_recSoundFile)
    {
      return 0;
    }

  ++frame_num;
	
  //update one frame before pausing again
  if(gdata->getRunning() == STREAM_UPDATE)
    {
      //update then pause
      gdata->setRunning(STREAM_PAUSE);
      force_update = true;
    }
  if(gdata->getRunning() != STREAM_FORWARD)
    {
      return 0;
    }
  
  //This is the main block of code for reading or write the next chunk to the soundcard or file
  if(gdata->getSoundMode() == SOUND_PLAY)
    {
      if(!_playSoundFile->playChunk())
	{
	  //end of file
	  QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
	  //stop the audio thread playing
	  return 0;
	}
    if(!gdata->getAudioStream())
      {
	if (++sleepCount % 4 == 0)
	  {
	    int sleepval = (1000 * _playSoundFile->framesPerChunk()) / _playSoundFile->rate();
	    msleep(sleepval * 4);
	  }
      }
    }
  else if(gdata->getSoundMode() == SOUND_REC)
    {
      // SOUND_REC
      int bufferChunks = gdata->getAudioStream()->inTotalBufferFrames() / _recSoundFile->framesPerChunk();
      if(frame_num > bufferChunks)
	{
	  _recSoundFile->recordChunk(_recSoundFile->framesPerChunk());
	}
    }
  else if(gdata->getSoundMode() == SOUND_PLAY_REC)
    {
      int bufferChunks = gdata->getAudioStream()->inTotalBufferFrames() / _recSoundFile->framesPerChunk();
      if(frame_num > bufferChunks)
	{
	  if(!SoundFile::playRecordChunk(_playSoundFile, _recSoundFile))
	    {
	      //end of file
	      QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
	      //stop the audio thread playing
	      return 0;
	    }
	}
    }

  //Set some flags to cause an update of views, every now and then
  fast_update_count++;
  slow_update_count++;

  int slowUpdateAfter = toInt(double(gdata->slowUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
  int fastUpdateAfter = toInt(double(gdata->fastUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
  if(!gdata->needUpdate())
    {
      if((slow_update_count >= slowUpdateAfter) || force_update)
	{
	  gdata->setNeedUpdate(true);
	  fast_update_count = 0;
	  slow_update_count = 0;
	  QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_SLOW));
	}
      else if(fast_update_count >= fastUpdateAfter)
	{
	  gdata->setNeedUpdate(true);
	  fast_update_count = 0;
	  QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_FAST));
	}
    }
  gdata->doChunkUpdate();

  return 1;
}

//EOF
