/***************************************************************************
                          vibratocirclewidget.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOCIRCLEWIDGET_H
#define VIBRATOCIRCLEWIDGET_H

#include <QGLWidget>

class VibratoCircleWidget: public QGLWidget
{
  Q_OBJECT

 public:
  VibratoCircleWidget(QWidget * parent);
  virtual ~VibratoCircleWidget(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  QSize minimumSizeHint(void) const;
  int getType(void);

 public slots:
  void doUpdate(void);
  void setAccuracy(int value);
  void setType(int value);

 private:
  float accuracy;
  int type;
  int lastPeriodToDraw;

  GLuint referenceCircle;
  GLuint currentPeriod;
  GLuint prevPeriods[6];
};

#endif // VIBRATOCIRCLEWIDGET_H
// EOF
