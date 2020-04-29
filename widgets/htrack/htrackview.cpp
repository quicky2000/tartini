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

#include <QLayout>
#include <QSlider>
#include <qwt_wheel.h>
#include <QSizeGrip>
#include <QToolTip>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QResizeEvent>

//------------------------------------------------------------------------------
HTrackView::HTrackView( int p_view_ID
                      , QWidget *p_parent
                      )
: ViewWidget( p_view_ID, p_parent)
{
    QGridLayout * l_main_layout = new QGridLayout(this);
    l_main_layout->setSizeConstraint(QLayout::SetNoConstraint);
    QVBoxLayout * l_right_layout = new QVBoxLayout();
    QHBoxLayout * l_bottom_layout = new QHBoxLayout();

    QFrame * l_frame = new QFrame(this);
    l_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QWidget * l_a_widget = new QWidget(l_frame);
    m_h_track_widget = new HTrackWidget(l_a_widget);
    m_h_track_widget->setWhatsThis("Shows a 3D keyboard with the current note coloured. "
    "Vertical columns (or tracks), each representing a harmonic (or component frequency), protrude from the back, and move further away over time. "
    "The height of each track is related to how much energy is at that frequency. "
    "Tracks alternate in colour for better visibility. It can be seen how the hamonics in a note fit into the musical scale.");

    m_peak_threshold_slider = new QSlider(Qt::Vertical, this);
    m_peak_threshold_slider->setRange(0, 100);
    m_peak_threshold_slider->setPageStep(10);
    m_peak_threshold_slider->setValue(5);
    m_peak_threshold_slider->setToolTip("Thresholding of harmonics");

    m_rotate_X_wheel = new QwtWheel(this);
    m_rotate_X_wheel->setWheelWidth(20);
#if QWT_VERSION >= 0x060000
    m_rotate_X_wheel->setRange(-180, 180);
    m_rotate_X_wheel->setSingleStep(0.1);
    // Multiplicator value is 10 = 1 / 0.1
    m_rotate_X_wheel->setPageStepCount(10);
    m_rotate_X_wheel->setMass(0.1);
#else
    m_rotate_X_wheel->setRange(-180, 180, 0.1, 1);
#endif // QWT_VERSION >= 0x060000
    m_rotate_X_wheel->setToolTip("Rotate piano horizonally");

    m_rotate_Y_wheel = new QwtWheel(this);
    m_rotate_Y_wheel->setOrientation(Qt::Vertical);
    m_rotate_Y_wheel->setWheelWidth(20);
#if QWT_VERSION >= 0x060000
    m_rotate_Y_wheel->setRange(-90, 0);
    m_rotate_Y_wheel->setSingleStep(0.1);
    // Multiplicator value is 10 = 1 / 0.1
    m_rotate_Y_wheel->setPageStepCount(10);
    m_rotate_Y_wheel->setMass(0.1);
#else
    m_rotate_Y_wheel->setRange(-90, 0, 0.1, 1);
#endif // QWT_VERSION >= 0x060000
    m_rotate_Y_wheel->setToolTip("Rotate piano vertically");

    m_distance_wheel = new QwtWheel(this);
    m_distance_wheel->setOrientation(Qt::Vertical);
#if QWT_VERSION >= 0x060000
    m_distance_wheel->setRange(100, 5000);
    m_distance_wheel->setSingleStep(10);
    // Multiplicator value is 2 = 20 / 10
    m_distance_wheel->setPageStepCount(2);
    m_distance_wheel->setMass(0.1);
#else
    m_distance_wheel->setRange(100, 5000, 10, 20);
#endif // QWT_VERSION >= 0x060000
    m_distance_wheel->setTotalAngle(20 * 360);
    m_distance_wheel->setToolTip("Move towards/away from piano");

    QPushButton * l_home_button = new QPushButton("Reset", this);
    l_home_button->setObjectName("homebutton");
    l_home_button->setToolTip("Return to the original view");
  
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
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_h_track_widget, SLOT(update()));
    connect(m_peak_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(setPeakThreshold(int)));
    connect(m_rotate_Y_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setViewAngleVertical(double)));
    connect(m_rotate_Y_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    // Connecting the wheel widget to the view and the view back to the wheel makes the wheel unresponsive.
    // Setting the value on the wheel interrupts the curerent interaction with the mouse, so the wheel only advances one tick.
    // connect(m_h_track_widget, SIGNAL(viewAngleVerticalChanged(double)), m_rotate_Y_wheel, SLOT(setValue(double)));
    connect(m_distance_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setDistanceAway(double)));
    connect(m_distance_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    // Connecting the wheel widget to the view and the view back to the wheel makes the wheel unresponsive.
    // Setting the value on the wheel interrupts the curerent interaction with the mouse, so the wheel only advances one tick.
    // connect(m_h_track_widget, SIGNAL(distanceAwayChanged(double)), m_distance_wheel, SLOT(setValue(double)));
    connect(m_rotate_X_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(setViewAngleHorizontal(double)));
    connect(m_rotate_X_wheel, SIGNAL(valueChanged(double)), m_h_track_widget, SLOT(update()));
    // Connecting the wheel widget to the view and the view back to the wheel makes the wheel unresponsive.
    // Setting the value on the wheel interrupts the curerent interaction with the mouse, so the wheel only advances one tick.
    // connect(m_h_track_widget, SIGNAL(viewAngleHorizontalChanged(double)), m_rotate_X_wheel, SLOT(setValue(double)));
    connect(l_home_button, SIGNAL(clicked()), m_h_track_widget, SLOT(home()));
}

//------------------------------------------------------------------------------
HTrackView::~HTrackView()
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
QSize HTrackView::sizeHint() const
{
    return QSize(500, 400);
}

// EOF
