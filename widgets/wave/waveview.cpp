/***************************************************************************
                          waveview.cpp  -  description
                             -------------------
    begin                : Mon Mar 14 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
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
#include "qgrid.h"
#include "qtooltip.h"

WaveView::WaveView( int viewID_, QWidget *parent, const char *name )
 : ViewWidget( viewID_, parent, name)
{
  //setCaption("Wave view");

  QBoxLayout *mainLayout = new QHBoxLayout(this);

  QGrid *waveFrame = new QGrid(1, this);
  waveFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  waveWidget = new WaveWidget(waveFrame);
  //waveWidget->show();
  //mainLayout->addWidget(waveWidget);
  mainLayout->addWidget(waveFrame);

  //QBoxLayout *leftLayout = new QVBoxLayout(mainLayout);
  QBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
  
  QwtWheel *freqWheelY = new QwtWheel(this);
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
  //connect(gdata->view, SIGNAL(onFastUpdate()), waveWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), waveWidget, SLOT(update()));
}

WaveView::~WaveView()
{
  //delete waveWidget;
}

void WaveView::resizeEvent(QResizeEvent *)
{
  //waveWidget->resize(size());
}
