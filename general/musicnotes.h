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
#ifndef MUSICNOTES_H
#define MUSICNOTES_H

#include "useful.h"
#include "array1d.h"
#include "gdata.h"
#include <string>

class music_notes
{
  public:

    static
    const char * noteName(int p_pitch);

    static inline
    const char * noteName(const double & p_pitch);

    static
    void init_note_names();

  private:
    static std::string m_note_names[12];
};

/**
   Converts the frequencies freq (in hertz) into their note number on the midi scale
   i.e. the number of semi-tones above C0
   Note: The note's pitch will contain its fractional part
   Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
   @param p_freq The frequency in Hz
   @return The pitch in fractional part semitones from the midi scale.
*/
inline double freq2pitch(const double & p_freq);

/**
   Does the opposite of the function above
*/
inline double pitch2freq(const double & p_note);

/**
   @param note The midi note number
   @return The note octave. Middle C (midi note 60) is defined as octave 4. Making midi note 0 in octave -1
*/
int noteOctave(int p_pitch);
inline int noteOctave(const double & p_pitch);

/**
   @param p_pitch The midi note number
   @return The midi note within one octave. Range = 0 to 11. Where 0=C, 1=C# ... 11 = B.
*/
int noteValue(int p_pitch);
inline int noteValue(const double & p_pitch);

bool isBlackNote(int p_pitch);
inline bool isBlackNote(const double & p_pitch);


void initMusicStuff();

#include "musicnotes.hpp"

#endif // MUSICNOTES_H
//EOF
