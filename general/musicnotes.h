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
   @param freq The frequency in Hz
   @return The pitch in fractional part semitones from the midi scale.
*/
inline double freq2pitch(double freq);

/**
   Does the opposite of the function above
*/
inline double pitch2freq(double note);

const char* noteName(int pitch);
inline const char* noteName(double pitch);

/**
   @param note The midi note number
   @return The note octave. Middle C (midi note 60) is defined as octave 4. Making midi note 0 in octave -1
*/
int noteOctave(int pitch);
inline int noteOctave(double pitch);

/**
   @param pitch The midi note number
   @return The midi note within one octave. Range = 0 to 11. Where 0=C, 1=C# ... 11 = B.
*/
int noteValue(int pitch);
inline int noteValue(double pitch);

bool isBlackNote(int pitch);
inline bool isBlackNote(double pitch);

class MusicScale
{
 public:
  enum MusicScale_t { Chromatic, Major, NaturalMinor, HarmonicMinor, MelodicMinor };

  inline MusicScale(void);
  ~MusicScale(void);

  void addScale(const char *theName, const int *theNotes, int length, int semitoneOffset_);

  inline int size(void);
  inline int note(int j);
  inline bool hasSemitone(int j);
  inline const char * name(void);
  inline int semitoneOffset(void);

 private:
  Array1d<int> pNotes;
  std::vector<bool> pSemitoneLookup;
  char *pName;
  int _semitoneOffset;

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
     @param n the size of the array
     e.g. [0.0, 1.0, 2.0, 3.0, 4.0, ... 11.0] for equal tempered
  */
  void setScaleMidi(double *theNoteOffsets, int *types, int n);

  /**
     @param a the array of ratios
     @param n the size of the array
     e.g. [0, 100, 200, 300, 400, ... 1100] for equal tempered
  */
  void setScaleCents(double *theNoteOffsets, int *types, int n);

  /**
      @param a the array of ratios
      @param n the size of the array
      e.g. [1.0, 5.0/4, 4.0/3, 3.0/2]
  */
  void setScaleRatios(double *theNoteOffsets, int *types, int n);
  void setName(const char *theName);
  inline const char * name(void);
  inline int size(void) const;
  inline double noteOffset(int j) const;
  inline int noteType(int j) const;
  int nearestNoteIndex(double x);
  double nearestNote(double x);
  double nearestNoteDistance(double x);

 private:
  Array1d<double> noteOffsets; //ordered midi values of the notes in 1 octave
  Array1d<int> noteTypes;
  char *pName;

};

void initMusicStuff(void);

#define NUM_MUSIC_KEYS 12
extern std::vector<MusicKey> gMusicKeys;
extern std::vector<MusicScale> gMusicScales;

extern char *gMusicKeyName[NUM_MUSIC_KEYS];
extern int gMusicKeyRoot[NUM_MUSIC_KEYS];
extern int gMusicKey;

#include "musicnotes.hpp"

#endif // MUSICNOTES_H
//EOF
