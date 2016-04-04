/***************************************************************************
                          htrackview.cpp  -  description
                             -------------------
    begin                : 29 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "htrackview.h"
#include "htrackwidget.h"
#include "gdata.h"

#include <qlayout.h>
#include <qslider.h>
#include <qwt_wheel.h>
#include <qsizegrip.h>
#include <qgrid.h>
#include <qtooltip.h>
#include <qpushbutton.h>

HTrackView::HTrackView( int viewID_, QWidget *parent, const char *name )
 : ViewWidget( viewID_, parent, name)
{
  //setCaption("HTrack view");
  QGridLayout *mainLayout = new QGridLayout(this, 2, 2);
  mainLayout->setResizeMode(QLayout::FreeResize);
  //QBoxLayout *topLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
  QBoxLayout *rightLayout = new QVBoxLayout();
  QBoxLayout *bottomLayout = new QHBoxLayout();

  QGrid *frame = new QGrid(1, this);
  frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  //frame->setLineWidth(2);
  //frame->setMidLineWidth(2);
  QWidget *aWidget = new QWidget(frame);
  hTrackWidget = new HTrackWidget(aWidget);
  //hTrackWidget->show();

  peakThresholdSlider = new QSlider(0, 100, 10, 5, Qt::Vertical, this);
  QToolTip::add(peakThresholdSlider, "Thresholding of harmonics");
  
  rotateXWheel = new QwtWheel(this);
  rotateXWheel->setWheelWidth(20);
  rotateXWheel->setRange(-180, 180, 0.1, 1);
  QToolTip::add(rotateXWheel, "Rotate piano horizonally");
  
  rotateYWheel = new QwtWheel(this);
  rotateYWheel->setOrientation(Qt::Vertical);
  rotateYWheel->setWheelWidth(20);
  rotateYWheel->setRange(-90, 0, 0.1, 1);
  QToolTip::add(rotateYWheel, "Rotate piano vertically");
  
  distanceWheel = new QwtWheel(this);
  distanceWheel->setOrientation(Qt::Vertical);
  //distanceWheel->setWheelWidth(20);
  distanceWheel->setRange(100, 5000, 10, 20);
  distanceWheel->setTotalAngle(20*360);
  QToolTip::add(distanceWheel, "Move towards/away from piano");
  
  QPushButton *homeButton = new QPushButton("Reset", this, "homebutton");
  QToolTip::add(homeButton, "Return to the original view");
  
  QSizeGrip *sizeGrip = new QSizeGrip(this);
  //sizeGrip->setFixedSize(15, 15);
  
  mainLayout->addWidget(frame, 0, 0);
  mainLayout->addLayout(bottomLayout, 1, 0);
  mainLayout->addLayout(rightLayout, 0, 1);
  rightLayout->addStretch(2);
  rightLayout->addWidget(peakThresholdSlider);
  rightLayout->addStretch(4);
  rightLayout->addWidget(rotateYWheel);
  rightLayout->addSpacing(14);
  rightLayout->addWidget(distanceWheel);
  bottomLayout->addStretch(0);
  bottomLayout->addWidget(homeButton);
  bottomLayout->addSpacing(14);
  bottomLayout->addWidget(rotateXWheel);
  //bottomLayout->addSpacing(20);
  mainLayout->addWidget(sizeGrip, 1, 1);

  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onSlowUpdate()), hTrackWidget, SLOT(update()));
  connect(peakThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setPeakThreshold(int)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleVertical(double)));
  connect(rotateYWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleVerticalChanged(double)), rotateYWheel, SLOT(setValue(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setDistanceAway(double)));
  connect(distanceWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(distanceAwayChanged(double)), distanceWheel, SLOT(setValue(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(setViewAngleHorizontal(double)));
  connect(rotateXWheel, SIGNAL(valueChanged(double)), hTrackWidget, SLOT(update()));
  connect(hTrackWidget, SIGNAL(viewAngleHorizontalChanged(double)), rotateXWheel, SLOT(setValue(double)));
  connect(homeButton, SIGNAL(clicked()), hTrackWidget, SLOT(home()));
}

HTrackView::~HTrackView()
{
  delete hTrackWidget;
}

void HTrackView::resizeEvent(QResizeEvent *)
{
  hTrackWidget->resize(size());
}

void HTrackView::setPeakThreshold(int value)
{
  hTrackWidget->setPeakThreshold(float(value) / 100.0);
  hTrackWidget->update();
}
