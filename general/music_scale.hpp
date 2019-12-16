/***************************************************************************
                          music_scale.h
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
//------------------------------------------------------------------------------
MusicScale::MusicScale()
: m_p_name(NULL)
, m_semitone_offset(0)
{
}

//------------------------------------------------------------------------------
int MusicScale::size()const
{
    return m_p_notes.size();
}

//------------------------------------------------------------------------------
int MusicScale::note(int j)const
{
#ifdef MYDEBUG
    return m_p_notes.at(j);
#else // MYDEBUG
    return m_p_notes[j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
bool MusicScale::hasSemitone(int p_j)const
{
#ifdef MYDEBUG
    return m_p_semitone_lookup.at(p_j);
#else // MYDEBUG
    return m_p_semitone_lookup[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
const char * MusicScale::name()const
{
    return m_p_name;
}

//------------------------------------------------------------------------------
int MusicScale::semitoneOffset()const
{
    return m_semitone_offset;
}

// EOF
