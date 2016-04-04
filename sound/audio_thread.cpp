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

#include "audio_thread.h"
#include "audio_stream.h"
#include "sound_file_stream.h"
#include "freqpair.h"
#include "main.h"
#include "gdata.h"
#include "filter.h"
#include "bspline.h"
//#include "phils_templates.h"
//#include "freqwidget.h"
//#include "fundamental.h"
#include "soundfile.h"
#include "channel.h"

AudioThread::AudioThread() {
  //printf("Warning - audio thread created with no sound file specified.\n");
  _soundFile = NULL;
  sleepCount = 0;
}

void AudioThread::start() {
  fprintf(stderr, "Warning - audio thread created with no sound file specified.\n");
  _soundFile = NULL;
  stopping = false;
  QThread::start(QThread::HighPriority/*TimeCriticalPriority*/);
}

void AudioThread::start(SoundFile *s) {
  fprintf(stderr, "Audio thread created for sound file %s\n", s->filename); fflush(stdout);
  _soundFile = s;
  stopping = false;
  QThread::start(QThread::HighPriority/*TimeCriticalPriority*/);
}

/** Causes the audio thread to stop at the end of the next loop
*/
void AudioThread::stop()
{
  stopping = true;
}

/** Stop the audio thread and waits for it to finish
*/
void AudioThread::stopAndWait()
{
  stop();
  wait();
}

void AudioThread::run()
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
  //if(((MainWindow*) qApp->mainWidget())->freqView) ((MainWindow*) qApp->mainWidget())->freqView->offset_x = 0;
  //if(((MainWindow*) qApp->mainWidget())->zoomFreqView) ((MainWindow*) qApp->mainWidget())->zoomFreqView->offset_x = 0;

  //read to the 1 chunk befor time 0
  if(gdata->soundMode == SOUND_REC) {
    gdata->setDoingActiveAnalysis(true);
    //_soundFile->initRecordingChunk();
    _soundFile->recordChunk(_soundFile->offset());
  }
  
  QApplication::postEvent(mainWindow, new QCustomEvent(SOUND_STARTED));
  gdata->running = STREAM_FORWARD;

  while(!stopping) {
    if(doStuff() == 0) break;
//#ifdef _OS_WIN32_
#ifdef WINDOWS
    Sleep(0); //make other threads do some stuff
#else
    sleep(0); //make other threads do some stuff
    //usleep(1);
    //qApp->wakeUpGuiThread();
    //qApp->processEvents();
#endif
  }

  gdata->running = STREAM_STOP;

  if(gdata->soundMode == SOUND_REC) {
    gdata->setDoingActiveAnalysis(false);
    _soundFile->rec2play();
    gdata->soundMode = SOUND_PLAY;
  }

  if(gdata->audio_stream) {
    delete gdata->audio_stream;
    gdata->audio_stream = NULL;
  }
  _soundFile = NULL;
  
  QApplication::postEvent(mainWindow, new QCustomEvent(SOUND_STOPPED));
}

int AudioThread::doStuff()
{
  int force_update = false;
  if(gdata->running == STREAM_PAUSE) { msleep(20); return 1; } //paused
  if(!_soundFile) return 0;

  ++frame_num;

  //printf("audio_thread got lock\n"); fflush(stdout);
	
  //update one frame before pausing again
  if(gdata->running == STREAM_UPDATE) {
    gdata->running = STREAM_PAUSE; //update then pause
    force_update = true;
  }
  if(gdata->running != STREAM_FORWARD) return 0;


  
  //This is the main block of code for reading or write the next chunk to the soundcard or file
  if(gdata->soundMode == SOUND_PLAY) {
    if(!_soundFile->playChunk()) { //end of file
      QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
      return 0; //stop the audio thread playing
    }
    if(!gdata->audio_stream) {
      if (++sleepCount % 4 == 0) {
        int sleepval = (1000 * _soundFile->framesPerChunk()) / _soundFile->rate();
        msleep(sleepval * 4);
      }
    }
  } else {  // SOUND_REC
    //_soundFile->recordChunk();
    _soundFile->recordChunk(_soundFile->framesPerChunk());
	//msleep(1);
  }
  
/*
  if(gdata->soundMode == SOUND_PLAY) {
    //_soundFile->playChunk();
    //if(_soundFile->readChunk() < _soundFile->framesPerChunk()) { _soundFile->reset(); return 0; }
    if(_soundFile->readChunk() < _soundFile->framesPerChunk()) { //if reached the end of the file
      //_soundFile->incrementChunkNum();
      QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW)); //for qt3.x
      return 0; //stop the audio thread playing
    }
    if (gdata->audio_stream) _soundFile->writeChunk(gdata->audio_stream);
    else {
      if (++sleepCount % 4 == 0) {
        int sleepval = (1000 * _soundFile->framesPerChunk()) / _soundFile->rate();
        //printf("Sleeping for %d mseconds\n", sleepval);
        msleep(sleepval * 4);
      }
    }
    Channel *active = gdata->getActiveChannel();
    if(active && gdata->doingActive()) {
      active->processChunk(active->currentChunk());
    }
  } else { // SOUND_REC
    if(_soundFile->readChunk(gdata->audio_stream) < _soundFile->framesPerChunk()) {
      printf("Missed reading some audio data\n");
    }
    _soundFile->writeChunk();
    _soundFile->processNewChunk();
  }
  //_soundFile->incrementChunkNum();
*/

  /*
    if(gdata->sync_flag == 1) {
    gdata->audio_stream->signal1();
    gdata->sync_flag = 0;
    }
  */



  //Set some flags to cause an update of views, every now and then
  fast_update_count++;
  slow_update_count++;
  //int slowUpdateAfter = (_soundFile->bufferSize() / _soundFile->framesPerChunk()) * 4;
  //int fastUpdateAfter = (_soundFile->bufferSize() / _soundFile->framesPerChunk()) / 2;
  int slowUpdateAfter = toInt(double(gdata->slowUpdateSpeed()) / 1000.0 / _soundFile->timePerChunk());
  int fastUpdateAfter = toInt(double(gdata->fastUpdateSpeed()) / 1000.0 / _soundFile->timePerChunk());
  if(!gdata->need_update) {
    if((slow_update_count >= slowUpdateAfter) || force_update) {
      gdata->need_update = true;
      fast_update_count = 0;
      slow_update_count = 0;
      //QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_SLOW));
      QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_SLOW));
    } else if(fast_update_count >= fastUpdateAfter) {
      gdata->need_update = true;
      fast_update_count = 0;
      //QApplication::postEvent( ((MainWindow*)qApp->mainWidget()), new QCustomEvent(UPDATE_FAST));
      QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_FAST));
    }
  }

  return 1;
}
