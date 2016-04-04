/***************************************************************************
                          hstackwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qpixmap.h>
#include <qpainter.h>

#include "hstackwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"

HStackWidget::HStackWidget(QWidget *parent)
  : DrawWidget(parent)
{
  //make the widget get updated when the view changes
  connect(gdata->view, SIGNAL(onFastUpdate()), this, SLOT(update()));
}

HStackWidget::~HStackWidget()
{
}

void HStackWidget::paintEvent( QPaintEvent * )
{
  //Channel *active = gdata->getActiveChannel();
  beginDrawing();
  
  // Drawing code goes here

  endDrawing();
}
