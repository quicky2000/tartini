/***************************************************************************
                          drawwidget.h  -  description
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
#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QPainter>
#include <QWidget>
#include <QGLWidget>
#include <QObject>
#include <QPixmap>
#include <vector>
#include <map>
#include <QPaintDevice>

#include "useful.h"

#define DRAW_VIEW_NORMAL   0
#define DRAW_VIEW_SUMMARY  1
#define DRAW_VIEW_PRINT    2

class Channel;
class QPixmap;
class ZoomElement;

class DrawWidget : public QWidget
{
  Q_OBJECT

public:
  DrawWidget(QWidget * p_parent, const char* p_name = 0, Qt::WFlags p_flags = Qt::WDestructiveClose);
  virtual ~DrawWidget(void);


  /**
     Draw the channel onto the painter using the MinMax algorithm or individual samples if the zoom is high enough
  */
  static void drawChannel(QPaintDevice & p_paint_device,
			  Channel * p_channel,
			  QPainter & p_painter,
			  double p_left_time,
			  double p_current_time,
			  double p_zoom_X,
			  double p_view_bottom,
			  double p_zoom_Y,
			  int p_view_type
			  );
  void drawChannelFilled(Channel * p_channel,
			 QPainter & p_painter,
			 double p_left_time,
			 double p_current_time,
			 double p_zoom_X,
			 double p_view_bottom,
			 double p_zoom_Y,
			 int p_view_type
			 );
  void setChannelVerticalView(Channel * p_channel,
			      double p_left_time,
			      double p_current_time,
			      double p_zoom_X,
			      double p_view_bottom,
			      double p_zoom_Y
			      );
  static void setLineWidth(int p_width);
  
  void beginDrawing(bool p_clear_background_=true);
  void endDrawing(bool p_draw_to_screen_=true);
  void clearBackground(void);
  void fillBackground(const QColor & p_color);
  void checkSize(void);
  void drawToScreen(void);

  /**
     calculates elements in the zoom lookup table
     @param p_channel The channel we are working with
     @param p_base_element The element's index in the zoom lookup table
     @param p_base_X  The number of chunks each pixel represents (can include a fraction part)
     @return false if a zoomElement can't be calculated, else true
  */
  static bool calcZoomElement(Channel * p_channel,
			      ZoomElement & p_zoom_element,
			      int p_base_element,
			      double p_base_X
			      );
  void drawArray(float * p_input,
		 int p_n,
		 int p_sample_step = 1,
		 double p_the_zoom_Y = 1.0,
		 double p_offset = 0
		 );
  
 protected:
  inline QPainter & get_painter(void);
 private:
  static int m_line_width;
  static int m_line_top_half_width;
  static int m_line_bottom_half_width;

  QPaintDevice * m_paint_device;

  QPainter m_painter;
};

#include "drawwidget.hpp"

#endif // DRAWWIDGET_H
// EOF
