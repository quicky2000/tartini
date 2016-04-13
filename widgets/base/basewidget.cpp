/***************************************************************************
                          basewidget.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <QPixmap>
#include <QPainter>

#include "basewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"

//------------------------------------------------------------------------------
BaseWidget::BaseWidget(QWidget * p_parent)
: DrawWidget(p_parent)
{
}

//------------------------------------------------------------------------------
BaseWidget::~BaseWidget(void)
{
}

//------------------------------------------------------------------------------
void BaseWidget::paintEvent( QPaintEvent * )
{
    Channel * l_active_channel = gdata->getActiveChannel();

    beginDrawing();

    // Drawing code goes here
    p.drawLine(0, 0, width(), height());

    endDrawing();
}

//------------------------------------------------------------------------------
QSize BaseWidget::sizeHint(void) const
{
    return QSize(300, 200);
}

// EOF
