/***************************************************************************
                          freqdrawwidget.h  -  description
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

#ifndef FREQDRAWWIDGET_H
#define FREQDRAWWIDGET_H

#include "drawwidget.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QPaintDevice>

class FreqDrawWidget : public DrawWidget
{

Q_OBJECT

public:
  enum DragModes
    {
      DragNone = 0,
      DragChannel = 1,
      DragBackground = 2,
      DragTimeBar = 3
    };


  FreqDrawWidget(QWidget *parent, const char* name = 0);
  virtual ~FreqDrawWidget(void);

  static void drawReferenceLines(QPaintDevice & pd,
				 QPainter & p,
				 double currentTime,
				 double zoomX,
				 double viewBottom,
				 double zoomY,
				 int viewType
				 );
  void paintEvent(QPaintEvent *);
  QSize sizeHint(void) const;

 private:  
  /*
   * Changes the cursor icon to be one of the zoom ones depending on if the control or alt keys were pressed.
   * Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyPressEvent( QKeyEvent *k );

  /*
   * Unsets the cursor icon if the control or alt key was released. Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyReleaseEvent( QKeyEvent *k);
  void leaveEvent ( QEvent * e);
  
  /*
   * If control or alt is pressed, zooms. If shift is also pressed, it 'reverses' the zoom: ie ctrl+shift zooms x
   * out, alt+shift zooms y out. Otherwise, does internal processing.
   *
   * @param e the QMouseEvent to respond to.
   */
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void wheelEvent(QWheelEvent * e);
  void resizeEvent (QResizeEvent *q);
  
  /**
     Calculates at what time the mouse is.
     @param x the mouse's x co-ordinate
     @return the time the mouse is positioned at.
  */
  double mouseTime(int x);

  /**
     Calculates at what note pitch the mouse is at.
     @param x the mouse's y co-ordinate
     @return the pitch the mouse is positioned at.
  */
  double mousePitch(int y);
  Channel *channelAtPixel(int x, int y);

  int dragMode;
  int mouseX;
  int mouseY;
  double downTime;
  double downNote;
  
  QPixmap *buffer;
  
};

#endif // FREQDRAWWIDGET_H
// EOF
