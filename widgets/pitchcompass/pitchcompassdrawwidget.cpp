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

#include <QMap>
#include <QString>

#include <qwt_compass.h>
#include <qwt_dial_needle.h>
#include <QResizeEvent>

#include "assert.h"

//------------------------------------------------------------------------------
PitchCompassDrawWidget::PitchCompassDrawWidget( QWidget *p_parent
                                              , const std::string & p_name
                                              , int p_mode
                                              )
: QWidget(p_parent)
, m_compass(new QwtCompass(this))
, m_blank_count(9)
, m_mode(p_mode)
{
    setObjectName(QString::fromStdString(p_name));
    setAttribute(Qt::WA_DeleteOnClose);

    m_compass->setLineWidth(4);
    m_compass->setFrameShadow(QwtCompass::Sunken);

#if QWT_VERSION >= 0x060000
    QwtCompassScaleDraw *l_scale_draw = new QwtCompassScaleDraw();
#endif // QWT_VERSION >= 0x060000

    if(p_mode == 0)
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
#if QWT_VERSION >= 0x060000
        m_compass->setScale(36, 5);
	    // Stepping is now defined by qwt_abstract_slider
	    m_compass->setSingleSteps(0);
#else
        m_compass->setScale(36, 5, 0);
#endif // QWT_VERSION >= 0x060000
    }
    else if(p_mode == 1)
    {
        m_compass->setMode(QwtCompass::RotateScale);
        m_compass->setScale(360, 0);
    }
    else if(p_mode == 2)
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
        QMap< double, QString > l_notes;
#if QWT_VERSION >= 0x060000
        m_compass->setScale(0, 12);
        // Stepping is now defined by qwt_abstract_slider
        m_compass->setSingleSteps(30);
        for(int l_index = 0; l_index < 12; l_index++)
        {
            l_notes[l_index] = QString::fromStdString(music_notes::noteName(l_index));
        }
        l_scale_draw->setLabelMap(l_notes);
#else
        m_compass->setScale(11, 2, 30);
        for(int l_index = 0; l_index < 12; l_index++)
        {
            l_notes[l_index * 30] = noteName(l_index);
        }
        m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
    }

#if QWT_VERSION >= 0x060000
    // I assume that y defualt Ticks Labels and backbone where displayed with Qwt 5.x
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Ticks, true );
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Labels, true );
    l_scale_draw->enableComponent( QwtAbstractScaleDraw::Backbone, true );

    // setScaleTicks method is now replaced by setTickLegnth of QwtAbstractScaleDraw
    l_scale_draw->setTickLength( QwtScaleDiv::MinorTick,10);
    l_scale_draw->setTickLength( QwtScaleDiv::MediumTick,20);
    l_scale_draw->setTickLength( QwtScaleDiv::MajorTick,30);
    m_compass->setScaleDraw(l_scale_draw);
#else
    m_compass->setScaleTicks(1, 1, 3);
#endif // QWT_VERSION >= 0x060000

    m_compass->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle));
    m_compass->setValue(0.0);

    // Force a blank update
    blank();

    m_compass->setReadOnly(true);
    m_compass->show();

}

//------------------------------------------------------------------------------
PitchCompassDrawWidget::~PitchCompassDrawWidget()
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
    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();
    if(l_active_channel == nullptr)
    {
        blank();
        return;
    }

    const AnalysisData *l_data = l_active_channel->dataAtTime(p_time);

    if(l_data && l_data->getCorrelation() >= 0.9)
    {
        double l_pitch = l_data->getPitch();
        unsigned int l_interval = 90;

        if(m_mode == 0)
        {
            QMap< double, QString > l_notes;
            double l_zero_val = myround(l_pitch);
            double l_value = (l_pitch - l_zero_val) * l_interval;
            m_compass->setValue(l_value);

#if QWT_VERSION >= 0x060000
            // With Qwt 6.x the map values should match the scale
            // In mode 0 scale is 36 isntead of 360 so need to divide keys by 10
            unsigned int l_div = 10;
#else
            unsigned int l_div = 1;
#endif // QWT_VERSION >= 0x060000

            l_notes[(l_interval * 3 ) / l_div] = QString::fromStdString(music_notes::noteName(toInt(l_zero_val)));
            l_notes[0 / l_div] = QString::fromStdString(music_notes::noteName(toInt(l_zero_val += 2)));
            l_notes[l_interval / l_div] = QString::fromStdString(music_notes::noteName(toInt(l_zero_val)));

#if QWT_VERSION >= 0x060000
            QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(m_compass->scaleDraw());
            myassert(l_scale_draw);
            l_scale_draw->setLabelMap(l_notes);
#else
            m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
        }
        else if(m_mode == 1)
        {
            QMap< double, QString > l_notes;
            double l_close_pitch = myround(l_pitch);
            double l_start = toInt((l_close_pitch - l_pitch) * l_interval);

            if(l_start < 0)
            {
                l_start += 360;
            }
            if(l_start > 360)
            {
                l_start = fmod(l_start, 360.0);
            }

            l_notes[l_start] = QString::fromStdString(music_notes::noteName(toInt(l_close_pitch)));
            l_notes[l_start - l_interval] = QString::fromStdString(music_notes::noteName(toInt(l_close_pitch - 1)));
            l_notes[l_start + l_interval] = QString::fromStdString(music_notes::noteName(toInt(l_close_pitch + 1)));

#if QWT_VERSION >= 0x060000
            QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(m_compass->scaleDraw());
            myassert(l_scale_draw);
            l_scale_draw->setLabelMap(l_notes);
#else
            m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
        }
        else
        {
            // mode == 2
            m_compass->setValue(noteValue(l_pitch) + l_pitch - toInt(l_pitch));
        }

        m_compass->setValid(true);
    }
    else
    {
        blank();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::blank()
{
    if(++m_blank_count % 10 == 0)
    {
        if(m_mode != 2)
        {
            QMap< double, QString > l_notes;
#if QWT_VERSION >= 0x060000
            QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(m_compass->scaleDraw());
            myassert(l_scale_draw);
            l_scale_draw->setLabelMap(l_notes);
#else
            m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
        }
        m_compass->setValid(false);
        m_blank_count = 1;
    }
}
// EOF
