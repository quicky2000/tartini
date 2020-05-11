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

#include "sound_stream.h"

//------------------------------------------------------------------------------
SoundStream::SoundStream()
: m_freq(0)
, m_channels(0)
, m_bits(0)
, m_mode(t_open_mode::F_NONE)
{
}

//------------------------------------------------------------------------------
SoundStream::~SoundStream()
{
}

//------------------------------------------------------------------------------
int SoundStream::sample_size() const
{
    return (m_bits + 7) / 8;
}

//------------------------------------------------------------------------------
int SoundStream::frame_size() const
{
    return sample_size() * m_channels;
}

//------------------------------------------------------------------------------
long SoundStream::wait_bytes(long /*length*/)
{
    return 0;
}

//------------------------------------------------------------------------------
long SoundStream::wait_frames(long /*length*/)
{
    return 0;
}

//------------------------------------------------------------------------------
void
SoundStream::set_frequency(int p_frequency)
{
    m_freq = p_frequency;
}

//------------------------------------------------------------------------------
int
SoundStream::get_frequency() const
{
    return m_freq;
}

//------------------------------------------------------------------------------
void
SoundStream::set_channels(int p_channels)
{
    m_channels = p_channels;
}

//------------------------------------------------------------------------------
int
SoundStream::get_channels() const
{
    return m_channels;
}

//------------------------------------------------------------------------------
void
SoundStream::set_bits(int p_bits)
{
    m_bits = p_bits;
}

//------------------------------------------------------------------------------
int
SoundStream::get_bits() const
{
    return m_bits;
}

//------------------------------------------------------------------------------
void
SoundStream::set_mode(t_open_mode p_mode)
{
    m_mode = p_mode;
}

//------------------------------------------------------------------------------
SoundStream::t_open_mode
SoundStream::get_mode() const
{
    return m_mode;
}

//------------------------------------------------------------------------------
bool SoundStream::is_write_mode() const
{
    return t_open_mode::F_WRITE == m_mode || t_open_mode::F_RDWR == m_mode;
}

//------------------------------------------------------------------------------
bool SoundStream::is_read_mode() const
{
    return t_open_mode::F_READ == m_mode || t_open_mode::F_RDWR == m_mode;
}
//EOF
