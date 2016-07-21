/***************************************************************************
                          correlationwidget.cpp  -  description
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
#include <qpixmap.h>
#include <qpainter.h>
#include <q3memarray.h>
#include <QPaintEvent>

#include "correlationwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"
#include "myalgo.h"

//------------------------------------------------------------------------------
CorrelationWidget::CorrelationWidget(QWidget *parent):
  DrawWidget(parent)
{
  aggregateMode = 0;
}

//------------------------------------------------------------------------------
CorrelationWidget::~CorrelationWidget(void)
{
}

//------------------------------------------------------------------------------
void CorrelationWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();

  AnalysisData *data = NULL;
  int chunk=0;
  double dh2 = double(height() - 1) / 2.0;
  int j, x, y;
    
  beginDrawing(false);
    
  if(active)
    {
    
      active->lock();
      chunk = active->currentChunk();
      data = active->dataAtChunk(chunk);

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
	      for(j = 0; j<n; j++)
		{
		  x = toInt(scaleX*double(j));
		  m_painter.setBrush((j%2) ? color1 : color2);
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
	}
      else
	{
	  clearBackground();
	}
    }
  else
    {
      clearBackground();
    }

  //draw the horizontal center line
  m_painter.setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
  m_painter.drawLine(0, toInt(dh2), width(), toInt(dh2));

  if(active)
    { 
      if(gdata->doingFreqAnalysis())
	{
	  //only do every second pixel (for speed)
	  int w = width() / 2;
	  //draw the waveform
	  if(int(pointArray.size()) != w)
	    {
	      pointArray.resize(w);
	    }
	  if(lookup.size() != w)
	    {
	      lookup.resize(w);
	    }

	  const NoteData *currentNote = active->getCurrentNote();
	  const Array1d<float> *input = &(active->get_nsdf_data());
	  if(currentNote)
	    {
	      if(aggregateMode == 1)
		{
		  input = &currentNote->get_nsdf_aggregate_data();
		}
	      else if(aggregateMode == 2)
		{
		  input = &currentNote->get_nsdf_aggregate_data_scaled();
		}
	    }
	  maxAbsDecimate1d(*input, lookup);
	  for(int j = 0; j < w; j++)
	    {
	      pointArray.setPoint(j, j * 2, toInt(dh2 - lookup[j] * dh2));
	    }

	  m_painter.setPen(QPen(active->get_color(), 0));
	  m_painter.drawPolyline(pointArray);
	}
      if(data && (aggregateMode == 0))
	{
	  //pixels per index
	  double ratio = double(width()) / double(active->get_nsdf_data().size());
      
	  //draw a dot at all the period estimates
	  m_painter.setPen(Qt::blue);
	  m_painter.setBrush(Qt::blue);
	  for(j = 0; j < int(data->getPeriodEstimatesSize()); j++)
	    {
	      x = toInt(double(data->getPeriodEstimatesAt(j)) * ratio);
	      y = toInt(dh2 - data->getPeriodEstimatesAmpAt(j) * dh2);
	      m_painter.drawEllipse(x - 2, y - 2, 5, 5);
	    }
      
	  if(data->getHighestCorrelationIndex() >= 0)
	    {
	      float highest = data->getPeriodEstimatesAmpAt(data->getHighestCorrelationIndex());
	      //draw threshold line
	      m_painter.setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
	      y = toInt(dh2 - (highest * active->threshold()) * dh2);
	      m_painter.drawLine(0, y, width(), y);
      
	      //draw a dot at the highest correlation period
	      m_painter.setPen(Qt::black);
	      m_painter.setBrush(Qt::black);
	      x = toInt(double(data->getPeriodEstimatesAt(data->getHighestCorrelationIndex())) * ratio);
	      y = toInt(dh2 - highest * dh2);
	      m_painter.drawEllipse(x - 2, y - 2, 5, 5);
	    }
      
	  //draw a dot at the chosen correlation period
	  if(data->getChosenCorrelationIndex() >= 0)
	    {
	      m_painter.setPen(Qt::red);
	      m_painter.setBrush(Qt::red);
	      x = toInt(double(data->getPeriodEstimatesAt(data->getChosenCorrelationIndex())) * ratio);
	      y = toInt(dh2 - data->getPeriodEstimatesAmpAt(data->getChosenCorrelationIndex()) * dh2);
	      m_painter.drawEllipse(x - 2, y - 2, 5, 5);
	    }

	  //draw a line at the chosen correlation period
	  if(data->getChosenCorrelationIndex() >= 0)
	    {
	      m_painter.setPen(Qt::green);
	      m_painter.setBrush(Qt::green);
	      x = toInt(double(active->periodOctaveEstimate(chunk)) * ratio);
	      m_painter.drawLine(x, 0, x, height());
	    }
	}
      active->unlock();
    }
  endDrawing();
}

//------------------------------------------------------------------------------
void CorrelationWidget::setAggregateMode(int mode)
{
  aggregateMode = mode;
  update();
}

//------------------------------------------------------------------------------
QSize CorrelationWidget::sizeHint(void) const
{
  return QSize(500, 128);
}

// EOF
