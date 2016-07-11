/***************************************************************************
                          vibratospeedwidget.h  -  description
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
#ifndef VIBRATOSPEEDWIDGET_H
#define VIBRATOSPEEDWIDGET_H

#include <QGLWidget>

class VibratoSpeedWidget: public QGLWidget
{
  Q_OBJECT

 public:
  VibratoSpeedWidget(QWidget * parent);
  virtual ~VibratoSpeedWidget(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  QSize minimumSizeHint(void) const;

 public slots:
  void doUpdate(void);
  void setUseProny(bool value);

 private:
  //data goes here
  float speedValueToDraw, widthValueToDraw;
  float prevVibratoSpeed, prevVibratoWidth;

  bool useProny;

  QFont speedWidthFont;

  int widthLimit;
  int prevNoteNumber;

  float hzLabelX, hzLabelY;
  float centsLabelX, centsLabelY;

  struct labelStruct
  {
    QString label;
    float x;
    float y;
  };

  int speedLabelCounter;
  labelStruct speedLabels[100];

  int widthLabelCounter;
  labelStruct widthLabels[100];

  GLuint speedDial;
  GLuint speedNeedle;
  GLuint widthDial;
  GLuint widthNeedle;
};
#endif // VIBRATOSPEEDWIDGET_H
// EOF
