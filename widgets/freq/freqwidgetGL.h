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

#ifndef FREQWIDGETGL_H
#define FREQWIDGETGL_H

#include <QMouseEvent>
#include <QPixmap>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QPaintDevice>
#include <QGLWidget>

#include "gdata.h"
#include "channel.h"

#define DRAW_VIEW_NORMAL   0
#define DRAW_VIEW_SUMMARY  1
#define DRAW_VIEW_PRINT    2

class FreqWidgetGL : public QGLWidget
{

Q_OBJECT

public:
  FreqWidgetGL(QWidget *parent, const char* name = NULL);
  virtual ~FreqWidgetGL(void);

private:  
  enum DragModes
    {
      DragNone = 0,
      DragChannel = 1,
      DragBackground = 2,
      DragTimeBar = 3
    };

  void initializeGL(void);
  void resizeGL(int w, int h);
  void drawReferenceLinesGL(const double & leftTime,
			    const double & currentTime,
			    const double & zoomX,
			    const double & viewBottom,
			    const double & zoomY,
			    int viewType);
  void drawChannelGL(Channel * ch,
		     const double & leftTime,
		     const double & currentTime,
		     const double & zoomX,
		     double viewBottom,
		     const double & zoomY,
		     int viewType);
  void drawChannelFilledGL(Channel * ch,
			   const double & leftTime,
			   const double & currentTime,
			   const double & zoomX,
			   double viewBottom,
			   const double & zoomY,
			   int viewType);

  /** calculates elements in the zoom lookup table
      @param ch The channel we are working with
      @param baseElement The element's index in the zoom lookup table
      @param baseX  The number of chunks each pixel represents (can include a fraction part)
      @return false if a zoomElement can't be calculated, else true
  */
  bool calcZoomElement(Channel * ch,
		       ZoomElement & ze,
		       int baseElement,
		       const double & baseX);
  void paintGL(void);
  inline QSize sizeHint(void)const;

  /*
   * Changes the cursor icon to be one of the zoom ones depending on if the control or alt keys were pressed.
   * Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyPressEvent( QKeyEvent * k);

  /*
   * Unsets the cursor icon if the control or alt key was released. Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyReleaseEvent( QKeyEvent * k);
  void leaveEvent ( QEvent * e);
  
  /*
   * If control or alt is pressed, zooms. If shift is also pressed, it 'reverses' the zoom: ie ctrl+shift zooms x
   * out, alt+shift zooms y out. Otherwise, does internal processing.
   *
   * @param e the QMouseEvent to respond to.
   */
  void mousePressEvent( QMouseEvent * e);
  void mouseMoveEvent( QMouseEvent * e);
  void mouseReleaseEvent( QMouseEvent * e);
  void wheelEvent(QWheelEvent * e);
  void resizeEvent (QResizeEvent * q);
  
  /**
     Calculates at what time the mouse is.
     @param x the mouse's x co-ordinate
     @return the time the mouse is positioned at.
  */
  double mouseTime(int x)const;

  /**
     Calculates at what note pitch the mouse is at.
     @param x the mouse's y co-ordinate
     @return the pitch the mouse is positioned at.
  */
  double mousePitch(int y)const;
  Channel *channelAtPixel(int x, int y)const;
  void setChannelVerticalView(Channel * ch,
			      const double & leftTime,
			      const double & currentTime,
			      const double & zoomX,
			      double viewBottom,
			      const double & zoomY);

  inline double leftTime(void)const;
  inline double rightTime(void)const;
  inline double timeWidth(void)const;

  int dragMode;
  int mouseX;
  int mouseY;
  double downTime;
  double downNote;
};

#include "freqwidgetGL.hpp"
#endif
//EOF
