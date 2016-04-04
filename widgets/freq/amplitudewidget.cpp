/***************************************************************************
                          amplitudewidget.cpp  -  description
                             -------------------
    begin                : 19 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/


#include "amplitudewidget.h"
#include "gdata.h"
#include "channel.h"
#include "zoomlookup.h"
#include "qcursor.h"

#include <qpixmap.h>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

AmplitudeWidget::AmplitudeWidget(QWidget *parent, const char* name)
  : DrawWidget(parent, name, Qt::WDestructiveClose)
{
  setMouseTracking(true);
  
  dragMode = DragNone;
  
  //QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
  //setSizePolicy(sizePolicy);
  
  //setFocusPolicy(QWidget::StrongFocus);
  setDBRange(120.0);
  //lineWidth = 2;
  //lineTopHalfWidth = lineWidth/2 + (lineWidth % 2);
  //lineBottomHalfWidth = lineWidth/2;
}

AmplitudeWidget::~AmplitudeWidget()
{
}

void AmplitudeWidget::setDBRange(double range)
{
  if(_dBRange != range) {
    _dBRange = range;
    emit dBRangeChanged(_dBRange);
  }
}

void AmplitudeWidget::paintEvent(QPaintEvent *)
{
  View *view = gdata->view;
  beginDrawing();

  //double step = 1.0 / view->zoomY();
  //double start = (1.0 - fmod(view->viewBottom(), 1.0)) * step;
  //double stop = start + (view->viewHeight() * step);
  //int nameIndex = int(ceil(view->viewBottom()));

  //draw the red/blue background color shading if needed
  if(view->backgroundShading() && gdata->getActiveChannel())
    drawChannelAmplitudeFilled(gdata->getActiveChannel(), p);
  
  //draw all the visible channels
  for (uint i = 0; i < gdata->channels.size(); i++) {
    Channel *ch = gdata->channels.at(i);
    if(!ch->isVisible()) continue;
    //drawChannel(ch, p, view->viewLeft(), view->currentTime(), view->zoomX(), view->viewBottom(), view->zoomY(), DRAW_VIEW_NORMAL);
    drawChannelAmplitude(ch, p);
  }

  // Draw the current time line
  p.setPen(QPen(colorGroup().foreground(), 1));
  double curScreenTime = (view->currentTime() - view->viewLeft()) / view->zoomX();
  p.drawLine(toInt(curScreenTime), 0, toInt(curScreenTime), height() - 1);

  double heightRatio = double(height()) / dBRange();
  int y = -toInt(gdata->noiseThresholdDB() * heightRatio);
  // Draw a horizontal line at the noise threshold.
  p.setPen(QPen(Qt::black, 1));
  p.drawLine(0, y, width(), y);

  //display the threshold value
  QString thresholdStr;
  thresholdStr.sprintf("Noise threshold = %0.1lfdB", gdata->noiseThresholdDB());
  p.drawText(2, 13, thresholdStr);
  
  endDrawing();
}

/** This function has the side effect of changing ze
*/
void AmplitudeWidget::calculateZoomElement(ZoomElement &ze, Channel *ch, int startChunk, int finishChunk)
{
  myassert(startChunk < finishChunk);
  if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
    //std::pair<AnalysisData*, AnalysisData*> a =
    //  minMaxElement(ch->dataAtChunk(0)+startChunk, ch->dataAtChunk(0)+finishChunk, lessRMS());
    //myassert(a.second != ch->dataAtChunk(0)+finishChunk);
    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
      minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessRMS());
    myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
    ze.low = (a.first->rms > 0.0f) ? log10(a.first->rms) * 20.0 : -160.0f;
    ze.high = (a.second->rms > 0.0f) ? log10(a.second->rms) * 20.0 : -160.0f;
  } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
    //std::pair<AnalysisData*, AnalysisData*> a =
    //  minMaxElement(ch->dataAtChunk(0)+startChunk, ch->dataAtChunk(0)+finishChunk, lessMaxIntensity());
    //myassert(a.second != ch->dataAtChunk(0)+finishChunk);
    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
      minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessMaxIntensity());
    myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
    ze.low = (a.first->maxIntensity > 0.0f) ? log10(a.first->maxIntensity) * 20.0 : -160.0f;
    ze.high = (a.second->maxIntensity > 0.0f) ? log10(a.second->maxIntensity) * 20.0 : -160.0f;
  } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
    //std::pair<AnalysisData*, AnalysisData*> a =
    //  minMaxElement(ch->dataAtChunk(0)+startChunk, ch->dataAtChunk(0)+finishChunk, lessMaxIntensity());
    //myassert(a.second != ch->dataAtChunk(0)+finishChunk);
    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
      minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessCorrelation());
    myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
    ze.low  = (a.first->correlation-1.0f) * 160.0f;
    ze.high = (a.second->correlation-1.0f) * 160.0f;
  } else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
    //std::pair<AnalysisData*, AnalysisData*> a =
    //  minMaxElement(ch->dataAtChunk(0)+startChunk, ch->dataAtChunk(0)+finishChunk, lessMaxIntensity());
    //myassert(a.second != ch->dataAtChunk(0)+finishChunk);
    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
      minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessVolumeValue());
    myassert(a.second != ch->dataIteratorAtChunk(finishChunk));
    //ze.low  = (a.first->volumeValue>0.0f)  ? log10(a.first->volumeValue)  * 20.0 : -200.0f;
    //ze.high = (a.second->volumeValue>0.0f) ? log10(a.second->volumeValue) * 20.0 : -200.0f;
    //ze.low  = (a.first->volumeValue-1.0f) * 160.0f;
    //ze.high = (a.second->volumeValue-1.0f) * 160.0f;
    ze.low  = a.first->volumeValue;
    ze.high = a.second->volumeValue;
  }
  
  //ze.color = colorBetween(colorGroup().background(), ch->color, err->volumeValue);
  ze.color = ch->color;
}

void AmplitudeWidget::drawChannelAmplitude(Channel *ch, QPainter &p)
{
  View *view = gdata->view;
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->amplitudeZoomLookup;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view->zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view->currentTime());
  double leftFrameTime = currentChunk - ((view->currentTime() - view->viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  double heightRatio = double(height()) / dBRange();
  
  QPointArray pointArray(width()*2);
  int pointIndex = 0;

  if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    //if(baseElement + theWidth > z->size()) z->setSize(baseElement + theWidth);
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      if(!z->hasValue(baseElement)) {
        int startChunk = int(floor(double(baseElement) * baseX));
        myassert(startChunk <= ch->totalChunks());
        int finishChunk = int(floor(double(baseElement+1) * baseX)) + 1;
        myassert(finishChunk <= ch->totalChunks());
        if(startChunk == finishChunk) continue;
        
        calculateZoomElement(ze, ch, startChunk, finishChunk);
        z->setHasValue(baseElement);
      }
      
      pointArray.setPoint(pointIndex++, n, -toInt(ze.high * heightRatio) - lineTopHalfWidth);
      pointArray.setPoint(pointIndex++, n, -toInt(ze.low  * heightRatio) + lineBottomHalfWidth);
    }
    p.setPen(ch->color);
    myassert(pointIndex <= width()*2);
    p.drawLineSegments(pointArray, 0, pointIndex/2);
  } else { //baseX <= 1
    float err = 0.0;
    float val = 0.0;
    int intChunk = (int) floor(leftFrameTime); // Integer version of frame time
    double stepSize = 1.0 / baseX; // So we skip some pixels
    int x = 0, y;

    //double start = 0 - stepSize;
    double start = (double(intChunk) - leftFrameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    //int halfSquareSize = squareSize/2;

    double dn = start;
    int totalChunks = ch->totalChunks();
    if(intChunk < 0) { dn += stepSize * -intChunk; intChunk = 0; }
    for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++) {
      AnalysisData *data = ch->dataAtChunk(intChunk);
      myassert(data);
      
      if(!data) continue;
      err = data->correlation;

      if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
        val = (data->rms > 0.0f) ? log10(data->rms) * 20.0 : -160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
        val = (data->maxIntensity > 0.0f) ? log10(data->maxIntensity) * 20.0 : -160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
        val = (data->correlation-1.0f) * 160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
        //val = (data->volumeValue>0.0f) ? log10(data->volumeValue) * 20.0 : -200.0f;
        val = data->volumeValue;
      }
      //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, val), 1));

      x = toInt(dn);
      y = -toInt(val * heightRatio);
      pointArray.setPoint(pointIndex++, x, y);
    }
    myassert(pointIndex <= width()*2);
    p.setPen(QPen(ch->color, lineWidth));
    p.drawPolyline(pointArray, 0, pointIndex);
  }
}

void AmplitudeWidget::drawChannelAmplitudeFilled(Channel *ch, QPainter &p)
{
  View *view = gdata->view;
  
  ChannelLocker channelLocker(ch);
  ZoomLookup *z = &ch->amplitudeZoomLookup;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = view->zoomX() / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(view->currentTime());
  double leftFrameTime = currentChunk - ((view->currentTime() - view->viewLeft()) / ch->timePerChunk());
  int n = 0;
  int baseElement = int(floor(leftFrameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  double heightRatio = double(height()) / dBRange();
  
  int firstN = n;
  int lastN = firstN;
  
  //QPointArray pointArray(width()*2);
  //QPointArray topPoints(width()*2);
  QPointArray bottomPoints(width()*2);
  //int pointIndex = 0;
  int topBottomPointIndex = 0;

  if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    //if(baseElement + theWidth > z->size()) z->setSize(baseElement + theWidth);
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      if(!z->hasValue(baseElement)) {
        int startChunk = int(floor(double(baseElement) * baseX));
        myassert(startChunk <= ch->totalChunks());
        int finishChunk = int(floor(double(baseElement+1) * baseX)) + 1;
        myassert(finishChunk <= ch->totalChunks());
        if(startChunk == finishChunk) continue;
        
        calculateZoomElement(ze, ch, startChunk, finishChunk);
        z->setHasValue(baseElement);
      }
      
      int y = -toInt(ze.high  * heightRatio);
      //topPoints.setPoint(topBottomPointIndex, n, 0);
      bottomPoints.setPoint(topBottomPointIndex++, n, y);
      //topPoints.setPoint(topBottomPointIndex, n, y);
      bottomPoints.setPoint(topBottomPointIndex++, n, height());
      lastN = n;
    }
    //p.setPen(gdata->shading1Color());
    //p.drawLineSegments(topPoints, 0, topBottomPointIndex/2);
    p.setPen(Qt::NoPen);
    p.setBrush(gdata->shading1Color());
    p.drawRect(firstN, 0, lastN, height());
    p.setPen(gdata->shading2Color());
    p.drawLineSegments(bottomPoints, 0, topBottomPointIndex/2);
  } else { //baseX <= 1
    float err = 0.0;
    float val = 0.0;
    int intChunk = (int) floor(leftFrameTime); // Integer version of frame time
    double stepSize = 1.0 / baseX; // So we skip some pixels
    int x = 0, y;

    //double start = 0 - stepSize;
    double start = (double(intChunk) - leftFrameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    //int halfSquareSize = squareSize/2;

    double dn = start;
    int totalChunks = ch->totalChunks();
    if(intChunk < 0) { dn += stepSize * -intChunk; intChunk = 0; }
    //topPoints.setPoint(topBottomPointIndex, toInt(dn), 0);
    bottomPoints.setPoint(topBottomPointIndex++, toInt(dn), height());
    
    firstN = toInt(dn);
    for(; dn < stop && intChunk < totalChunks; dn += stepSize, intChunk++) {
      AnalysisData *data = ch->dataAtChunk(intChunk);
      myassert(data);
      
      if(!data) continue;
      err = data->correlation;

      if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
        val = (data->rms > 0.0f) ? log10(data->rms) * 20.0 : -160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
        val = (data->maxIntensity > 0.0f) ? log10(data->maxIntensity) * 20.0 : -160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
        val = (data->correlation-1.0f) * 160.0f;
      } else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
        //val = (data->volumeValue>0.0f) ? log10(data->volumeValue) * 20.0 : -200.0f;
        val = data->volumeValue;
      }
      //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, val), 1));

      x = toInt(dn);
      y = -toInt(val * heightRatio);
      //topPoints.setPoint(topBottomPointIndex, x, y);
      bottomPoints.setPoint(topBottomPointIndex++, x, y);
      lastN = x;
    }
    //topPoints.setPoint(topBottomPointIndex, topPoints.point(topBottomPointIndex-1).x(), 0);
    bottomPoints.setPoint(topBottomPointIndex, bottomPoints.point(topBottomPointIndex-1).x(), height());
    topBottomPointIndex++;
    
    //p.setPen(gdata->shading1Color());
    //p.setBrush(gdata->shading1Color());
    //myassert(topBottomPointIndex <= width()*2);
    //p.drawPolygon(topPoints, false, 0, topBottomPointIndex);
    p.setPen(Qt::NoPen);
    p.setBrush(gdata->shading1Color());
    p.drawRect(firstN, 0, lastN, height());
    p.setPen(gdata->shading2Color());
    p.setBrush(gdata->shading2Color());
    p.drawPolygon(bottomPoints, false, 0, topBottomPointIndex);
  }
}

void AmplitudeWidget::mousePressEvent(QMouseEvent *e)
{
  View *view = gdata->view;
  int timeX = toInt(view->viewOffset() / view->zoomX());
  int pixelAtCurrentNoiseThresholdY = -toInt(gdata->noiseThresholdDB() / dBRange() * double(height()));
  dragMode = DragNone;
  
  //Check if user clicked on center bar, to drag it
  if(within(4, e->x(), timeX)) {
    dragMode = DragTimeBar;
    mouseX = e->x();
    return;
  } else if(within(4, e->y(), pixelAtCurrentNoiseThresholdY)) {
    dragMode = DragNoiseThreshold;
    mouseY = e->y();
    return;
  } else {
    mouseX = e->x();
    dragMode = DragBackground;
    downTime = view->currentTime();
  }
}

void AmplitudeWidget::mouseMoveEvent(QMouseEvent *e)
{
  View *view = gdata->view;
  int pixelAtCurrentTimeX = toInt(view->viewOffset() / view->zoomX());
  int pixelAtCurrentNoiseThresholdY = -toInt(gdata->noiseThresholdDB() / dBRange() * double(height()));
  
  switch(dragMode) {
  case DragTimeBar:
    {
      int newX = pixelAtCurrentTimeX + (e->x() - mouseX);
      view->setViewOffset(double(newX) * view->zoomX());
      mouseX = e->x();
      view->doSlowUpdate();
    }
    break;
  case DragNoiseThreshold:
    {
      //int newY = pixelAtCurrentNoiseThreshold + (e->y() - mouseY);
      int newY = e->y();
      gdata->setNoiseThresholdDB(double(-newY) / double(height()) * dBRange());
      mouseY = e->y();
      gdata->view->doSlowUpdate();
    }
    break;
  case DragBackground:
    //view->setViewBottom(downNote - (mouseY - e->y()) * view->zoomY());
    gdata->updateActiveChunkTime(downTime - (e->x() - mouseX) * view->zoomX());
    view->doSlowUpdate();
    break;
  case DragNone:
    if(within(4, e->x(), pixelAtCurrentTimeX)) {
	  setCursor(QCursor(Qt::SplitHCursor));
    } else if(within(4, e->y(), pixelAtCurrentNoiseThresholdY)) {
      setCursor(QCursor(Qt::SplitVCursor));
    } else {
      unsetCursor();
    }
  }
}

void AmplitudeWidget::mouseReleaseEvent(QMouseEvent *)
{
  dragMode = DragNone;
}

void AmplitudeWidget::wheelEvent(QWheelEvent * e)
{
    View *view = gdata->view;
    if (!(e->state() & (Qt::ControlButton | Qt::ShiftButton))) {
      //view->setViewBottom(view->viewBottom() + double(e->delta()/WHEEL_DELTA) * view->viewHeight() * 0.1);
      if(gdata->running == STREAM_FORWARD) {
        view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3);
      } else {
        if(e->delta() < 0) {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, width()/2);
        } else {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, e->x());
        }
      }
      view->doSlowUpdate();
    }

    e->accept();
}
