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
: DrawWidget(p_parent, "DebugWidget")
, m_text_Y(0)
, m_font_height(0)
{
}

//------------------------------------------------------------------------------
DebugWidget::~DebugWidget()
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
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();

    beginDrawing();

    // Drawing code goes here
    QFontMetrics l_font_metric = get_painter().fontMetrics();
    m_font_height = l_font_metric.height();

    if(l_active_channel != nullptr && l_active_channel->isValidChunk(l_active_channel->currentChunk()))
    {
        //move text position back to the top
        m_text_Y = 0;
        int l_chunk = l_active_channel->currentChunk();
        AnalysisData &l_data = *l_active_channel->dataAtChunk(l_chunk);
        printString(QString::fromStdString(std::string("Chunk = ") + std::to_string(l_chunk)));
        printString(QString::fromStdString(std::string("period = ") + std::to_string(l_data.getPeriod())));
        printString(QString::fromStdString(std::string("fundamentalFreq = ") + std::to_string(l_data.getFundamentalFreq())));
        printString(QString::fromStdString(std::string("pitch = ") + std::to_string(l_data.getPitch())));
        printString(QString::fromStdString(std::string("m_pitch_sum = ") + std::to_string(l_data.getPitchSum())));
        printString(QString::fromStdString(std::string("m_pitch_squared_sum = ") + std::to_string(l_data.getPitch2Sum())));
        printString(QString::fromStdString(std::string("m_freq_centroid = ") + std::to_string(l_data.getFreqCentroid())));
        printString(QString::fromStdString(std::string("shortTermMean = ") + std::to_string(l_data.getShortTermMean())));
        printString(QString::fromStdString(std::string("m_short_term_deviation = ") + std::to_string(l_data.getShortTermDeviation())));
        printString(QString::fromStdString(std::string("longTermMean = ") + std::to_string(l_data.getLongTermMean())));
        printString(QString::fromStdString(std::string("m_long_term_deviation = ") + std::to_string(l_data.getLongTermDeviation())));
        printString(QString::fromStdString(std::string("m_highest_correlation_index = ") + std::to_string(l_data.getHighestCorrelationIndex())));
        printString(QString::fromStdString(std::string("m_chosen_correlation_index = ") + std::to_string(l_data.getChosenCorrelationIndex())));
        printString(QString::fromStdString(std::string("periodOctaveEstimate = ") + std::to_string(l_active_channel->periodOctaveEstimate(l_chunk))));
        printString(QString::fromStdString(std::string("m_note_index = ") + std::to_string(l_data.getNoteIndex())));
        printString(QString::fromStdString(std::string("done = ") + (l_data.isDone() ? "true" : "false")));
        printString(QString::fromStdString(std::string("reason = ") + std::to_string(l_data.getReason())));
        printString(QString::fromStdString(std::string("m_note_playing = ") + std::to_string(l_data.isNotePlaying())));
        printString(QString::fromStdString(std::string("m_spread = ") + std::to_string(l_data.getSpread())));
        printString(QString::fromStdString(std::string("m_spread_2 = ") + std::to_string(l_data.getSpread2())));
        printString(QString::fromStdString(std::string("logrms = ") + std::to_string(l_data.getLogRms())));
        printString(QString::fromStdString(std::string("normalised_logrms = ") + std::to_string(dB2Normalised(l_data.getLogRms(),GData::getUniqueInstance()))));
        printString(QString::fromStdString(std::string("detailedPeriod.size() = ") + std::to_string(l_active_channel->get_detailed_pitch_data().size())));
        printString(QString::fromStdString(std::string("m_vibrato_pitch = ") + std::to_string(l_data.getVibratoPitch())));
        printString(QString::fromStdString(std::string("m_vibrato_width = ") + std::to_string(l_data.getVibratoWidth())));
        printString(QString::fromStdString(std::string("m_vibrato_speed = ") + std::to_string(l_data.getVibratoSpeed())));
        printString(QString::fromStdString(std::string("m_vibrato_phase = ") + std::to_string(l_data.getVibratoPhase())));
        printString(QString::fromStdString(std::string("m_vibrato_error = ") + std::to_string(l_data.getVibratoError())));
        printString(QString::fromStdString(std::string("m_vibrato_width_adjust = ") + std::to_string(l_data.getVibratoWidthAdjust())));
        printString(QString::fromStdString(std::string("m_period_ratio = ") + std::to_string(l_data.getPeriodRatio())));
    }
    endDrawing();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: DebugWidget::paintEvent()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
QSize DebugWidget::sizeHint() const
{
    return QSize(250, 500);
}
// EOF
