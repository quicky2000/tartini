/***************************************************************************
                          cepstrumwidget.cpp  -  description
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
#include <qpixmap.h>
#include <qpainter.h>
#include <q3memarray.h>
#include <QPaintEvent>

#include "cepstrumwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
CepstrumWidget::CepstrumWidget(QWidget *parent):
  DrawWidget(parent)
{
}

//------------------------------------------------------------------------------
CepstrumWidget::~CepstrumWidget(void)
{
}

//------------------------------------------------------------------------------
void CepstrumWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();

  AnalysisData *data = NULL;
  double pixelStep;
  int j, x;
    
  beginDrawing(false);
  
  if(active)
    {
      pixelStep = double(active->get_cepstrum_data().size()) / double(width());
      if(int(pointArray.size()) != width())
	{
	  pointArray.resize(width());
	}
    
      active->lock();
      data = active->dataAtCurrentChunk();
      if(data)
	{
	  double freq = data->getFundamentalFreq();
	  double period = double(active->rate()) / freq;
	  //pixels per period
	  double scaleX = period * double(width()) / double(active->get_nsdf_data().size());
      
	  //draw alternating background color indicating period
	  if(gdata->getView().backgroundShading() && period > 4.0 && period < double(active->get_nsdf_data().size()))
	    {
	      //number of colored patches
	      int n = int(ceil(double(width()) / scaleX));
	      m_painter.setPen(Qt::NoPen);
	      QColor color1 = colorBetween(gdata->backgroundColor(), gdata->shading1Color(), data->getCorrelation());
	      QColor color2 = colorBetween(gdata->backgroundColor(), gdata->shading2Color(), data->getCorrelation());
	      for(j = 0; j < n; j++)
		{
		  x = toInt(scaleX * double(j));
		  m_painter.setBrush((j % 2) ? color1 : color2);
		  m_painter.drawRect(x, 0, toInt(scaleX * double(j + 1)) - toInt(scaleX * double(j)), height());
		}
	      m_painter.setPen(colorBetween(gdata->backgroundColor(), Qt::black, 0.3 * data->getCorrelation()));
	      for(j = 0; j < n; j++)
		{
		  x = toInt(scaleX * double(j));
		  m_painter.drawLine(x, 0, x, height());
		}
	    }
	  else
	    {
	      clearBackground();
	    }
	  QString numPeriodsText;
	  numPeriodsText.sprintf("Period = %lf", period);
	  m_painter.setPen(Qt::black);
	  m_painter.drawText(5, height() - 8, numPeriodsText);

	  m_painter.drawLine(0, height() / 2, width(), height() / 2);

	  //draw the waveform
	  double ratio = double(height()) / 2.0; //TODO: remove magic number
	  m_painter.setPen(QPen(active->get_color(), 0));
	  for(int j = 0; j < width(); j++)
	    {
	      //cheap hack to go faster (by drawing less points)
	      myassert(int(pixelStep*j) < active->get_cepstrum_data().size());
	      pointArray.setPoint(j, j, height() / 2 - toInt(active->get_cepstrum_data().at(int(pixelStep * j)) * ratio));
	    }
	  m_painter.drawPolyline(pointArray);

	  if(data->getCepstrumIndex() >= 0)
	    {
	      m_painter.setPen(Qt::blue);
	      m_painter.setBrush(Qt::blue);
	      int x1 = toInt(double(data->getCepstrumIndex()) / double(active->get_cepstrum_data().size()) * double(width()));
	      int y1 = height() / 2 - toInt(active->get_cepstrum_data().at(data->getCepstrumIndex()) * height() / 2);
	      m_painter.drawEllipse(x1 - 2, y1 - 2, 5, 5);
	    }
	}
      else
	{
	  clearBackground();
	}
      active->unlock();
    }
  else
    {
      clearBackground();
    }
  endDrawing();
}

//------------------------------------------------------------------------------
QSize CepstrumWidget::sizeHint(void) const
{
  return QSize(500, 128);
}
// EOF
