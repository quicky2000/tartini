/***************************************************************************
                          htrackview.cpp  -  description
                             -------------------
    begin                : 29 Mar 2005
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
#include "htrackview.h"
#include "htrackwidget.h"
#include "gdata.h"

#include <qlayout.h>
#include <qslider.h>
#include <qwt_wheel.h>
#include <qsizegrip.h>
#include <q3grid.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3Frame>
#include <QResizeEvent>

//------------------------------------------------------------------------------
HTrackView::HTrackView( int p_view_ID
                      , QWidget *p_parent
                      )
: ViewWidget( p_view_ID, p_parent)
{
    Q3GridLayout * l_main_layout = new Q3GridLayout(this, 2, 2);
    l_main_layout->setResizeMode(QLayout::SetNoConstraint);
    Q3BoxLayout * l_right_layout = new Q3VBoxLayout();
    Q3BoxLayout * l_bottom_layout = new Q3HBoxLayout();

    Q3Grid * l_frame = new Q3Grid(1, this);
    l_frame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    QWidget * l_a_widget = new QWidget(l_frame);
    m_h_track_widget = new HTrackWidget(l_a_widget);
    m_h_track_widget->setWhatsThis("Shows a 3D keyboard with the current note coloured. "
    "Vertical columns (or tracks), each representing a harmonic (or component frequency), protrude from the back, and move further away over time. "
    "The height of each track is related to how much energy is at that frequency. "
    "Tracks alternate in colour for better visibility. It can be seen how the hamonics in a note fit into the musical scale.");

    m_peak_threshold_slider = new QSlider(0, 100, 10, 5, Qt::Vertical, this);
    QToolTip::add(m_peak_threshold_slider, "Thresholding of harmonics");

    m_rotate_X_wheel = new QwtWheel(this);
    m_rotate_X_wheel->setWheelWidth(20);
    m_rotate_X_wheel->setRange(-180, 180, 0.1, 1);
    QToolTip::add(m_rotate_X_wheel, "Rotate piano horizonally");

    m_rotate_Y_wheel = new QwtWheel(this);
    m_rotate_Y_wheel->setOrientation(Qt::Vertical);
    m_rotate_Y_wheel->setWheelWidth(20);
    m_rotate_Y_wheel->setRange(-90, 0, 0.1, 1);
    QToolTip::add(m_rotate_Y_wheel, "Rotate piano vertically");

    m_distance_wheel = new QwtWheel(this);
    m_distance_wheel->setOrientation(Qt::Vertical);
    m_distance_wheel->setRange(100, 5000, 10, 20);
    m_distance_wheel->setTotalAngle(20 * 360);
    QToolTip::add(m_distance_wheel, "Move towards/away from piano");

    QPushButton * l_home_button = new QPushButton("Reset", this, "homebutton");
    QToolTip::add(l_home_button, "Return to the original view");
  
    QSizeGrip * l_size_grip = new QSizeGrip(this);
  
    l_main_layout->addWidget(l_frame, 0, 0);
    l_main_layout->addLayout(l_bottom_layout, 1, 0);
    l_main_layout->addLayout(l_right_layout, 0, 1);
    l_right_layout->addStretch(2);
    l_right_layout->addWidget(m_peak_threshold_slider);
    l_right_layout->addStretch(4);
    l_right_layout->addWidget(m_rotate_Y_wheel);
    l_right_layout->addSpacing(14);
    l_right_layout->addWidget(m_distance_wheel);
    l_bottom_layout->addStretch(0);
    l_bottom_layout->addWidget(l_home_button);
    l_bottom_layout->addSpacing(14);
    l_bottom_layout->addWidget(m_rotate_X_wheel);
    l_main_layout->addWidget(l_size_grip, 1, 1);

    //make the widget get updated when the view changes
    connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), m_h_track_widget, SLOT(update()));
    connect(m_peak_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(setPeakThreshold(int)));
    connect(m_rotate_Y_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setViewAngleVertical(double)));
    connect(m_rotate_Y_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    connect(m_h_track_widget, SIGNAL(viewAngleVerticalChanged(double)), m_rotate_Y_wheel, SLOT(setValue(double)));
    connect(m_distance_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setDistanceAway(double)));
    connect(m_distance_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    connect(m_h_track_widget, SIGNAL(distanceAwayChanged(double)), m_distance_wheel, SLOT(setValue(double)));
    connect(m_rotate_X_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setViewAngleHorizontal(double)));
    connect(m_rotate_X_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    connect(m_h_track_widget, SIGNAL(viewAngleHorizontalChanged(double)), m_rotate_X_wheel, SLOT(setValue(double)));
    connect(l_home_button, SIGNAL(clicked()), m_h_track_widget, SLOT(home()));
}

//------------------------------------------------------------------------------
HTrackView::~HTrackView(void)
{
    delete m_h_track_widget;
}

//------------------------------------------------------------------------------
void HTrackView::resizeEvent(QResizeEvent *)
{
    m_h_track_widget->resize(size());
}

//------------------------------------------------------------------------------
void HTrackView::setPeakThreshold(int value)
{
    m_h_track_widget->setPeakThreshold(float(value) / 100.0);
    m_h_track_widget->update();
}

//------------------------------------------------------------------------------
QSize HTrackView::sizeHint(void) const
{
    return QSize(500, 400);
}

// EOF
