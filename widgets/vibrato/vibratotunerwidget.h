/***************************************************************************
                          vibratotunerwidget.h  -  description
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
#ifndef VIBRATOTUNERWIDGET_H
#define VIBRATOTUNERWIDGET_H

#include <QGLWidget>

class VibratoTunerWidget: public QGLWidget
{
  Q_OBJECT

 public:
  VibratoTunerWidget(QWidget *parent);
  virtual ~VibratoTunerWidget(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  QSize minimumSizeHint(void) const;

 signals:
  void ledSet(int index, bool value);

  public slots:
  void doUpdate(double thePitch);

 private:
  //data goes here
  float needleValueToDraw;
  float prevNeedleValue;
  int prevClosePitch;
  bool prevIsPitchBlackNote;
  double curPitch;

  QFont tunerFont;

  float centsLabelX, centsLabelY;

  int tunerLabelCounter;
  struct tunerLabelStruct
  {
    QString label;
    float x;
    float y;
  };
  tunerLabelStruct tunerLabels[100];

  void resetLeds(void);

  GLuint dial;
  GLuint needle;
};
#endif // VIBRATOTUNERWIDGET_H
// EOF
