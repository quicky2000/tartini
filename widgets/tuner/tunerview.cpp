/***************************************************************************
                          tunerview.cpp  -  description
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
#include <qwt_slider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <Q3GridLayout>
#include <QResizeEvent>
#include <QPaintEvent>

#include "tunerview.h"
#include "tunerwidget.h"
#include "ledindicator.h"
#include "useful.h"
#include "gdata.h"
#include "channel.h"
#include "musicnotes.h"

int LEDLetterLookup[12] =
  {
    2,
    2,
    3,
    3,
    4,
    5,
    5,
    6,
    6,
    0,
    0,
    1
  };

//------------------------------------------------------------------------------
TunerView::TunerView(int viewID_, QWidget *parent ):
  ViewWidget( viewID_, parent)
{

  Q3GridLayout * layout = new Q3GridLayout(this, 9, 3, 2);
  layout->setResizeMode(QLayout::SetNoConstraint);

  // Tuner widget goes from (0, 0) to (0, 8);
  tunerWidget = new VibratoTunerWidget(this);
  layout->addMultiCellWidget(tunerWidget, 0, 0, 0, 8);

  // Slider goes from (2,0) to (2,9)

#if QWT_VERSION == 0x050000
  slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::Bottom, QwtSlider::BgTrough);
#else
  slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::BottomScale, QwtSlider::BgTrough);
#endif
  slider->setRange(0, 2);
  slider->setReadOnly(false);
  layout->addMultiCellWidget(slider, 1, 1, 0, 8);
  QToolTip::add(slider, tr("Increase slider to smooth the pitch over a longer time period"));

  ledBuffer = new QPixmap();
  leds.push_back(new LEDIndicator(ledBuffer, this, "A"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "B"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "C"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "D"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "E"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "F"));
  leds.push_back(new LEDIndicator(ledBuffer, this, "G"));

  leds.push_back(new LEDIndicator(ledBuffer, this, "#"));


  // Add the leds for note names into the positions (1, 0) to (1, 6)
  for (int n = 0; n < 7; n++)
    {
      layout->addWidget(leds.at(n), 2, n);
    }

  // (1, 7) is blank
  
  // Add the flat led
  layout->addWidget(leds.at(7), 2, 8);

  layout->setRowStretch( 0, 4 );
  layout->setRowStretch( 1, 1 );
  layout->setRowStretch( 2, 0 ); 
    
  connect(gdata, SIGNAL(onChunkUpdate()), this, SLOT(doUpdate()));
  connect(tunerWidget, SIGNAL(ledSet(int, bool)), this, SLOT(setLed(int, bool)));
}

//------------------------------------------------------------------------------
TunerView::~TunerView(void)
{
  delete slider;
  for (uint i = 0; i < leds.size(); i++)
    {
      delete leds[i];
    }
  delete ledBuffer;
  delete tunerWidget;
}

//------------------------------------------------------------------------------
void TunerView::resizeEvent(QResizeEvent *)
{
}

//------------------------------------------------------------------------------
void TunerView::resetLeds(void)
{
  for (uint i = 0; i < leds.size(); i++)
    {
      leds[i]->setOn(false);
    }
}

//------------------------------------------------------------------------------
void TunerView::slotCurrentTimeChanged(double /*time*/)
{
}

//------------------------------------------------------------------------------
void TunerView::paintEvent( QPaintEvent* )
{
}

//------------------------------------------------------------------------------
void TunerView::setLed(int index, bool value)
{
  leds[index]->setOn(value);
}

//------------------------------------------------------------------------------
void TunerView::doUpdate(void)
{
  Channel *active = gdata->getActiveChannel();
  if (active == NULL || !active->hasAnalysisData())
    {
      tunerWidget->doUpdate(0.0);
      return;
    }
  ChannelLocker channelLocker(active);
  double time = gdata->getView().currentTime();

  // To work out note:
  //   * Find the slider's value. This tells us how many seconds to average over.
  //   * Start time is currentTime() - sliderValue.
  //   * Finish time is currentTime().
  //   * Calculate indexes for these times, and use them to call average.
  //

  double sliderVal = slider->value();

  double pitch = 0.0;
  if (sliderVal == 0)
    {
      int chunk = active->currentChunk();
      if(chunk >= active->totalChunks()) chunk = active->totalChunks() - 1;
      if(chunk >= 0)
	{
	  pitch = active->dataAtChunk(chunk)->getPitch();
	}
    }
  else
    {
      double startTime = time - sliderVal;
      double stopTime = time;
  
      int startChunk = active->chunkAtTime(startTime);
      int stopChunk = active->chunkAtTime(stopTime) + 1;
      pitch = active->averagePitch(startChunk, stopChunk);
    }
  tunerWidget->doUpdate(pitch);
}

//------------------------------------------------------------------------------
QSize TunerView::sizeHint(void) const
{
  return QSize(200, 200);
}
// EOF
