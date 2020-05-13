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
#include "myassert.h"

//------------------------------------------------------------------------------
MusicScale::MusicScale( const std::string & p_name
                      , ScaleType p_scale_type
                      , const std::vector<int> & p_notes
                      )
: m_name(p_name)
, m_scale_type(p_scale_type)
, m_notes(p_notes)
, m_semitone_lookup(12, false)
{
    for(int l_note : m_notes)
    {
        myassert(l_note >= 0 && l_note < 12);
        m_semitone_lookup[l_note] = true;
    }
}

//------------------------------------------------------------------------------
MusicScale::~MusicScale()
{
}

//------------------------------------------------------------------------------
const MusicScale & MusicScale::getScale(ScaleType p_scale_type)
{
    return g_music_scales[static_cast<int>(p_scale_type)];
}

//------------------------------------------------------------------------------
bool MusicScale::isMajorScaleNote(int p_note)
{
    return getScale(ScaleType::Major).hasSemitone(p_note);
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
void MusicScale::init()
{
    const std::vector<int> l_all_note_scale = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    const std::vector<int> l_major_scale                    = {0, 2, 4, 5, 7, 9, 11 };
    const std::vector<int> l_natural_minor_scale            = {0, 2, 3, 5, 7, 8, 10 };
    const std::vector<int> l_harmonic_minor_scale           = {0, 2, 3, 5, 7, 8, 11 };
    const std::vector<int> l_ascending_melodic_minor_scale  = {0, 2, 3, 5, 7, 9, 11 };
    
    g_music_scales =
    { MusicScale("All Notes", ScaleType::Chromatic, l_all_note_scale)
    , MusicScale("Major", ScaleType::Major, l_major_scale)
    , MusicScale("Minor (Natural)", ScaleType::NaturalMinor, l_natural_minor_scale)
    , MusicScale("Minor (Harmonic)", ScaleType::HarmonicMinor, l_harmonic_minor_scale)
    , MusicScale("Minor (Ascending Melodic)", ScaleType::MelodicMinor, l_ascending_melodic_minor_scale)
    };
}

std::vector<MusicScale> MusicScale::g_music_scales;

// EOF
