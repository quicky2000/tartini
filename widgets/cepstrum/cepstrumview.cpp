/***************************************************************************
                          cepstrumview.cpp  -  description
                             -------------------
    begin                : May 21 2005
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
#include "cepstrumview.h"
#include "cepstrumwidget.h"
#include "gdata.h"
#include "channel.h"
#include <QResizeEvent>

//------------------------------------------------------------------------------
CepstrumView::CepstrumView( int p_view_id
                          , QWidget *p_parent
                          )
: ViewWidget( p_view_id, p_parent)
{
    g_data->setDoingActiveCepstrum(true);

    Channel * l_active_channel = g_data->getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->lock();
        l_active_channel->processChunk(l_active_channel->currentChunk());
        l_active_channel->unlock();
    }

    m_cepstrum_widget = new CepstrumWidget(this);
    m_cepstrum_widget->setWhatsThis("Note: The 'MPM + MODIFIED CEPSTRUM' option in the preferences needs to be seleted to see this. This is actually a kind of modified cepstrum. ");
    m_cepstrum_widget->show();

    //make the widget get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), m_cepstrum_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
CepstrumView::~CepstrumView(void)
{
    g_data->setDoingActiveCepstrum(false);
    delete m_cepstrum_widget;
}

//------------------------------------------------------------------------------
void CepstrumView::resizeEvent(QResizeEvent *)
{
    m_cepstrum_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize CepstrumView::sizeHint(void) const
{
    return QSize(400, 128);
}

// EOF
