/***************************************************************************
                          debugwidget.cpp  -  description
                             -------------------
    begin                : Wed Feb 22 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

#include "debugwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"

//------------------------------------------------------------------------------
DebugWidget::DebugWidget(QWidget * p_parent)
: DrawWidget(p_parent)
{
    m_text_Y = 0;
}

//------------------------------------------------------------------------------
DebugWidget::~DebugWidget(void)
{
}

//------------------------------------------------------------------------------
void DebugWidget::printString(const QString & p_string)
{
    m_text_Y += m_font_height;
    get_painter().drawText(0, m_text_Y, p_string);
}

//------------------------------------------------------------------------------
void DebugWidget::paintEvent( QPaintEvent * )
{
    Channel *l_active_channel = g_data->getActiveChannel();

    beginDrawing();

    // Drawing code goes here
    QFontMetrics l_font_metric = get_painter().fontMetrics();
    m_font_height = l_font_metric.height();

    if(l_active_channel != NULL && l_active_channel->isValidChunk(l_active_channel->currentChunk()))
    {
        //move text position back to the top
        m_text_Y = 0;
        int l_chunk = l_active_channel->currentChunk();
        AnalysisData &l_data = *l_active_channel->dataAtChunk(l_chunk);
        QString l_string;
        printString(l_string.sprintf("Chunk = %d", l_chunk));
        printString(l_string.sprintf("period = %f", l_data.getPeriod()));
        printString(l_string.sprintf("fundamentalFreq = %f", l_data.getFundamentalFreq()));
        printString(l_string.sprintf("pitch = %f", l_data.getPitch()));
        printString(l_string.sprintf("m_pitch_sum = %f", l_data.getPitchSum()));
        printString(l_string.sprintf("m_pitch_squared_sum = %f", l_data.getPitch2Sum()));
        printString(l_string.sprintf("m_freq_centroid = %f", l_data.getFreqCentroid()));
        printString(l_string.sprintf("shortTermMean = %f", l_data.getShortTermMean()));
        printString(l_string.sprintf("m_short_term_deviation = %f", l_data.getShortTermDeviation()));
        printString(l_string.sprintf("longTermMean = %f", l_data.getLongTermMean()));
        printString(l_string.sprintf("m_long_term_deviation = %f", l_data.getLongTermDeviation()));
        printString(l_string.sprintf("m_highest_correlation_index = %d", l_data.getHighestCorrelationIndex()));
        printString(l_string.sprintf("m_chosen_correlation_index = %d", l_data.getChosenCorrelationIndex()));
        printString(l_string.sprintf("periodOctaveEstimate = %f", l_active_channel->periodOctaveEstimate(l_chunk)));
        printString(l_string.sprintf("m_note_index = %d", l_data.getNoteIndex()));
        printString(l_string.sprintf("done = %s", l_data.isDone() ? "true" : "false"));
        printString(l_string.sprintf("reason = %d", l_data.getReason()));
        printString(l_string.sprintf("m_note_playing = %d", l_data.isNotePlaying()));
        printString(l_string.sprintf("m_spread = %f", l_data.getSpread()));
        printString(l_string.sprintf("m_spread_2 = %f", l_data.getSpread2()));
        printString(l_string.sprintf("logrms = %f", l_data.getLogRms()));
        printString(l_string.sprintf("normalised_logrms = %f", dB2Normalised(l_data.getLogRms(),*g_data)));
        printString(l_string.sprintf("detailedPeriod.size() = %d", l_active_channel->get_detailed_pitch_data().size()));
        printString(l_string.sprintf("m_vibrato_pitch = %f", l_data.getVibratoPitch()));
        printString(l_string.sprintf("m_vibrato_width = %f", l_data.getVibratoWidth()));
        printString(l_string.sprintf("m_vibrato_speed = %f", l_data.getVibratoSpeed()));
        printString(l_string.sprintf("m_vibrato_phase = %f", l_data.getVibratoPhase()));
        printString(l_string.sprintf("m_vibrato_error = %f", l_data.getVibratoError()));
        printString(l_string.sprintf("m_vibrato_width_adjust = %f", l_data.getVibratoWidthAdjust()));
        printString(l_string.sprintf("m_period_ratio = %f", l_data.getPeriodRatio()));
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize DebugWidget::sizeHint(void) const
{
    return QSize(250, 500);
}
// EOF
