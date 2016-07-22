/***************************************************************************
                          hstackwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#include <Q3PointArray>
#include <QPaintEvent>

#include "hstackwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
HStackWidget::HStackWidget(QWidget *parent):
  DrawWidget(parent)
{
  //make the widget get updated when the view changes
  windowSize = 128;
  viewheight = 100;
  top = 0;

  connect(&(gdata->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

//------------------------------------------------------------------------------
void HStackWidget::setWindowSize(double _windowsize)
{
  if(windowSize != _windowsize)
    {
      windowSize = toInt(_windowsize);
      emit windowSizeChanged((float)windowSize);
    }
}

//------------------------------------------------------------------------------
void HStackWidget::setDBRange(double range)
{
  if(viewheight != range)
    {
      viewheight = range;
      emit dBRangeChanged(viewheight);
    }
}


//------------------------------------------------------------------------------
HStackWidget::~HStackWidget(void)
{
} 

//------------------------------------------------------------------------------
void HStackWidget::paintEvent(QPaintEvent *)
{
  Channel * active = gdata->getActiveChannel();
  Q3PointArray points;

  beginDrawing();

  if (active)
  {
    AnalysisData *data;
    active->lock();

    int startChunk = active->currentChunk() - windowSize / 2;
    int i,j;
    int numHarmonics = 16;


    float scaleY = height() / viewheight;
    float scaleX = (float)width() / (float)windowSize;
    int notOnGraph = height() + 10;

    Q3PointArray points[numHarmonics];

    for (j = 0; j < numHarmonics; j++)
    {
      points[j].resize(windowSize + 5);
    }

    QString s;
    get_painter().setPen(qRgb(128,128,128));

    float lbl;

    char * txt = (char*)"%1.1f";
    float increase = 10;

    if (viewheight < 50)
      {
	increase = 5;
      }
    if (viewheight < 10) 
    {
      txt = (char*)"%1.2f";
      increase = 1;
    }

    for (lbl = 0; lbl < viewheight; lbl += increase)
    {
      get_painter().drawLine(0, -toInt((-top + lbl) * scaleY), width(), -toInt((-top + lbl) * scaleY));
      get_painter().drawText(0, -toInt((-top + lbl) * scaleY), s.sprintf(txt, lbl));

      get_painter().drawLine(0, -toInt((-top - lbl) * scaleY), width(), -toInt((-top - lbl) * scaleY));
      get_painter().drawText(0, -toInt((-top - lbl) * scaleY), s.sprintf(txt, -lbl));
    }

    for (i = -1; i <= windowSize + 1; i++)
    {
      data = active->dataAtChunk(startChunk + i);
      if (data != 0)
      {
        int m = MIN(data->getHarmonicAmpNoCutOffSize(), (unsigned) numHarmonics);
        for (j = 0; j < m;j++)
        {
          if (!isinf(data->getHarmonicAmpNoCutOffAt(j)))
          {
            points[j].setPoint(i + 2,toInt(scaleX * (float)i),-toInt((-top + data->getHarmonicAmpNoCutOffAt(j)) * scaleY)); 
          }
          else
	    {
	      points[j].setPoint(i + 2,toInt(scaleX * (float)i), notOnGraph); 
	    }
        }
        for (j = m; j < numHarmonics; j++)
	  {
	    points[j].setPoint(i + 2,toInt(scaleX * (float)i),notOnGraph); 
	  }
      }
      else
	{
	  for (j = 0; j < numHarmonics;j++)
	    {
	      points[j].setPoint(i + 2,toInt(scaleX * (float)i),notOnGraph); 
	    }
	}
    }

    for (i = 0; i < numHarmonics; i++)
    {
      points[i].setPoint(0, -1, notOnGraph);
      points[i].setPoint(windowSize + 4, width() + 1, notOnGraph);
      get_painter().setBrush(colorBetween(qRgb(255,255,255), qRgb(0, 255, 0), float(i) / numHarmonics));
      get_painter().setPen(colorBetween(qRgb(128,128,128), qRgb(0, 128, 0), float(i) / numHarmonics));
      get_painter().drawPolygon(points[i]);
    }

    get_painter().drawLine(width() / 2, 0, width() / 2, height());
    get_painter().drawLine(width() - 1, 0, width() - 1, height());

    active->unlock();
  } 

  endDrawing();
}

// EOF
