/***************************************************************************
                          useful.h  -  Generic useful functions
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

#ifndef TARTINI_MUSIC_KEY_HPP
#define TARTINI_MUSIC_KEY_HPP

//------------------------------------------------------------------------------
const std::string & MusicKey::name()const
{
    return m_name;
}

//------------------------------------------------------------------------------
MusicKey::TemparamentType MusicKey::temparament_type()const
{
    return m_temparament_type;
}

//------------------------------------------------------------------------------
int MusicKey::size() const
{
    return m_note_offsets.size();
}

//------------------------------------------------------------------------------
double MusicKey::noteOffset(int p_j) const
{
#ifdef MYDEBUG
    return m_note_offsets.at(p_j);
#else // MYDEBUG
    return m_note_offsets[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
int MusicKey::noteType(int p_j) const
{
#ifdef MYDEBUG
    return m_note_types.at(p_j);
#else // MYDEBUG
    return m_note_types[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
const std::vector<MusicKey> & MusicKey::getKeys()
{
    return g_music_keys;
}

#endif //TARTINI_MUSIC_KEY_HPP
// EOF
