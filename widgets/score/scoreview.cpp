/***************************************************************************
                          scoreview.cpp  -  description
                             -------------------
    begin                : Mar 26 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "scoreview.h"
#include "scorewidget.h"
#include "gdata.h"
#include "qwt_wheel.h"
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QToolTip>
#include <QComboBox>

//------------------------------------------------------------------------------
ScoreView::ScoreView(int p_view_id
                    ,QWidget * p_parent
                    )
: ViewWidget( p_view_id, p_parent)
{

    QHBoxLayout * l_main_layout = new QHBoxLayout(this);

    QVBoxLayout * l_left_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_left_layout);
    QVBoxLayout * l_right_layout = new QVBoxLayout();
    l_main_layout->addLayout(l_right_layout);

    //create a drawing object
    m_score_widget = new ScoreWidget(this);
    l_left_layout->addWidget(m_score_widget);
    QHBoxLayout * l_bottom_layout = new QHBoxLayout();
    l_left_layout->addLayout(l_bottom_layout);

    //vertical scroll wheel
    QwtWheel *l_scale_wheel_Y = new QwtWheel(this);
    l_scale_wheel_Y->setOrientation(Qt::Vertical);
    l_scale_wheel_Y->setWheelWidth(14);
#if QWT_VERSION >= 0x060000
    l_scale_wheel_Y->setRange(1.0, 30.0);
    l_scale_wheel_Y->setSingleStep(0.1);
    // Multiplicator value is 100 = 1 / 0.01
    l_scale_wheel_Y->setPageStepCount(10);
#else
    l_scale_wheel_Y->setRange(1.0, 30.0, 0.1, 1);
#endif // QWT_VERSION >= 0x060000
    l_scale_wheel_Y->setValue(m_score_widget->scaleY());
    l_scale_wheel_Y->setToolTip(tr("Zoom vertically"));
    l_right_layout->addWidget(l_scale_wheel_Y, 1);
    l_right_layout->addStretch(4);
    connect(l_scale_wheel_Y, SIGNAL(wheelMoved(double)), m_score_widget, SLOT(setScaleY(double)));
    connect(l_scale_wheel_Y, SIGNAL(wheelMoved(double)), m_score_widget, SLOT(update()));

    //horizontal scroll wheel
    QwtWheel * l_scale_wheel_X = new QwtWheel(this);
    l_scale_wheel_X->setOrientation(Qt::Horizontal);
    l_scale_wheel_X->setWheelWidth(16);
#if QWT_VERSION >= 0x060000
    l_scale_wheel_X->setRange(1.0, 100.0);
    l_scale_wheel_X->setSingleStep(1.1);
    l_scale_wheel_X->setPageStepCount(1);
#else
    l_scale_wheel_X->setRange(1.0, 100.0, 1.1, 1);
#endif // QWT_VERSION >= 0x060000
    l_scale_wheel_X->setValue(m_score_widget->scaleX());
    l_scale_wheel_X->setToolTip(tr("Zoom horizontally"));
    connect(l_scale_wheel_X, SIGNAL(wheelMoved(double)), m_score_widget, SLOT(setScaleX(double)));
    connect(l_scale_wheel_X, SIGNAL(wheelMoved(double)), m_score_widget, SLOT(update()));

    QComboBox * l_sharps_mode_combo_box = new QComboBox(this);
    l_sharps_mode_combo_box->setObjectName("sharpsModeComboBox");

    QStringList l_string_list;
    l_string_list << tr("Sharps") << tr("Flats");
    l_sharps_mode_combo_box->addItems(l_string_list);
    connect(l_sharps_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setSharpsMode(int)));
    connect(l_sharps_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    QComboBox * l_notes_mode_combo_box = new QComboBox(this);
    l_notes_mode_combo_box->setObjectName("notesModeComboBox");

    l_string_list.clear();
    l_string_list << tr("Notes") << tr("MIDI blocks");
    l_notes_mode_combo_box->addItems(l_string_list);
    connect(l_notes_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setNotesMode(int)));
    connect(l_notes_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    QComboBox * l_clef_mode_combo_box = new QComboBox(this);
    l_clef_mode_combo_box->setObjectName("clefModeComboBox");

    l_string_list.clear();
    l_string_list << tr("Both clefs") << tr("Treble clef");
    l_clef_mode_combo_box->addItems(l_string_list);
    connect(l_clef_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setClefMode(int)));
    connect(l_clef_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    QComboBox * l_opaque_mode_combo_box = new QComboBox(this);
    l_opaque_mode_combo_box->setObjectName("opaqueModeComboBox");

    l_string_list.clear();
    l_string_list << tr("See-through notes") << tr("Opaque notes");
    l_opaque_mode_combo_box->addItems(l_string_list);
    connect(l_opaque_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setOpaqueMode(int)));
    connect(l_opaque_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    QComboBox * l_transpose_combo_box = new QComboBox(this);
    l_transpose_combo_box->setObjectName("transposeComboBox");

    l_string_list.clear();
    l_string_list << tr("Up 2 octaves") << tr("Up 1 octave") << tr("As played") << tr("Down 1 octave") << tr("Down 2 octaves");
    l_transpose_combo_box->addItems(l_string_list);
    l_transpose_combo_box->setCurrentIndex(2);
    connect(l_transpose_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setTransposeLevel(int)));
    connect(l_transpose_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    QComboBox * l_show_all_mode_combo_box = new QComboBox(this);
    l_show_all_mode_combo_box->setObjectName("showAllModeComboBox");

    l_string_list.clear();
    l_string_list << tr("Hide extream notes") << tr("Show all notes");
    l_show_all_mode_combo_box->addItems(l_string_list);
    connect(l_show_all_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(setShowAllMode(int)));
    connect(l_show_all_mode_combo_box, SIGNAL(activated(int)), m_score_widget, SLOT(update()));

    l_bottom_layout->addWidget(l_scale_wheel_X, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_sharps_mode_combo_box, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_notes_mode_combo_box, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_clef_mode_combo_box, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_opaque_mode_combo_box, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_transpose_combo_box, 0);
    l_bottom_layout->addStretch(1);
    l_bottom_layout->addWidget(l_show_all_mode_combo_box, 0);
    l_bottom_layout->addStretch(4);

    //make any connections
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), m_score_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
ScoreView::~ScoreView(void)
{
    delete m_score_widget;
}

//------------------------------------------------------------------------------
void ScoreView::resizeEvent(QResizeEvent *)
{
}

//------------------------------------------------------------------------------
QSize ScoreView::sizeHint(void) const
{
    return QSize(800, 240);
}
// EOF
