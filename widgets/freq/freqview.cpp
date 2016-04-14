/***************************************************************************
                          freqview.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

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

#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QSplitter>
#include <QToolTip>
#include <QFrame>
#include <qwt_wheel.h>
#include <QCursor>
#include <QComboBox>
#include <QSpinBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>

//------------------------------------------------------------------------------
FreqView::FreqView( int p_view_id
                  , QWidget *p_parent
                  )
: ViewWidget(p_view_id, p_parent)
{
    View & l_view = g_data->getView();

    QVBoxLayout *l_main_layout = new QVBoxLayout(this);
    l_main_layout->setSizeConstraint(QLayout::SetNoConstraint);

    QSplitter * l_splitter = new QSplitter(Qt::Vertical, this);
    QWidget * l_top_widget = new QWidget(l_splitter);
    QHBoxLayout * l_top_layout = new QHBoxLayout(l_top_widget);
    QVBoxLayout * l_top_left_layout = new QVBoxLayout();
    l_top_layout->addLayout(l_top_left_layout);
  
    m_time_axis = new TimeAxis(l_top_widget, g_data->leftTime(), g_data->rightTime(), true);
    m_time_axis->setWhatsThis("The time in seconds");
    l_top_left_layout->addWidget(m_time_axis);

    QFrame * l_freq_frame = new QFrame;
    l_freq_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout *l_freq_frame_layout = new QVBoxLayout;
    m_freq_widget_GL = new FreqWidgetGL(NULL);
    m_freq_widget_GL->setWhatsThis("The line represents the musical pitch of the sound. A higher pitch moves up, with note names shown at the left, with octave numbers. "
    "The black vertical line shows the current time. This line's position can be moved. "
    "Pitch-lines are drawn connected only because they change a small amount over a small time step. "
    "Note: This may cause semi-tone note changes appear to be joined. Clicking the background and dragging moves the view. "
    "Clicking a pitch-line selects it. Mouse-Wheel scrolls. Shift-Mouse-Wheel zooms");
    l_freq_frame_layout->addWidget(m_freq_widget_GL);
    l_freq_frame_layout->setMargin(0);
    l_freq_frame_layout->setSpacing(0);
    l_freq_frame->setLayout(l_freq_frame_layout);
    l_top_left_layout->addWidget(l_freq_frame);

    QVBoxLayout * l_top_right_layout = new QVBoxLayout();

    m_freq_wheel_Y = new QwtWheel(l_top_widget);
    m_freq_wheel_Y->setOrientation(Qt::Vertical);
    m_freq_wheel_Y->setWheelWidth(14);
    m_freq_wheel_Y->setRange(1.6, 5.0, 0.001, 1);
    m_freq_wheel_Y->setValue(l_view.logZoomY());
    m_freq_wheel_Y->setToolTip("Zoom pitch contour view vertically");
    l_top_right_layout->addSpacing(20);
    l_top_right_layout->addWidget(m_freq_wheel_Y, 0);
  
    //Create the vertical scrollbar
    m_freq_scroll_bar = new MyScrollBar(0, g_data->topPitch() - l_view.viewHeight(), 0.5, l_view.viewHeight(), 0, 20, Qt::Vertical, l_top_widget);
    m_freq_scroll_bar->setValue(g_data->topPitch() - l_view.viewHeight() - l_view.viewBottom());
    l_top_right_layout->addWidget(m_freq_scroll_bar, 4);

    l_top_layout->addLayout(l_top_right_layout);

    //------------bottom half------------------
    QWidget * l_bottom_widget = new QWidget(l_splitter);
    QVBoxLayout * l_bottom_layout = new QVBoxLayout(l_bottom_widget);
    QHBoxLayout * l_bottom_top_layout = new QHBoxLayout();
    l_bottom_layout->addLayout(l_bottom_top_layout);

    QFrame * l_amplitude_frame = new QFrame;
    l_amplitude_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout * l_amplitude_frame_layout = new QVBoxLayout;
    m_amplitude_widget = new AmplitudeWidget(NULL);
    m_amplitude_widget->setWhatsThis("Shows the volume (or other parameters) at time lined up with the pitch above. Note: You can move the lines to change some thresholds.");
    l_amplitude_frame_layout->addWidget(m_amplitude_widget);
    l_amplitude_frame_layout->setMargin(0);
    l_amplitude_frame_layout->setSpacing(0);
    l_amplitude_frame->setLayout(l_amplitude_frame_layout);
    l_bottom_top_layout->addWidget(l_amplitude_frame);

    QVBoxLayout *l_bottom_top_right_layout = new QVBoxLayout();
    l_bottom_top_layout->addLayout(l_bottom_top_right_layout);

    m_amplitude_wheel_Y = new QwtWheel(l_bottom_widget);
    m_amplitude_wheel_Y->setOrientation(Qt::Vertical);
    m_amplitude_wheel_Y->setWheelWidth(14);
    m_amplitude_wheel_Y->setRange(0.2, 1.00, 0.01, 1);
    m_amplitude_wheel_Y->setValue(m_amplitude_widget->range());
    m_amplitude_wheel_Y->setToolTip("Zoom pitch contour view vertically");
    l_bottom_top_right_layout->addWidget(m_amplitude_wheel_Y, 0);

    //Create the vertical scrollbar
    m_amplitude_scroll_bar = new MyScrollBar(0.0, 1.0 - m_amplitude_widget->range(), 0.20, m_amplitude_widget->range(), 0, 20, Qt::Vertical, l_bottom_widget);
    l_bottom_top_right_layout->addWidget(m_amplitude_scroll_bar, 4);

    QHBoxLayout * l_bottom_bottom_layout = new QHBoxLayout();
    l_bottom_layout->addLayout(l_bottom_bottom_layout);

    QComboBox * l_amplitude_mode_combo_box = new QComboBox(l_bottom_widget);
    l_amplitude_mode_combo_box->setObjectName("amplitudeTypeModeBox");
    l_amplitude_mode_combo_box->setWhatsThis("Select different algorithm parameters to view in the bottom pannel");
    int l_j;
    QStringList l_string_list;
    for(l_j = 0; l_j < NUM_AMP_MODES; l_j++)
    {
        l_string_list << g_amp_mode_names[l_j];
    }
    l_amplitude_mode_combo_box->addItems(l_string_list);
    connect(l_amplitude_mode_combo_box, SIGNAL(activated(int)), g_data, SLOT(setAmplitudeMode(int)));
    connect(l_amplitude_mode_combo_box, SIGNAL(activated(int)), m_amplitude_widget, SLOT(update()));

    QComboBox * l_pitch_contour_mode_combo_box = new QComboBox(l_bottom_widget);
    l_pitch_contour_mode_combo_box->setObjectName("pitchContourModeComboBox");
    l_pitch_contour_mode_combo_box->setWhatsThis("Select whether the Pitch Contour line fades in/out with clarity/loudness of the sound or is a solid dark line");
    l_string_list.clear();
    l_string_list << "Clarity fading" << "Note grouping";
    l_pitch_contour_mode_combo_box->addItems(l_string_list);
    connect(l_pitch_contour_mode_combo_box, SIGNAL(activated(int)), g_data, SLOT(setPitchContourMode(int)));
    connect(l_pitch_contour_mode_combo_box, SIGNAL(activated(int)), m_freq_widget_GL, SLOT(update()));

    m_freq_wheel_X = new QwtWheel(l_bottom_widget);
    m_freq_wheel_X->setOrientation(Qt::Horizontal);
    m_freq_wheel_X->setWheelWidth(16);
    m_freq_wheel_X->setRange(0.5, 9.0, 0.001, 1);
    m_freq_wheel_X->setValue(2.0);
    m_freq_wheel_X->setToolTip("Zoom horizontally");

    l_bottom_bottom_layout->addStretch(2);
    l_bottom_bottom_layout->addWidget(l_amplitude_mode_combo_box, 0);
    l_bottom_bottom_layout->addStretch(2);
    l_bottom_bottom_layout->addWidget(l_pitch_contour_mode_combo_box, 0);
    l_bottom_bottom_layout->addStretch(2);
    l_bottom_bottom_layout->addWidget(m_freq_wheel_X, 1);
    l_bottom_bottom_layout->addSpacing(16);

    //Create the resize grip. The thing in the bottom right hand corner
    QSizeGrip * l_size_grip = new QSizeGrip(l_bottom_widget);
    l_size_grip->setFixedSize(15, 15);
    QSizePolicy l_size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    l_size_policy.setHeightForWidth(false);
    l_size_grip->setSizePolicy(l_size_policy);
    l_bottom_bottom_layout->addWidget(l_size_grip);

    //Actually add all the widgets into the widget layout
    l_main_layout->addWidget(l_splitter);

    //Setup all the signals and slots
    // vertical

    connect(m_freq_scroll_bar, SIGNAL(sliderMoved(double)), &l_view, SLOT(changeViewY(double)));
    connect(m_freq_scroll_bar, SIGNAL(sliderMoved(double)), &l_view, SLOT(doSlowUpdate()));

    connect(&l_view, SIGNAL(viewBottomChanged(double)), m_freq_scroll_bar, SLOT(setValue(double)));
    connect(m_freq_wheel_Y, SIGNAL(valueChanged(double)), &l_view, SLOT(setZoomFactorY(double)));
    connect(&l_view, SIGNAL(logZoomYChanged(double)), m_freq_wheel_Y, SLOT(setValue(double)));

    //horizontal
    connect(m_freq_wheel_X, SIGNAL(valueChanged(double)), &l_view, SLOT(setZoomFactorX(double)));
    connect(&l_view, SIGNAL(logZoomXChanged(double)), m_freq_wheel_X, SLOT(setValue(double)));
    connect(m_amplitude_wheel_Y, SIGNAL(valueChanged(double)), m_amplitude_widget, SLOT(setRange(double)));
    connect(m_amplitude_wheel_Y, SIGNAL(valueChanged(double)), m_amplitude_widget, SLOT(update()));

    connect(m_amplitude_scroll_bar, SIGNAL(sliderMoved(double)), m_amplitude_widget, SLOT(setOffset(double)));
    connect(m_amplitude_scroll_bar, SIGNAL(sliderMoved(double)), m_amplitude_widget, SLOT(update()));

    connect(m_amplitude_widget, SIGNAL(rangeChanged(double)), this, SLOT(setAmplitudeZoom(double)));
    connect(m_amplitude_widget, SIGNAL(rangeChanged(double)), m_amplitude_wheel_Y, SLOT(setValue(double)));
    connect(m_amplitude_widget, SIGNAL(offsetChanged(double)), m_amplitude_scroll_bar, SLOT(setValue(double)));

    //make the widgets get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_freq_widget_GL, SLOT(update()));
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_amplitude_widget, SLOT(update()));
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_time_axis, SLOT(update()));
    connect(&(g_data->getView()), SIGNAL(timeViewRangeChanged(double, double)), m_time_axis, SLOT(setRange(double, double)));
}

//------------------------------------------------------------------------------
FreqView::~FreqView(void)
{
    //Qt deletes the child widgets automatically
}

//------------------------------------------------------------------------------
void FreqView::zoomIn(void)
{
    bool l_done_it = false;
    if(g_data->getRunning() != STREAM_FORWARD)
    {
        if(m_freq_widget_GL->testAttribute(Qt::WA_UnderMouse))
        {
            QPoint l_mouse_pos = m_freq_widget_GL->mapFromGlobal(QCursor::pos());
            g_data->getView().setZoomFactorX(g_data->getView().logZoomX() + 0.1, l_mouse_pos.x());
            g_data->getView().setZoomFactorY(g_data->getView().logZoomY() + 0.1, m_freq_widget_GL->height() - l_mouse_pos.y());
            l_done_it = true;
        }
        else if(m_amplitude_widget->testAttribute(Qt::WA_UnderMouse))
        {
            QPoint l_mouse_pos = m_amplitude_widget->mapFromGlobal(QCursor::pos());
            g_data->getView().setZoomFactorX(g_data->getView().logZoomX() + 0.1, l_mouse_pos.x());
            l_done_it = true;

        }
    }
    if(!l_done_it)
    {
        g_data->getView().setZoomFactorX(g_data->getView().logZoomX() + 0.1);
        g_data->getView().setZoomFactorY(g_data->getView().logZoomY() + 0.1);
        l_done_it = true;
    }
}

//------------------------------------------------------------------------------
void FreqView::zoomOut(void)
{
    g_data->getView().setZoomFactorX(g_data->getView().logZoomX() - 0.1);
    if(!m_amplitude_widget->testAttribute(Qt::WA_UnderMouse))
    {
        g_data->getView().setZoomFactorY(g_data->getView().logZoomY() - 0.1);
    }
}

//------------------------------------------------------------------------------
void FreqView::setAmplitudeZoom(double newRange)
{
    m_amplitude_scroll_bar->setRange(0.0, 1.0-newRange);
    m_amplitude_scroll_bar->setPageStep(newRange);
}

//------------------------------------------------------------------------------
QSize FreqView::sizeHint(void) const
{
    return QSize(600, 560);
}
// EOF
