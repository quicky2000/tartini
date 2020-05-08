/***************************************************************************
                          useful.h  -  Generic useful functions
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
#ifndef TARTINI_MUSIC_KEY_H
#define TARTINI_MUSIC_KEY_H

#include "array1d.h"
#include <vector>

/**
   This defines the notes relative to the root for 1 octave of the scale
*/
class MusicKey
{
  public:
    inline MusicKey();
    ~MusicKey();

    /**
       @param a the array of ratios
       @param p_n the size of the array
       e.g. [0.0, 1.0, 2.0, 3.0, 4.0, ... 11.0] for equal tempered
    */
    void setScaleMidi( double * p_note_offsets
            , int * p_types
            , int p_n
                     );

    /**
       @param a the array of ratios
       @param p_n the size of the array
       e.g. [0, 100, 200, 300, 400, ... 1100] for equal tempered
    */
    void setScaleCents( double * p_note_offsets
            , int * p_types
            , int p_n
                      );

    /**
       @param a the array of ratios
       @param p_n the size of the array
       e.g. [1.0, 5.0/4, 4.0/3, 3.0/2]
    */
    void setScaleRatios( double * p_note_offsets
            , int * p_types
            , int p_n
                       );

    void setName(const char * p_name);
    inline const char * name()const;
    inline int size() const;
    inline double noteOffset(int p_j) const;
    inline int noteType(int p_j) const;
    int nearestNoteIndex(const double & p_x)const;
    double nearestNote(const double & p_x)const;
    double nearestNoteDistance(const double & p_x)const;

  private:

    /**
     * ordered midi values of the notes in 1 octave
     */
    Array1d<double> m_note_offsets;
    Array1d<int> m_note_types;
    char * m_name;

};

#include "music_key.hpp"

#define NUM_MUSIC_KEYS 12
extern std::vector<MusicKey> g_music_keys;

extern const char * g_music_key_name[NUM_MUSIC_KEYS];
extern int g_music_key_root[NUM_MUSIC_KEYS];
extern int g_music_key;


#endif //TARTINI_MUSIC_KEY_H
// EOF
