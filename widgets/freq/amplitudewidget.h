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

  inline QSize sizeHint(void) const;
  inline const double & range(void)const;

public slots:
  void setRange(double newRange);
  void setOffset(double newOffset);

signals:
  void rangeChanged(double);
  void offsetChanged(double);
  void offsetInvChanged(double);
  
private:
  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  void setLineWidth(float width);
  void drawVerticalRefLines(void);
  double getCurrentThreshold(int index);
  QString getCurrentThresholdString(void)const;
  void drawChannelAmplitudeGL(Channel *ch);
  void drawChannelAmplitudeFilledGL(Channel *ch);

  /**
     This function has the side effect of changing zoom element
  */
  bool calcZoomElement(ZoomElement &ze, Channel *ch, int baseElement, double baseX);
  double calculateElement(AnalysisData * data);
  void setCurrentThreshold(double newThreshold, int index);

  void mousePressEvent( QMouseEvent * e);
  void mouseMoveEvent( QMouseEvent * e);
  void mouseReleaseEvent( QMouseEvent * e);
  void wheelEvent(QWheelEvent * e);

  inline double leftTime(void)const;
  inline double rightTime(void)const;
  inline double timeWidth(void)const;
	
  inline double maxOffset(void)const;
  inline const double & offset(void)const;
  inline const double & offsetInv(void)const;

  int dragMode;
  int mouseX;
  int mouseY;
  double downTime;
  double downOffset;
  int thresholdIndex;
  double _range;
  double _offset;
  double _offsetInv;
  float lineWidth;
  float halfLineWidth;

};

#include "amplitudewidget.hpp"

#endif
//EOF
