/***************************************************************************
                          audio_thread.h  -  description
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#ifndef AUDIO_THREAD_H
#define AUDIO_THREAD_H

#include <qthread.h>

#include <time.h>
//#include "fbuffer.h"

#ifdef _OS_WIN32_
#include <windows.h>
#endif

#define UPDATE_FAST      QEvent::User + 1
#define UPDATE_SLOW      QEvent::User + 2
#define SOUND_STARTED    QEvent::User + 3
#define SOUND_STOPPED    QEvent::User + 4
#define SETTINGS_CHANGED QEvent::User + 5

class GData;
class SoundFile;

class AudioThread : public QThread
{

 public:
  AudioThread(void);
  //AudioThread(SoundFile *s);
  virtual ~AudioThread(void)
    {
    }

  virtual void run(void);
  void start(void);
  void start(SoundFile *sPlay, SoundFile *sRec);

  /**
     Causes the audio thread to stop at the end of the next loop
  */
  void stop(void);

  /**
     Stop the audio thread and waits for it to finish
  */
  void stopAndWait(void);
      
  int doStuff(void);
  SoundFile * playSoundFile(void) const
    {
      return _playSoundFile;
    }
  SoundFile * recSoundFile(void) const
    {
      return _recSoundFile;
    }
  //SoundFile *curSoundFile() { return (_playSoundFile) ? _playSoundFile : _recSoundFile; }
  SoundFile * curSoundFile(void) const
    {
      return (_recSoundFile) ? _recSoundFile : _playSoundFile;
    }
  
 private:
  SoundFile *_playSoundFile;
  SoundFile *_recSoundFile;
  
  bool stopping;
  bool first;
  int fast_update_count;
  int slow_update_count;
  int frame_num;

  bool useFile;
  //FILE *freqFile;

  int sleepCount;
};

#endif // AUDIO_THREAD_H
// EOF
