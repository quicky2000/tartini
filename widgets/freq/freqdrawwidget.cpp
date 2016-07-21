/***************************************************************************
                          freqdrawwidget.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
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
#include <qcursor.h>
#include <q3simplerichtext.h>
#include <QMouseEvent>
#include <Q3PointArray>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPaintEvent>

// zoom cursors
#include "pics/zoomx.xpm"
#include "pics/zoomy.xpm"
#include "pics/zoomxout.xpm"
#include "pics/zoomyout.xpm"

#include "freqdrawwidget.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "musicnotes.h"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
FreqDrawWidget::FreqDrawWidget(QWidget *parent, const char* name):
  DrawWidget(parent, name, Qt::WDestructiveClose)
{
  setMouseTracking(true);
  
   dragMode = DragNone;

   QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
   setSizePolicy(sizePolicy);

   setFocusPolicy(Qt::StrongFocus);
   gdata->getView().setPixelHeight(height());
}

//------------------------------------------------------------------------------
FreqDrawWidget::~FreqDrawWidget(void)
{
}

//------------------------------------------------------------------------------
void FreqDrawWidget::drawReferenceLines(QPaintDevice & pd,
					QPainter & p,
					double currentTime,
					double zoomX,
					double viewBottom,
					double zoomY,
					int viewType
					)
{
  // Draw the lines and notes
  QFontMetrics fm = p.fontMetrics();
  int fontHeightSpace = fm.height() / 4;
  int fontWidth = fm.width("C#0") + 3;
  
  //number of pixels to jump between each line
  double step = 1.0 / zoomY;
  //in semi-tones
  int noteJump;
  if(step > 10.0)
    {
      noteJump = 1;
    }
  else if(step > 5.0)
    {
      noteJump = 6;
    }
  else
    {
      noteJump = 12;
    }
  double remainder = cycle(viewBottom, double(noteJump));
  double toFirstNote = double(noteJump) - remainder;
  double start = toFirstNote * step;
  double stop = double(pd.height());
  int nameIndex = toInt(viewBottom + toFirstNote);
  step *= noteJump;

  //draw the note names and reference lines
  QString noteLabel;
  int lineY = 0;
  for (double y = start; y < stop; y += step, nameIndex+=noteJump)
    {
      lineY = pd.height() - toInt(y);
      if(!isBlackNote(nameIndex))
	{
	  p.setPen(Qt::black);
	  noteLabel.sprintf("%s%d", noteName(nameIndex), noteOctave(nameIndex));
	  p.drawText(2, lineY + fontHeightSpace, noteLabel);
	  if(noteValue(nameIndex) == 0)
	    {
	      p.setPen(QPen(Qt::black, 1, Qt::SolidLine));
	    }
	  else
	    {
	      //transperenct colors don't seem to work on the printer
	      if(viewType == DRAW_VIEW_PRINT)
		{
		  p.setPen(QPen(QColor(128, 128, 128), 1, Qt::SolidLine));
		}
	      else
		{
		  p.setPen(QPen(QColor(144, 156, 170), 1, Qt::DashDotDotLine));
		}
	    }
	}
      else
	{
	  if(viewType == DRAW_VIEW_PRINT)
	    {
	      p.setPen(QPen(QColor(196, 196, 196), 1, Qt::SolidLine));
	    }
	  else
	    {
	      p.setPen(QPen(QColor(136, 161, 180), 1, Qt::DotLine));
	    }
	}
      int offset = toInt(currentTime / zoomX) % 32;
      p.setClipRect(fontWidth, 0, pd.width()-fontWidth, pd.height());
      p.drawLine(fontWidth - offset, lineY, pd.width() - 1, lineY);
      p.setClipRect(0, 0, pd.width(), pd.height());
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::paintEvent(QPaintEvent *)
{
  View & view = gdata->getView();

  if(view.autoFollow() && gdata->getActiveChannel() && gdata->getRunning() == STREAM_FORWARD)
    {
      setChannelVerticalView(gdata->getActiveChannel(), view.viewLeft(), view.currentTime(), view.zoomX(), view.viewBottom(), view.zoomY());
    }
    
  int curTimePixel = view.screenPixelX(view.currentTime());

  beginDrawing();

  DrawWidget::setLineWidth(3);

  //draw the red/blue background color shading if needed
  if(view.backgroundShading() && gdata->getActiveChannel())
    {
      drawChannelFilled(gdata->getActiveChannel(), m_painter, view.viewLeft(), view.currentTime(), view.zoomX(), view.viewBottom(), view.zoomY(), DRAW_VIEW_NORMAL);
    }
  

  drawReferenceLines(*this, m_painter, view.currentTime(), view.zoomX(), view.viewBottom(), view.zoomY(), DRAW_VIEW_NORMAL);

  //draw all the visible channels
  for (uint i = 0; i < gdata->getChannelsSize(); i++)
    {
      Channel *ch = gdata->getChannelAt(i);
      if(!ch->isVisible())
	{
	  continue;
	}
      drawChannel(*this, ch, m_painter, view.viewLeft(), view.currentTime(), view.zoomX(), view.viewBottom(), view.zoomY(), DRAW_VIEW_NORMAL);
    }

  // Draw a light grey band indicating which time is being used in the current window
  if(gdata->getActiveChannel())
    {
      QColor lineColor = colorGroup().foreground();
      lineColor.setAlpha(50);
      Channel *ch = gdata->getActiveChannel();
      double halfWindowTime = (double)ch->size() / (double)(ch->rate() * 2);
      int pixelLeft = view.screenPixelX(view.currentTime() - halfWindowTime);
      int pixelRight = view.screenPixelX(view.currentTime() + halfWindowTime);
      m_painter.fillRect(pixelLeft, 0, pixelRight - pixelLeft, height() - 1, lineColor);
    }

  // Draw the current time line
  m_painter.setPen(QPen(colorGroup().foreground(), 1));
  m_painter.drawLine(curTimePixel, 0, curTimePixel, height() - 1);

  endDrawing();
}

//------------------------------------------------------------------------------
Channel * FreqDrawWidget::channelAtPixel(int x, int y)
{
  double time = mouseTime(x);
  float pitch = mousePitch(y);
  //10 pixel tolerance
  float tolerance = 6 * gdata->getView().zoomY();

  std::vector<Channel*> channels;

  //loop through channels in reverse order finding which one the user clicked on
  unsigned int l_index = 0;
  for (l_index = 0 ; l_index < gdata->getChannelsSize() ; ++l_index)
    {
      Channel * l_channel = gdata->getChannelAt(gdata->getChannelsSize() - 1 - l_index);
      if(l_channel->isVisible())
	{
	  const AnalysisData *data = l_channel->dataAtTime(time);
	  if(data && within(tolerance, data->getPitch(), pitch)) return l_channel;
	}
    }
  return NULL;
}

//------------------------------------------------------------------------------
QSize FreqDrawWidget::sizeHint(void) const
{
  return QSize(400, 350);
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mousePressEvent( QMouseEvent *e)
{
  View & view = gdata->getView();
  int timeX = toInt(view.viewOffset() / view.zoomX());
  dragMode = DragNone;
  
  
  //Check if user clicked on center bar, to drag it
  if(within(4, e->x(), timeX))
    {
      dragMode = DragTimeBar;
      mouseX = e->x();
      return;
    }

  // If the control or alt keys are pressed, zoom in or out on the correct axis, otherwise scroll.
  if (e->state() & Qt::ControlModifier)
    {
      // Do we zoom in or out?
      if (e->state() & Qt::ShiftModifier)
	{
	}
      else
	{
	}
    }
  else if (e->state() & Qt::AltModifier)
    {
      // Do we zoom in or out?
      if (e->state() & Qt::ShiftModifier)
	{
	}
      else
	{
	}
    }
  else
    {
      //mouseDown = true;
      mouseX = e->x();
      mouseY = e->y();

      Channel *ch = channelAtPixel(e->x(), e->y());
      if(ch)
	{
	  //Clicked on a channel
	  gdata->setActiveChannel(ch);
	  dragMode = DragChannel;
	}
      else
	{
	  //Must have clicked on background
	  dragMode = DragBackground;
	  downTime = view.currentTime();
	  downNote = view.viewBottom();
	}
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mouseMoveEvent( QMouseEvent *e )
{
  View & view = gdata->getView();
  int pixelAtCurrentTimeX = toInt(view.viewOffset() / view.zoomX());
  
  switch(dragMode)
    {
    case DragTimeBar:
      {
	int newX = pixelAtCurrentTimeX + (e->x() - mouseX);
	view.setViewOffset(double(newX) * view.zoomX());
	mouseX = e->x();
	view.doSlowUpdate();
      }
      break;
    case DragBackground:
      view.setViewBottom(downNote - (mouseY - e->y()) * view.zoomY());
      gdata->updateActiveChunkTime(downTime - (e->x() - mouseX) * view.zoomX());
      view.doSlowUpdate();
      break;
    case DragNone:
      if(within(4, e->x(), pixelAtCurrentTimeX))
	{
	  setCursor(QCursor(Qt::SplitHCursor));
	}
      else if(channelAtPixel(e->x(), e->y()) != NULL)
	{
	  setCursor(QCursor(Qt::PointingHandCursor));
	}
      else
	{
	  unsetCursor();
	}
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mouseReleaseEvent( QMouseEvent * )
{
  dragMode = DragNone;
}

//------------------------------------------------------------------------------
double FreqDrawWidget::mouseTime(int x)
{
  return gdata->getView().viewLeft() + gdata->getView().zoomX() * x;

}

//------------------------------------------------------------------------------
double FreqDrawWidget::mousePitch(int y)
{
  return gdata->getView().viewBottom() + gdata->getView().zoomY() * (height() - y);
}

//------------------------------------------------------------------------------
void FreqDrawWidget::wheelEvent(QWheelEvent * e)
{
  View & view = gdata->getView();
    double amount = double(e->delta() / WHEEL_DELTA);
    bool isZoom = gdata->mouseWheelZooms();
    if(e->state() & (Qt::ControlModifier | Qt::ShiftModifier))
      {
	isZoom = !isZoom;
      }

  if(isZoom)
    {
      if(e->delta() >= 0)
	{
	  //zooming in
	  if(gdata->getRunning() == STREAM_FORWARD)
	    {
	      view.setZoomFactorX(view.logZoomX() + amount * 0.3);
	    }
	  else
	    {
	      //zoom toward mouse pointer
	      view.setZoomFactorX(view.logZoomX() + amount * 0.3, e->x());
	    }
	  view.setZoomFactorY(view.logZoomY() + amount * 0.2, height() - e->y());
	}
      else
	{
	  //zoom out toward center
	  if(gdata->getRunning() == STREAM_FORWARD)
	    {
	      view.setZoomFactorX(view.logZoomX() + amount * 0.3);
	    }
	  else
	    {
	      view.setZoomFactorX(view.logZoomX() + amount * 0.3, width() / 2);
	    }
	  view.setZoomFactorY(view.logZoomY() + amount * 0.2, height() / 2);
	}
    }
  else
    {
      //mouse wheel scrolls
      view.setViewBottom(view.viewBottom() + amount * view.viewHeight() * 0.1);
    }
  view.doSlowUpdate();

  e->accept();
}

//------------------------------------------------------------------------------
void FreqDrawWidget::resizeEvent (QResizeEvent *q)
{
  if (q->size() == q->oldSize())
    {
      return;
    }
    
  View & v = gdata->getView();

  double oldViewWidth = double(v.viewWidth());
    
  v.setPixelHeight(height());
  v.setPixelWidth(width());
  // Work out what the times/heights of the view should be based on the zoom factors
  float newYHeight = height() * v.zoomY();
  float newYBottom = v.viewBottom() - ((newYHeight - v.viewHeight()) / 2.0);

  v.setViewOffset(v.viewOffset() / oldViewWidth * v.viewWidth());
  v.setViewBottom(newYBottom);
}

//------------------------------------------------------------------------------
void FreqDrawWidget::keyPressEvent( QKeyEvent *k )
{
  switch (k->key())
    {
    case Qt::Key_Control:
      setCursor(QCursor(QPixmap(zoomx)));
      break;
    case Qt::Key_Alt:
      setCursor(QCursor(QPixmap(zoomy)));
      break;
    case Qt::Key_Shift:
      if (k->state() & Qt::ControlModifier)
	{
	  setCursor(QCursor(QPixmap(zoomxout)));
	}
      else if (k->state() & Qt::AltModifier)
	{
	  setCursor(QCursor(QPixmap(zoomyout)));
	}
      else
	{
	  k->ignore();
	}
      break;
    default:
      k->ignore();
      break;
    } 
}

//------------------------------------------------------------------------------
void FreqDrawWidget::keyReleaseEvent( QKeyEvent *k)
{
  switch (k->key())
    {
    case Qt::Key_Control: // Unset the cursor if the control or alt keys were released, ignore otherwise
    case Qt::Key_Alt:
      unsetCursor();
      break;
    case Qt::Key_Shift:
      if (k->state() & Qt::ControlModifier)
	{
	  setCursor(QCursor(QPixmap(zoomx)));
	}
      else if (k->state() & Qt::AltModifier)
	{
	  setCursor(QCursor(QPixmap(zoomy)));
	}
      else
	{
	  k->ignore();
	}
      break;
    default:
      k->ignore();
      break;
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::leaveEvent ( QEvent * e)
{
  unsetCursor();
  QWidget::leaveEvent(e);
}

// EOF
