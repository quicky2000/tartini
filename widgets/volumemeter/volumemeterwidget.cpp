/***************************************************************************
                          volumemeterwidget.cpp  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qpixmap.h>
#include <qpainter.h>
#include <QPaintEvent>

#include <vector>

#include "volumemeterwidget.h"
#include "gdata.h"
#include "channel.h"
#include "soundfile.h"
#include "analysisdata.h"
#include "useful.h"

VolumeMeterWidget::VolumeMeterWidget(QWidget *parent)
  : DrawWidget(parent)
{
  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));

  // Define the number of labels to use for available places
  labelNumbers.push_back(1);
  labelNumbers.push_back(2);
  labelNumbers.push_back(3);
  labelNumbers.push_back(3);
  labelNumbers.push_back(3);
  labelNumbers.push_back(6);

  setFontSize(9);
}

VolumeMeterWidget::~VolumeMeterWidget()
{
}

void VolumeMeterWidget::setFontSize(int fontSize)
{
  _fontSize = fontSize;
  _font = QFont("AnyStyle", _fontSize);
}

void VolumeMeterWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();

  beginDrawing(false);
  fillBackground(colorGroup().background());
  p.setFont(_font);
  
  // Work out how many labels we can draw
  QFontMetrics fm = p.fontMetrics();
  int labelWidth = fm.width("-60"); // Since we'll have two characters normally
  int halfLabelWidth = labelWidth / 2;
  int stopWidth = fm.width("0dB") / 2;
  int realWidth = width() - stopWidth - halfLabelWidth; // The actual width of the data
  int places = labelNumbers[MIN(realWidth / 20, 6) - 1]; // How many labels we should have
  int pixelStep = toInt(double(realWidth) / double(places)); // How many pixels to jump between labels/markers
  int lineY = height() - _fontSize - 2;

  int labelStep = 10;

  // Determine how many dB we should jump by each time
  switch (places) {
    case 1:
    labelStep = 60;
    break;
    case 2:
    labelStep = 30;
    break;
    case 3:
    labelStep = 20;
    break;
    case 6:
    labelStep = 10;
    break;
    default:
    myassert(false); // This should never happen!
    break;
  }


  // Draw horizontal line
  p.setPen(Qt::black);

  int y = height() - (lineY/2)- 1;

  for (int i=0; i < places; i++) {
    int x = i * pixelStep;
    p.drawText(x, y, QString::number(-60 + (i * labelStep)));
  }
  p.drawText(places * pixelStep - 2, y, "0dB");

  
  QColor colour;
  float theVal[2];
  if (active != NULL && active->isValidChunk(active->currentChunk())) {
    int chunk = active->currentChunk();
    if(active->getParent()->numChannels() > 1) {
      theVal[0] = active->getParent()->getChannel(0).dataAtChunk(chunk)->getMaxIntensityDB();
      theVal[1] = active->getParent()->getChannel(1).dataAtChunk(chunk)->getMaxIntensityDB();
    } else {
      theVal[0] = theVal[1] = active->dataAtChunk(chunk)->getMaxIntensityDB();
    }
  } else {
    theVal[0] = theVal[1] = 0.0;
  }
  
  for(int chnl=0; chnl<2; chnl++) {
    double decibels = theVal[chnl];
    // We'll show 60 dB
    int val = toInt((double(realWidth / 60.0) * decibels) + realWidth);

    int barWidth = 5; // Not right - needs to be based on realWidth

    for(int j=halfLabelWidth; j<=realWidth; j+=10) {
      if(j < val) {
        if ( j < (width() * 0.5)) colour = Qt::blue;
        else if ( j < (width() * 0.85)) colour = QColor(255, 120, 60);
        else colour = Qt::red;
      } else {
        colour = QColor(198, 198, 198);
      }
      
      if(chnl == 0)
        p.fillRect(j, 2, barWidth, lineY/2 -3, colour);
      else
        p.fillRect(j, height() - (lineY/2) + 1, barWidth, lineY/2 -3, colour);
    }
  }

  endDrawing();  
}
