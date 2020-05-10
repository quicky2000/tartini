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

#include "music_scale.h"

//------------------------------------------------------------------------------
MusicScale::~MusicScale()
{
}

//------------------------------------------------------------------------------
void MusicScale::addScale( const std::string & p_name
        , const int * p_notes
        , int p_length
        , int p_semitone_offset
                         )
{
    m_p_notes.resize_copy(p_notes, p_length);
    m_p_semitone_lookup.resize(12, false);
    for(int l_j = 0; l_j < p_length; l_j++)
    {
        myassert(p_notes[l_j] >= 0 && p_notes[l_j] < 12);
        m_p_semitone_lookup[p_notes[l_j]] = true;
    }
    m_name = p_name;
    m_semitone_offset = p_semitone_offset;
}

std::vector<MusicScale> g_music_scales;

// EOF
