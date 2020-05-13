/***************************************************************************
                          music_temperament.cpp
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
#include "music_temperament.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
MusicTemperament::MusicTemperament(const std::string & p_name
                 , TemperamentType p_temparament_type
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
MusicTemperament::~MusicTemperament()
{
}

//------------------------------------------------------------------------------
int MusicTemperament::nearestNoteIndex(const double & p_x)const
{
    return static_cast<int>(binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x) - m_note_offsets.begin());
}

//------------------------------------------------------------------------------
double MusicTemperament::nearestNote(const double & p_x)const
{
    return *binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x);
}

//------------------------------------------------------------------------------
double MusicTemperament::nearestNoteDistance(const double & p_x)const
{
    return fabs(*binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x) - p_x);
}

//------------------------------------------------------------------------------
const MusicTemperament & MusicTemperament::getTemperament(TemperamentType p_temparament_type)
{
    return g_music_temperaments[static_cast<int>(p_temparament_type)];
}

/**
   Just Intonation
   1:1   Root
   9:8   2nd
   6:5   Minor 3rd
   5:4   Major 3rd
   4:3   Forth
   25:18 F# - Geoff
   3:2   Fifth
   (8:5   Minor 6th?) (25:16 Minor 6th - Geoff)
   5:3   Major 6th
  
   15:8  Major 7th
   2:1   Octave
*/

/**
   Pythagorean Tuning
   Ref = Wikipedia
   D  1:1     Root
   Eb 256:242
   E  9:8
   F  32:27   Minor 3rd
   F# 81:64   Major 3rd
   G  4:3     4th
   G# 729:512
   A  3:2     5th
   Bb 128:81
   B  27:16
   C  16:9
   C# 243:128
*/

void MusicTemperament::init()
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

    g_music_temperaments =
    { MusicTemperament("Even Tempered"
                     , TemperamentType::Even
                     , NoteOffsetType::Midi
                     , l_even_tempered_scale
                     , l_twelve_note_type
                       )
    , MusicTemperament("Just Intonation"
                     , TemperamentType::Just
                     , NoteOffsetType::Ratio
                     , l_just_intonation_ratios
                     , l_just_intonation_type
                       )
    , MusicTemperament("Pythagorean Tuning"
                     , TemperamentType::Pythagorean
                     , NoteOffsetType::Ratio
                     , l_pythagorean_ratio
                     , l_twelve_note_type
                       )
    , MusicTemperament("Meantone Temperament"
                     , TemperamentType::Meantone
                     , NoteOffsetType::Cents
                     , l_meantone_temperament_scale
                     , l_twelve_note_type
                       )
       };
}

std::vector<MusicTemperament> MusicTemperament::g_music_temperaments;

const std::array<std::string, 12> g_music_key_names =
        {{ "A             "
         , "A♯/B♭"
         , "B"
         , "C"
         , "C♯/D♭"
         , "D"
         , "D♯/E♭"
         , "E"
         , "F"
         , "F♯/G♭"
         , "G"
         , "G♯/A♭"
         }
        };
const std::array<int, 12> g_music_key_roots = {{ 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8 }};

// EOF
