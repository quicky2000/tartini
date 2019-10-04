/***************************************************************************
                          debugview.cpp  -  description
                             -------------------
    begin                : Wed Feb 22 2006
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
#include "debugview.h"
#include "debugwidget.h"
#include "gdata.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
DebugView::DebugView( int p_view_id, QWidget *p_parent )
 : ViewWidget( p_view_id, p_parent)
{
  m_debug_widget = new DebugWidget(this);
  m_debug_widget->show();

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(viewChanged()), m_debug_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
DebugView::~DebugView(void)
{
  delete m_debug_widget;
}

//------------------------------------------------------------------------------
void DebugView::resizeEvent(QResizeEvent *)
{
  m_debug_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize DebugView::sizeHint(void) const
{
  return QSize(250, 500);
}
// EOF
