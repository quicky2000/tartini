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
MusicScale::ScaleType MusicScale::scaleType()const
{
    return m_scale_type;
}

//------------------------------------------------------------------------------
int MusicScale::size()const
{
    return m_notes.size();
}

//------------------------------------------------------------------------------
int MusicScale::note(int j)const
{
#ifdef MYDEBUG
    return m_notes.at(j);
#else // MYDEBUG
    return m_notes[j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
bool MusicScale::hasSemitone(int p_j)const
{
#ifdef MYDEBUG
    return m_semitone_lookup.at(p_j);
#else // MYDEBUG
    return m_semitone_lookup[p_j];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
const std::string & MusicScale::name()const
{
    return m_name;
}

//------------------------------------------------------------------------------
int MusicScale::semitoneOffset()const
{
    return m_semitone_offset;
}

//------------------------------------------------------------------------------
bool MusicScale::isChromaticScale()const
{
    return m_scale_type == ScaleType::Chromatic;
}

//------------------------------------------------------------------------------
bool MusicScale::isMinorScale()const
{
    bool l_result;
    switch(m_scale_type)
    {
        case ScaleType::Chromatic:
        case ScaleType::Major:
            l_result = false;
            break;
        case ScaleType::NaturalMinor:
        case ScaleType::HarmonicMinor:
        case ScaleType::MelodicMinor:
            l_result = true;
            break;
    }
    return l_result;
}

//------------------------------------------------------------------------------
bool MusicScale::isCompatibleWithTemparament(MusicTemperament::TemperamentType p_temparament_type)const
{
    return (p_temparament_type == MusicTemperament::TemperamentType::Even || !isMinorScale());
}

//------------------------------------------------------------------------------
const std::vector<MusicScale> & MusicScale::getScales()
{
    return g_music_scales;
}

// EOF
