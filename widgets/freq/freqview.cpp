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
#include "freqwidgetGL.h"
#include "amplitudewidget.h"
#include "timeaxis.h"
#include "mainwindow.h"
#include "analysisdata.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qsizegrip.h>
#include <qsplitter.h>
#include <qtooltip.h>
#include <q3grid.h>
#include <qwt_wheel.h>
#include <qcursor.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <Q3Frame>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//------------------------------------------------------------------------------
FreqView::FreqView(int viewID_, QWidget *parent):
  ViewWidget(viewID_, parent)
{
  View & view = gdata->getView();

  Q3BoxLayout *mainLayout = new Q3VBoxLayout(this);
  mainLayout->setResizeMode(QLayout::SetNoConstraint);


  QSplitter * splitter = new QSplitter(Qt::Vertical, this);
  QWidget * topWidget = new QWidget(splitter);
  Q3BoxLayout * topLayout = new Q3HBoxLayout(topWidget);
  Q3BoxLayout * topLeftLayout = new Q3VBoxLayout(topLayout);
  
  timeAxis = new TimeAxis(topWidget, gdata->leftTime(), gdata->rightTime(), true);
  timeAxis->setWhatsThis("The time in seconds");
  topLeftLayout->addWidget(timeAxis);
  
  QFrame * freqFrame = new QFrame;
  freqFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout *freqFrameLayout = new QVBoxLayout;
  freqWidgetGL = new FreqWidgetGL(NULL);
  freqWidgetGL->setWhatsThis("The line represents the musical pitch of the sound. A higher pitch moves up, with note names shown at the left, with octave numbers. "
    "The black vertical line shows the current time. This line's position can be moved. "
    "Pitch-lines are drawn connected only because they change a small amount over a small time step. "
    "Note: This may cause semi-tone note changes appear to be joined. Clicking the background and dragging moves the view. "
    "Clicking a pitch-line selects it. Mouse-Wheel scrolls. Shift-Mouse-Wheel zooms");
  freqFrameLayout->addWidget(freqWidgetGL);
  freqFrameLayout->setMargin(0);
  freqFrameLayout->setSpacing(0);
  freqFrame->setLayout(freqFrameLayout);
  topLeftLayout->addWidget(freqFrame);


  QVBoxLayout * topRightLayout = new QVBoxLayout();
  
  freqWheelY = new QwtWheel(topWidget);
  freqWheelY->setOrientation(Qt::Vertical);
  freqWheelY->setWheelWidth(14);
  freqWheelY->setRange(1.6, 5.0, 0.001, 1);
  freqWheelY->setValue(view.logZoomY());
  QToolTip::add(freqWheelY, "Zoom pitch contour view vertically");
  topRightLayout->addSpacing(20);
  topRightLayout->addWidget(freqWheelY, 0);
  
  //Create the vertical scrollbar
  freqScrollBar = new MyScrollBar(0, gdata->topPitch() - view.viewHeight(), 0.5, view.viewHeight(), 0, 20, Qt::Vertical, topWidget);
  freqScrollBar->setValue(gdata->topPitch() - view.viewHeight() - view.viewBottom());
  topRightLayout->addWidget(freqScrollBar, 4);

  topLayout->addLayout(topRightLayout);

  //------------bottom half------------------
  
  QWidget * bottomWidget = new QWidget(splitter);
  Q3BoxLayout * bottomLayout = new Q3VBoxLayout(bottomWidget);
  Q3BoxLayout * bottomTopLayout = new Q3HBoxLayout(bottomLayout);
  
  QFrame * amplitudeFrame = new QFrame;
  amplitudeFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  QVBoxLayout * amplitudeFrameLayout = new QVBoxLayout;
  amplitudeWidget = new AmplitudeWidget(NULL);
  amplitudeWidget->setWhatsThis("Shows the volume (or other parameters) at time lined up with the pitch above. Note: You can move the lines to change some thresholds.");
  amplitudeFrameLayout->addWidget(amplitudeWidget);
  amplitudeFrameLayout->setMargin(0);
  amplitudeFrameLayout->setSpacing(0);
  amplitudeFrame->setLayout(amplitudeFrameLayout);
  bottomTopLayout->addWidget(amplitudeFrame);

  Q3BoxLayout *bottomTopRightLayout = new Q3VBoxLayout(bottomTopLayout);
  
  amplitudeWheelY = new QwtWheel(bottomWidget);
  amplitudeWheelY->setOrientation(Qt::Vertical);
  amplitudeWheelY->setWheelWidth(14);
  amplitudeWheelY->setRange(0.2, 1.00, 0.01, 1);
  amplitudeWheelY->setValue(amplitudeWidget->range());
  QToolTip::add(amplitudeWheelY, "Zoom pitch contour view vertically");
  bottomTopRightLayout->addWidget(amplitudeWheelY, 0);
  
  //Create the vertical scrollbar
  amplitudeScrollBar = new MyScrollBar(0.0, 1.0 - amplitudeWidget->range(), 0.20, amplitudeWidget->range(), 0, 20, Qt::Vertical, bottomWidget);
  bottomTopRightLayout->addWidget(amplitudeScrollBar, 4);
  
  Q3BoxLayout * bottomBottomLayout = new Q3HBoxLayout(bottomLayout);
  
  QComboBox * amplitudeModeComboBox = new QComboBox(bottomWidget, "amplitudeTypeModeBox");
  amplitudeModeComboBox->setWhatsThis("Select different algorithm parameters to view in the bottom pannel");
  int j;
  QStringList s;
  for(j = 0; j < NUM_AMP_MODES; j++)
    {
      s << amp_mode_names[j];
    }
  amplitudeModeComboBox->addItems(s);
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setAmplitudeMode(int)));
  connect(amplitudeModeComboBox, SIGNAL(activated(int)), amplitudeWidget, SLOT(update()));

  QComboBox * pitchContourModeComboBox = new QComboBox(bottomWidget, "pitchContourModeComboBox");
  pitchContourModeComboBox->setWhatsThis("Select whether the Pitch Contour line fades in/out with clarity/loudness of the sound or is a solid dark line");
  s.clear();
  s << "Clarity fading" << "Note grouping";
  pitchContourModeComboBox->addItems(s);
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), gdata, SLOT(setPitchContourMode(int)));
  connect(pitchContourModeComboBox, SIGNAL(activated(int)), freqWidgetGL, SLOT(update()));

  freqWheelX = new QwtWheel(bottomWidget);
  freqWheelX->setOrientation(Qt::Horizontal);
  freqWheelX->setWheelWidth(16);
  freqWheelX->setRange(0.5, 9.0, 0.001, 1);
  freqWheelX->setValue(2.0);
  QToolTip::add(freqWheelX, "Zoom horizontally");
  
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(amplitudeModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(pitchContourModeComboBox, 0);
  bottomBottomLayout->addStretch(2);
  bottomBottomLayout->addWidget(freqWheelX, 1);
  bottomBottomLayout->addSpacing(16);

  //Create the resize grip. The thing in the bottom right hand corner
  QSizeGrip * sizeGrip = new QSizeGrip(bottomWidget);
  sizeGrip->setFixedSize(15, 15);
  sizeGrip->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, false));
  bottomBottomLayout->addWidget(sizeGrip);
  
  //Actually add all the widgets into the widget layout
  mainLayout->addWidget(splitter);

  //Setup all the signals and slots
  //vertical

  connect(freqScrollBar, SIGNAL(sliderMoved(double)), &view, SLOT(changeViewY(double)));
  connect(freqScrollBar, SIGNAL(sliderMoved(double)), &view, SLOT(doSlowUpdate()));

  connect(&view, SIGNAL(viewBottomChanged(double)), freqScrollBar, SLOT(setValue(double)));
  connect(freqWheelY, SIGNAL(valueChanged(double)), &view, SLOT(setZoomFactorY(double)));
  connect(&view, SIGNAL(logZoomYChanged(double)), freqWheelY, SLOT(setValue(double)));
  
  //horizontal
  connect(freqWheelX, SIGNAL(valueChanged(double)), &view, SLOT(setZoomFactorX(double)));
  connect(&view, SIGNAL(logZoomXChanged(double)), freqWheelX, SLOT(setValue(double)));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(setRange(double)));
  connect(amplitudeWheelY, SIGNAL(valueChanged(double)), amplitudeWidget, SLOT(update()));

  connect(amplitudeScrollBar, SIGNAL(sliderMoved(double)), amplitudeWidget, SLOT(setOffset(double)));
  connect(amplitudeScrollBar, SIGNAL(sliderMoved(double)), amplitudeWidget, SLOT(update()));

  connect(amplitudeWidget, SIGNAL(rangeChanged(double)), this, SLOT(setAmplitudeZoom(double)));
  connect(amplitudeWidget, SIGNAL(rangeChanged(double)), amplitudeWheelY, SLOT(setValue(double)));
  connect(amplitudeWidget, SIGNAL(offsetChanged(double)), amplitudeScrollBar, SLOT(setValue(double)));

  //make the widgets get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), freqWidgetGL, SLOT(update()));
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), amplitudeWidget, SLOT(update()));
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), timeAxis, SLOT(update()));
  connect(&(gdata->getView()), SIGNAL(timeViewRangeChanged(double, double)), timeAxis, SLOT(setRange(double, double)));
}

//------------------------------------------------------------------------------
FreqView::~FreqView(void)
{
  //Qt deletes the child widgets automatically
}

//------------------------------------------------------------------------------
void FreqView::zoomIn(void)
{
  bool doneIt = false;
  if(gdata->getRunning() != STREAM_FORWARD)
    {
      if(freqWidgetGL->hasMouse())
	{
	  QPoint mousePos = freqWidgetGL->mapFromGlobal(QCursor::pos());
	  gdata->getView().setZoomFactorX(gdata->getView().logZoomX() + 0.1, mousePos.x());
	  gdata->getView().setZoomFactorY(gdata->getView().logZoomY() + 0.1, freqWidgetGL->height() - mousePos.y());
	  doneIt = true;
	}
      else if(amplitudeWidget->hasMouse())
	{
	  QPoint mousePos = amplitudeWidget->mapFromGlobal(QCursor::pos());
	  gdata->getView().setZoomFactorX(gdata->getView().logZoomX() + 0.1, mousePos.x());
	  doneIt = true;
	}
    }
  if(!doneIt)
    {
      gdata->getView().setZoomFactorX(gdata->getView().logZoomX() + 0.1);
      gdata->getView().setZoomFactorY(gdata->getView().logZoomY() + 0.1);
      doneIt = true;
    }
}

//------------------------------------------------------------------------------
void FreqView::zoomOut(void)
{
  gdata->getView().setZoomFactorX(gdata->getView().logZoomX() - 0.1);
  if(!amplitudeWidget->hasMouse())
    {
      gdata->getView().setZoomFactorY(gdata->getView().logZoomY() - 0.1);
    }
}

//------------------------------------------------------------------------------
void FreqView::setAmplitudeZoom(double newRange)
{
  amplitudeScrollBar->setRange(0.0, 1.0-newRange);
  amplitudeScrollBar->setPageStep(newRange);
}

//------------------------------------------------------------------------------
QSize FreqView::sizeHint(void) const
{
  return QSize(600, 560);
}
// EOF
