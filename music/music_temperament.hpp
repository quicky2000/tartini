/***************************************************************************
                          music_temperament.hpp
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

#ifndef TARTINI_MUSIC_TEMPERAMENT_HPP
#define TARTINI_MUSIC_TEMPERAMENT_HPP

//------------------------------------------------------------------------------
const std::string & MusicTemperament::name()const
{
    return m_name;
}

//------------------------------------------------------------------------------
MusicTemperament::TemperamentType MusicTemperament::temparament_type()const
{
    return m_temparament_type;
}

//------------------------------------------------------------------------------
bool MusicTemperament::isEvenTempered() const
{
    return m_temparament_type == TemperamentType::Even;
}

//------------------------------------------------------------------------------
int MusicTemperament::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------
double MusicTemperament::noteOffset(int p_j) const
{
#ifdef MYDEBUG
    return m_note_offsets.at(p_j);
#else // MYDEBUG
    return m_note_offsets[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
int MusicTemperament::noteType(int p_j) const
{
#ifdef MYDEBUG
    return m_note_types.at(p_j);
#else // MYDEBUG
    return m_note_types[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
const std::array<MusicTemperament, 4> & MusicTemperament::getTemperaments()
{
    return g_music_temperaments;
}

#endif //TARTINI_MUSIC_TEMPERAMENT_HPP
// EOF
