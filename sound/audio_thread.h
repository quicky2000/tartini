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

#include <QThread>

#include <time.h>

#ifdef _OS_WIN32_
#include <windows.h>
#endif

#define UPDATE_FAST      QEvent::User + 1
#define UPDATE_SLOW      QEvent::User + 2
#define SOUND_STARTED    QEvent::User + 3
#define SOUND_STOPPED    QEvent::User + 4
#define SETTINGS_CHANGED QEvent::User + 5
#define UPDATE_VERY_FAST QEvent::User + 6

class GData;
class SoundFile;

class AudioThread : public QThread
{

  public:
    AudioThread();
    inline virtual ~AudioThread();

    virtual void run();
    void start();
    void start(SoundFile *p_s_play, SoundFile *p_s_rec);

    /**
       Causes the audio thread to stop at the end of the next loop
    */
    void stop();

    /**
       Stop the audio thread and waits for it to finish
    */
    void stopAndWait();
      
    int doStuff();
    inline SoundFile * playSoundFile() const;
    inline SoundFile * recSoundFile() const;
    inline SoundFile * curSoundFile() const;
  
  private:
    SoundFile * m_play_sound_file;
    SoundFile * m_rec_sound_file;
  
    bool m_stopping;
    bool m_first;
    int m_very_fast_update_count;
    int m_fast_update_count;
    int m_slow_update_count;
    int m_frame_num;

    int m_sleep_count;
};

#include "audio_thread.hpp"

#endif // AUDIO_THREAD_H
// EOF
