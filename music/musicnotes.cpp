/***************************************************************************
                          musicnotes.cpp
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
#include "music_temperament.h"
#include <QObject>

std::array<std::string, 12> music_notes::m_note_names;

//------------------------------------------------------------------------------
void initMusicStuff()
{
    music_notes::init_note_names();
    MusicTemperament::init();
    MusicScale::init();
}

//------------------------------------------------------------------------------
const std::string & music_notes::noteName(int p_pitch)
{
    return m_note_names[cycle(p_pitch, 12)];
}

//------------------------------------------------------------------------------
void
music_notes::init_note_names()
{
    m_note_names =
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
