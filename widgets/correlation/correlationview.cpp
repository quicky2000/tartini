/***************************************************************************
                          correlationview.cpp  -  description
                             -------------------
    begin                : May 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "correlationview.h"
#include "correlationwidget.h"
#include "gdata.h"
#include "channel.h"

CorrelationView::CorrelationView( int viewID_, QWidget *parent, const char *name )
 : ViewWidget( viewID_, parent, name)
{
  //setCaption("Wave view");
  gdata->setDoingActiveAnalysis(true);

  Channel *active = gdata->getActiveChannel();
  if(active) {
    active->lock();
    active->processChunk(active->currentChunk());
    active->unlock();
  }

  correlationWidget = new CorrelationWidget(this);
  correlationWidget->show();

  //make the widget get updated when the view changes
  //connect(gdata->view, SIGNAL(onFastUpdate()), correlationWidget, SLOT(update()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), correlationWidget, SLOT(update()));
}

CorrelationView::~CorrelationView()
{
  gdata->setDoingActiveAnalysis(false);
  delete correlationWidget;
}

void CorrelationView::resizeEvent(QResizeEvent *)
{
  correlationWidget->resize(size());
}
