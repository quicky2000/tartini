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
WaveView::WaveView(int p_view_ID
                  ,QWidget * p_parent
                  )
: ViewWidget(p_view_ID, p_parent)
{
    Q3BoxLayout * l_main_layout = new Q3HBoxLayout(this);

    Q3Grid * l_wave_frame = new Q3Grid(1, this);
    l_wave_frame->setFrameStyle(Q3Frame::WinPanel | Q3Frame::Sunken);
    m_wave_widget = new WaveWidget(l_wave_frame);
    l_main_layout->addWidget(l_wave_frame);

    Q3BoxLayout * l_right_layout = new Q3VBoxLayout(l_main_layout);

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
    QToolTip::add(l_freq_wheel_Y, "Zoom oscilloscope vertically");
    l_right_layout->addWidget(l_freq_wheel_Y, 1);
    l_right_layout->addStretch(2);

    connect(l_freq_wheel_Y, SIGNAL(valueChanged(double)), m_wave_widget, SLOT(setZoomY(const double &)));
    connect(m_wave_widget, SIGNAL(zoomYChanged(double)), m_wave_widget, SLOT(update()));

    //make the widget get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_wave_widget, SLOT(update()));
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
