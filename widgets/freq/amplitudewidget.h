/***************************************************************************
                          amplitudewidget.h  -  description
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

#ifndef AMPLITUDEWIDGET_H
#define AMPLITUDEWIDGET_H

#include "drawwidget.h"

class QPointArray;
struct ZoomElement;

class AmplitudeWidget : public DrawWidget {

Q_OBJECT

public:
  enum DragModes {
    DragNone = 0,
    DragChannel = 1,
    DragBackground = 2,
    DragTimeBar = 3,
    DragNoiseThreshold = 4
  };

  AmplitudeWidget(QWidget *parent, const char* name = 0);
  virtual ~AmplitudeWidget();

  //int offset_x;

  void paintEvent( QPaintEvent * );
  void calculateZoomElement(ZoomElement &ze, Channel *ch, int startChunk, int finishChunk);
  void drawChannelAmplitude(Channel *ch, QPainter &p);
  void drawChannelAmplitudeFilled(Channel *ch, QPainter &p);
  void setColors();
  QSize sizeHint() const { return QSize(400, 100); }
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void wheelEvent(QWheelEvent * e);
  double dBRange() { return _dBRange; }

public slots:
  void setDBRange(double range);

signals:
  void dBRangeChanged(double);
  
private:
  int dragMode;
  int mouseX, mouseY;
  double downTime, downNote;
/*
 void keyPressEvent( QKeyEvent *k );
 void keyReleaseEvent( QKeyEvent *k);
 void leaveEvent ( QEvent * e);
 
 void mousePressEvent( QMouseEvent *e );
 void mouseMoveEvent( QMouseEvent *e );
 void mouseReleaseEvent( QMouseEvent *e );
 void wheelEvent(QWheelEvent * e);
 void resizeEvent (QResizeEvent *q);

 double mouseTime(int x);
 double mouseNote(int y);
*/

  QPixmap *buffer;
  double _dBRange;
};

#endif
