/***************************************************************************
                          correlationwidget.cpp  -  description
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
#include <qpixmap.h>
#include <qpainter.h>
#include <qmemarray.h>

#include "correlationwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"

CorrelationWidget::CorrelationWidget(QWidget *parent)
  : DrawWidget(parent)
{
}

CorrelationWidget::~CorrelationWidget()
{
}

void CorrelationWidget::paintEvent( QPaintEvent * )
{
  Channel *active = gdata->getActiveChannel();

  AnalysisData *data = NULL;
  double dh2 = double(height()-1) / 2.0;
  double pixelStep;
  int j, x, y;
    
  beginDrawing(false);
    
  if(active) {
    pixelStep = double(active->nsdfData.size()) / double(width());
    
    active->lock();
    data = active->dataAtCurrentChunk();

    //int centerX = width() / 2;
    if(data) {
      double freq = data->fundamentalFreq;
      double period = double(active->rate()) / freq;
      //double numPeriods = double(active->size()) / period;
      double scaleX = period * double(width()) / double(active->nsdfData.size()); //pixels per period
      
      //draw alternating background color indicating period
      if(gdata->view->backgroundShading() && period > 4.0 && period < double(active->nsdfData.size())) {
        int n = int(ceil(double(width()) / scaleX)); //number of colored patches
        p.setPen(Qt::NoPen);
        QColor color1 = colorBetween(gdata->backgroundColor(), gdata->shading1Color(), data->correlation);
        QColor color2 = colorBetween(gdata->backgroundColor(), gdata->shading2Color(), data->correlation);
        for(j = 0; j<n; j++) {
          x = toInt(scaleX*double(j));
          p.setBrush((j%2) ? color1 : color2);
          p.drawRect(x, 0, toInt(scaleX*double(j+1)) - toInt(scaleX*double(j)), height());
        }
        p.setPen(colorBetween(gdata->backgroundColor(), Qt::black, 0.3*data->correlation));
        for(j = 0; j<n; j++) {
          x = toInt(scaleX*double(j));
          p.drawLine(x, 0, x, height());
        }
      } else {
        clearBackground();
      }
      QString numPeriodsText;
      numPeriodsText.sprintf("Period = %lf", period);
      p.setPen(Qt::black);
      p.drawText(5, height() - 8, numPeriodsText);
    } else {
      clearBackground();
    }
  } else {
    clearBackground();
  }

  //draw the horizontal center line
  p.setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
  p.drawLine(0, toInt(dh2), width(), toInt(dh2));

  if(active) { 
    if(gdata->doingFreqAnalysis()) {
      int w = width() / 2; //only do every second pixel (for speed)
      //draw the waveform
      if(int(pointArray.size()) != w) pointArray.resize(w);

	  //Use Bresenham's algorithm in 1d to choose the points to draw
	  Array1d<float> &nsdfData = active->nsdfData;
	  int intStep = int(active->nsdfData.size() / w);
	  int remainderStep = active->nsdfData.size() - (intStep * w);
	  int pos = 0;
	  int remainder = 0;
	  for(int j=0; j<w; j++, pos+=intStep, remainder+=remainderStep) {
		  if(remainder >= w) {
			  pos++;
			  remainder -= w;
		  }
		  myassert(pos < active->nsdfData.size());
		  pointArray.setPoint(j, j*2, toInt(dh2 - nsdfData.at(pos)*dh2));
	  }
	  p.setPen(QPen(active->color, 0));
	  p.drawPolyline(pointArray);
	  
/*
      for(j=0; j<width(); j++) { //cheap hack to go faster (by drawing less points) - causes aliasing
        myassert(int(pixelStep*j) < active->nsdfData.size());
        pointArray.setPoint(j, j, toInt(dh2 - active->nsdfData.at(int(pixelStep*j))*dh2));
      }
	  p.setPen(QPen(active->color, 0));
      p.drawPolyline(pointArray);
*/
    }
    if(data) {
      double ratio = double(width()) / double(active->nsdfData.size()); //pixels per index
      //float highest = active->nsdfData.at(data->highestCorrelationIndex);
      //float chosen = active->nsdfData.at(data->chosenCorrelationIndex);
      
      //draw a dot at all the period estimates
      p.setPen(Qt::blue);
      p.setBrush(Qt::blue);
      for(j=0; j<int(data->periodEstimates.size()); j++) {
        x = toInt(double(data->periodEstimates[j]) * ratio);
        y = toInt(dh2 - data->periodEstimatesAmp[j] * dh2);
        p.drawEllipse(x-2, y-2, 5, 5);
      }
      
      if(data->highestCorrelationIndex >= 0) {
        float highest = data->periodEstimatesAmp[data->highestCorrelationIndex];
        //draw threshold line
        p.setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
        y = toInt(dh2 - (highest * active->threshold()) * dh2);
        p.drawLine(0, y, width(), y);
      
        //draw a dot at the highest correlation period
        p.setPen(Qt::black);
        p.setBrush(Qt::black);
        //x = toInt(double(data->highestCorrelationIndex) * ratio);
        x = toInt(double(data->periodEstimates[data->highestCorrelationIndex]) * ratio);
        y = toInt(dh2 - highest * dh2);
        p.drawEllipse(x-2, y-2, 5, 5);
      }
      
      //draw a dot at the chosen correlation period
      if(data->chosenCorrelationIndex >= 0) {
        p.setPen(Qt::red);
        p.setBrush(Qt::red);
        //x = toInt(double(data->chosenCorrelationIndex) * ratio);
        //y = toInt(dh2 - chosen * dh2);
        x = toInt(double(data->periodEstimates[data->chosenCorrelationIndex]) * ratio);
        y = toInt(dh2 - data->periodEstimatesAmp[data->chosenCorrelationIndex] * dh2);
        p.drawEllipse(x-2, y-2, 5, 5);
      }
    }
    
    active->unlock();
    
  }
  endDrawing();
}
