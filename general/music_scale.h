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

#include "array1d.h"
#include <vector>

class MusicScale
{
  public:

    enum MusicScale_t
    { Chromatic
    , Major
    , NaturalMinor
    , HarmonicMinor
    , MelodicMinor
    };

    inline MusicScale();
    ~MusicScale();

    void addScale( const char *p_name
                 , const int * p_notes
                 , int p_length
                 , int p_semitone_offset
                 );

    inline int size()const;
    inline int note(int j)const;
    inline bool hasSemitone(int p_j)const;
    inline const char * name()const;
    inline int semitoneOffset()const;

  private:

    Array1d<int> m_p_notes;
    std::vector<bool> m_p_semitone_lookup;
    char * m_p_name;
    int m_semitone_offset;

};

#include "music_scale.hpp"

extern std::vector<MusicScale> g_music_scales;

#endif //TARTINI_MUSIC_SCALE_H
// EOF