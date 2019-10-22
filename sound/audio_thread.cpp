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
AudioThread::AudioThread(void):
        m_play_sound_file(NULL),
        m_rec_sound_file(NULL),
        m_sleep_count(0)
{
  //printf("Warning - audio thread created with no sound file specified.\n");
}

//------------------------------------------------------------------------------
void AudioThread::start(void)
{
  fprintf(stderr, "Warning - audio thread created with no sound file specified.\n");
    m_play_sound_file = NULL;
    m_rec_sound_file = NULL;
  m_stopping = false;
  QThread::start(QThread::HighPriority);
}

//------------------------------------------------------------------------------
void AudioThread::start(SoundFile *p_s_play, SoundFile *p_s_rec)
{
  fprintf(stderr, "Audio thread created for\n");
  if(p_s_play)
    {
      fprintf(stderr, "Playing file %s\n", p_s_play->getFileName());
      fflush(stdout);
    }
  if(p_s_rec)
    {
      fprintf(stderr, "Recording file %s\n", p_s_rec->getFileName());
      fflush(stdout);
    }
    m_play_sound_file = p_s_play;
    m_rec_sound_file = p_s_rec;
  m_stopping = false;
  QThread::start(QThread::HighPriority/*TimeCriticalPriority*/);
}

//------------------------------------------------------------------------------
void AudioThread::stop(void)
{
  m_stopping = true;
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

  m_first = true;
    m_fast_update_count = 0;
    m_slow_update_count = 0;
    m_frame_num = 0;

  //read to the 1 chunk befor time 0
  if((gdata->getSoundMode() & SOUND_REC))
    {
      gdata->setDoingActiveAnalysis(true);
      myassert(m_rec_sound_file->isFirstTimeThrough() == true);
      m_rec_sound_file->recordChunk(m_rec_sound_file->offset());
    }
  
  QApplication::postEvent(g_main_window, new QCustomEvent(SOUND_STARTED));
  gdata->setRunning(STREAM_FORWARD);

  while(!m_stopping)
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
      m_rec_sound_file->setFirstTimeThrough(false);
      m_rec_sound_file->rec2play();
      gdata->setSoundMode(SOUND_PLAY);
    }

  if(gdata->getAudioStream())
    {
      delete gdata->getAudioStream();
      gdata->setAudioStream(NULL);
    }
    m_play_sound_file = NULL;
    m_rec_sound_file = NULL;
  
  QApplication::postEvent(g_main_window, new QCustomEvent(SOUND_STOPPED));
}

//------------------------------------------------------------------------------
int AudioThread::doStuff(void)
{
  int l_force_update = false;
  if(gdata->getRunning() == STREAM_PAUSE)
    {
      msleep(20);
      //paused
      return 1;
    }
  if(!m_play_sound_file && !m_rec_sound_file)
    {
      return 0;
    }

  ++m_frame_num;
	
  //update one frame before pausing again
  if(gdata->getRunning() == STREAM_UPDATE)
    {
      //update then pause
      gdata->setRunning(STREAM_PAUSE);
        l_force_update = true;
    }
  if(gdata->getRunning() != STREAM_FORWARD)
    {
      return 0;
    }
  
  //This is the main block of code for reading or write the next chunk to the soundcard or file
  if(gdata->getSoundMode() == SOUND_PLAY)
    {
      if(!m_play_sound_file->playChunk())
	{
	  //end of file
	  QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
	  //stop the audio thread playing
	  return 0;
	}
    if(!gdata->getAudioStream())
      {
	if (++m_sleep_count % 4 == 0)
	  {
	    int l_sleepval = (1000 * m_play_sound_file->framesPerChunk()) / m_play_sound_file->rate();
	    msleep(l_sleepval * 4);
	  }
      }
    }
  else if(gdata->getSoundMode() == SOUND_REC)
    {
      // SOUND_REC
      int l_buffer_chunks = gdata->getAudioStream()->inTotalBufferFrames() / m_rec_sound_file->framesPerChunk();
      if(m_frame_num > l_buffer_chunks)
	{
	  m_rec_sound_file->recordChunk(m_rec_sound_file->framesPerChunk());
	}
    }
  else if(gdata->getSoundMode() == SOUND_PLAY_REC)
    {
      int l_buffer_chunks = gdata->getAudioStream()->inTotalBufferFrames() / m_rec_sound_file->framesPerChunk();
      if(m_frame_num > l_buffer_chunks)
	{
	  if(!SoundFile::playRecordChunk(m_play_sound_file, m_rec_sound_file))
	    {
	      //end of file
	      QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
	      //stop the audio thread playing
	      return 0;
	    }
	}
    }

  //Set some flags to cause an update of views, every now and then
  m_fast_update_count++;
  m_slow_update_count++;

  int l_slow_update_after = toInt(double(gdata->slowUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
  int l_fast_update_after = toInt(double(gdata->fastUpdateSpeed()) / 1000.0 / curSoundFile()->timePerChunk());
  if(!gdata->needUpdate())
    {
      if((m_slow_update_count >= l_slow_update_after) || l_force_update)
	{
	  gdata->setNeedUpdate(true);
        m_fast_update_count = 0;
        m_slow_update_count = 0;
	  QApplication::postEvent(g_main_window, new QCustomEvent(UPDATE_SLOW));
	}
      else if(m_fast_update_count >= l_fast_update_after)
	{
	  gdata->setNeedUpdate(true);
        m_fast_update_count = 0;
	  QApplication::postEvent(g_main_window, new QCustomEvent(UPDATE_FAST));
	}
    }
  gdata->doChunkUpdate();

  return 1;
}

//EOF
