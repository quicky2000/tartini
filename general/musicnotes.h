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

const char * noteName(int p_pitch);
inline const char * noteName(const double & p_pitch);

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

    inline MusicScale(void);
    ~MusicScale(void);

    void addScale( const char *p_name
                 , const int * p_notes
                 , int p_length
                 , int p_semitone_offset
                 );

    inline int size(void)const;
    inline int note(int j)const;
    inline bool hasSemitone(int p_j)const;
    inline const char * name(void)const;
    inline int semitoneOffset(void)const;

  private:

    Array1d<int> m_p_notes;
    std::vector<bool> m_p_semitone_lookup;
    char * m_p_name;
    int m_semitone_offset;

};

/**
   This defines the notes relative to the root for 1 octave of the scale
*/
class MusicKey
{
  public:
    inline MusicKey(void);
    ~MusicKey(void);

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
    inline const char * name(void)const;
    inline int size(void) const;
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

void initMusicStuff(void);

#define NUM_MUSIC_KEYS 12
extern std::vector<MusicKey> g_music_keys;
extern std::vector<MusicScale> g_music_scales;

extern char * g_music_key_name[NUM_MUSIC_KEYS];
extern int g_music_key_root[NUM_MUSIC_KEYS];
extern int g_music_key;

#include "musicnotes.hpp"

#endif // MUSICNOTES_H
//EOF
