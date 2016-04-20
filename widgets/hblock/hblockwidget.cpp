/***************************************************************************
                          hblockwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

#include "hblockwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

HBlockWidget::HBlockWidget(QWidget *parent)
  : DrawWidget(parent)
{
}

HBlockWidget::~HBlockWidget()
{
}

void HBlockWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();
  
  beginDrawing();
    
  if(active) {
    AnalysisData *theData = active->dataAtCurrentChunk();
    if(theData) {
  
      //get a copy of theData so we don't hold the mutex for too long
      active->lock();
      AnalysisData data = *theData;
      active->unlock();
    
      // Get the frame's fundamental frequency
      //float fund = active->lookup[frame].fundamentalFreq;
      float fund = data.getFundamentalFreq();
  
      // Work out the bar height for each harmonic
      double barHeight = double(height()) / double(data.getHarmonicFreqSize());
      QColor fillColor = colorBetween(colorGroup().background(), active->color, data.volumeValue());
      QColor outlineColor = colorBetween(colorGroup().background(), Qt::black, data.volumeValue());
      p.setBrush(fillColor);
  
      int barStart = 0;
      float barWidth = 0;
      int diff = 0;
      /*
      * Each note has a fundamental frequency f, which comes from the lookup table.
      * The harmonic frequencies are defined as f, 2f, 3f, 4f, 5f...
      * harmonicFreq stores what the harmonics have been calculated to be.
      */
      for (uint i = 0; i < data.getHarmonicFreqSize(); i++) {
        p.setPen(outlineColor);
        p.setBrush(colorBetween(fillColor, Qt::black, data.harmonicNoise[i]));
        // Work out how many pixels wide the harmonic should be
        barWidth = (data.getHarmonicAmpAt(i)) * width();
        /* Work out how many pixels the harmonic should be offset from where it would be
        * if it were exactly (i+1)f   */
        //diff = toInt( (data.getHarmonicFreqAt(i) - (i+1) * fund) / fund * width() / 10.0 );
        diff = toInt( (data.getHarmonicFreqAt(i) - (i+1) * fund) / fund * barWidth );
        // Work out the starting position, and draw the bar
        barStart = toInt( ((width() / 2) + diff) - barWidth / 2);
	     int barBottom = height() - toInt(barHeight * i);
        p.drawRect(barStart, barBottom, toInt(barWidth), -toInt(barHeight));
        // Draw the centre line on the bar
        p.setPen(Qt::white);
        p.drawLine((width() / 2) + diff, barBottom, (width() / 2) + diff, barBottom - toInt(barHeight));
      }
      // Draw the exact line (f, 2f, 3f...)
      p.setPen(Qt::white);
      p.drawLine(width() / 2, 0, width() /2, height());
    }
  }
  endDrawing();
}
