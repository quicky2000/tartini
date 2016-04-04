/***************************************************************************
                          freqview.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/



#include "freqview.h"
#include "view.h"
#include "gdata.h"
#include "myscrollbar.h"
#include "freqdrawwidget.h"
#include "amplitudewidget.h"
#include "timeaxis.h"
#include "mainwindow.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <qgrid.h>
#include <qwt_wheel.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qspinbox.h>

FreqView::FreqView(int viewID_, QWidget *parent, const char *name)
  : ViewWidget(viewID_, parent, name)
{
  View *view = gdata->view;

  //setCaption("Frequency View");

  QBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setResizeMode(QLayout::FreeResize);
  //QBoxLayout *topLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *rightLayout = new QVBoxLayout(mainLayout);
  //QBoxLayout *bottomLayout = new QHBoxLayout(mainLayout);


  QSplitter *splitter = new QSplitter(Qt::Vertical, this);
  QWidget *topWidget = new QWidget(splitter);
  //topSplitter->setOpaqueResize(true);
  QBoxLayout *topLayout = new QHBoxLayout(topWidget);
  QBoxLayout *topLeftLayout = new QVBoxLayout(topLayout);
  
  timeAxis = new TimeAxis(topWidget, gdata->leftTime(), gdata->rightTime(), true);
  topLeftLayout->addWidget(timeAxis);
  
  QGrid *freqFrame = new QGrid(1, topWidget);
  freqFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  freqDrawWidget = new FreqDrawWidget(freqFrame);
  freqDrawWidget->setFocusPolicy(QWidget::NoFocus);
  topLeftLayout->addWidget(freqFrame);
  
  QBoxLayout *topRightLayout = new QVBoxLayout(topLayout);
  
  freqWheelY = new QwtWheel(topWidget);
  freqWheelY->setOrientation(Qt::Vertical);
  freqWheelY->setWheelWidth(14);
  freqWheelY->setRange(1.6, 5.0, 0.001, 1);
  freqWheelY->setValue(3.2);
  view->setZoomFactorY(3.2);
  QToolTip::add(freqWheelY, "Zoom pitch contour view vertically");
  topRightLayout->addSpacing(20);
  topRightLayout->addWidget(freqWheelY, 0);
  
  //Create the vertical scrollbar
  //vScrollBar = new MyScrollBar(0, toInt((view->topNote()-view->viewHeight())*view->stepY()), 20, toInt(view->viewHeight()*view->stepY()), toInt(view->viewBottom()*view->stepY()), Qt::Vertical, this);
  //vScrollBar = new MyScrollBar(0, gdata->topNote()-view->viewHeight(), 20, view->viewHeight(), view->viewBottom(), 20, Qt::Vertical, this);
  freqScrollBar = new MyScrollBar(0, gdata->topNote()-view->viewHeight(), 0.5, view->viewHeight(), view->viewBottom(), 20, Qt::Vertical, topWidget);
  topRightLayout->addWidget(freqScrollBar, 4);
/*
  QPushButton *buttonPlusY = new QPushButton("+", topSplitter);
  buttonPlusY->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonPlusY->setFixedSize(15, 15);
  QToolTip::add(buttonPlusY, "Zoom in vertically");
  QPushButton *buttonMinusY = new QPushButton("-", topSplitter);
  buttonMinusY->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonMinusY->setFixedSize(15, 15);
  QToolTip::add(buttonMinusY, "Zoom out vertically");
*/  
  

  //------------bottom half------------------
  
  QWidget *bottomWidget = new QWidget(splitter);
  QBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);
  QBoxLayout *bottomTopLayout = new QHBoxLayout(bottomLayout);
  
  QGrid *amplitudeFrame = new QGrid(1, bottomWidget);
  amplitudeFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  amplitudeWidget = new AmplitudeWidget(amplitudeFrame);
  bottomTopLayout->addWidget(amplitudeFrame);
  
  QBoxLayout *bottomTopRightLayout = new QVBoxLayout(bottomTopLayout);
  
  amplitudeWheelY = new QwtWheel(bottomWidget);
  amplitudeWheelY->setOrientation(Qt::Vertical);
  amplitudeWheelY->setWheelWidth(14);
  amplitudeWheelY->setRange(30, 150, 0.1, 1);
  amplitudeWheelY->setValue(amplitudeWidget->dBRange());
  QToolTip::add(amplitudeWheelY, "Zoom pitch contour view vertically");
  bottomTopRightLayout->addWidget(amplitudeWheelY, 0);
  
  //Create the vertical scrollbar
  amplitudeScrollBar = new MyScrollBar(0, 150, 20, 80, 0, 20, Qt::Vertical, bottomWidget);
  bottomTopRightLayout->addWidget(amplitudeScrollBar, 4);
  
  QBoxLayout *bottomBottomLayout = new QHBoxLayout(bottomLayout);
  
  QSpinBox *thresholdSpinBox = new QSpinBox(80, 100, 1, bottomWidget, "thresholdSpinBox");
  thresholdSpinBox->setSuffix("%");
  thresholdSpinBox->setFocusPolicy(QWidget::NoFocus);
  thresholdSpinBox->setValue(gdata->getActiveIntThreshold());
  QToolTip::add(thresholdSpinBox, "Threshold used in choosing the octave");
  connect(thresholdSpinBox, SIGNAL(valueChanged(int)), gdata, SLOT(resetActiveIntThreshold(int)));
  connect(thresholdSpinBox, SIGNAL(valueChanged(int)), freqDrawWidget, SLOT(update()));
  connect(gdata, SIGNAL(activeIntThresholdChanged(int)), thresholdSpinBox, SLOT(setValue(int)));
  
  QComboBox *amplitudeModeComboBox = new QComboBox(bottomWidget, "amplitudeTypeModeBox");
  static const char* amplitudeModeItems[] = { "RMS (Log)", "Peak Amplitude (Log)", "Peak Correlation", "Purity", 0 };
  amplitudeModeComboBox->insertStrList(amplitudeModeItems);
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setAmplitudeMode(int)));
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), amplitudeWidget, SLOT(update()));
  
  QComboBox *pitchContourModeComboBox = new QComboBox(bottomWidget, "pitchContourModeComboBox");
  static const char* pitchContourModeItems[] = { "Clarity fading", "Note grouping", 0 };
  pitchContourModeComboBox->insertStrList(pitchContourModeItems);
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setPitchContourMode(int)));
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), freqDrawWidget, SLOT(update()));
  
  freqWheelX = new QwtWheel(bottomWidget);
  freqWheelX->setOrientation(Qt::Horizontal);
  freqWheelX->setWheelWidth(16);
  freqWheelX->setRange(0.5, 9.0, 0.001, 1);
  freqWheelX->setValue(2.0);
  QToolTip::add(freqWheelX, "Zoom horizontally");
  
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(thresholdSpinBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(amplitudeModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(pitchContourModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(freqWheelX, 1);
  bottomBottomLayout->addSpacing(16);
 

  
  //Create the horizontal scrollbar and +/- buttons
  //hScrollBar = new MyScrollBar(gdata->leftTime(), gdata->rightTime(), 0.1, view->viewWidth(), view->currentTime(), 10000, Qt::Horizontal, this);
/*  
  QPushButton *buttonPlusX = new QPushButton("+", this);
  buttonPlusX->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonPlusX->setFixedSize(15, 15);
  QToolTip::add(buttonPlusX, "Zoom in horizontal");
  QPushButton *buttonMinusX = new QPushButton("-", this);
  buttonMinusX->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  buttonMinusX->setFixedSize(15, 15);
  QToolTip::add(buttonMinusX, "Zoom out horizontal");
*/

  //Create the resize grip. The thing in the bottom right hand corner
  QSizeGrip *sizeGrip = new QSizeGrip(bottomWidget);
  sizeGrip->setFixedSize(15, 15);
  //sizeGrip->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
  sizeGrip->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, false));
  bottomBottomLayout->addWidget(sizeGrip);
  
  //Actually add all the widgets into the widget layout
  //topLayout->addWidget(timeAxis);
  mainLayout->addWidget(splitter);
  //rightLayout->addWidget(buttonMinusY);
  //rightLayout->addWidget(buttonPlusY);
  //rightLayout->addWidget(vScrollBar);
  //bottomLayout->addWidget(buttonMinusX);
  //bottomLayout->addWidget(buttonPlusX);
  //mainLayout->addWidget(sizeGrip, 1, 1);
      
  //Setup all the signals and slots
  //vertical
  connect(freqScrollBar, SIGNAL(sliderMoved(double)), view, SLOT(changeViewY(double)));
  connect(freqScrollBar, SIGNAL(sliderMoved(double)), view, SLOT(doSlowUpdate()));
  
  //connect(buttonPlusY, SIGNAL(clicked()), view, SLOT(viewZoomInY()));
  //connect(buttonMinusY, SIGNAL(clicked()), view, SLOT(viewZoomOutY()));
  connect(view, SIGNAL(viewBottomChanged(double)), freqScrollBar, SLOT(setValue(double)));
  connect(view, SIGNAL(scrollableYChanged(double)), freqScrollBar, SLOT(setMaxValue(double)));
  connect(view, SIGNAL(viewHeightChanged(double)), freqScrollBar, SLOT(setPageStep(double)));

  connect(freqWheelY, SIGNAL(valueChanged(double)), view, SLOT(setZoomFactorY(double)));
  connect(view, SIGNAL(logZoomYChanged(double)), freqWheelY, SLOT(setValue(double)));
  //connect(view, SIGNAL(logZoomYChanged(double)), freqDrawWidget, SLOT(update()));
  
  //horizontal
  connect(freqWheelX, SIGNAL(valueChanged(double)), view, SLOT(setZoomFactorX(double)));
  connect(view, SIGNAL(logZoomXChanged(double)), freqWheelX, SLOT(setValue(double)));
  //connect(view, SIGNAL(logZoomXChanged(double)), view, SLOT(doSlowUpdate()));
  //connect(buttonPlusX, SIGNAL(clicked()), view, SLOT(viewZoomInX()));
  //connect(buttonMinusX, SIGNAL(clicked()), view, SLOT(viewZoomOutX()));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(setDBRange(double)));
  //connect(amplitudeWidget, SIGNAL(dBRangeChanged(double)), amplitudeScrollBar, SLOT(setPageStep(double)));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(update()));
/*
  connect(hScrollBar, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  connect(hScrollBar, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(gdata, SIGNAL(leftTimeChanged(double)), hScrollBar, SLOT(setMinValue(double)));
  connect(gdata, SIGNAL(rightTimeChanged(double)), hScrollBar, SLOT(setMaxValue(double)));
  connect(view, SIGNAL(currentTimeChanged(double)), hScrollBar, SLOT(setValue(double)));
  connect(view, SIGNAL(viewWidthChanged(double)), hScrollBar, SLOT(setPageStep(double)));
*/
  //make the widgets get updated when the view changes
  //connect(gdata->view, SIGNAL(onSlowUpdate()), freqDrawWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), freqDrawWidget, SLOT(repaint()));
  //connect(gdata->view, SIGNAL(onSlowUpdate()), amplitudeWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), amplitudeWidget, SLOT(repaint()));
  //connect(gdata->view, SIGNAL(onSlowUpdate()), timeAxis, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), timeAxis, SLOT(repaint()));
  connect(gdata->view, SIGNAL(timeViewRangeChanged(double, double)), timeAxis, SLOT(setRange(double, double)));
}

FreqView::~FreqView()
{
  //Qt deletes the child widgets automatically
}

void FreqView::zoomIn()
{
  bool doneIt = false;
  if(gdata->running != STREAM_FORWARD) {
    if(freqDrawWidget->hasMouse()) {
      QPoint mousePos = freqDrawWidget->mapFromGlobal(QCursor::pos());
      gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1, mousePos.x());
      gdata->view->setZoomFactorY(gdata->view->logZoomY() + 0.1, freqDrawWidget->height() - mousePos.y());
      doneIt = true;
    } else if(amplitudeWidget->hasMouse()) {
      QPoint mousePos = amplitudeWidget->mapFromGlobal(QCursor::pos());
      gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1, mousePos.x());
      doneIt = true;
    }
  }
  if(!doneIt) {
    gdata->view->setZoomFactorX(gdata->view->logZoomX() + 0.1);
    gdata->view->setZoomFactorY(gdata->view->logZoomY() + 0.1);
    doneIt = true;
  }
}

void FreqView::zoomOut()
{
  gdata->view->setZoomFactorX(gdata->view->logZoomX() - 0.1);
  if(!amplitudeWidget->hasMouse()) {
    gdata->view->setZoomFactorY(gdata->view->logZoomY() - 0.1);
  }
}

