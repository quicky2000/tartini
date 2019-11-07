/***************************************************************************
                          hblockview.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "hblockview.h"
#include "hblockwidget.h"
#include "gdata.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
HBlockView::HBlockView( int p_view_id
                      , QWidget *p_parent
                      )
: ViewWidget( p_view_id, p_parent)
{
    m_h_block_widget = new HBlockWidget(this);
    m_h_block_widget->show();

    //make the widget get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), m_h_block_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
HBlockView::~HBlockView()
{
    delete m_h_block_widget;
}

//------------------------------------------------------------------------------
void HBlockView::resizeEvent(QResizeEvent *)
{
    m_h_block_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize HBlockView::sizeHint() const
{
    return QSize(200, 180);
}

// EOF
