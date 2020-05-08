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
#include "music_key.h"
#include "musicnotes.h"
#include "mystring.h"

//------------------------------------------------------------------------------
MusicKey::~MusicKey()
{
    if(m_name)
    {
        free(m_name);
    }
}

//------------------------------------------------------------------------------
void MusicKey::setScaleMidi( double * p_note_offsets
                           , int * p_types
                           , int p_n
                           )
{
    m_note_offsets.resize(p_n);
    m_note_types.resize(p_n);
    for(int l_j = 0; l_j < p_n; l_j++)
    {
        m_note_offsets[l_j] = p_note_offsets[l_j];
        m_note_types[l_j] = p_types[l_j];
    }
}

//------------------------------------------------------------------------------
void MusicKey::setScaleCents( double * p_note_offsets
                            , int *p_types
                            , int p_n
                            )
{
    m_note_offsets.resize(p_n);
    m_note_types.resize(p_n);
    for(int l_j = 0; l_j < p_n; l_j++)
    {
        m_note_offsets[l_j] = p_note_offsets[l_j] / 100.0;
        m_note_types[l_j] = p_types[l_j];
    }
}

//------------------------------------------------------------------------------
void MusicKey::setScaleRatios( double * p_note_offsets
                             , int * p_types
                             , int p_n
                             )
{
    double k = freq2pitch(1.0);
    m_note_offsets.resize(p_n);
    m_note_types.resize(p_n);
    for(int l_j = 0; l_j < p_n; l_j++)
    {
        m_note_offsets[l_j] = freq2pitch(p_note_offsets[l_j]) - k;
        m_note_types[l_j] = p_types[l_j];
    }
}

//------------------------------------------------------------------------------
void MusicKey::setName(const char * p_name)
{
    if(m_name)
    {
        free(m_name);
    }
    m_name = copy_string(p_name);
}

//------------------------------------------------------------------------------
int MusicKey::nearestNoteIndex(const double & p_x)const
{
    return static_cast<int>(binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x) - m_note_offsets.begin());
}

//------------------------------------------------------------------------------
double MusicKey::nearestNote(const double & p_x)const
{
    return *binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x);
}

//------------------------------------------------------------------------------
double MusicKey::nearestNoteDistance(const double & p_x)const
{
    return fabs(*binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x) - p_x);
}


std::vector<MusicKey> g_music_keys;

const char *g_music_key_name[NUM_MUSIC_KEYS] =
        {"A             ",
         "A#/Bb",
         "B",
         "C",
         "C#/Db",
         "D",
         "D#/Eb",
         "E",
         "F",
         "F#/Gb",
         "G",
         "G#/Ab"
        };
int g_music_key_root[NUM_MUSIC_KEYS] = {9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8      };
int g_music_key = 3; //C
//                     0    1     2    3     4    5    6     7    8     9    10    11

// EOF
