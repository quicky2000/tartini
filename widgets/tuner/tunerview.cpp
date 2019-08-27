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

int g_LED_letter_lookup[12] =
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
TunerView::TunerView(int p_view_iD_, QWidget *p_parent ):
  ViewWidget( p_view_iD_, p_parent)
{

  Q3GridLayout * l_layout = new Q3GridLayout(this, 9, 3, 2);
  l_layout->setResizeMode(QLayout::SetNoConstraint);

  // Tuner widget goes from (0, 0) to (0, 8);
  m_tuner_widget = new VibratoTunerWidget(this);
  l_layout->addMultiCellWidget(m_tuner_widget, 0, 0, 0, 8);

  // Slider goes from (2,0) to (2,9)

#if QWT_VERSION == 0x050000
  m_slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::Bottom, QwtSlider::BgTrough);
#else
  m_slider = new QwtSlider(this, Qt::Horizontal, QwtSlider::BottomScale, QwtSlider::BgTrough);
#endif
  m_slider->setRange(0, 2);
  m_slider->setReadOnly(false);
  l_layout->addMultiCellWidget(m_slider, 1, 1, 0, 8);
  QToolTip::add(m_slider, tr("Increase slider to smooth the pitch over a longer time period"));

  m_led_buffer = new QPixmap();
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "A"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "B"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "C"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "D"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "E"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "F"));
  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "G"));

  m_leds.push_back(new LEDIndicator(m_led_buffer, this, "#"));


  // Add the leds for note names into the positions (1, 0) to (1, 6)
  for (int l_n = 0; l_n < 7; l_n++)
    {
      l_layout->addWidget(m_leds.at(l_n), 2, l_n);
    }

  // (1, 7) is blank
  
  // Add the flat led
  l_layout->addWidget(m_leds.at(7), 2, 8);

  l_layout->setRowStretch( 0, 4 );
  l_layout->setRowStretch( 1, 1 );
  l_layout->setRowStretch( 2, 0 );
    
  connect(gdata, SIGNAL(onChunkUpdate()), this, SLOT(doUpdate()));
  connect(m_tuner_widget, SIGNAL(ledSet(int, bool)), this, SLOT(setLed(int, bool)));
}

//------------------------------------------------------------------------------
TunerView::~TunerView(void)
{
  delete m_slider;
  for (uint l_index = 0; l_index < m_leds.size(); l_index++)
    {
      delete m_leds[l_index];
    }
  delete m_led_buffer;
  delete m_tuner_widget;
}

//------------------------------------------------------------------------------
void TunerView::resizeEvent(QResizeEvent *)
{
}

//------------------------------------------------------------------------------
void TunerView::resetLeds(void)
{
  for (uint l_index = 0; l_index < m_leds.size(); l_index++)
    {
      m_leds[l_index]->setOn(false);
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
void TunerView::setLed(int p_index, bool p_value)
{
  m_leds[p_index]->setOn(p_value);
}

//------------------------------------------------------------------------------
void TunerView::doUpdate(void)
{
  Channel *l_active_channel = gdata->getActiveChannel();
  if (l_active_channel == NULL || !l_active_channel->hasAnalysisData())
    {
      m_tuner_widget->doUpdate(0.0);
      return;
    }
  ChannelLocker l_channel_locker(l_active_channel);
  double l_time = gdata->getView().currentTime();

  // To work out note:
  //   * Find the slider's value. This tells us how many seconds to average over.
  //   * Start time is currentTime() - sliderValue.
  //   * Finish time is currentTime().
  //   * Calculate indexes for these times, and use them to call average.
  //

  double l_slider_val = m_slider->value();

  double l_pitch = 0.0;
  if (l_slider_val == 0)
    {
      int l_chunk = l_active_channel->currentChunk();
      if(l_chunk >= l_active_channel->totalChunks()) l_chunk = l_active_channel->totalChunks() - 1;
      if(l_chunk >= 0)
	{
	  l_pitch = l_active_channel->dataAtChunk(l_chunk)->getPitch();
	}
    }
  else
    {
      double l_start_time = l_time - l_slider_val;
      double l_stop_time = l_time;
  
      int l_start_chunk = l_active_channel->chunkAtTime(l_start_time);
      int l_stop_chunk = l_active_channel->chunkAtTime(l_stop_time) + 1;
      l_pitch = l_active_channel->averagePitch(l_start_chunk, l_stop_chunk);
    }
  m_tuner_widget->doUpdate(l_pitch);
}

//------------------------------------------------------------------------------
QSize TunerView::sizeHint(void) const
{
  return QSize(200, 200);
}
// EOF
