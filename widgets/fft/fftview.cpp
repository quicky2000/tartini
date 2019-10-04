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
FFTView::FFTView( int p_view_id
                , QWidget *p_parent
                )
: ViewWidget( p_view_id, p_parent)
{
    gdata->setDoingActiveFFT(true);

    Channel *l_active_channel = gdata->getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->lock();
        l_active_channel->processChunk(l_active_channel->currentChunk());
        l_active_channel->unlock();
    }

    m_fft_widget = new FFTWidget(this);
    m_fft_widget->show();

    //make the widget get updated when the view changes
    connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), m_fft_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
FFTView::~FFTView(void)
{
    gdata->setDoingActiveFFT(false);
    delete m_fft_widget;
}

//------------------------------------------------------------------------------
void FFTView::resizeEvent(QResizeEvent *)
{
    m_fft_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize FFTView::sizeHint(void) const
{
    return QSize(400, 128);
}

// EOF
