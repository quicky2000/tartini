/***************************************************************************
                          useful.cpp  -  Generic useful functions
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
#include "musicnotes.h"
#include "mystring.h"
#include "myassert.h"
#include "music_scale.h"
#include <QObject>

std::vector<MusicKey> g_music_keys;

char *g_music_key_name[NUM_MUSIC_KEYS] =
  {(char*)"A             ",
   (char*)"A#/Bb",
   (char*)"B",
   (char*)"C",
   (char*)"C#/Db",
   (char*)"D",
   (char*)"D#/Eb",
   (char*)"E",
   (char*)"F",
   (char*)"F#/Gb",
   (char*)"G",
   (char*)"G#/Ab"
  };
int g_music_key_root[NUM_MUSIC_KEYS] = {9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8      };
int g_music_key = 3; //C
//                     0    1     2    3     4    5    6     7    8     9    10    11
const QString g_note_names[12] =
  {
    QObject::tr("C"),
    QObject::tr("C#"),
    QObject::tr("D"),
    QObject::tr("D#"),
    QObject::tr("E"),
    QObject::tr("F"),
    QObject::tr("F#"),
    QObject::tr("G"),
    QObject::tr("G#"),
    QObject::tr("A"),
    QObject::tr("A#"),
    QObject::tr("B")
  };

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

//------------------------------------------------------------------------------
void initMusicStuff()
{
    double l_even_tempered_scale[12]  = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
    /**
       Note types: 0x01=Major Scale Note, 0x02=Minor Scale Note, 0x04=Root Note
    */
    int l_twelve_note_type[12]      = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    double l_just_intonation_ratios[10] = {1.0, 9.0 / 8, 6.0 / 5, 5.0 / 4, 4.0 / 3, 25.0 / 18, 3.0 / 2, 25.0 / 16, 5.0 / 3, 15.0 / 8 };
    int l_just_intonationT_type[10]   = {0, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
    double l_pythagorean_ratio[12]    = {1.0, 256.0 / 242, 9.0 / 8, 32.0 / 27, 81.0 / 64, 4.0 / 3, 729.0 / 512, 3.0 / 2, 128.0 / 81, 27.0 / 16, 16.0 / 9, 243.0 / 128 };
    double l_meantone_temperament_scale[12]  = {0, 76, 193, 310, 386, 503, 579, 697, 773, 890, 1007, 1083 };
    g_music_keys.resize(4);
    g_music_keys[0].setName("Even Tempered");
    g_music_keys[0].setScaleMidi(l_even_tempered_scale, l_twelve_note_type, 12);
    g_music_keys[1].setName("Just Intonation");
    g_music_keys[1].setScaleRatios(l_just_intonation_ratios, l_just_intonationT_type, 10);
    g_music_keys[2].setName("Pythagorean Tuning");
    g_music_keys[2].setScaleRatios(l_pythagorean_ratio, l_twelve_note_type, 12);
    g_music_keys[3].setName("Meantone Temperament");
    g_music_keys[3].setScaleCents(l_meantone_temperament_scale, l_twelve_note_type, 12);

    int l_all_note_scale[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    int l_major_scale[7]                   = {0, 2, 4, 5, 7, 9, 11 };
    int l_natural_minor_scale[7]           = {0, 2, 3, 5, 7, 8, 10 };
    int l_harmonic_minor_scale[7]          = {0, 2, 3, 5, 7, 8, 11 };
    int l_ascending_melodic_minor_scale[7] = {0, 2, 3, 5, 7, 9, 11 };
    g_music_scales.resize(5);
    g_music_scales[MusicScale::Chromatic].addScale("All Notes", l_all_note_scale, 12, 0);
    g_music_scales[MusicScale::Major].addScale("Major", l_major_scale, 7, 0);
    g_music_scales[MusicScale::NaturalMinor].addScale("Minor (Natural)", l_natural_minor_scale, 7, 0);
    g_music_scales[MusicScale::HarmonicMinor].addScale("Minor (Harmonic)", l_harmonic_minor_scale, 7, 0);
    g_music_scales[MusicScale::MelodicMinor].addScale("Minor (Ascending Melodic)", l_ascending_melodic_minor_scale, 7, 0);
}

/**
   0  Root (1st)
   1
   2  (2nd)
   3  Minor 3rd
   4  Major 3rd (3rd)
   5  4th
   6
   7  5th
   8  Minor 6th
   9  Major 6th (6th)
   10 Minor 7th
   11 Major 7th (7th)
*/

//------------------------------------------------------------------------------
const char * noteName(int p_pitch)
{
    return g_note_names[cycle(p_pitch, 12)].toStdString().c_str();
}

//------------------------------------------------------------------------------
int noteOctave(int p_pitch)
{
    return (p_pitch / 12) - 1;
}

//------------------------------------------------------------------------------
int noteValue(int p_pitch)
{
    return cycle(p_pitch, 12);
}

//------------------------------------------------------------------------------
bool isBlackNote(int p_pitch)
{
    switch(cycle(p_pitch, 12))
    {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            return true;
        default:
            return false;
    }
}

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
    return (int)(binary_search_closest(m_note_offsets.begin(), m_note_offsets.end(), p_x) - m_note_offsets.begin());
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

//EOF
