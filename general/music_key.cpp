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

//------------------------------------------------------------------------------
MusicKey::MusicKey(const std::string & p_name
                 , TemparamentType p_temparament_type
                 , NoteOffsetType p_note_offset_type
                 , const std::vector<double> & p_note_offsets
                 , const std::vector<int> & p_note_types
                   )
: m_name(p_name)
, m_temparament_type(p_temparament_type)
, m_note_offsets(p_note_offsets)
, m_note_types(p_note_types)
{
    myassert(p_note_offsets.size() == p_note_types.size());
    
    switch (p_note_offset_type)
    {
        case NoteOffsetType::Midi:
            break;
            
        case NoteOffsetType::Cents:
            for (double & l_note_offset : m_note_offsets)
            {
                l_note_offset /= 100.0;
            }
            break;
            
        case NoteOffsetType::Ratio:
            const double l_k = freq2pitch(1.0);
            for (double & l_note_offset : m_note_offsets)
            {
                l_note_offset = freq2pitch(l_note_offset) - l_k;
            }
            break;
    }
}

//------------------------------------------------------------------------------
MusicKey::~MusicKey()
{
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

void MusicKey::init()
{
    const std::vector<double> l_even_tempered_scale =
        {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };

    const std::vector<int> l_twelve_note_type =
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    const std::vector<double> l_just_intonation_ratios =
        {1.0, 9.0 / 8, 6.0 / 5, 5.0 / 4, 4.0 / 3, 25.0 / 18, 3.0 / 2, 25.0 / 16, 5.0 / 3, 15.0 / 8 };

    const std::vector<int> l_just_intonation_type =
    {0, 2, 3, 4, 5, 6, 7, 8, 9, 11 };

    const std::vector<double> l_pythagorean_ratio =
        {1.0, 256.0 / 242, 9.0 / 8, 32.0 / 27, 81.0 / 64, 4.0 / 3, 729.0 / 512, 3.0 / 2, 128.0 / 81, 27.0 / 16, 16.0 / 9, 243.0 / 128 };

    const std::vector<double> l_meantone_temperament_scale =
        {0, 76, 193, 310, 386, 503, 579, 697, 773, 890, 1007, 1083 };

    g_music_keys.push_back(MusicKey("Even Tempered"
                                  , TemparamentType::Even
                                  , NoteOffsetType::Midi
                                  , l_even_tempered_scale
                                  , l_twelve_note_type
                                    ));

    g_music_keys.push_back(MusicKey("Just Intonation"
                                  , TemparamentType::Just
                                  , NoteOffsetType::Ratio
                                  , l_just_intonation_ratios
                                  , l_just_intonation_type
                                    ));

    g_music_keys.push_back(MusicKey("Pythagorean Tuning"
                                  , TemparamentType::Pythagorean
                                  , NoteOffsetType::Ratio
                                  , l_pythagorean_ratio
                                  , l_twelve_note_type
                                    ));

    g_music_keys.push_back(MusicKey("Meantone Temperament"
                                  , TemparamentType::Meantone
                                  , NoteOffsetType::Cents
                                  , l_meantone_temperament_scale
                                  , l_twelve_note_type
                                    ));
}

std::vector<MusicKey> MusicKey::g_music_keys;

const std::string g_music_key_name[NUM_MUSIC_KEYS] =
        {"A             ",
         "A♯/B♭",
         "B",
         "C",
         "C♯/D♭",
         "D",
         "D♯/E♭",
         "E",
         "F",
         "F♯/G♭",
         "G",
         "G♯/A♭"
        };
int g_music_key_root[NUM_MUSIC_KEYS] = { 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

// EOF
