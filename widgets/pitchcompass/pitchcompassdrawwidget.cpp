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
#include "music_note.h"

#include <QMap>
#include <QString>

#include <qwt_compass.h>
#include <qwt_dial_needle.h>
#include <QResizeEvent>

//------------------------------------------------------------------------------
PitchCompassDrawWidget::PitchCompassDrawWidget( QWidget *p_parent
                                              , const std::string & p_name
                                              , PitchCompassView::CompassMode p_mode
                                              )
: QWidget(p_parent)
, m_compass(new QwtCompass(this))
, m_blank_count(0)
, m_mode(p_mode)
{
    setObjectName(QString::fromStdString(p_name));
    setAttribute(Qt::WA_DeleteOnClose);

    m_compass->setLineWidth(4);
    m_compass->setFrameShadow(QwtCompass::Sunken);

    setCompassScale();

    m_compass->setNeedle(new QwtCompassMagnetNeedle(QwtCompassMagnetNeedle::ThinStyle));
    m_compass->setValue(0.0);

    // Force a blank update
    blank(true);

    m_compass->setReadOnly(true);
    m_compass->show();

}

//------------------------------------------------------------------------------
PitchCompassDrawWidget::~PitchCompassDrawWidget()
{
    delete m_compass;
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::resizeEvent(QResizeEvent *)
{
    m_compass->resize(size());
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::setCompassScale()
{
#if QWT_VERSION >= 0x060000
    QwtCompassScaleDraw *l_scale_draw = new QwtCompassScaleDraw();
#endif // QWT_VERSION >= 0x060000

    if(m_mode == PitchCompassView::CompassMode::Mode0)
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
    else if(m_mode == PitchCompassView::CompassMode::Mode1)
    {
        m_compass->setMode(QwtCompass::RotateScale);
        m_compass->setScale(360, 0);
    }
    else if(m_mode == PitchCompassView::CompassMode::Mode2)
    {
        int l_music_key = GData::getUniqueInstance().musicKey();
        const MusicScale &l_music_scale = MusicScale::getScale(GData::getUniqueInstance().musicScale());

        m_compass->setMode(QwtCompass::RotateNeedle);
        QMap< double, QString > l_notes;
#if QWT_VERSION >= 0x060000
        m_compass->setScale(0, 12);
        // Stepping is now defined by qwt_abstract_slider
        m_compass->setSingleSteps(30);
        for(int l_index = 0; l_index < 12; l_index++)
        {
            if(l_music_scale.hasSemitone(l_index))
            {
                std::string label = MusicNote::semitoneName(cycle(l_index + g_music_key_roots[l_music_key], 12));
                
                // Minimize how much the compass resizes due to differences in the lengths of the labels.
                // Make all of the labels two characters wide.  Don't use regular spaces, since the compass will trim them.
                if (label.length() < 2)
                {
                    if (l_index > 0 && l_index < 6)
                    {
                        // Right side of the compass -- pad on the right
                        label += "\u2002";  // Unicode "En Space"
                    }
                    else if (l_index > 6)
                    {
                        // Left side of the compass -- pad on the left
                        label = "\u2002" + label;  // Unicode "En Space"
                    }
                }
                l_notes[l_index] = QString::fromStdString(label);
            }
            else
            {
                // Pad with two spaces
                l_notes[l_index] = "\u2002\u2002";  // Unicode "En Space"
            }
        }
        l_scale_draw->setLabelMap(l_notes);
#else
        m_compass->setScale(11, 2, 30);
        for(int l_index = 0; l_index < 12; l_index++)
        {
            l_notes[l_index * 30] = MusicNote::semitoneName(cycle(l_index + g_music_key_roots[l_music_key], 12));
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

    // Force the compass to invalidate its cache and redraw.
    // QwtDial::invalidateCache() is a protected member.
    // TODO: Is there a cleaner way to do this?
    m_compass->setMode(QwtCompass::RotateScale);
    m_compass->setMode(QwtCompass::RotateNeedle);
#else
    m_compass->setScaleTicks(1, 1, 3);
#endif // QWT_VERSION >= 0x060000
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::updateMusicKey(int)
{
    if (m_mode == PitchCompassView::CompassMode::Mode2)
    {
        setCompassScale();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::updateMusicScale(int)
{
    if (m_mode == PitchCompassView::CompassMode::Mode2)
    {
        setCompassScale();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::updateCompass(double p_time)
{
    const AnalysisData *l_data = nullptr;
    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();
    
    if(l_active_channel)
    {
        l_data = l_active_channel->dataAtTime(p_time);
    }
    
    if(l_data && l_data->getCorrelation() >= 0.9)
    {
        double l_pitch = l_data->getPitch() - GData::getUniqueInstance().semitoneOffset();
        unsigned int l_interval = 90;

        if(m_mode == PitchCompassView::CompassMode::Mode0)
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

            l_notes[(l_interval * 3 ) / l_div] = QString::fromStdString(MusicNote::semitoneName(toInt(l_zero_val)));
            l_notes[0 / l_div] = QString::fromStdString(MusicNote::semitoneName(toInt(l_zero_val += 2)));
            l_notes[l_interval / l_div] = QString::fromStdString(MusicNote::semitoneName(toInt(l_zero_val)));

#if QWT_VERSION >= 0x060000
            QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(m_compass->scaleDraw());
            myassert(l_scale_draw);
            l_scale_draw->setLabelMap(l_notes);
#else
            m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
        }
        else if(m_mode == PitchCompassView::CompassMode::Mode1)
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

            l_notes[l_start] = QString::fromStdString(MusicNote::semitoneName(toInt(l_close_pitch)));
            l_notes[l_start - l_interval] = QString::fromStdString(MusicNote::semitoneName(toInt(l_close_pitch - 1)));
            l_notes[l_start + l_interval] = QString::fromStdString(MusicNote::semitoneName(toInt(l_close_pitch + 1)));

#if QWT_VERSION >= 0x060000
            QwtCompassScaleDraw * l_scale_draw = dynamic_cast<QwtCompassScaleDraw*>(m_compass->scaleDraw());
            myassert(l_scale_draw);
            l_scale_draw->setLabelMap(l_notes);
#else
            m_compass->setLabelMap(l_notes);
#endif // QWT_VERSION >= 0x060000
        }
        else if (m_mode == PitchCompassView::CompassMode::Mode2)
        {
            int l_music_key = GData::getUniqueInstance().musicKey();
            m_compass->setValue(cycle(l_pitch - (double)g_music_key_roots[l_music_key], 12.0) );
        }
        else
        {
            myassert(false);
        }

        m_compass->setValid(true);
    }
    else
    {
        blank();
    }
}

//------------------------------------------------------------------------------
void PitchCompassDrawWidget::blank(bool p_force)
{
    if(p_force || ++m_blank_count >= 10)
    {
        if(m_mode != PitchCompassView::CompassMode::Mode2)
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
