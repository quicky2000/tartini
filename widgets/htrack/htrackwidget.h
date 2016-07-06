/***************************************************************************
                          htrackwidget.h  -  description
                             -------------------
    begin                : Aug 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef HTRACK_H
#define HTRACK_H

#include <qgl.h>
#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>

class Piano3d;

class HTrackWidget : public QGLWidget
{
  Q_OBJECT        // must include this if you use Qt signals/slots

public:
  HTrackWidget(QWidget *parent, const char *name = NULL);
  ~HTrackWidget(void);
  
public slots:
  inline void setPeakThreshold(float peakThreshold);
  inline void setViewAngleHorizontal(const double & angle);
  inline void setViewAngleVertical(const double & angle);
  inline void setDistanceAway(const double & distance);
  void home(void);

signals:
  void distanceAwayChanged(double);
  void viewAngleHorizontalChanged(double);
  void viewAngleVerticalChanged(double);

private:
  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);
  
  void rotateX(const double & angle);
  void rotateY(const double & angle);
  void translate(float x, float y, float z);
  void mousePressEvent(QMouseEvent * e);
  void mouseMoveEvent(QMouseEvent * e);
  void mouseReleaseEvent(QMouseEvent * e);
  void wheelEvent(QWheelEvent * e);
  
  inline float peakThreshold(void)const;
  inline const double & viewAngleHorizontal(void)const;
  inline const double & viewAngleVertical(void)const;
  
  float _peakThreshold;
  bool mouseDown;
  int mouseX;
  int mouseY;
  float gCurrentMatrix[16];
  double _distanceAway;
  double _viewAngleHorizontal;
  double _viewAngleVertical;
  Piano3d *piano3d;
  double translateX;
  double translateY;
};

#include "htrackwidget.hpp"

#endif // HTRACK_H
// EOF
