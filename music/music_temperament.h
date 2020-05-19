/***************************************************************************
                          music_temperament.h
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
#ifndef TARTINI_MUSIC_TEMPERAMENT_H
#define TARTINI_MUSIC_TEMPERAMENT_H

#include <vector>
#include <array>

/**
   This defines the notes relative to the root for 1 octave of the scale
*/
class MusicTemperament
{
  public:

    enum class TemperamentType
    { Even
    , Just
    , Pythagorean
    , Meantone
    };
    
    enum class NoteOffsetType
    { Midi      // e.g. [0.0, 1.0, 2.0, 3.0, 4.0, ... 11.0] for equal tempered
    , Cents     // e.g. [0, 100, 200, 300, 400, ... 1100] for equal tempered
    , Ratio     // e.g. [1.0, 5.0/4, 4.0/3, 3.0/2]
    };

    MusicTemperament() = default;
    MusicTemperament(const std::string & p_name
           , TemperamentType p_temparament_type
           , NoteOffsetType p_note_offset_type
           , const std::vector<double> & p_note_offsets
           , const std::vector<int> & p_note_types
            );
    ~MusicTemperament();

    inline const std::string & name()const;
    inline TemperamentType temparament_type()const;
    inline bool isEvenTempered() const;
    inline int size() const;
    /**
       The pitch offset (0.0 - 12.0) of the temparament entry.  Includes an extra entry for the octave.
       @param p_j 0..size()
    */
    inline double noteOffset(int p_j) const;
    /**
       The nominal note (0 - 12) of the temparament entry.  Includes an extra entry for the octave.
       @param p_j 0..size()
    */
    inline int noteType(int p_j) const;
    /**
       @return The index of the  temparament entry that has the closest pitch offset to the given offset.  Returns size() if the octave is closest.
    */
    int nearestNoteIndex(const double & p_x)const;
    /**
       @return The pitch offset of the temparament entry that is closest to the given offset.  Returns 12.0 if the octave is closest.
    */
    double nearestNoteOffset(const double & p_x)const;
    double nearestNoteDistance(const double & p_x)const;
    /**
       @return The nominal note of the temparament entry that is closest to the given offset.  Returns 12 if the octave is closest.
    */
    int nearestNoteType(const double & p_x)const;
    
    static void init();
    static inline const std::array<MusicTemperament, 4> & getTemperaments();
    static const MusicTemperament & getTemperament(TemperamentType p_temparament_type);

  private:

    std::string m_name;
    TemperamentType m_temparament_type;
    int m_size;
    /**
     * ordered midi values of the notes in 1 octave
     */
    std::vector<double> m_note_offsets;
    std::vector<int> m_note_types;

    static std::array<MusicTemperament, 4> g_music_temperaments;
};

#include "music_temperament.hpp"

extern const std::array<std::string, 12> g_music_key_names;
extern const std::array<int, 12> g_music_key_roots;


#endif //TARTINI_MUSIC_TEMPERAMENT_H
// EOF
