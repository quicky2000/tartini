/***************************************************************************
                          correlationview.cpp  -  description
                             -------------------
    begin                : May 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
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
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

//------------------------------------------------------------------------------
CorrelationView::CorrelationView( int viewID_, QWidget *parent )
 : ViewWidget( viewID_, parent)
{
  gdata->setDoingActiveAnalysis(true);

  Channel *active = gdata->getActiveChannel();
  if(active)
    {
      active->lock();
      active->processChunk(active->currentChunk());
      active->unlock();
    }

  correlationWidget = new CorrelationWidget(this);
  QStringList s;
  s << "Chunk correlation" << "Note Aggregate Correlation" << "Note Aggregate Correlation Scaled";
  QComboBox *aggregateModeComboBox = new QComboBox(this, "aggregateModeComboBox");
  aggregateModeComboBox->addItems(s);
  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->setMargin(0);
  hLayout->addWidget(aggregateModeComboBox);
  hLayout->addStretch(1);
  connect(aggregateModeComboBox, SIGNAL(activated(int)), correlationWidget, SLOT(setAggregateMode(int)));

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(0);
  mainLayout->addWidget(correlationWidget);
  mainLayout->addLayout(hLayout);

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onSlowUpdate(double)), correlationWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
CorrelationView::~CorrelationView(void)
{
  gdata->setDoingActiveAnalysis(false);
  delete correlationWidget;
}

//------------------------------------------------------------------------------
QSize CorrelationView::sizeHint(void) const
{
  return QSize(400, 128);
}

// EOF
