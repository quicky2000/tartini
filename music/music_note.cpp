/***************************************************************************
                          music_note.cpp
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
#include "music_note.h"
#include "myassert.h"
#include "music_scale.h"
#include "music_temperament.h"
#include <QObject>

std::array<std::string, 12> MusicNote::m_semitone_names;

//------------------------------------------------------------------------------
void MusicNote::initMusicStuff()
{
    init();
    MusicTemperament::init();
    MusicScale::init();
}

//------------------------------------------------------------------------------
std::string MusicNote::noteName(int p_note)
{
    return semitoneName(semitoneValue(p_note)) + octaveName(noteOctave(p_note));
}

//------------------------------------------------------------------------------
const std::string & MusicNote::semitoneName(int p_semitone)
{
    myassert(p_semitone >= 0 && p_semitone < 12);
    
    return m_semitone_names[semitoneValue(p_semitone)];
}

//------------------------------------------------------------------------------
void
MusicNote::init()
{
    m_semitone_names =
    {{ QObject::tr("C").toStdString()
     , QObject::tr("C♯").toStdString()
     , QObject::tr("D").toStdString()
     , QObject::tr("D♯").toStdString()
     , QObject::tr("E").toStdString()
     , QObject::tr("F").toStdString()
     , QObject::tr("F♯").toStdString()
     , QObject::tr("G").toStdString()
     , QObject::tr("G♯").toStdString()
     , QObject::tr("A").toStdString()
     , QObject::tr("A♯").toStdString()
     , QObject::tr("B").toStdString()
    }};
}

//------------------------------------------------------------------------------
std::string MusicNote::octaveName(int p_octave)
{
    std::string l_string;

    for (char l_digit : std::to_string(p_octave))
    {
        switch(l_digit)
        {
            // Convert characters to corresponding Unicode subscript characters.
            case '0': l_string += "\u2080"; break;
            case '1': l_string += "\u2081"; break;
            case '2': l_string += "\u2082"; break;
            case '3': l_string += "\u2083"; break;
            case '4': l_string += "\u2084"; break;
            case '5': l_string += "\u2085"; break;
            case '6': l_string += "\u2086"; break;
            case '7': l_string += "\u2087"; break;
            case '8': l_string += "\u2088"; break;
            case '9': l_string += "\u2089"; break;
            case '+': l_string += "\u208A"; break;
            case '-': l_string += "\u208B"; break;
            default: myassert(false); break;
        }
    }
    
    return l_string;
}

//------------------------------------------------------------------------------
int MusicNote::noteOctave(int p_note)
{
    return (p_note / 12) - 1;
}

//------------------------------------------------------------------------------
int MusicNote::semitoneValue(int p_note)
{
    return cycle(p_note, 12);
}

//------------------------------------------------------------------------------
int MusicNote::semitoneValueInKey(int p_note, int p_music_key)
{
    return semitoneValue(p_note - p_music_key);
}

//------------------------------------------------------------------------------
double MusicNote::pitchOffsetInKey(const double & p_pitch, int p_music_key)
{
    return cycle(p_pitch - (double)p_music_key, 12.0);
}

//------------------------------------------------------------------------------
double MusicNote::temperedPitch(int p_note
                                , int p_music_key
                                , const MusicTemperament & p_music_temperament
                                  )
{
    if (p_music_temperament.isEvenTempered())
    {
        return (double)p_note;
    }
    
    int l_semitone = semitoneValueInKey(p_note, p_music_key);
    int l_temperament_index = -1;
    
    for (int l_i = 0; l_i < p_music_temperament.size(); l_i++)
    {
        if (p_music_temperament.noteType(l_i) == l_semitone)
        {
            l_temperament_index = l_i;
            break;
        }
    }
    
    if (l_temperament_index == -1)
    {
        // The temperament does not contain the note.
        return 0;
    }
    
    double l_temperament_pitch = p_music_temperament.noteOffset(l_temperament_index);
    double l_tempered_pitch = p_note + (l_temperament_pitch - l_semitone);

#ifdef DEBUG_PRINTF
    std::cout << ">>> temperedPitch() <<<" << std::endl;
    std::cout << "  nominal pitch = " << p_note << " (" << noteName(p_note) << ")" << std::endl;
    std::cout << "  music key = " << p_music_key << " (" << noteName(p_music_key) << ")" << std::endl;
    std::cout << "  music temperament = " << p_music_temperament.name() << std::endl;
    std::cout << "  semitone = " << l_semitone << std::endl;
    std::cout << "  tempered offset = " << l_temperament_pitch << std::endl;
    std::cout << "  tempered pitch = " << l_tempered_pitch << std::endl;
#endif // DEBUG_PRINTF
    
    // Return the even tempered pitch adjusted by the difference in the tempered pitch and the nominal note).
    return l_tempered_pitch;
}

//------------------------------------------------------------------------------
int MusicNote::closestNote(const double & p_pitch
                           , int p_music_key
                           , const MusicTemperament & p_music_temperament
                             )
{
    if (p_music_temperament.isEvenTempered())
    {
        return toInt(p_pitch);
    }
    
    double l_offset_in_key = pitchOffsetInKey(p_pitch, p_music_key);
    int l_closest_offset = p_music_temperament.nearestNoteType(l_offset_in_key);
    int l_root_of_key = toInt(p_pitch - l_offset_in_key);
    double l_closest_note = l_root_of_key + l_closest_offset;

#ifdef DEBUG_PRINTF
    std::cout << ">>> closestNote() <<<" << std::endl;
    std::cout << "  input pitch = " << p_pitch << std::endl;
    std::cout << "  music key = " << p_music_key << " (" << noteName(p_music_key) << ")" << std::endl;
    std::cout << "  music temperament = " << p_music_temperament.name() << std::endl;
    std::cout << "  offset in key = " << l_offset_in_key << std::endl;
    std::cout << "  closest key offset = " << l_closest_offset << std::endl;
    std::cout << "  root of key = " << l_root_of_key << std::endl;
    std::cout << "  closest note = " << l_closest_note << " (" << noteName(l_closest_note) << ")" << std::endl;
#endif // DEBUG_PRINTF

    return l_closest_note;
}

//------------------------------------------------------------------------------
bool MusicNote::isBlackNote(int p_note)
{
    switch(semitoneValue(p_note))
    {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            return true;
        default:
            return false;
    }
}

//EOF
