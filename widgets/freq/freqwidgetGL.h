/***************************************************************************
                          freqdrawwidget.h  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

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
  FreqWidgetGL(QWidget *p_parent, const char* p_name = NULL);
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
  void resizeGL(int p_w, int p_h);
  void drawReferenceLinesGL(const double & p_leftTime,
			    const double & p_current_time,
			    const double & p_zoom_X,
			    const double & p_view_bottom,
			    const double & p_zoom_Y,
			    int p_view_type);
  void drawChannelGL(Channel * p_channel,
		     const double & p_left_time,
		     const double & p_current_time,
		     const double & p_zoom_X,
		     double p_view_bottom,
		     const double & p_zoom_Y,
		     int p_view_type);
  void drawChannelFilledGL(Channel * p_channel,
			   const double & p_left_time,
			   const double & p_current_time,
			   const double & p_zoom_X,
			   double p_view_bottom,
			   const double & p_zoom_Y,
			   int p_view_type);

  /** calculates elements in the zoom lookup table
      @param p_channel The channel we are working with
      @param p_base_element The element's index in the zoom lookup table
      @param p_base_X  The number of chunks each pixel represents (can include a fraction part)
      @return false if a zoomElement can't be calculated, else true
  */
  bool calcZoomElement(Channel * p_channel,
		       ZoomElement & p_zoom_element,
		       int p_base_element,
		       const double & p_base_X);
  void paintGL(void);
  inline QSize sizeHint(void)const;

  /*
   * Changes the cursor icon to be one of the zoom ones depending on if the control or alt keys were pressed.
   * Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyPressEvent( QKeyEvent * p_key_event);

  /*
   * Unsets the cursor icon if the control or alt key was released. Otherwise, ignores the event.
   *
   * @param k the QKeyEvent to respond to.
   */
  void keyReleaseEvent( QKeyEvent * p_key_event);
  void leaveEvent ( QEvent * p_event);
  
  /*
   * If control or alt is pressed, zooms. If shift is also pressed, it 'reverses' the zoom: ie ctrl+shift zooms x
   * out, alt+shift zooms y out. Otherwise, does internal processing.
   *
   * @param e the QMouseEvent to respond to.
   */
  void mousePressEvent( QMouseEvent * p_mouse_event);
  void mouseMoveEvent( QMouseEvent * p_mouse_event);
  void mouseReleaseEvent( QMouseEvent * p_mouse_event);
  void wheelEvent(QWheelEvent * p_wheel_event);
  void resizeEvent (QResizeEvent * p_resize_event);
  
  /**
     Calculates at what time the mouse is.
     @param p_x the mouse's x co-ordinate
     @return the time the mouse is positioned at.
  */
  double mouseTime(int p_x)const;

  /**
     Calculates at what note pitch the mouse is at.
     @param x the mouse's y co-ordinate
     @return the pitch the mouse is positioned at.
  */
  double mousePitch(int p_y)const;
  Channel *channelAtPixel(int p_x, int p_y)const;
  void setChannelVerticalView(Channel * p_channel,
			      const double & p_left_time,
			      const double & p_current_time,
			      const double & p_zoom_X,
			      double p_view_bottom,
			      const double & p_zoom_Y);

  inline double leftTime(void)const;
  inline double rightTime(void)const;
  inline double timeWidth(void)const;

  int m_drag_mode;
  int m_mouse_X;
  int m_mouse_Y;
  double m_down_time;
  double m_down_note;
};

#include "freqwidgetGL.hpp"
#endif
//EOF
