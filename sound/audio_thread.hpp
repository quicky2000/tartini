/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
AudioThread::~AudioThread()
{
}

//------------------------------------------------------------------------------
SoundFile * AudioThread::playSoundFile() const
{
    return m_play_sound_file;
}

//------------------------------------------------------------------------------
SoundFile * AudioThread::recSoundFile() const
{
    return m_rec_sound_file;
}

//------------------------------------------------------------------------------
SoundFile * AudioThread::curSoundFile() const
{
    return (m_rec_sound_file) ? m_rec_sound_file : m_play_sound_file;
}

//------------------------------------------------------------------------------
QEvent::Type AudioThread::get_update_fast_event_id()
{
    return m_update_fast_event_id;
}

//------------------------------------------------------------------------------
QEvent::Type AudioThread::get_update_slow_event_id()
{
    return m_update_slow_event_id;
}

//------------------------------------------------------------------------------
QEvent::Type AudioThread::get_sound_started_event_id()
{
    return m_sound_started_event_id;
}

//------------------------------------------------------------------------------
QEvent::Type AudioThread::get_sound_stopped_event_id()
{
    return m_sound_stopped_event_id;
}

//------------------------------------------------------------------------------
QEvent::Type AudioThread::get_settings_changed_event_id()
{
    return m_settings_changed_event_id;
}
//EOF
