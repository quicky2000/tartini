/***************************************************************************
                          summaryview.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "summaryview.h"
#include "summarydrawwidget.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
SummaryView::SummaryView(int p_view_id
                        ,QWidget *p_parent
                        )
: ViewWidget( p_view_id, p_parent)
{
    m_summary_draw_widget = new SummaryDrawWidget(this);
    m_summary_draw_widget->show();
}

//------------------------------------------------------------------------------
SummaryView::~SummaryView()
{
    delete m_summary_draw_widget;
}

//------------------------------------------------------------------------------
void SummaryView::resizeEvent(QResizeEvent *)
{
    m_summary_draw_widget->resize(size());
}
 
//------------------------------------------------------------------------------
QSize SummaryView::sizeHint() const
{
    return QSize(512, 256);
}

// EOF
