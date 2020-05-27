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

    if(m_mode == PitchCompassView::CompassMode::Mode0)
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
        m_compass->setScale(0, 4);
    }
    else if(m_mode == PitchCompassView::CompassMode::Mode1)
    {
        m_compass->setMode(QwtCompass::RotateScale);
        m_compass->setScale(0, 4);
    }
    else if(m_mode == PitchCompassView::CompassMode::Mode2)
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
        m_compass->setScale(0, 12);
    }

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
void PitchCompassDrawWidget::setCompassScale(double p_pitch)
{
    QwtCompassScaleDraw *l_scale_draw = new QwtCompassScaleDraw();

    if(m_mode == PitchCompassView::CompassMode::Mode0)
    {
        QMap< double, QString > l_notes;
        int l_zero_note = MusicNote::closestNote(p_pitch);

        l_notes[3] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_zero_note - 1)), -1));
        l_notes[0] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_zero_note)), 0));
        l_notes[1] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_zero_note + 1)), 1));
        l_notes[2] = QString::fromStdString(padLabel("", 0));

        myassert(l_scale_draw);
        l_scale_draw->setLabelMap(l_notes);
    }
    else if(m_mode == PitchCompassView::CompassMode::Mode1)
    {
        QMap< double, QString > l_notes;
        int l_close_note = MusicNote::closestNote(p_pitch);

        l_notes[3] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_close_note - 1)), -1));
        l_notes[0] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_close_note)), 0));
        l_notes[1] = QString::fromStdString(padLabel(MusicNote::semitoneName(MusicNote::semitoneValue(l_close_note + 1)), 1));
        l_notes[2] = QString::fromStdString(padLabel("", 0));

        myassert(l_scale_draw);
        l_scale_draw->setLabelMap(l_notes);
    }
    else if(m_mode == PitchCompassView::CompassMode::Mode2)
    {
        int l_music_key = GData::getUniqueInstance().musicKey();
        const MusicScale &l_music_scale = MusicScale::getScale(GData::getUniqueInstance().musicScale());

        QMap< double, QString > l_notes;
        for(int l_index = 0; l_index < 12; l_index++)
        {
            if(l_music_scale.hasSemitone(l_index))
            {
                int l_side = (l_index > 0 && l_index < 6) ? 1 : ( (l_index > 6) ? -1 : 0 );
                std::string label = padLabel(MusicNote::semitoneName(cycle(l_index + g_music_key_roots[l_music_key], 12)), l_side);
                l_notes[l_index] = QString::fromStdString(label);
            }
            else
            {
                // Pad with two spaces
                l_notes[l_index] = "\u2002\u2002";  // Unicode "En Space"
            }
        }
        l_scale_draw->setLabelMap(l_notes);
    }

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
    if (m_mode != PitchCompassView::CompassMode::Mode1)
    {
        m_compass->setMode(QwtCompass::RotateScale);
        m_compass->setMode(QwtCompass::RotateNeedle);
    }
    else
    {
        m_compass->setMode(QwtCompass::RotateNeedle);
        m_compass->setMode(QwtCompass::RotateScale);
    }
}

//------------------------------------------------------------------------------
// Minimize how much the compass resizes due to differences in the lengths of the labels.
// Make all of the labels two characters wide.  Don't use regular spaces, since the compass will trim them.
std::string PitchCompassDrawWidget::padLabel(const std::string & p_label, int p_side)
{
    std::string l_label = p_label;
    
    if (p_side == 0 && l_label.length() == 0)
    {
        // Top or bottom of the compass -- set to two spaces
        l_label = "\u2002\u2002";  // Unicode "En Space"
    }
    else if (l_label.length() < 2)
    {
        if (p_side == 1)
        {
            // Right side of the compass -- pad on the right
            l_label += "\u2002";  // Unicode "En Space"
        }
        else if (p_side == -1)
        {
            // Left side of the compass -- pad on the left
            l_label = "\u2002" + l_label;  // Unicode "En Space"
        }
    }
    
    return l_label;
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
    
    double l_pitch = 0.0;
    if(l_data && l_active_channel->hasAnalysisData())
    {
        int l_chunk = l_active_channel->currentChunk();
        if(l_chunk >= l_active_channel->totalChunks())
        {
            l_chunk = l_active_channel->totalChunks() - 1;
        }
        if(l_chunk >= 0)
        {
            l_pitch = l_active_channel->dataAtChunk(l_chunk)->getPitch();
        }
    }
    
    if (l_pitch > 0)
    {
        if(m_mode == PitchCompassView::CompassMode::Mode0)
        {
            setCompassScale(l_pitch);
            
            int l_zero_note = MusicNote::closestNote(l_pitch);
            double l_zero_pitch = MusicNote::temperedPitch(l_zero_note);
            double l_value = cycle(l_pitch - l_zero_pitch, 4.0);
            m_compass->setValue(l_value);
        }
        else if(m_mode == PitchCompassView::CompassMode::Mode1)
        {
            setCompassScale(l_pitch);
            
            int l_close_note = MusicNote::closestNote(l_pitch);
            double l_close_pitch = MusicNote::temperedPitch(l_close_note);
            double l_value = cycle(l_pitch - l_close_pitch, 4.0);
            m_compass->setValue(l_value);
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
        m_compass->setValid(false);
        m_blank_count = 1;
    }
}
// EOF
