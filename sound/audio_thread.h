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

    inline static
    QEvent::Type get_update_fast_event_id();

    inline static
    QEvent::Type get_update_slow_event_id();

    inline static
    QEvent::Type get_sound_started_event_id();

    inline static
    QEvent::Type get_sound_stopped_event_id();

    inline static
    QEvent::Type get_settings_changed_event_id();

  private:
    SoundFile * m_play_sound_file;
    SoundFile * m_rec_sound_file;
  
    bool m_stopping;
    int m_fast_update_count;
    int m_slow_update_count;
    int m_frame_num;

    int m_sleep_count;

    static const QEvent::Type m_update_fast_event_id = static_cast<QEvent::Type>(QEvent::User + 1);
    static const QEvent::Type m_update_slow_event_id = static_cast<QEvent::Type>(QEvent::User + 2);
    static const QEvent::Type m_sound_started_event_id = static_cast<QEvent::Type>(QEvent::User + 3);
    static const QEvent::Type m_sound_stopped_event_id = static_cast<QEvent::Type>(QEvent::User + 4);
    static const QEvent::Type m_settings_changed_event_id = static_cast<QEvent::Type>(QEvent::User + 5);

};

#include "audio_thread.hpp"

#endif // AUDIO_THREAD_H
// EOF
