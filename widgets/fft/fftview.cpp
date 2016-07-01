/***************************************************************************
                          fftview.cpp  -  description
                             -------------------
    begin                : May 18 2005
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
#include "fftview.h"
#include "fftwidget.h"
#include "gdata.h"
#include "channel.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
FFTView::FFTView( int viewID_, QWidget *parent ):
  ViewWidget( viewID_, parent)
{
  gdata->setDoingActiveFFT(true);
  
  Channel *active = gdata->getActiveChannel();
  if(active)
    {
      active->lock();
      active->processChunk(active->currentChunk());
      active->unlock();
    }

  fftWidget = new FFTWidget(this);
  fftWidget->show();

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), fftWidget, SLOT(update()));
}

//------------------------------------------------------------------------------
FFTView::~FFTView(void)
{
  gdata->setDoingActiveFFT(false);
  delete fftWidget;
}

//------------------------------------------------------------------------------
void FFTView::resizeEvent(QResizeEvent *)
{
  fftWidget->resize(size());
}

//------------------------------------------------------------------------------
  QSize FFTView::sizeHint(void) const
{
  return QSize(400, 128);
}

// EOF
