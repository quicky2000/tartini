/***************************************************************************
                          paino.cpp  -  description
                             -------------------
    begin                : 17 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "pianowidget.h"
#include <QPainter>
#include <QPaintEvent>
#include "drawwidget.h"
#include "gdata.h"
#include "myqt.h"

static int g_is_black_key_array[12] =
  {
    1,
    -1,
    2,
    -2,
    3,
    4,
    -3,
    5,
    -4,
    6,
    -5,
    7
  };

//------------------------------------------------------------------------------
PianoWidget::PianoWidget(QWidget *p_parent)
: DrawWidget(p_parent)
{
    setNoNote();
}

//------------------------------------------------------------------------------
PianoWidget::~PianoWidget(void)
{
}

//------------------------------------------------------------------------------
void PianoWidget::setCurrentNote( int p_n
                                , float p_amount_pressed
                                )
{
    if(p_n >= 0 && p_n < 12)
    {
        m_current_note = p_n;
    }
    else
    {
        setNoNote();
    }
    m_amount_pressed = p_amount_pressed;
}

//------------------------------------------------------------------------------
void PianoWidget::paintEvent(QPaintEvent *)
{
    int l_j;
    beginDrawing(false);
    fillBackground(Qt::white);
  
    //1650mm
    double l_scale_X = double(width()) / 1650.0;
    //1480mm
    double l_scale_Y = double(height()) / 1480.0;

    if(isNote())
    {
        int l_note_part = g_is_black_key_array[currentNote()];
        if(l_note_part >= 0)
        {
            //it's a white key
            get_painter().fillRect(toInt((l_note_part - 1) * 235.0 * l_scale_X), 0, toInt(234.0 * l_scale_X) + 1, height(), colorBetween(Qt::white, Qt::red, m_amount_pressed));
        }
    }

    get_painter().setPen(Qt::black);
    //draw the lines between the white keys
    for(l_j = 1; l_j < 7; l_j++)
    {
        get_painter().drawLine(toInt(double(l_j) * 235.0 * l_scale_X), 0, toInt(double(l_j) * 235.0 * l_scale_X), height());
    }

    //draw the black keys
    double l_black_key_offset[5] =
    { 
      150.0,
      430.0,
      855.0,
      1125.0,
      1390.0
    };

    for(l_j = 0; l_j < 5; l_j++)
    {
        get_painter().fillRect(toInt(l_black_key_offset[l_j]*l_scale_X), 0, toInt(110.0 * l_scale_X), toInt(1000.0 * l_scale_Y), Qt::black);
    }

    if(isNote())
    {
        int l_note_part = g_is_black_key_array[currentNote()];
        if(l_note_part < 0)
        {
            //it's a black key
            get_painter().fillRect(toInt(l_black_key_offset[-l_note_part - 1] * l_scale_X), 0, toInt(110.0 * l_scale_X), toInt(1000.0 * l_scale_Y), colorBetween(Qt::black, Qt::red, m_amount_pressed));
        }
    }
    endDrawing();
}

// EOF
