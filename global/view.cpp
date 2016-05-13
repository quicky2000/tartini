/***************************************************************************
                          view.cpp  -  description
                             -------------------
    begin                : Mon Jul 26 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "view.h"

#include "useful.h"
#include "gdata.h"
#include "channel.h"
#include "conversions.h"

View::View()
{
  _currentTime = 1.0; //to force a change in the setCurrentTime call
  setCurrentTime(0.0); //in seconds

  setLogZoomX(1.0);
  setLogZoomY(1.0);
  _pixelWidth = 0; //to force a change in the setPixelWidth call
  setPixelWidth(400);
  _pixelHeight = 0; //to force a change in the setPixelWidth call
  setPixelHeight(350);

  _viewOffset = 0.0; //to force a change in the setViewOffset call

  setZoomFactorY(3.2);

  _viewBottom = 0.0;
  setViewBottom(62.0); //the lowest note visible (in semitones) from C0

  _autoFollow = gdata->getSettingsValue("View/autoFollow", true);
  _backgroundShading = gdata->getSettingsValue("View/backgroundShading", true);

  needFastUpdate = false;
  needSlowUpdate = false;
  fastUpdateTimer = new QTimer(this);
  slowUpdateTimer = new QTimer(this);
  connect(fastUpdateTimer, SIGNAL(timeout()), this, SLOT(nextFastUpdate()));
  connect(slowUpdateTimer, SIGNAL(timeout()), this, SLOT(nextSlowUpdate()));
  connect(this, SIGNAL(viewChanged()), this, SLOT(newUpdate()));
}

View::~View()
{
  delete fastUpdateTimer;
  delete slowUpdateTimer;
}

//plase call this after a window has been created
void View::init()
{
  setViewOffset(viewWidth()/2.0);
}

void View::doUpdate()
{
  doSlowUpdate();
  doFastUpdate();
}

void View::doSlowUpdate()
{
  needSlowUpdate = false;
  if(!slowUpdateTimer->isActive()) {
    slowUpdateTimer->start(gdata->slowUpdateSpeed());
  }
  emit onSlowUpdate(_currentTime);
}

void View::doFastUpdate()
{
  needFastUpdate = false;
  if(!fastUpdateTimer->isActive()) {
    fastUpdateTimer->start(gdata->fastUpdateSpeed());
  }
  emit onFastUpdate(_currentTime);
}

void View::newUpdate()
{
  if(slowUpdateTimer->isActive()) {
	needSlowUpdate = true;
  } else {
  	needSlowUpdate = false;
  	slowUpdateTimer->start(gdata->slowUpdateSpeed());
    emit onSlowUpdate(_currentTime);
  }
  if(fastUpdateTimer->isActive()) {
    needFastUpdate = true;
  } else {
    needFastUpdate = false;
    fastUpdateTimer->start(gdata->fastUpdateSpeed());
    emit onFastUpdate(_currentTime);
  }
}

void View::nextFastUpdate()
{
  if(needFastUpdate) {
  	needFastUpdate = false;
  	fastUpdateTimer->start(gdata->fastUpdateSpeed());
    emit onFastUpdate(_currentTime);
  }
}

void View::nextSlowUpdate()
{
  if(needSlowUpdate) {
    needSlowUpdate = false;
    slowUpdateTimer->start(gdata->slowUpdateSpeed());
    emit onSlowUpdate(_currentTime);
  }
}

void View::setCurrentTimeRaw(double x)
{
  if(x != _currentTime) {
    Channel *active = gdata->getActiveChannel();
    if(active)
      x = active->timeAtChunk(active->chunkAtTime(x)); //align time to an integer sample step

    _currentTime = x;
  }
}

void View::setCurrentTime(double x)
{
  if(x != _currentTime) {
    Channel *active = gdata->getActiveChannel();
    if(active)
      x = active->timeAtChunk(active->chunkAtTime(x)); //align time to an integer sample step

    _currentTime = x;
    emit currentTimeChanged(x);
    emit timeViewRangeChanged(viewLeft(), viewRight());
    emit viewChanged();
  }
}
void View::setViewOffset(double x)
{
  if(x < 0) x = 0;
  if(x > viewWidth()) x = viewWidth();
  if(x != _viewOffset) {
    _viewOffset = x;
    emit timeViewRangeChanged(viewLeft(), viewRight());
    emit viewChanged();
  }
}

void View::setViewBottomRaw(double y)
{
  if(y != _viewBottom) {
    _viewBottom = y;
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-y);
  }
}

void View::setViewBottom(double y)
{
  if(y != _viewBottom) {
    _viewBottom = y;
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-y);
    emit viewChanged();
  }
}


// Changes the view without using a step value
void View::changeViewX(double x)
{
  setCurrentTime(x);
  emit viewChanged();
}

void View::changeViewY(double y)
{
  setViewBottom(gdata->topPitch() - viewHeight() - y);
  emit viewChanged();
}

void View::setPixelHeight(int h)
{
  if(h != _pixelHeight) {
    _pixelHeight = h;
    if(viewHeight() > gdata->topPitch()) {
      setLogZoomY(log(double(_pixelHeight) / gdata->topPitch()));
      emit logZoomYChanged(logZoomY());
    }
    emit scrollableYChanged(gdata->topPitch() - viewHeight());
    emit viewHeightChanged(viewHeight());
  }
}

void View::setPixelWidth(int w)
{
  if(w != _pixelWidth) {
    _pixelWidth = w;
    if(viewWidth() > gdata->totalTime() * 2.0) {
      setLogZoomX(log(double(_pixelWidth) / (gdata->totalTime() * 2.0)));
      emit logZoomXChanged(logZoomX());
    }
    emit viewWidthChanged(viewWidth());
  }
}

void View::setZoomFactorX(double x)
{
  if(x != logZoomX()) {
    double oldViewWidth = viewWidth();
    setLogZoomX(x);
    emit logZoomXChanged(logZoomX());
    emit viewWidthChanged(viewWidth());
    
    setViewOffset(viewOffset() * (viewWidth()/oldViewWidth));
  }
}

void View::setZoomFactorX(double x, int fixedX)
{
  if(x != logZoomX()) {
    double fixedTime = viewLeft() + zoomX() * double(fixedX);
    double oldViewWidth = viewWidth();
    setLogZoomX(x);
    emit logZoomXChanged(logZoomX());
    emit viewWidthChanged(viewWidth());
    
    double ratio = viewWidth() / oldViewWidth;
    setViewOffset(viewOffset() * ratio);
    
    //shift the current time to keep the keep the time fixed at the mouse pointer
    double newTime = viewLeft() + zoomX() * double(fixedX);
    gdata->updateActiveChunkTime(currentTime() - (newTime - fixedTime));
  }
}

void View::setZoomFactorY(double y)
{
  if(y != logZoomY()) {
    double prevCenterY = viewBottom() + viewHeight()/2.0;
    setLogZoomY(y);
    emit logZoomYChanged(logZoomY());
    emit scrollableYChanged(bound(gdata->topPitch() - viewHeight(), 0.0, gdata->topPitch()));
    emit viewHeightChanged(viewHeight());
    
    setViewBottom(prevCenterY - viewHeight()/2.0);
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-viewBottom());
  }
}

void View::setZoomFactorY(double y, int fixedY)
{
  if(y != logZoomY()) {
    double fixedNote = viewBottom() + zoomY() * fixedY;
    setLogZoomY(y);
    emit logZoomYChanged(logZoomY());
    emit scrollableYChanged(bound(gdata->topPitch() - viewHeight(), 0.0, gdata->topPitch()));
    emit viewHeightChanged(viewHeight());
    
    double newNote = viewBottom() + zoomY() * fixedY;
    setViewBottom(viewBottom() - (newNote - fixedNote));
    emit viewBottomChanged(gdata->topPitch()-viewHeight()-viewBottom());
  }
}

void View::doAutoFollowing()
{
  if(!autoFollow()) return;
  double time = currentTime();
  // We want the average note value for the time period currentTime to viewRight
 
  Channel *active = gdata->getActiveChannel();

  if (active == NULL) {
    return;
  }
  

  double startTime = time - (viewWidth() / 8.0);
  double stopTime = time + (viewWidth() / 8.0);
  
  int startFrame = bound(toInt(startTime / active->timePerChunk()), 0, active->totalChunks()-1);
  int stopFrame = bound(toInt(stopTime / active->timePerChunk()), 0, active->totalChunks()-1);

  float pitch = active->averagePitch(startFrame, stopFrame);

  if (pitch < 0) return; // There were no good notes detected

  float newBottom = pitch - (viewHeight() / 2.0);

  setViewBottom(newBottom);
}

void View::setAutoFollow(bool isChecked)
{
	_autoFollow = isChecked;
  gdata->setSettingsValue("View/autoFollow", isChecked);
}

void View::setBackgroundShading(bool isChecked)
{
  _backgroundShading = isChecked;
  gdata->setSettingsValue("View/backgroundShading", isChecked);
  emit onSlowUpdate(_currentTime);
}
