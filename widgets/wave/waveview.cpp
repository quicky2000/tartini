/***************************************************************************
                          waveview.cpp  -  description
                             -------------------
    begin                : Mon Mar 14 2005
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
#include "waveview.h"
#include "wavewidget.h"
#include "gdata.h"
#include "qwt_wheel.h"
#include "qlayout.h"
#include "q3grid.h"
#include "qtooltip.h"
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>
#include <QResizeEvent>

//------------------------------------------------------------------------------
WaveView::WaveView( int viewID_, QWidget *parent ):
  ViewWidget( viewID_, parent)
{
  Q3BoxLayout * mainLayout = new Q3HBoxLayout(this);

  Q3Grid * waveFrame = new Q3Grid(1, this);
  waveFrame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
  waveWidget = new WaveWidget(waveFrame);
  mainLayout->addWidget(waveFrame);

  Q3BoxLayout * rightLayout = new Q3VBoxLayout(mainLayout);
  
  QwtWheel * freqWheelY = new QwtWheel(this);
  freqWheelY->setOrientation(Qt::Vertical);
  freqWheelY->setWheelWidth(14);
  freqWheelY->setRange(1.0, 20.0, 0.1, 1);
  freqWheelY->setValue(1.0);
  QToolTip::add(freqWheelY, "Zoom oscilloscope vertically");
  rightLayout->addWidget(freqWheelY, 1);
  rightLayout->addStretch(2);
  
  connect(freqWheelY, SIGNAL(valueChanged(double)), waveWidget, SLOT(setZoomY(double)));
  connect(waveWidget, SIGNAL(zoomYChanged(double)), waveWidget, SLOT(update()));
  
  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), waveWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
WaveView::~WaveView(void)
{
}

//------------------------------------------------------------------------------
void WaveView::resizeEvent(QResizeEvent *)
{
}

//------------------------------------------------------------------------------
QSize WaveView::sizeHint(void) const
{
  return QSize(400, 128);
}

// EOF
