/***************************************************************************
                          pianoview.cpp  -  description
                             -------------------
    begin                : 24 Mar 2005
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
#include <QResizeEvent>

#include "pianoview.h"
#include "pianowidget.h"
#include "gdata.h"
#include "channel.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
PianoView::PianoView(int p_view_id, QWidget * p_parent):
  ViewWidget( p_view_id, p_parent)
{
  m_piano_widget = new PianoWidget(this);
  m_piano_widget->show();

  //make the widget get updated when the view changes
  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(changeKey()));
}

//------------------------------------------------------------------------------
PianoView::~PianoView(void)
{
  delete m_piano_widget;
}

//------------------------------------------------------------------------------
void PianoView::resizeEvent(QResizeEvent *)
{
  m_piano_widget->resize(size());
}

//------------------------------------------------------------------------------
void PianoView::changeKey(void)
{
  Channel *l_active_channel = gdata->getActiveChannel();
  if(l_active_channel)
    {
      AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
      if(l_data && l_active_channel->isVisibleChunk(l_data))
	{
	  float l_pitch = l_data->getPitch();
	  m_piano_widget->setCurrentNote(noteValue(l_pitch), l_data->getCorrelation());
	}
      else
	{
	  m_piano_widget->setNoNote();
	}
    }
  else
    {
      m_piano_widget->setNoNote();
    }
  m_piano_widget->update();
}

//------------------------------------------------------------------------------
QSize PianoView::sizeHint(void) const
{
  return QSize(200, 100);
}

// EOF
