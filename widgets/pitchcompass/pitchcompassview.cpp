/***************************************************************************
                          pitchcompassview.cpp  -  description
                             -------------------
    begin                : Wed Dec 15 2004
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
#include "pitchcompassview.h"
#include "pitchcompassdrawwidget.h"
#include "gdata.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
PitchCompassView::PitchCompassView( int p_view_id
                                  , QWidget *p_parent
                                  , int p_mode
                                  )
: ViewWidget(p_view_id, p_parent)
{
    m_pitch_compass_draw_widget = new PitchCompassDrawWidget(this, "compass", p_mode);
    connect(&(g_data->getView()), SIGNAL(onSlowUpdate(double)), m_pitch_compass_draw_widget, SLOT(updateCompass(double)));
    m_pitch_compass_draw_widget->show();
}

//------------------------------------------------------------------------------
PitchCompassView::~PitchCompassView()
{
}

//------------------------------------------------------------------------------
void PitchCompassView::resizeEvent(QResizeEvent *)
{
    m_pitch_compass_draw_widget->resize(size());
}

//------------------------------------------------------------------------------
void PitchCompassView::changeMode(int p_mode)
{
    delete m_pitch_compass_draw_widget;
    m_pitch_compass_draw_widget = new PitchCompassDrawWidget(this, "compass", p_mode);
    connect(&(g_data->getView()), SIGNAL(currentTimeChanged(double)), m_pitch_compass_draw_widget, SLOT(updateCompass(double)));
}

//------------------------------------------------------------------------------
QSize PitchCompassView::sizeHint() const
{
    return QSize(200, 200);
}
// EOF
