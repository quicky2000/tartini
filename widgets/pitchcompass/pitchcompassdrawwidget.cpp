/***************************************************************************
                          pitchcompassdrawwidget.cpp  -  description
                             -------------------
    begin                : Wed Dec 15 2004
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

#include "pitchcompassdrawwidget.h"
#include "channel.h"
#include "gdata.h"
#include "musicnotes.h"

#include <qmap.h>
#include <qstring.h>

#include <qwt_compass.h>
#include <qwt_dial_needle.h>
#include <QResizeEvent>

#define INTERVAL 90

//------------------------------------------------------------------------------
PitchCompassDrawWidget::PitchCompassDrawWidget( QWidget *p_parent
                                              , const char* p_name
                                              , int p_mode
                                              )
: QWidget(p_parent, p_name, Qt::WDestructiveClose)
{
    this->m_mode = p_mode;

    m_compass = new QwtCompass(this);
    m_compass->setLineWidth(4);
    m_compass->setFrameShadow(QwtCompass::Sunken);
    
    if(p_mode == 0)
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
        m_compass->setScale(36, 5, 0);
    }
    else if(p_mode == 1)
    {
        m_compass->setMode(QwtCompass::RotateScale);
        m_compass->setScale(360, 0);
    }
    else if(p_mode == 2)
    {
        QMap< double, QString > l_notes;
        m_compass->setMode(QwtCompass::RotateNeedle);
        m_compass->setScale(11, 2, 30);
        for(int l_i = 0; l_i < 12; l_i++)
        {
            l_notes[l_i * 30] = noteName(l_i);
        }
        m_compass->setLabelMap(l_notes);
    }

    m_compass->setScaleTicks(1, 1, 3);

    m_compass->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle));
    m_compass->setValue(0.0);

    // Force a blank update
    m_blank_count = 9;
    blank();

    m_compass->setReadOnly(true);
    m_compass->show();

}

//------------------------------------------------------------------------------
PitchCompassDrawWidget::~PitchCompassDrawWidget(void)
{
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::resizeEvent(QResizeEvent *)
{
    m_compass->resize(size());
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::updateCompass(double p_time)
{
    Channel *l_active_channel = g_data->getActiveChannel();
    if(l_active_channel == NULL)
    {
        blank();
        return;
    }

    const AnalysisData *l_data = l_active_channel->dataAtTime(p_time);

    if(l_data && l_data->getCorrelation() >= 0.9)
    {
        double l_pitch = l_data->getPitch();

        if(m_mode == 0)
        {
            QMap< double, QString > l_notes;
            double l_zero_val = myround(l_pitch);

            double l_value = (l_pitch - l_zero_val) * INTERVAL;
            m_compass->setValue(l_value);

            l_notes[INTERVAL * 3] = noteName(toInt(l_zero_val));
            l_notes[0] = noteName(toInt(l_zero_val += 2));
            l_notes[INTERVAL] = noteName(toInt(l_zero_val));
            m_compass->setLabelMap(l_notes);
        }
        else if(m_mode == 1)
        {
            QMap< double, QString > l_notes;
            double l_close_pitch = myround(l_pitch);
            double l_start = toInt((l_close_pitch - l_pitch) * INTERVAL);

            if(l_start < 0)
            {
                l_start += 360;
            }
            if(l_start > 360)
            {
                l_start = fmod(l_start, 360.0);
            }

            l_notes[l_start] = noteName(toInt(l_close_pitch));
            l_notes[l_start - INTERVAL] = noteName(toInt(l_close_pitch - 1));
            l_notes[l_start + INTERVAL] = noteName(toInt(l_close_pitch + 1));
            m_compass->setLabelMap(l_notes);
        }
        else
        {
            // mode == 2
            double l_value = l_pitch * 30;
            m_compass->setValue(l_value);
        }

        m_compass->setValid(true);
    }
    else
    {
        blank();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::blank(void)
{
    if(++m_blank_count % 10 == 0)
    {
        if(m_mode != 2)
        {
            QMap< double, QString > l_notes;
            m_compass->setLabelMap(l_notes);
        }
        m_compass->setValid(false);
        m_blank_count = 1;
    }
}
// EOF
