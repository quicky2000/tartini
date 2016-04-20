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
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

#include "debugwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"

DebugWidget::DebugWidget(QWidget *parent)
  : DrawWidget(parent)
{
  textY = 0;
}

DebugWidget::~DebugWidget()
{
}

void DebugWidget::printString(const QString &s) {
  textY += fontHeight;
  p.drawText(0, textY, s);
}

void DebugWidget::paintEvent( QPaintEvent * )
{
  Channel *ch = gdata->getActiveChannel();

  beginDrawing();

  // Drawing code goes here
  QFontMetrics fm = p.fontMetrics();
  fontHeight = fm.height();

  if (ch != NULL && ch->isValidChunk(ch->currentChunk())) {
    textY = 0; //move text position back to the top
    int chunk = ch->currentChunk();
    AnalysisData &data = *ch->dataAtChunk(chunk);
    QString s;
    printString(s.sprintf("Chunk = %d", chunk));
    printString(s.sprintf("period = %f", data.getPeriod()));
    printString(s.sprintf("fundamentalFreq = %f", data.getFundamentalFreq()));
    printString(s.sprintf("pitch = %f", data.getPitch()));
    printString(s.sprintf("pitchSum = %f", data.getPitchSum()));
    printString(s.sprintf("pitch2Sum = %f", data.getPitch2Sum()));
    printString(s.sprintf("_freqCentroid = %f", data.getFreqCentroid()));
    printString(s.sprintf("shortTermMean = %f", data.getShortTermMean()));
    printString(s.sprintf("shortTermDeviation = %f", data.getShortTermDeviation()));
    printString(s.sprintf("longTermMean = %f", data.getLongTermMean()));
    printString(s.sprintf("longTermDeviation = %f", data.getLongTermDeviation()));
    printString(s.sprintf("highestCorrelationIndex = %d", data.getHighestCorrelationIndex()));
    printString(s.sprintf("chosenCorrelationIndex = %d", data.chosenCorrelationIndex));
    //printString(s.sprintf("periodOctaveEstimate = %f", data.periodOctaveEstimate));
    printString(s.sprintf("periodOctaveEstimate = %f", ch->periodOctaveEstimate(chunk)));
    printString(s.sprintf("noteIndex = %d", data.noteIndex));
    printString(s.sprintf("done = %s", data.done ? "true" : "false"));
    printString(s.sprintf("reason = %d", data.reason));
    printString(s.sprintf("notePlaying = %d", data.notePlaying));
    printString(s.sprintf("spread = %f", data.getSpread()));
    printString(s.sprintf("spread2 = %f", data.getSpread2()));
    printString(s.sprintf("logrms = %f", data.logrms()));
    printString(s.sprintf("normalised_logrms = %f", dB2Normalised(data.logrms())));
    printString(s.sprintf("detailedPeriod.size() = %d", ch->detailedPitchData.size()));
    printString(s.sprintf("vibratoPitch = %f", data.getVibratoPitch()));
    printString(s.sprintf("vibratoWidth = %f", data.getVibratoWidth()));
    printString(s.sprintf("vibratoSpeed = %f", data.vibratoSpeed));
    printString(s.sprintf("vibratoPhase = %f", data.vibratoPhase));
    printString(s.sprintf("vibratoError = %f", data.vibratoError));
    printString(s.sprintf("vibratoWidthAdjust = %f", data.getVibratoWidthAdjust()));
    printString(s.sprintf("periodRatio = %f", data.periodRatio));
  }

  endDrawing();
}
