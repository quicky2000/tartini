/***************************************************************************
                          drawwidget.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
 
//#include <qmutex.h>
#include <qthread.h>
#include <utility>
#include <algorithm>
#include <qpixmap.h>

#include "drawwidget.h"
#include "channel.h"
#include "view.h"
#include "gdata.h"
#include "analysisdata.h"
#include "useful.h"
//#include "myqmutex.h"

#ifndef CERTAIN_THRESHOLD
#define CERTAIN_THRESHOLD 0.9
//#define CERTAIN_THRESHOLD 120.0
#endif

DrawWidget::DrawWidget(QWidget *parent, const char* name, WFlags f)
: QWidget(parent, name, f)
{
#ifdef SHARED_DRAWING_BUFFER
  buffer = gdata->drawingBuffer();
  buffer->resize(buffer->size().expandedTo(size()));
#else
  buffer = new QPixmap(size());
#endif
  
  setLineWidth(3);
  
  // With double buffering, we don't want QT to clear the display all the time
  setBackgroundMode(NoBackground);
}

DrawWidget::~DrawWidget()
{
#ifndef SHARED_DRAWING_BUFFER
  delete buffer;
#endif
}

void DrawWidget::beginDrawing(bool clearBackground_)
{
  checkSize();
  p.begin(buffer, this);
  if(clearBackground_) clearBackground();
}

void DrawWidget::endDrawing(bool drawToScreen_)
{
  p.end();
  if(drawToScreen_) drawToScreen();
}

void DrawWidget::drawToScreen()
{
  bitBlt(this, 0, 0, buffer, 0, 0, width(), height());
}

void DrawWidget::clearBackground()
{
  p.fillRect(0, 0, width(), height(), gdata->backgroundColor());
}

void DrawWidget::fillBackground(const QColor &color)
{
  p.fillRect(0, 0, width(), height(), color);
}

void DrawWidget::checkSize()
{
#ifndef SHARED_DRAWING_BUFFER
  if(buffer->size() != size()) buffer->resize(size()); //resize the local double buffer
#else
  if(width() > buffer->width() || height() > buffer->height()) buffer->resize(buffer->size().expandedTo(size()));
#endif
}

void DrawWidget::setLineWidth(int width)
{
  lineWidth = width;
  lineTopHalfWidth = lineWidth/2 + (lineWidth % 2);
  lineBottomHalfWidth = lineWidth/2;
}

/** Gives a color between a and b, using ratio (a number between 0 and 1)
*/
QColor DrawWidget::colorBetween(QColor a, QColor b, double ratio)
{
  //myassert(ratio >= 0.0 && ratio <= 1.0);
  ratio = MAX(MIN(ratio, 1.0), 0.0);
  return QColor(
    a.red()+int(ratio*float(b.red()-a.red())),
    a.green()+int(ratio*float(b.green()-a.green())),
    a.blue()+int(ratio*float(b.blue()-a.blue())));
}

void DrawWidget::drawChannel(Channel *ch, QPainter &p, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType)
{
  ZoomLookup *z;
  if(viewType == DRAW_VIEW_NORMAL) z = &ch->normalZoomLookup;
  else z = &ch->summaryZoomLookup;
    
  ChannelLocker channelLocker(ch);

  QColor current = ch->color;
 	QColor invert(255 - current.red(), 255 - current.green(), 255 - current.blue());
 	p.setPen(current);

 	int viewBottomOffset = toInt(viewBottom / zoomY);
  viewBottom = double(viewBottomOffset) * zoomY;
  
  // baseX is the no. of chunks a pixel must represent.
 	double baseX = zoomX / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(currentTime);
  double leftFrameTime = currentChunk - ((currentTime - leftTime) / ch->timePerChunk());
  
  //double leftFrameTime = leftTime / ch->timePerChunk();

 	double frameTime = leftFrameTime;
  //if(frameTime < 0.0) frameTime = 0.0;
  int n = 0;
  int baseElement = int(floor(frameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  
  QPointArray pointArray(width()*2);
  //QPointArray topPoints(width()*2);
  //QPointArray bottomPoints(width()*2);
  int pointIndex = 0;
  //int topBottomPointIndex = 0;
      
 	if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    //if(baseElement + theWidth > z->size()) z->setSize(baseElement + theWidth);
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      if(!z->hasValue(baseElement)) {
        int startChunk = toInt(double(baseElement) * baseX);
        int finishChunk = toInt(double(baseElement+1) * baseX) + 1;
        if(finishChunk >= (int)ch->totalChunks()) finishChunk--; //dont go off the end
        if(finishChunk >= (int)ch->totalChunks()) continue; //that data doesn't exist yet
        
        //std::pair<AnalysisData*, AnalysisData*> a =
        //  minMaxElement(ch->dataAtChunk(startChunk), ch->dataAtChunk(finishChunk), lessNote());
        //if(a.second == ch->dataAtChunk(finishChunk)) continue;
        std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
          minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessNote());
        if(a.second == ch->dataIteratorAtChunk(finishChunk)) continue;
        //AnalysisData* err = std::max_element(ch->dataAtChunk(startChunk), ch->dataAtChunk(finishChunk), lessVolumeValue());
        //if(err == ch->dataAtChunk(finishChunk)) continue;
        large_vector<AnalysisData>::iterator err = std::max_element(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessVolumeValue());
        if(err == ch->dataIteratorAtChunk(finishChunk)) continue;
        //if(err->isValid()) {
        //if(ch->isVisibleNote(err->noteIndex)) {
        if(ch->isVisibleChunk(&*err)) {
          ze.low = a.first->note;
          ze.high = a.second->note;
        } else {
          ze.low = 0;
          ze.high = 0;
        }
        ze.corr = err->correlation*sqrt(err->rms)*10.0;
        //printf("size=%d, baseElement=%d\n", z->size(), baseElement);
        //printf("start=%d, finish=%d\n", start, finish);
        //printf("less=%f, high=%f\n", ze.noteLow, ze.noteHigh); fflush(stdout);

        if(gdata->pitchContourMode() == 0)
          //ze.color = colorBetween(colorGroup().background(), ch->color, err->correlation*2.0-1.0);
          ze.color = colorBetween(gdata->backgroundColor(), ch->color, ze.corr);
        else
          ze.color = ch->color;
        z->setHasValue(baseElement);
      }
     
      if(ze.high != 0.0f && ze.high - ze.low < 1.0) { //if range is closer than one semi-tone then draw a line between them
      //if(ze.noteLow > 0) {
        p.setPen(ze.color);
        //Note: lineTo doen't draw a pixel on the last point of the line
        p.drawLine(n, height() - lineTopHalfWidth - toInt(ze.high / zoomY) + viewBottomOffset, n, height() + lineBottomHalfWidth - toInt(ze.low / zoomY) + viewBottomOffset);
        //pointArray.setPoint(pointIndex++, n, height() - lineTopHalfWidth    - toInt(ze.high / zoomY) + viewBottomOffset);
        //pointArray.setPoint(pointIndex++, n, height() + lineBottomHalfWidth - toInt(ze.low  / zoomY) + viewBottomOffset);
      }
    }
    myassert(pointIndex <= width()*2);
    //p.setPen(ch->color);
    //p.drawLineSegments(pointArray, 0, pointIndex/2);

 	} else { // More pixels than samples
    float err = 0.0, note = 0.0, prevNote = 0.0;
    int intChunk = (int) floor(frameTime); // Integer version of frame time
    if(intChunk < 0) intChunk = 0;
    double stepSize = 1.0 / baseX; // So we skip some pixels
    int x = 0, y;
  
    //double start = 0 - stepSize;
    double start = (double(intChunk) - frameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    int halfSquareSize = squareSize/2;
    //topPoints.setPoint(topBottomPointIndex, toInt(start), 0);
    //bottomPoints.setPoint(topBottomPointIndex++, toInt(start), height());
    
    for (double n = start; n < stop && intChunk < (int)ch->totalChunks(); n += stepSize, intChunk++) {
      myassert(intChunk >= 0);
      //if (intChunk < 0) continue; // So we don't go off the beginning of the array
      AnalysisData *data = ch->dataAtChunk(intChunk);
      err = data->correlation;
      //if (err >= CERTAIN_THRESHOLD) {
      
      //float val = MIN(ch->dataAtChunk(intChunk)->volumeValue, 1.0);
      if(gdata->pitchContourMode() == 0)
        //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, err*2.0-1.0), lineWidth));
        p.setPen(QPen(colorBetween(gdata->backgroundColor(),  ch->color, err*sqrt(data->rms)*10.0), lineWidth));
      else
        p.setPen(QPen(ch->color, lineWidth));
      
      x = toInt(n);
      //note = (data->isValid()) ? data->note : 0.0f;
      //note = (ch->isVisibleNote(data->noteIndex)) ? data->note : 0.0f;
      note = (ch->isVisibleChunk(data)) ? data->note : 0.0f;
      myassert(note >= 0.0 && note <= gdata->topNote());
      //note = bound(note, 0, gdata->topNote());
      y = height() - 1 - toInt(note / zoomY) + viewBottomOffset;
      //y = height() - 1 - int((note / zoomY) - (viewBottom / zoomY));
      if(note > 0.0f) {
        if(fabs(prevNote - note) < 1.0 && n != start) { //if closer than one semi-tone from previous then draw a line between them
          p.lineTo(x, y);
        } else {
          p.drawPoint(x, y);
          p.moveTo(x, y);
        }
        if(stepSize > 10) { //draw squares on the data points
          //p.setPen(invert);
          p.setBrush(Qt::NoBrush);
          p.drawRect(x - halfSquareSize, y - halfSquareSize, squareSize, squareSize);
          //p.setPen(QPen(current, 2));
        }
        //} else {
        //  p.moveTo(x, height()-1-int(((note-viewBottom) / zoomY)));
        //}
      }
      prevNote = note;
    }
  }
}

void DrawWidget::drawChannelFilled(Channel *ch, QPainter &p, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType)
{
  ZoomLookup *z;
  if(viewType == DRAW_VIEW_NORMAL) z = &ch->normalZoomLookup;
  else z = &ch->summaryZoomLookup;
    
  ChannelLocker channelLocker(ch);

  QColor current = ch->color;
  QColor invert(255 - current.red(), 255 - current.green(), 255 - current.blue());
  p.setPen(current);

  int viewBottomOffset = toInt(viewBottom / zoomY);
  viewBottom = double(viewBottomOffset) * zoomY;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = zoomX / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(currentTime);
  double leftFrameTime = currentChunk - ((currentTime - leftTime) / ch->timePerChunk());
  
  //double leftFrameTime = leftTime / ch->timePerChunk();

  double frameTime = leftFrameTime;
  //if(frameTime < 0.0) frameTime = 0.0;
  int n = 0;
  int baseElement = int(floor(frameTime / baseX));
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  
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
        int startChunk = toInt(double(baseElement) * baseX);
        int finishChunk = toInt(double(baseElement+1) * baseX) + 1;
        if(finishChunk >= (int)ch->totalChunks()) finishChunk--; //dont go off the end
        if(finishChunk >= (int)ch->totalChunks()) continue; //that data doesn't exist yet
        
        //std::pair<AnalysisData*, AnalysisData*> a =
        //  minMaxElement(ch->dataAtChunk(startChunk), ch->dataAtChunk(finishChunk), lessNote());
        //if(a.second == ch->dataAtChunk(finishChunk)) continue;
        std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
          minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessNote());
        if(a.second == ch->dataIteratorAtChunk(finishChunk)) continue;
        //AnalysisData* err = std::max_element(ch->dataAtChunk(startChunk), ch->dataAtChunk(finishChunk), lessVolumeValue());
        //if(err == ch->dataAtChunk(finishChunk)) continue;
        large_vector<AnalysisData>::iterator err = std::max_element(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessVolumeValue());
        if(err == ch->dataIteratorAtChunk(finishChunk)) continue;
        //if(err->isValid()) {
        //if(ch->isVisibleNote(err->noteIndex)) {
        if(ch->isVisibleChunk(&*err)) {
          ze.low = a.first->note;
          ze.high = a.second->note;
        } else {
          ze.low = 0;
          ze.high = 0;
        }
        ze.corr = err->correlation*sqrt(err->rms)*10.0;
        //printf("size=%d, baseElement=%d\n", z->size(), baseElement);
        //printf("start=%d, finish=%d\n", start, finish);
        //printf("less=%f, high=%f\n", ze.noteLow, ze.noteHigh); fflush(stdout);

        if(gdata->pitchContourMode() == 0)
          //ze.color = colorBetween(colorGroup().background(), ch->color, err->correlation*2.0-1.0);
          ze.color = colorBetween(gdata->backgroundColor(), ch->color, ze.corr);
        else
          ze.color = ch->color;
        z->setHasValue(baseElement);
      }
     
      /*p.setPen(gdata->shading1Color());
      p.moveTo(n, 0);
      p.lineTo(n, height() - 1 - toInt(ze.high / zoomY) + viewBottomOffset);
      p.setPen(gdata->shading2Color());
      p.lineTo(n, height());*/
      int y = height() - 1 - toInt(ze.high / zoomY) + viewBottomOffset;
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

  } else { // More pixels than samples
    float err = 0.0, note = 0.0, prevNote = 0.0;
    int intChunk = (int) floor(frameTime); // Integer version of frame time
    if(intChunk < 0) intChunk = 0;
    double stepSize = 1.0 / baseX; // So we skip some pixels
    int x = 0, y;
  
    //double start = 0 - stepSize;
    double start = (double(intChunk) - frameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //int squareSize = (int(sqrt(stepSize)) / 2) * 2 + 1; //make it an odd number
    //int halfSquareSize = squareSize/2;
    //QPointArray topPoints(0);
    //QPointArray bottomPoints(0);
    //int pointIndex = 0;
    //topPoints.putPoints(pointIndex, 1, toInt(start), 0);
    //bottomPoints.putPoints(pointIndex, 1, toInt(start), height());
    //pointIndex++;
    //topPoints.setPoint(topBottomPointIndex, toInt(start), 0);
    bottomPoints.setPoint(topBottomPointIndex++, toInt(start), height());
    lastN = firstN = toInt(start);
    for (double n = start; n < stop && intChunk < (int)ch->totalChunks(); n += stepSize, intChunk++) {
      myassert(intChunk >= 0);
      //if (intChunk < 0) continue; // So we don't go off the beginning of the array
      AnalysisData *data = ch->dataAtChunk(intChunk);
      err = data->correlation;
      //if (err >= CERTAIN_THRESHOLD) {
      
      //float val = MIN(ch->dataAtChunk(intChunk)->volumeValue, 1.0);
      if(gdata->pitchContourMode() == 0)
        //p.setPen(QPen(colorBetween(colorGroup().background(), ch->color, err*2.0-1.0), lineWidth));
        p.setPen(QPen(colorBetween(gdata->backgroundColor(),  ch->color, err*sqrt(data->rms)*10.0), lineWidth));
      else
        p.setPen(QPen(ch->color, lineWidth));
      
      x = toInt(n);
      lastN = x;
      //note = (data->isValid()) ? data->note : 0.0f;
      //note = (ch->isVisibleNote(data->noteIndex)) ? data->note : 0.0f;
      note = (ch->isVisibleChunk(data)) ? data->note : 0.0f;
      myassert(note >= 0.0 && note <= gdata->topNote());
      //note = bound(note, 0, gdata->topNote());
      y = height() - 1 - toInt(note / zoomY) + viewBottomOffset;
      //y = height() - 1 - int((note / zoomY) - (viewBottom / zoomY));
      //topPoints.putPoints(pointIndex, 1, x, y);
      //bottomPoints.putPoints(pointIndex, 1, x, y);
      //pointIndex++;
      //topPoints.setPoint(topBottomPointIndex, x, y);
      bottomPoints.setPoint(topBottomPointIndex++, x, y);
      prevNote = note;
    }
    //topPoints.putPoints(pointIndex, 1, topPoints.point(pointIndex-1).x(), 0);
    //bottomPoints.putPoints(pointIndex, 1, bottomPoints.point(pointIndex-1).x(), height());
    //pointIndex++;
    //topPoints.setPoint(topBottomPointIndex, topPoints.point(topBottomPointIndex-1).x(), 0);
    bottomPoints.setPoint(topBottomPointIndex, bottomPoints.point(topBottomPointIndex-1).x(), height());
    topBottomPointIndex++;

    //p.setPen(gdata->shading1Color());
    //p.setBrush(gdata->shading1Color());
    //p.drawPolygon(topPoints);
    //p.setPen(gdata->shading2Color());
    //p.setBrush(gdata->shading2Color());
    //p.drawPolygon(bottomPoints);
  
    myassert(topBottomPointIndex <= width()*2);
    //p.setPen(gdata->shading1Color());
    p.setPen(Qt::NoPen);
    p.setBrush(gdata->shading1Color());
    //p.drawPolygon(topPoints, false, 0, topBottomPointIndex);
    p.drawRect(firstN, 0, lastN, height());
    //p.setPen(gdata->shading2Color());
    p.setBrush(gdata->shading2Color());
    p.drawPolygon(bottomPoints, false, 0, topBottomPointIndex);
  }
}

void DrawWidget::setChannelVerticalView(Channel *ch, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY)
{
  ZoomLookup *z = &ch->normalZoomLookup;
    
  ChannelLocker channelLocker(ch);

  int viewBottomOffset = toInt(viewBottom / zoomY);
  viewBottom = double(viewBottomOffset) * zoomY;

  std::vector<float> ys;
  ys.reserve(width());
  std::vector<float> weightings;
  weightings.reserve(width());
  float maxY = 0.0f, minY = gdata->topNote();
  float totalY = 0.0f;
  float numY = 0.0f;
  
  // baseX is the no. of chunks a pixel must represent.
  double baseX = zoomX / ch->timePerChunk();

  z->setZoomLevel(baseX);
  
  double currentChunk = ch->chunkFractionAtTime(currentTime);
  double leftFrameTime = currentChunk - ((currentTime - leftTime) / ch->timePerChunk());
  
  double frameTime = leftFrameTime;
  int n = 0;
  int currentBaseElement = int(floor(currentChunk / baseX));
  int firstBaseElement = int(floor(frameTime / baseX));
  int baseElement = firstBaseElement;
  if(baseElement < 0) { n -= baseElement; baseElement = 0; }
  int lastBaseElement = int(floor(double(ch->totalChunks()) / baseX));
  double leftBaseWidth = MAX(1.0, double(currentBaseElement - firstBaseElement));
  double rightBaseWidth = MAX(1.0, double(lastBaseElement - currentBaseElement));
  
/*
  We calculate the auto follow and scale by averaging all the note elements in view.
  We weight the frequency averaging by a triangle weighting function centered on the current time.
  Also it is weighted by the corr value of each frame.
             /|\
            / | \
           /  |  \
          /   |   \
            ^   ^
 leftBaseWidth rightBaseWidth
*/

  int firstN = n;
  int lastN = firstN;
  
  //QPointArray bottomPoints(width()*2);
  //int topBottomPointIndex = 0;
      
  if (baseX > 1) { // More samples than pixels
    int theWidth = width();
    if(lastBaseElement > z->size()) z->setSize(lastBaseElement);
    for(; n < theWidth && baseElement < lastBaseElement; n++, baseElement++) {
      myassert(baseElement >= 0);
      ZoomElement &ze = z->at(baseElement);
      if(!z->hasValue(baseElement)) {
        int startChunk = toInt(double(baseElement) * baseX);
        int finishChunk = toInt(double(baseElement+1) * baseX) + 1;
        if(finishChunk >= (int)ch->totalChunks()) finishChunk--; //dont go off the end
        if(finishChunk >= (int)ch->totalChunks()) continue; //that data doesn't exist yet
        
        std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
          minMaxElement(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessNote());
        if(a.second == ch->dataIteratorAtChunk(finishChunk)) continue;
        large_vector<AnalysisData>::iterator err = std::max_element(ch->dataIteratorAtChunk(startChunk), ch->dataIteratorAtChunk(finishChunk), lessVolumeValue());
        if(err == ch->dataIteratorAtChunk(finishChunk)) continue;
        if(ch->isVisibleChunk(&*err)) {
          ze.low = a.first->note;
          ze.high = a.second->note;
        } else {
          ze.low = 0.0f;
          ze.high = 0.0f;
        }
        ze.corr = err->correlation*sqrt(err->rms)*10.0;
        
        if(gdata->pitchContourMode() == 0)
          ze.color = colorBetween(gdata->backgroundColor(), ch->color, ze.corr);
        else
          ze.color = ch->color;
        z->setHasValue(baseElement);
      
      }
      if(ze.low > 0.0f && ze.high > 0.0f) {
        float weight = ze.corr;
        if(baseElement < currentBaseElement) weight *= double(currentBaseElement - baseElement) / leftBaseWidth;
        else if(baseElement > currentBaseElement) weight *= double(baseElement - currentBaseElement) / rightBaseWidth;
        if(ze.low < minY) minY = ze.low;
        if(ze.high > maxY) maxY = ze.high;
        totalY += (ze.low + ze.high) / 2.0f * weight;
        numY += weight;
        ys.push_back((ze.low + ze.high) / 2.0f);
        weightings.push_back(weight);
      }
/*
      int y = height() - 1 - toInt(ze.high / zoomY) + viewBottomOffset;
      bottomPoints.setPoint(topBottomPointIndex++, n, y);
      bottomPoints.setPoint(topBottomPointIndex++, n, height());
*/
      lastN = n;
    }
/*
    p.setPen(Qt::NoPen);
    p.setBrush(gdata->shading1Color());
    p.drawRect(firstN, 0, lastN, height());
    p.setPen(gdata->shading2Color());
    p.drawLineSegments(bottomPoints, 0, topBottomPointIndex/2);
*/
  } else { // More pixels than samples
    float err = 0.0, note = 0.0, prevNote = 0.0;
    int intChunk = (int) floor(frameTime); // Integer version of frame time
    if(intChunk < 0) intChunk = 0;
    double stepSize = 1.0 / baseX; // So we skip some pixels
    int x = 0, y;
    float corr;
    
    double start = (double(intChunk) - frameTime) * stepSize;
    double stop = width() + (2 * stepSize);
    //bottomPoints.setPoint(topBottomPointIndex++, toInt(start), height());
    lastN = firstN = toInt(start);
    for (double n = start; n < stop && intChunk < (int)ch->totalChunks(); n += stepSize, intChunk++) {
      myassert(intChunk >= 0);
      AnalysisData *data = ch->dataAtChunk(intChunk);
      err = data->correlation;
      
/*
      if(gdata->pitchContourMode() == 0)
        p.setPen(QPen(colorBetween(gdata->backgroundColor(),  ch->color, err*sqrt(data->rms)*10.0), lineWidth));
      else
        p.setPen(QPen(ch->color, lineWidth));
*/
      x = toInt(n);
      lastN = x;
      note = (ch->isVisibleChunk(data)) ? data->note : 0.0f;
      myassert(note >= 0.0 && note <= gdata->topNote());
      corr = data->correlation*sqrt(data->rms)*10.0;
      if(note > 0.0f) {
        float weight = corr;
        if(minY < note) minY = note;
        if(maxY > note) maxY = note;
        totalY += note * weight;
        numY += weight;
        ys.push_back(note);
        weightings.push_back(weight);
      }
      y = height() - 1 - toInt(note / zoomY) + viewBottomOffset;
      //bottomPoints.setPoint(topBottomPointIndex++, x, y);
      prevNote = note;
    }
    //bottomPoints.setPoint(topBottomPointIndex, bottomPoints.point(topBottomPointIndex-1).x(), height());
    //topBottomPointIndex++;

/*
    myassert(topBottomPointIndex <= width()*2);
    p.setPen(Qt::NoPen);
    p.setBrush(gdata->shading1Color());
    p.drawRect(firstN, 0, lastN, height());
    p.setBrush(gdata->shading2Color());
    p.drawPolygon(bottomPoints, false, 0, topBottomPointIndex);
*/
  }
  
  if(!ys.empty() > 0) {
    float meanY = totalY / numY;
    double spred = 0.0;
    myassert(ys.size() == weightings.size());
    //use a linear spred function. not a squared one like standard deviation
    for(uint j=0; j<ys.size(); j++) {
      spred += sq(ys[j] - meanY) * weightings[j];
    }
    spred = sqrt(spred / numY) * 4.0;
    //printf("spred = %f\n", spred);
    if(spred < 12.0) spred = 12.0; //show a minimum of 12 semi-tones
    gdata->view->setViewBottomRaw(meanY - gdata->view->viewHeight() / 2.0);
    //gdata->view->setLogZoomYRaw(log(height()/spred));
  }
}
