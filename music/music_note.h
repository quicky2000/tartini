/***************************************************************************
                          music_note.h
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
#include "myassert.h"
#include <string>
#include "gdata.h"
#include "music_temperament.h"

/**
 Helper class for musical notes -- all members are static.
 
 Terminology:
 - A "note" is an integer value that represents a musical note, such as C<SUB>4</SUB>.  The values are MIDI note numbers, e.g. C<SUB>4</SUB> = 60.
 - A "pitch" is a floating-point value that represents a musical pitch, also measured on the MIDI scale.
 
 The mapping between note numbers and pitches depends on the key and temperament.
 For example, the note G<SUB>4</SUB> (note number 66) in Pythagorean temperament (in the key of C) has a pitch of 66.0196, which is 1.96 cents higher than in even temperament.
 Only in even temperament do the note numbers correspond to the pitch values.
 - Use `temperedPitch()` to convert from a note number to the corresponding pitch.
 - Use `closestNote()` to convert from a pitch to the note number of the closest tempered pitch.
 
 You can pass the key and temperament explicitly or rely on the default values, which use the globally selected key and temperament in `GData`.
 
 - A "semitone" is the integer value of the position of the note within the octave (0..11).  It corresponds to the note names C, C♯, ..., B♭, B.
 - An "octave" is the octave number of the note in Scientific Pitch Notation. Middle C (note number 60) is defined as octave 4.  Note number 0 is in octave -1.
 */
class MusicNote
{
  public:

    /**
     The name of the note, including the octave number
     @param p_note The note number
     @return The name of the note: C<SUB>4</SUB>, B♭<SUB>5</SUB>, etc. Uses Unicode characters for sharps, flats and subscript digits
    */
    static std::string noteName(int p_note);

    /**
     The octave number of the note in Scientific Pitch Notation
     @param p_note The note number
     @return The octave number. Middle C (note number 60) is defined as octave 4.  Note number 0 is in octave -1
    */
    static int noteOctave(int p_note);

    /**
     The name of the octave, using Unicode subscript digits
     @param p_octave The octave number
     @return The name of the octave (e.g. <SUB>4</SUB>). Uses Unicode characters for subscript numbers
    */
    static std::string octaveName(int p_octave);

    /**
    The semitone value of the note within the octave
    @param p_note The note number
    @return The semitone value (0..11), which corresponds to the note names C, C♯, ..., B♭, B.
    */
    static int semitoneValue(int p_note);

    /**
     The name of the semitone
     @param p_semitone The semitone value (0..11)
     @return The name of the semitone: C, C♯, ..., B♭, B. Uses Unicode characters for sharps and flats.
    */
    static const std::string & semitoneName(int p_semitone);

    /**
     The semitone value of the note, transposed into the given musical key
     @param p_note The note number
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @return The semitone value (0..11), which corresponds to the note names C, C♯, ..., B♭, B.
    */
    static int semitoneValueInKey(int p_note
                                , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                                  );

    /**
     The pitch offset relative to the root of the given musical key
     @param p_pitch The pitch
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @return The pitch offset in the range 0.0 <= x < 12.0
    */
    static double pitchOffsetInKey(const double & p_pitch
                                 , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                                   );

    /**
     The tempered pitch of a note, given the key and temperament
     @param p_note The note number
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @param p_music_temperament The musical temperament.  If not specified, defaults to the globally selected temperament: `GData::musicTemperament()`
     @return The tempered pitch.  Returns 0 if the temperament does not include the note
    */
    static double temperedPitch(int p_note
                              , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                              , const MusicTemperament & p_music_temperament = MusicTemperament::getTemperament(GData::getUniqueInstance().musicTemperament())
                                );

    /**
     The closest note to a pitch, given the key and temperament
     @param p_pitch The pitch
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @param p_music_temperament The musical temperament.  If not specified, defaults to the globally selected temperament: `GData::musicTemperament()`
     @return The note associated with the closest pitch in the temperament
    */
    static int closestNote(const double & p_pitch
                         , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                         , const MusicTemperament & p_music_temperament = MusicTemperament::getTemperament(GData::getUniqueInstance().musicTemperament())
                           );

    /**
     Does the note correspond to a black key on a piano?
     @param p_note The note number
     @return True if the note is not in the C Major scale
    */
    static bool isBlackNote(int p_note);
    
    /**
     Converts the frequency (in hertz) to the corresponding pitch on the MIDI scale
     @param p_freq The frequency in Hz
     @return The pitch on the MIDI scale
    */
    static inline double freq2pitch(const double & p_freq);

    /**
     Does the opposite of `freq2pitch()`
    */
    static inline double pitch2freq(const double & p_pitch);

    static void initMusicStuff();
    
    // Deprecated member functions (do not remove)
    
    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline std::string noteName(const double & p_pitch);

    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline int noteOctave(const double & p_pitch);

    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline int semitoneValue(const double & p_pitch);

    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline bool isBlackNote(const double & p_pitch);

    private:
      static void init();

      static std::array<std::string, 12> m_semitone_names;
};

#include "music_note.hpp"

#endif // MUSICNOTES_H
//EOF
