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

//#include "drawwidget.h"
//Added by qt3to4:
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
  enum DragModes
    {
      DragNone = 0,
      DragChannel = 1,
      DragBackground = 2,
      DragTimeBar = 3
    };


  FreqWidgetGL(QWidget *parent, const char* name = 0);
  virtual ~FreqWidgetGL(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void drawReferenceLinesGL(double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType);
  void drawChannelGL(Channel *ch, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType);
  void drawChannelFilledGL(Channel *ch, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY, int viewType);
  bool calcZoomElement(Channel *ch, ZoomElement &ze, int baseElement, double baseX);
  void paintGL(void);
  inline QSize sizeHint(void)const;

private:  
  void keyPressEvent( QKeyEvent *k );
  void keyReleaseEvent( QKeyEvent *k);
  void leaveEvent ( QEvent * e);
  
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void wheelEvent(QWheelEvent * e);
  void resizeEvent (QResizeEvent *q);
  
  double mouseTime(int x);
  double mousePitch(int y);
  Channel *channelAtPixel(int x, int y);
  void setChannelVerticalView(Channel *ch, double leftTime, double currentTime, double zoomX, double viewBottom, double zoomY);

  inline double leftTime(void)const;
  inline double rightTime(void)const;
  inline double timeWidth(void)const;

  int dragMode;
  int mouseX;
  int mouseY;
  double downTime;
  double downNote;
  QPixmap *buffer;
  
};

#include "freqwidgetGL.hpp"
#endif
//EOF
