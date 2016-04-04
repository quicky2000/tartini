/***************************************************************************
                          freqdrawwidget.cpp  -  description
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


#include "freqdrawwidget.h"
#include "gdata.h"
//#include "mycolor.h"
#include "channel.h"
#include "useful.h"
#include "soundfile.h" // Temporarily!!!

#include <qpixmap.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qsimplerichtext.h>

// zoom cursors
#include "pics/zoomx.xpm"
#include "pics/zoomy.xpm"
#include "pics/zoomxout.xpm"
#include "pics/zoomyout.xpm"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif


FreqDrawWidget::FreqDrawWidget(QWidget *parent, const char* name)
  : DrawWidget(parent, name, Qt::WDestructiveClose)
{
  setMouseTracking(true);
  
   //offset_y = 0.0;
   //offset_x = 0;
   dragMode = DragNone;
   //mouseDown = false;
   //dragCenter = false;
   //mouseX = mouseY = 0;

   QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
   setSizePolicy(sizePolicy);

   setFocusPolicy(QWidget::StrongFocus);
   gdata->view->setPixelHeight(height());
}

FreqDrawWidget::~FreqDrawWidget()
{
}

void FreqDrawWidget::paintEvent(QPaintEvent *)
{
  View *view = gdata->view;

  if(view->autoFollow() && gdata->getActiveChannel() && gdata->running == STREAM_FORWARD)
    setChannelVerticalView(gdata->getActiveChannel(), view->viewLeft(), view->currentTime(), view->zoomX(), view->viewBottom(), view->zoomY());
    
  beginDrawing();

  //draw the red/blue background color shading if needed
  if(view->backgroundShading() && gdata->getActiveChannel())
    drawChannelFilled(gdata->getActiveChannel(), p, view->viewLeft(), view->currentTime(), view->zoomX(), view->viewBottom(), view->zoomY(), DRAW_VIEW_NORMAL);
  
  // Draw the lines and notes
  QFontMetrics fm = p.fontMetrics();
  int fontHeightSpace = fm.height() / 4;
  int fontWidth = fm.width("C#0") + 3;
  
  double step = 1.0 / view->zoomY(); //number of pixels to jump between each line
  int noteJump; //in semi-tones
  //if(step > 20.0) {
  if(step > 10.0) {
    noteJump = 1;
  //} else if(step > 10.0) {
  //  noteJump = 2;
  } else if(step > 5.0) {
    noteJump = 6;
  } else {
    noteJump = 12;
  }
  double remainder = cycle(view->viewBottom(), double(noteJump));
  double toFirstNote = double(noteJump) - remainder;
  double start = toFirstNote * step;
  //double stop = start + (view->viewHeight() * step);
  double stop = double(height());
  //int nameIndex = int(ceil(view->viewBottom() / double(noteJump)) * double(noteJump));
  int nameIndex = toInt(view->viewBottom() + toFirstNote);
  step *= noteJump;

  //draw the note names and reference lines
  QString noteLabel;
  int lineY = 0;
  for (double y = start; y < stop; y += step, nameIndex+=noteJump) {
    lineY = height() - toInt(y);
    if(!isBlackNote(nameIndex)) {
      p.setPen(colorGroup().text());
//      if(view->backgroundShading()) {
//        noteLabel.sprintf("<qt>%s<small>%d</small></qt>", noteName(nameIndex), noteOctave(nameIndex));
//        QSimpleRichText noteRichText(noteLabel, font());
//        noteRichText.draw(&p, 2, lineY - noteRichText.height()/2, QRect(), colorGroup());
//      } else {
        noteLabel.sprintf("%s%d", noteName(nameIndex), noteOctave(nameIndex));
        //p.drawText(2, lineY + fontHeightSpace, noteName(nameIndex) + QString::number(noteOctave(nameIndex)));
        p.drawText(2, lineY + fontHeightSpace, noteLabel);
//      }
      if(noteValue(nameIndex) == 0) {
        p.setPen(QPen(Qt::black, 1, Qt::DashDotDotLine));
      } else {
        p.setPen(QPen(QColor(144, 156, 170), 1, Qt::DashDotDotLine));
      }
    } else {
      p.setPen(QPen(QColor(136, 161, 180), 0, Qt::DotLine));
    }
#ifdef MACX
    if(view->backgroundShading()) {
      int pixelOffset = fontWidth;
      int numLineParts = (width() - pixelOffset + 63) / 64;
      QPointArray macPointArray(numLineParts*6);
      for(int j=0; j<numLineParts*6; pixelOffset+=64) {
        macPointArray.setPoint(j++, pixelOffset, lineY);
        macPointArray.setPoint(j++, pixelOffset+24, lineY);
        macPointArray.setPoint(j++, pixelOffset+32, lineY);
        macPointArray.setPoint(j++, pixelOffset+40, lineY);
        macPointArray.setPoint(j++, pixelOffset+48, lineY);
        macPointArray.setPoint(j++, pixelOffset+56, lineY);
      }
      p.drawLineSegments(macPointArray, 0, numLineParts*3);
    } else {
      p.drawLine(fontWidth, lineY, width() - 1, lineY);
    }
#else
    p.drawLine(fontWidth, lineY, width() - 1, lineY);
#endif
  }

  //draw all the visible channels
  for (uint i = 0; i < gdata->channels.size(); i++) {
   	Channel *ch = gdata->channels.at(i);
   	if(!ch->isVisible()) continue;
    drawChannel(ch, p, view->viewLeft(), view->currentTime(), view->zoomX(), view->viewBottom(), view->zoomY(), DRAW_VIEW_NORMAL);
  }

  // Draw the current time line
  p.setPen(QPen(colorGroup().foreground(), 1));
  double curScreenTime = (view->currentTime() - view->viewLeft()) / view->zoomX();
  p.drawLine(toInt(curScreenTime), 0, toInt(curScreenTime), height() - 1);

  endDrawing();
}


Channel *FreqDrawWidget::channelAtPixel(int x, int y)
{
  double time = mouseTime(x);
  float note = mouseNote(y);
  float tolerance = 6 * gdata->view->zoomY(); //10 pixel tolerance

  std::vector<Channel*> channels;

  //loop through channels in reverse order finding which one the user clicked on
  for (std::vector<Channel*>::reverse_iterator it = gdata->channels.rbegin(); it != gdata->channels.rend();  it++) {
    if((*it)->isVisible()) {
      AnalysisData *data = (*it)->dataAtTime(time);
      if(data && within(tolerance, data->note, note)) return *it;
    }
  }
  return NULL;
}

/*
 * If control or alt is pressed, zooms. If shift is also pressed, it 'reverses' the zoom: ie ctrl+shift zooms x
 * out, alt+shift zooms y out. Otherwise, does internal processing.
 *
 * @param e the QMouseEvent to respond to.
 */
void FreqDrawWidget::mousePressEvent( QMouseEvent *e)
{
  View *view = gdata->view;
  int timeX = toInt(view->viewOffset() / view->zoomX());
  bool zoomed = false;
  dragMode = DragNone;
  
  
  //Check if user clicked on center bar, to drag it
  if(within(4, e->x(), timeX)) {
    dragMode = DragTimeBar;
    mouseX = e->x();
    return;
  }

  // If the control or alt keys are pressed, zoom in or out on the correct axis, otherwise scroll.
  if (e->state() & Qt::ControlButton) {
    // Do we zoom in or out?
    if (e->state() & Qt::ShiftButton) {
      //view->viewZoomOutX();
    } else {
      //view->viewZoomInX();
    }
	  zoomed = true;
  } else if (e->state() & Qt::AltButton) {
    // Do we zoom in or out?
    if (e->state() & Qt::ShiftButton) {
      //view->viewZoomOutY();
    } else {
      //view->viewZoomInY();
    }
	  zoomed = true;
  } else {
    //mouseDown = true;
    mouseX = e->x();
    mouseY = e->y();

    Channel *ch = channelAtPixel(e->x(), e->y());
    if(ch) { //Clicked on a channel
      gdata->setActiveChannel(ch);
      dragMode = DragChannel;
    } else {
      //Must have clicked on background
      dragMode = DragBackground;
      downTime = view->currentTime();
      downNote = view->viewBottom();
    }
  }
}

void FreqDrawWidget::mouseMoveEvent( QMouseEvent *e )
{
  View *view = gdata->view;
  int pixelAtCurrentTimeX = toInt(view->viewOffset() / view->zoomX());
  
  switch(dragMode) {
  case DragTimeBar:
    {
      int newX = pixelAtCurrentTimeX + (e->x() - mouseX);
      view->setViewOffset(double(newX) * view->zoomX());
      mouseX = e->x();
      view->doSlowUpdate();
	  }
    break;
  case DragBackground:
    //view->setCurrentTime(downTime - (e->x() - mouseX) * view->zoomX());
    view->setViewBottom(downNote - (mouseY - e->y()) * view->zoomY());
    gdata->updateActiveChunkTime(downTime - (e->x() - mouseX) * view->zoomX());
    view->doSlowUpdate();
    break;
  case DragNone:
    if(within(4, e->x(), pixelAtCurrentTimeX)) {
      setCursor(QCursor(Qt::SplitHCursor));
    } else if(channelAtPixel(e->x(), e->y()) != NULL) {
      setCursor(QCursor(Qt::PointingHandCursor));
    } else {
      unsetCursor();
    }
  }
}

void FreqDrawWidget::mouseReleaseEvent( QMouseEvent * )
{
  dragMode = DragNone;
}

/**
 Calculates at what time the mouse is.
 @param x the mouse's x co-ordinate
 @return the time the mouse is positioned at.
 */
double FreqDrawWidget::mouseTime(int x)
{
	return gdata->view->viewLeft() + gdata->view->zoomX() * x;

}


/**
 Calculates at what note the mouse is.
 @param x the mouse's y co-ordinate
 @return the note the mouse is positioned at.
 */
double FreqDrawWidget::mouseNote(int y)
{
	return gdata->view->viewBottom() + gdata->view->zoomY() * (height() - y);
}

void FreqDrawWidget::wheelEvent(QWheelEvent * e)
{
    View *view = gdata->view;
/*
    // If the control or alt keys are pressed, zoom in or out on the correct axis, otherwise scroll.
    if (e->state() & Qt::ControlButton) {
      if (e->delta() > 0) {
        //view->viewZoomInX();
      } else {
        //view->viewZoomOutX();
      }
    //} else if (e->state() == Qt::AltButton) {
    }
    if (e->state() & Qt::ShiftButton) {
      if (e->delta() > 0) {
        //view->viewZoomInY();
      } else {
        //view->viewZoomOutY();
      }
    }
*/
    if (!(e->state() & (Qt::ControlButton | Qt::ShiftButton))) {
      //view->setViewBottom(view->viewBottom() + double(e->delta()/WHEEL_DELTA) * view->viewHeight() * 0.1);
      if(e->delta() >= 0) { //zooming in
        if(gdata->running == STREAM_FORWARD) {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3);
        } else { //zoom toward mouse pointer
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, e->x());
        }
        view->setZoomFactorY(view->logZoomY() + double(e->delta()/WHEEL_DELTA)*0.2, height() - e->y());
      } else { //zoom out toward center
        if(gdata->running == STREAM_FORWARD) {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3);
        } else {
          view->setZoomFactorX(view->logZoomX() + double(e->delta()/WHEEL_DELTA)*0.3, width()/2);
        }
        view->setZoomFactorY(view->logZoomY() + double(e->delta()/WHEEL_DELTA)*0.2, height()/2);
      }
	  view->doSlowUpdate();
    }

    e->accept();
}

void FreqDrawWidget::resizeEvent (QResizeEvent *q)
{
    if (q->size() == q->oldSize()) return;
    
    View *v = gdata->view;

    double oldViewWidth = double(v->viewWidth());
    
    v->setPixelHeight(height());
    v->setPixelWidth(width());
    
    // Work out what the times/heights of the view should be based on the zoom factors
    //float newXTime = width() * v->zoomX();
    float newYHeight = height() * v->zoomY();
    float newYBottom = v->viewBottom() - ((newYHeight - v->viewHeight()) / 2.0);

    //v->setViewWidth(newXTime);
    v->setViewOffset(v->viewOffset() / oldViewWidth * v->viewWidth());

    //v->setViewHeight(newYHeight);
    v->setViewBottom(newYBottom);
}

/*
 * Changes the cursor icon to be one of the zoom ones depending on if the control or alt keys were pressed.
 * Otherwise, ignores the event.
 *
 * @param k the QKeyEvent to respond to.
 */
void FreqDrawWidget::keyPressEvent( QKeyEvent *k )
{
  switch (k->key()) {
    case Qt::Key_Control:
    setCursor(QCursor(QPixmap(zoomx)));
    break;
    case Qt::Key_Alt:
    setCursor(QCursor(QPixmap(zoomy)));
    break;
    case Qt::Key_Shift:
    if (k->state() & Qt::ControlButton) {
      setCursor(QCursor(QPixmap(zoomxout)));
    } else if (k->state() & Qt::AltButton) {
      setCursor(QCursor(QPixmap(zoomyout)));
    } else {
      k->ignore();
    }
    break;
    default:
    k->ignore();
    break;
  }
  
}

/*
 * Unsets the cursor icon if the control or alt key was released. Otherwise, ignores the event.
 *
 * @param k the QKeyEvent to respond to.
 */
void FreqDrawWidget::keyReleaseEvent( QKeyEvent *k)
{
  switch (k->key()) {
    case Qt::Key_Control: // Unset the cursor if the control or alt keys were released, ignore otherwise
    case Qt::Key_Alt:
      unsetCursor();
    break;
    case Qt::Key_Shift:
    if (k->state() & Qt::ControlButton) {
      setCursor(QCursor(QPixmap(zoomx)));
    } else if (k->state() & Qt::AltButton) {
      setCursor(QCursor(QPixmap(zoomy)));
    } else {
      k->ignore();
    }
    break;
    default:
    k->ignore();
    break;
  }
}

void FreqDrawWidget::leaveEvent ( QEvent * e) {
  unsetCursor();
  QWidget::leaveEvent(e);
}

void FreqDrawWidget::polish()
{
  //printf("width = %d\n", width());
  gdata->view->setPixelHeight(height());
  gdata->view->setPixelWidth(width());
  gdata->view->setViewOffset(gdata->view->viewWidth()/2.0);
}
