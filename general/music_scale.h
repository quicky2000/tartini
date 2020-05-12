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

#ifndef TARTINI_MUSIC_SCALE_H
#define TARTINI_MUSIC_SCALE_H

#include "music_key.h"
#include <vector>

class MusicScale
{
  public:

    enum class ScaleType
    { Chromatic
    , Major
    , NaturalMinor
    , HarmonicMinor
    , MelodicMinor
    };

    MusicScale( const std::string & p_name
              , ScaleType p_scale_type
              , const std::vector<int> & p_notes
              , int p_semitone_offset = 0
              );
    ~MusicScale();

    inline ScaleType scaleType()const;
    inline int size()const;
    inline int note(int j)const;
    inline bool hasSemitone(int p_j)const;
    inline const std::string & name()const;
    inline int semitoneOffset()const;
    inline bool isChromaticScale()const;
    inline bool isMinorScale()const;
    inline bool isCompatibleWithTemparament(MusicKey::TemparamentType p_temparament_type)const;

    static void init();
    static inline const std::vector<MusicScale> & getScales();
    static const MusicScale & getScale(ScaleType p_scale_type);
    static bool isMajorScaleNote(int p_note);

private:

    std::string m_name;
    ScaleType m_scale_type;
    std::vector<int> m_notes;
    std::vector<bool> m_semitone_lookup;
    int m_semitone_offset;
    
    static std::vector<MusicScale> g_music_scales;
};

#include "music_scale.hpp"

#endif //TARTINI_MUSIC_SCALE_H
// EOF
