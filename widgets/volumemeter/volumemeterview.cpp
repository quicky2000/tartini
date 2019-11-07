/***************************************************************************
                          volumemeterview.cpp  -  description
                             -------------------
    begin                : Tue Dec 21 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "volumemeterview.h"
#include "volumemeterwidget.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
VolumeMeterView::VolumeMeterView( int p_view_ID, QWidget * p_parent ):
  ViewWidget( p_view_ID, p_parent)
{
  m_volume_meter_widget = new VolumeMeterWidget(this);
  m_volume_meter_widget->show();
}

//------------------------------------------------------------------------------
VolumeMeterView::~VolumeMeterView()
{
  delete m_volume_meter_widget;
}

//------------------------------------------------------------------------------
void VolumeMeterView::resizeEvent(QResizeEvent *)
{
  m_volume_meter_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize VolumeMeterView::sizeHint() const
{
  return QSize(256, 30);
}
