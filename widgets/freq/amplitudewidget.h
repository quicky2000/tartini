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

#include <QGLWidget>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include <Q3PointArray>
#include <QWheelEvent>
#include <QPaintEvent>
#include "channel.h"

class Q3PointArray;
struct ZoomElement;
class AnalysisData;

class AmplitudeWidget : public QGLWidget
{

  Q_OBJECT

    public:
  enum DragModes
    {
      DragNone = 0,
      DragChannel = 1,
      DragBackground = 2,
      DragTimeBar = 3,
      DragNoiseThreshold = 4
    };

  AmplitudeWidget(QWidget *parent, const char* name = 0);
  virtual ~AmplitudeWidget(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  void drawVerticalRefLines(void);
  bool calcZoomElement(ZoomElement &ze, Channel *ch, int baseElement, double baseX);
  double calculateElement(AnalysisData *data);
  double getCurrentThreshold(int index);
  void setCurrentThreshold(double newThreshold, int index);
  QString getCurrentThresholdString();
  void setLineWidth(float width);
  void drawChannelAmplitudeGL(Channel *ch);
  void drawChannelAmplitudeFilledGL(Channel *ch);
  void setColors();
  inline QSize sizeHint(void) const;
  void mousePressEvent( QMouseEvent *e );
  void mouseMoveEvent( QMouseEvent *e );
  void mouseReleaseEvent( QMouseEvent *e );
  void wheelEvent(QWheelEvent * e);
  inline double range(void);
  inline double maxOffset(void);
  inline double offset(void);
  inline double offsetInv(void);

public slots:
  void setRange(double newRange);
  void setOffset(double newOffset);

signals:
  void rangeChanged(double);
  void offsetChanged(double);
  void offsetInvChanged(double);
  
private:

  inline double leftTime(void);
  inline double rightTime(void);
  inline double timeWidth(void);
	
  int dragMode;
  int mouseX, mouseY;
  double downTime, downNote;
  double downOffset;
  int thresholdIndex;

  QPixmap *buffer;
  double _range;
  double _offset;
  double _offsetInv;
  float lineWidth;
  float halfLineWidth;
};

#include "amplitudewidget.hpp"

#endif
//EOF
