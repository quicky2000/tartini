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
#include <QFrame>
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QResizeEvent>

//------------------------------------------------------------------------------
WaveView::WaveView(int p_view_ID
                  ,QWidget * p_parent
                  )
: ViewWidget(p_view_ID, p_parent)
{
    QHBoxLayout * l_main_layout = new QHBoxLayout(this);

    QFrame * l_wave_frame = new QFrame(this);
    l_wave_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    m_wave_widget = new WaveWidget(l_wave_frame);
    l_main_layout->addWidget(l_wave_frame);

    QVBoxLayout * l_right_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_right_layout);

    QwtWheel * l_freq_wheel_Y = new QwtWheel(this);
    l_freq_wheel_Y->setOrientation(Qt::Vertical);
    l_freq_wheel_Y->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
    l_freq_wheel_Y->setRange(1.0, 20.0);
    l_freq_wheel_Y->setSingleStep(0.1);
    // Multiplicator value is 10 = 1 / 0.1
    l_freq_wheel_Y->setPageStepCount(10);
#else
    l_freq_wheel_Y->setRange(1.0, 20.0, 0.1, 1);
#endif // QWT_VERSION >= 0x060000

    l_freq_wheel_Y->setValue(1.0);
    l_freq_wheel_Y->setToolTip("Zoom oscilloscope vertically");
    l_right_layout->addWidget(l_freq_wheel_Y, 1);
    l_right_layout->addStretch(2);

    connect(l_freq_wheel_Y, SIGNAL(valueChanged(double)), m_wave_widget, SLOT(setZoomY(const double &)));
    connect(m_wave_widget, SIGNAL(zoomYChanged(double)), m_wave_widget, SLOT(update()));

    //make the widget get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_wave_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
WaveView::~WaveView()
{
}

//------------------------------------------------------------------------------
void WaveView::resizeEvent(QResizeEvent *)
{
    m_wave_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize WaveView::sizeHint() const
{
    return QSize(400, 128);
}

// EOF
