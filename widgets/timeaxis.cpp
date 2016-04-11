/***************************************************************************
                          timeaxis.cpp  -  description
                             -------------------
    begin                : Mon Dec 13 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "timeaxis.h"
#include "useful.h"
#include "gdata.h"

#include <math.h>
#include <QPainter>
#include <QPaintEvent>

//------------------------------------------------------------------------------
TimeAxis::TimeAxis(QWidget * p_parent
                  ,bool p_numbers_on_top
                  )
: DrawWidget(p_parent)
, m_left_time(0.0)
, m_right_time(0.0)
, m_numbers_on_top(p_numbers_on_top)
{
    init();
}

//------------------------------------------------------------------------------
TimeAxis::TimeAxis(QWidget * p_parent
                  ,const double & p_left_time
                  ,const double & p_right_time
                  ,bool p_numbers_on_top
                  )
: DrawWidget(p_parent)
, m_left_time(p_left_time)
, m_right_time(p_right_time)
, m_numbers_on_top(p_numbers_on_top)
{
    init();
}

//------------------------------------------------------------------------------
void TimeAxis::init(void)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, false));

#ifdef MACX
    setFontSize(14);
#else // MACX
    setFontSize(12);
#endif // MACX
}

//------------------------------------------------------------------------------
TimeAxis::~TimeAxis(void)
{
}

//------------------------------------------------------------------------------
void TimeAxis::setFontSize(int p_font_size)
{
    m_font_size = p_font_size;
    m_font = QFont("AnyStyle", m_font_size);
}

//------------------------------------------------------------------------------
void TimeAxis::paintEvent(QPaintEvent *)
{
    int l_frame_width = 2;
    const int l_h = height();
    const int l_w = width() - 2 * l_frame_width;
    int l_font_space = m_font_size + 2;

    beginDrawing(false);
    fillBackground(colorGroup().background());

    //time per 150 pixels
    double l_time_step = timeWidth() / double(l_w) * 150.0;

    //round down to the nearest power of 10
    double l_time_scale_base = pow10(floor(log10(l_time_step)));

    //choose a timeScaleStep which is a multiple of 1, 2 or 5 of l_time_scale_base
    int l_large_freq;
    if(l_time_scale_base * 5.0 < l_time_step)
    {
        l_large_freq = 5;
    }
    else if (l_time_scale_base * 2.0 < l_time_step)
    {
        l_large_freq = 2;
    }
    else
    {
        l_large_freq = 2;
        l_time_scale_base /= 2;
    }

    // Draw Ruler Numbers
    get_painter().setBrush(Qt::black);
    get_painter().setFont(m_font);
    //calc the first one just off the left of the screen
    double l_time_pos = floor(leftTime() / (l_time_scale_base * l_large_freq)) * (l_time_scale_base * l_large_freq);
    int l_x, l_large_counter = -1;

    //precalculate line sizes (for efficiency)
    int l_small_line_top = 0;
    int l_small_line_bottom = 0;
    if(m_numbers_on_top)
    {
        l_small_line_top = l_h - 1 - (l_h - 1 - l_font_space) / 2;
        l_small_line_bottom = l_h - 1;
    }
    else
    {
        l_small_line_top = 0;
        l_small_line_bottom = (l_h - 1 - l_font_space) / 2;
    }
    int l_big_line_top = 0;
    int l_big_line_bottom = 0;
    if(m_numbers_on_top)
    {
        l_big_line_top = l_font_space;
        l_big_line_bottom = l_h - 1;
    }
    else
    {
        l_big_line_top = 0;
        l_big_line_bottom = l_h - 1 - l_font_space;
    }
    int l_text_bottom = 0;
    if(m_numbers_on_top)
    {
        l_text_bottom = m_font_size;
    }
    else
    {
        l_text_bottom = l_h - 1;
    }

    for(; l_time_pos <= rightTime(); l_time_pos += l_time_scale_base)
    {
        if(++l_large_counter == l_large_freq)
        {
            l_large_counter = 0;
            //draw the bigger lines and the numbers
            double l_new_time = myround(l_time_pos / l_time_scale_base) * l_time_scale_base;
            QString l_mins;
            double l_secs = fmod(l_new_time, 60.0);

            if (l_time_pos < 0)
            {
                l_mins = "-" + QString::number(int(ceil(l_new_time / 60)));
                l_secs *= -1;
            }
            else
            {
                l_mins = QString::number(int(floor(l_new_time / 60)));
            }

            QString l_seconds = QString::number(l_secs);
            if (l_secs < 10 && l_secs > -10)
            {
                l_seconds = "0" + l_seconds;
            }

            QString l_num_string = l_mins + ":" + l_seconds;
            l_x = l_frame_width + toInt((l_time_pos - leftTime()) / (timeWidth() / double(l_w)));
            get_painter().drawText(l_x - (get_painter().fontMetrics().width(l_num_string) / 2), l_text_bottom, l_num_string);
            get_painter().drawLine(l_x, l_big_line_top, l_x, l_big_line_bottom);
        }
        else
        {
            //draw the smaller lines
            l_x = l_frame_width + toInt((l_time_pos - leftTime()) / (timeWidth() / double(l_w)));
            get_painter().drawLine(l_x, l_small_line_top, l_x, l_small_line_bottom);
        }
    }
    //draw the horizontal line
    if(m_numbers_on_top)
    {
        get_painter().drawLine(0, l_h - 1, width(), l_h - 1);
    }
    else
    {
        get_painter().drawLine(0, 0, width(), 0);
    }
    endDrawing();
}

// EOF
