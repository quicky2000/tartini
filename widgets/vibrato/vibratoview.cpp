/***************************************************************************
                          vibratoview.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

/** To create a new tartini widget
    1. Copy the base folder (and files)
    2. Rename the base* files to myclass* files, e.g. mv baseview.cpp myclassview.cpp
    3. Change the class names to MyClassView & MyClassWidget (where MyClass is the name of your new class)
    4. In MainWindow.h increment NUM_VIEWS and add to the enum VIEW_MYCLASS
    5. Add a ViewData item to viewData in MainWindow.cpp
    6. Add #include "myclassview.h" near top of MainWindow.cpp
    7. In MainWindow::openView(int viewID) add a case to create your class
    8. Add your files names and folder names into the pitch.pro (HEADERS, SOURCES and MYPATHS respectivly)
    9. Done. Just rebuild the project (regenerating the Makefile)
*/

#include "vibratoview.h"
#include "vibratotunerwidget.h"
#include "vibratospeedwidget.h"
#include "vibratocirclewidget.h"
#include "vibratoperiodwidget.h"
#include "vibratotimeaxis.h"
#include "vibratowidget.h"
#include "ledindicator.h"
#include "timeaxis.h"
#include "gdata.h"
#include "channel.h"

#include <QGridLayout>
#include <QSplitter>
#include <QToolTip>
#include <QSizeGrip>
#include <QPushButton>
#include <QScrollBar>
#include <QSlider>
#include <qwt_wheel.h>

//------------------------------------------------------------------------------
VibratoView::VibratoView(int p_view_ID
                        ,QWidget * p_parent
                        )
: ViewWidget(p_view_ID, p_parent)
{
    int l_note_label_offset = 28;

    setWindowTitle("Vibrato View");

    QGridLayout * l_main_layout = new QGridLayout;

    QSplitter * l_vertical_splitter = new QSplitter(Qt::Vertical);

    // Second column: Label + speedwidget + button
    QGridLayout * l_speed_layout = new QGridLayout;

    // The label
    QLabel * l_speed_label = new QLabel;
    l_speed_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    l_speed_label->setText("Vibrato speed & width");
    l_speed_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l_speed_layout->addWidget(l_speed_label, 0, 0, 1, 1);
    l_speed_layout->setRowStretch(0, 0);

    // The speedwidget
    QFrame * l_speed_frame = new QFrame;
    l_speed_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout * l_speed_frame_layout = new QVBoxLayout;
    m_vibrato_speed_widget = new VibratoSpeedWidget(0);
    m_vibrato_speed_widget->setWhatsThis("Indicates the instantaneous speed and peek-to-peek amplitude of the vibrato. Note: 100 cents = 1 semi-tone (even tempered).");
    l_speed_frame_layout->addWidget(m_vibrato_speed_widget);
    l_speed_frame_layout->setMargin(0);
    l_speed_frame_layout->setSpacing(0);
    l_speed_frame->setLayout(l_speed_frame_layout);
    l_speed_layout->addWidget(l_speed_frame, 1, 0, 1, 1);
    l_speed_layout->setRowStretch(1, 1);

    // The button
    l_speed_layout->setMargin(1);
    l_speed_layout->setSpacing(1);

    QWidget * l_top_widget2 = new QWidget;
    l_top_widget2->setLayout(l_speed_layout);

    // Third column: Label + circlewidget + horizontal slider
    QGridLayout * l_circle_layout = new QGridLayout;

    // The label
    QLabel * l_circle_label = new QLabel;
    l_circle_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    l_circle_label->setText("The Vibrato circle");
    l_circle_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l_circle_layout->addWidget(l_circle_label, 0, 0, 1, 2);
    l_circle_layout->setRowStretch(0, 0);

    // The circlewidget
    QFrame * l_circle_frame = new QFrame;
    l_circle_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout *l_circle_frame_layout = new QVBoxLayout;
    m_vibrato_circle_widget = new VibratoCircleWidget(0);
    m_vibrato_circle_widget->setWhatsThis("Each cycle of your vibrato is represented by a 2D shape. A current cycle produces a circle if it has a perfect sine wave shape. "
    "Going outside the line indicates your phase is ahead of a sine-wave, and inside the line slower. Note: The shape of one cycle is blended into the next.");
    l_circle_frame_layout->addWidget(m_vibrato_circle_widget);
    l_circle_frame_layout->setMargin(0);
    l_circle_frame_layout->setSpacing(0);
    l_circle_frame->setLayout(l_circle_frame_layout);
    l_circle_layout->addWidget(l_circle_frame, 1, 0, 1, 2);
    l_circle_layout->setRowStretch(1, 1);

    l_circle_layout->setMargin(1);
    l_circle_layout->setSpacing(1);

    QWidget * l_top_widget3 = new QWidget;
    l_top_widget3->setLayout(l_circle_layout);

    // Fourth column: Label + periodwidget + buttons for periodwidget
    QGridLayout *l_period_layout = new QGridLayout;

    // The label
    QLabel * l_period_label = new QLabel;
    l_period_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    l_period_label->setText("Vibrato period view");
    l_period_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l_period_layout->addWidget(l_period_label, 0, 0, 1, 6);
    l_period_layout->setRowStretch(0, 0);

    // The periodwidget
    QFrame * l_period_frame = new QFrame;
    l_period_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout * l_period_frame_layout = new QVBoxLayout;
    m_vibrato_period_widget = new VibratoPeriodWidget(0);
    m_vibrato_period_widget->setWhatsThis("A detailed view of the current vibrato period. You can turn on and off some different options with the buttons. ");
    l_period_frame_layout->addWidget(m_vibrato_period_widget);
    l_period_frame_layout->setMargin(0);
    l_period_frame_layout->setSpacing(0);
    l_period_frame->setLayout(l_period_frame_layout);
    l_period_layout->addWidget(l_period_frame, 1, 0, 1, 6);
    l_period_layout->setRowStretch(1, 1);

    // The buttons
    QPushButton * l_smoothed_periods_button = new QPushButton("SP");
    l_smoothed_periods_button->setCheckable(true);
    l_smoothed_periods_button->setMinimumWidth(30);
    l_smoothed_periods_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_smoothed_periods_button->setFocusPolicy(Qt::NoFocus);
    l_smoothed_periods_button->setDown(true);
    l_smoothed_periods_button->setToolTip("Use smoothed periods");
    l_period_layout->addWidget(l_smoothed_periods_button, 2, 0, 1, 1);

    QPushButton * l_draw_sine_reference_button = new QPushButton("DSR");
    l_draw_sine_reference_button->setCheckable(true);
    l_draw_sine_reference_button->setMinimumWidth(30);
    l_draw_sine_reference_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_draw_sine_reference_button->setFocusPolicy(Qt::NoFocus);
    l_draw_sine_reference_button->setToolTip("Draw the reference sinewave");
    l_period_layout->addWidget(l_draw_sine_reference_button, 2, 1, 1, 1);

    QPushButton * l_sine_style_button = new QPushButton("SS");
    l_sine_style_button->setCheckable(true);
    l_sine_style_button->setMinimumWidth(30);
    l_sine_style_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_sine_style_button->setFocusPolicy(Qt::NoFocus);
    l_sine_style_button->setToolTip("Use sine style: /\\/ instead of cosine style: \\/");
    l_period_layout->addWidget(l_sine_style_button, 2, 2, 1, 1);

    QPushButton * l_draw_prev_periods_button = new QPushButton("DPP");
    l_draw_prev_periods_button->setCheckable(true);
    l_draw_prev_periods_button->setMinimumWidth(30);
    l_draw_prev_periods_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_draw_prev_periods_button->setFocusPolicy(Qt::NoFocus);
    l_draw_prev_periods_button->setToolTip("Draw previous periods");
    l_period_layout->addWidget(l_draw_prev_periods_button, 2, 3, 1, 1);

    QPushButton * l_period_scaling_button = new QPushButton("PS");
    l_period_scaling_button->setCheckable(true);
    l_period_scaling_button->setMinimumWidth(30);
    l_period_scaling_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_period_scaling_button->setFocusPolicy(Qt::NoFocus);
    l_period_scaling_button->setToolTip("Scale previous periods to the current period");
    l_period_layout->addWidget(l_period_scaling_button, 2, 4, 1, 1);

    QPushButton * l_draw_comparison_button = new QPushButton("DC");
    l_draw_comparison_button->setCheckable(true);
    l_draw_comparison_button->setMinimumWidth(30);
    l_draw_comparison_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    l_draw_comparison_button->setFocusPolicy(Qt::NoFocus);
    l_draw_comparison_button->setToolTip("Draw comparison between period and sinewave");
    l_period_layout->addWidget(l_draw_comparison_button, 2, 5, 1, 1);

    l_period_layout->setRowStretch(2, 0);

    l_period_layout->setMargin(1);
    l_period_layout->setSpacing(1);

    QWidget * l_top_widget4 = new QWidget;
    l_top_widget4->setLayout(l_period_layout);

    QSplitter * l_top_splitter = new QSplitter(Qt::Horizontal);
    l_top_splitter->addWidget(l_top_widget2);
    l_top_splitter->setStretchFactor(0, 10);
    l_top_splitter->addWidget(l_top_widget3);
    l_top_splitter->setStretchFactor(1, 10);
    l_top_splitter->addWidget(l_top_widget4);
    l_top_splitter->setStretchFactor(2, 5);

    l_vertical_splitter->addWidget(l_top_splitter);

    // Bottom half
    QWidget * l_bottom_widget = new QWidget;
    QGridLayout * l_bottom_layout = new QGridLayout;

    // The timeaxis
    m_vibrato_time_axis = new VibratoTimeAxis(0, l_note_label_offset);

    // The drawing object for displaying vibrato notes
    QFrame * l_vibrato_frame = new QFrame;
    l_vibrato_frame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    QVBoxLayout * l_vibrato_frame_layout = new QVBoxLayout;
    m_vibrato_widget = new VibratoWidget(0, l_note_label_offset);
    m_vibrato_widget->setWhatsThis("Shows the vibrato of the current note. "
    "Grey shading indicates the vibrato envelope. The black line indicates the center pitch. "
    "Other shading indicates half period times. "
    "If there is no vibrato (i.e. no wobbling frequency) it will probably just look a mess. ");
    l_vibrato_frame_layout->addWidget(m_vibrato_widget);
    l_vibrato_frame_layout->setMargin(0);
    l_vibrato_frame_layout->setSpacing(0);
    l_vibrato_frame->setLayout(l_vibrato_frame_layout);

    // The right side: vertical scrollwheel + vertical scrollbar
    QGridLayout * l_bottom_right_layout = new QGridLayout;

    // The vertical scrollwheel
    QwtWheel * l_zoom_wheel_V = new QwtWheel;
    l_zoom_wheel_V->setOrientation(Qt::Vertical);
    l_zoom_wheel_V->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
    l_zoom_wheel_V->setRange(0.3, 25.0);
    l_zoom_wheel_V->setSingleStep(0.1);
    // Multiplicator value is 10 = 1 / 0.1
    l_zoom_wheel_V->setPageStepCount(10);
#else
    l_zoom_wheel_V->setRange(0.3, 25.0, 0.1, 1);
#endif // QWT_VERSION >= 0x060000

    l_zoom_wheel_V->setValue(1.0);
    l_zoom_wheel_V->setFocusPolicy(Qt::NoFocus);
    l_zoom_wheel_V->setToolTip("Zoom vibrato view vertically");
    l_bottom_right_layout->addWidget(l_zoom_wheel_V, 0, 0, 1, 1);

    // The vertical scrollbar
    QScrollBar * l_scroll_bar_V = new QScrollBar(Qt::Vertical);
    l_scroll_bar_V->setRange(-250, 250);
    l_scroll_bar_V->setValue(0);
    l_scroll_bar_V->setPageStep(100);
    l_scroll_bar_V->setToolTip("Scroll vibrato view vertically");
    l_bottom_right_layout->addWidget(l_scroll_bar_V, 1, 0, 4, 1);
    l_bottom_right_layout->setRowStretch(1, 1);

    l_bottom_right_layout->setMargin(1);
    l_bottom_right_layout->setSpacing(1);

    // The bottom side: dummy spacer + horizontal scrollwheel + resize grip
    QGridLayout * l_bottom_bottom_layout = new QGridLayout;

    // The dummy spacer
    QLabel * l_dummy_H = new QLabel;
    l_bottom_bottom_layout->addWidget(l_dummy_H, 0, 0, 1, 4);
    l_bottom_bottom_layout->setColumnStretch(0, 1);

    // The horizontal scrollwheel
    QwtWheel *l_zoom_wheel_H = new QwtWheel;
    l_zoom_wheel_H->setOrientation(Qt::Horizontal);
    l_zoom_wheel_H->setWheelWidth(14);

#if QWT_VERSION >= 0x060000
    l_zoom_wheel_H->setRange(1, 100);
    l_zoom_wheel_H->setSingleStep(1);
    l_zoom_wheel_H->setPageStepCount(1);
#else
    l_zoom_wheel_H->setRange(1, 100, 1, 1);
#endif // QWT_VERSION >= 0x060000

    l_zoom_wheel_H->setValue(25);
    l_zoom_wheel_H->setFocusPolicy(Qt::NoFocus);
    l_zoom_wheel_H->setToolTip("Zoom vibrato view horizontally");
    l_bottom_bottom_layout->addWidget(l_zoom_wheel_H, 0, 4, 1, 1);

    // The resize grip
    QSizeGrip * l_size_grip = new QSizeGrip(0);
    l_size_grip->setFixedSize(15, 15);
    l_bottom_bottom_layout->addWidget(l_size_grip, 0, 5, 1, 1);

    l_bottom_bottom_layout->setMargin(1);
    l_bottom_bottom_layout->setSpacing(1);

    l_bottom_layout->addWidget(m_vibrato_time_axis, 0, 0, 1, 1);
    l_bottom_layout->addWidget(l_vibrato_frame, 1, 0, 1, 1);
    l_bottom_layout->addLayout(l_bottom_right_layout, 1, 1, 1, 1);
    l_bottom_layout->addLayout(l_bottom_bottom_layout, 2, 0, 1, 2);
    l_bottom_layout->setMargin(1);
    l_bottom_layout->setSpacing(1);

    l_bottom_widget->setLayout(l_bottom_layout);
    l_vertical_splitter->addWidget(l_bottom_widget);

    l_main_layout->addWidget(l_vertical_splitter);

    l_main_layout->setMargin(1);
    l_main_layout->setSpacing(1);

    setLayout(l_main_layout);

    // Make signal/slot connections
    connect(g_data, SIGNAL(onChunkUpdate()), m_vibrato_speed_widget, SLOT(doUpdate()));
    connect(g_data, SIGNAL(onChunkUpdate()), m_vibrato_circle_widget, SLOT(doUpdate()));
    connect(g_data, SIGNAL(onChunkUpdate()), m_vibrato_period_widget, SLOT(doUpdate()));
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), m_vibrato_time_axis, SLOT(update()));
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), m_vibrato_widget, SLOT(updateGL()));

    // The vertical scrollbar
    connect(l_scroll_bar_V, SIGNAL(valueChanged(int)), m_vibrato_widget, SLOT(setOffsetY(int)));

    // The zoomwheels
    connect(l_zoom_wheel_H, SIGNAL(valueChanged(double)), m_vibrato_widget, SLOT(setZoomFactorX(double)));
    connect(l_zoom_wheel_H, SIGNAL(valueChanged(double)), m_vibrato_time_axis, SLOT(setZoomFactorX(double)));
    connect(l_zoom_wheel_V, SIGNAL(valueChanged(double)), m_vibrato_widget, SLOT(setZoomFactorY(double)));

    // The buttons for the period view
    connect(l_smoothed_periods_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setSmoothedPeriods(bool)));
    connect(l_draw_sine_reference_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setDrawSineReference(bool)));
    connect(l_sine_style_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setSineStyle(bool)));
    connect(l_draw_prev_periods_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setDrawPrevPeriods(bool)));
    connect(l_period_scaling_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setPeriodScaling(bool)));
    connect(l_draw_comparison_button, SIGNAL(toggled(bool)), m_vibrato_period_widget, SLOT(setDrawComparison(bool)));
}

//------------------------------------------------------------------------------
VibratoView::~VibratoView()
{
    delete m_vibrato_widget;
}

//------------------------------------------------------------------------------
QSize VibratoView::sizeHint() const
{
    return QSize(500, 300);
}
// EOF
