/***************************************************************************
                          hstackview.cpp  -  description
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
#include "hcircleview.h"
#include "hcirclewidget.h"
#include "channel.h" 
#include "analysisdata.h"
#include "useful.h" 
#include "myscrollbar.h"

#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QSplitter>
#include <QToolTip>
#include <qwt_wheel.h>
#include <QCursor>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

//------------------------------------------------------------------------------
HCircleView::HCircleView( int p_view_id
                        , QWidget *p_parent
                        )
: ViewWidget( p_view_id, p_parent)
{
    setCaption("Harmonic Circle");

    QHBoxLayout * l_main_layout = new QHBoxLayout(this);

    QVBoxLayout * l_left_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_left_layout);
    QVBoxLayout * l_right_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_right_layout);

    QFrame * l_wave_frame = new QFrame(this);
    l_wave_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    l_left_layout->addWidget(l_wave_frame);

    m_h_circle_widget = new HCircleWidget(l_wave_frame);

    QHBoxLayout * l_bottom_layout = new QHBoxLayout();
    l_left_layout->addLayout(l_bottom_layout);
 
    QwtWheel * l_zoom_wheel = new QwtWheel(this);
    l_zoom_wheel->setOrientation(Qt::Vertical);
    l_zoom_wheel->setWheelWidth(14);
    l_zoom_wheel->setRange(0.001, 0.1, 0.001, 1);
    l_zoom_wheel->setValue(0.007);
    m_h_circle_widget->setZoom(0.007);
    l_zoom_wheel->setToolTip("Zoom in or out");
    l_right_layout->addWidget(l_zoom_wheel);

    QwtWheel * l_lowest_value_wheel = new QwtWheel(this);
    l_lowest_value_wheel->setOrientation(Qt::Vertical);
    l_lowest_value_wheel->setWheelWidth(14);
    l_lowest_value_wheel->setRange(-160, 10, 0.01, 1);
    l_lowest_value_wheel->setValue(-100);
    m_h_circle_widget->setLowestValue(-100);
    l_lowest_value_wheel->setToolTip("Change the lowest value");
    l_right_layout->addWidget(l_lowest_value_wheel);
    l_right_layout->addStretch(2);

    QwtWheel* l_threshold_wheel = new QwtWheel(this);
    l_threshold_wheel->setOrientation(Qt::Horizontal);
    l_threshold_wheel->setWheelWidth(14);
    l_threshold_wheel->setRange(-160, 10, 0.01, 1);
    l_threshold_wheel->setValue(-100);
    m_h_circle_widget->setThreshold(-100);
    l_threshold_wheel->setToolTip("Change the harmonic threshold");
    l_bottom_layout->addWidget(l_threshold_wheel);
    l_bottom_layout->addStretch(2);

    connect(l_zoom_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(setZoom(double)));
    connect(l_zoom_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(update()));

    connect(l_lowest_value_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(setLowestValue(double)));
    connect(l_lowest_value_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(update()));

    connect(l_threshold_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(setThreshold(double)));
    connect(l_threshold_wheel, SIGNAL(valueChanged(double)), m_h_circle_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
HCircleView::~HCircleView(void)
{
    delete m_h_circle_widget;
}

//------------------------------------------------------------------------------
QSize HCircleView::sizeHint(void) const
{
    return QSize(300, 200);
}

// EOF
