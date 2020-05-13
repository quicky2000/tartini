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

#include "music_temperament.h"
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

    MusicScale() = default;
    MusicScale( const std::string & p_name
              , ScaleType p_scale_type
              , const std::vector<int> & p_notes
              );
    ~MusicScale();

    inline ScaleType scaleType()const;
    inline int size()const;
    inline int note(int j)const;
    inline bool hasSemitone(int p_j)const;
    inline const std::string & name()const;
    inline bool isChromaticScale()const;
    inline bool isMinorScale()const;
    inline bool isCompatibleWithTemparament(MusicTemperament::TemperamentType p_temparament_type)const;

    static void init();
    static inline const std::array<MusicScale,5> & getScales();
    static const MusicScale & getScale(ScaleType p_scale_type);
    static bool isMajorScaleNote(int p_note);

private:

    std::string m_name;
    ScaleType m_scale_type;
    std::vector<int> m_notes;
    std::vector<bool> m_semitone_lookup;
    
    static std::array<MusicScale,5> g_music_scales;
};

#include "music_scale.hpp"

#endif //TARTINI_MUSIC_SCALE_H
// EOF
