/***************************************************************************
                          useful.cpp  -  Generic useful functions
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
#include "musicnotes.h"
#include "myassert.h"
#include "music_scale.h"
#include "music_key.h"
#include <QObject>

std::string music_notes::m_note_names[12];

/**
   Just Intonation
   1:1   Root
   9:8   2nd
   6:5   Minor 3rd
   5:4   Major 3rd
   4:3   Forth
   25:18 F# - Geoff
   3:2   Fifth
   (8:5   Minor 6th?) (25:16 Minor 6th - Geoff)
   5:3   Major 6th
  
   15:8  Major 7th
   2:1   Octave
*/

/**
   Pythagorean Tuning
   Ref = Wikipedia
   D  1:1     Root
   Eb 256:242
   E  9:8
   F  32:27   Minor 3rd
   F# 81:64   Major 3rd
   G  4:3     4th
   G# 729:512
   A  3:2     5th
   Bb 128:81
   B  27:16
   C  16:9
   C# 243:128
*/

//------------------------------------------------------------------------------
void initMusicStuff()
{
    music_notes::init_note_names();
    MusicKey::init();
    MusicScale::init();
}

/**
   0  Root (1st)
   1
   2  (2nd)
   3  Minor 3rd
   4  Major 3rd (3rd)
   5  4th
   6
   7  5th
   8  Minor 6th
   9  Major 6th (6th)
   10 Minor 7th
   11 Major 7th (7th)
*/

//------------------------------------------------------------------------------
const std::string & music_notes::noteName(int p_pitch)
{
    return m_note_names[cycle(p_pitch, 12)];
}

//------------------------------------------------------------------------------
void
music_notes::init_note_names()
{
    m_note_names[0] = QObject::tr("C").toStdString();
    m_note_names[1] = QObject::tr("C♯").toStdString();
    m_note_names[2] = QObject::tr("D").toStdString();
    m_note_names[3] = QObject::tr("D♯").toStdString();
    m_note_names[4] = QObject::tr("E").toStdString();
    m_note_names[5] = QObject::tr("F").toStdString();
    m_note_names[6] = QObject::tr("F♯").toStdString();
    m_note_names[7] = QObject::tr("G").toStdString();
    m_note_names[8] = QObject::tr("G♯").toStdString();
    m_note_names[9] = QObject::tr("A").toStdString();
    m_note_names[10] = QObject::tr("A♯").toStdString();
    m_note_names[11] = QObject::tr("B").toStdString();
}

//------------------------------------------------------------------------------
int noteOctave(int p_pitch)
{
    return (p_pitch / 12) - 1;
}

//------------------------------------------------------------------------------
int noteValue(int p_pitch)
{
    return cycle(p_pitch, 12);
}

//------------------------------------------------------------------------------
bool isBlackNote(int p_pitch)
{
    switch(cycle(p_pitch, 12))
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
