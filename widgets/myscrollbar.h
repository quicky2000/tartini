/***************************************************************************
                          myscrollbar.h  -  description
                             -------------------
    begin                : Tue Jul 20 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#ifndef MYSCROLLBAR_H
#define MYSCROLLBAR_H
#include <qwidget.h>
#include <QResizeEvent>
#include "useful.h"

class QScrollBar;

class MyScrollBar : public QWidget
{
  Q_OBJECT

 public:
  MyScrollBar(Qt::Orientation orientation,
	      QWidget * parent,
	      const char * name = 0
	      );
  MyScrollBar(double minValue_,
	      double maxValue_,
	      double linesPerPage_,
	      double pageStep_,
	      double value_,
	      double step_,
	      Qt::Orientation orientation,
	      QWidget * parent,
	      const char * name = 0
	      );
  virtual ~MyScrollBar(void);

  QSize sizeHint(void) const;
  void resizeEvent(QResizeEvent *q);

  inline double minValue(void) const;
  inline double maxValue(void) const;
  inline double lineStep(void) const;
  inline double pageStep(void) const;
  inline double value(void) const;
  inline double step(void) const;

 public slots:
  void setMinValue(double minValue_);
  void setMaxValue(double maxValue_);
  void setRange(double minValue_, double maxValue_);
  void setLineStep(double lineStep_);
  void setPageStep(double pageStep_);
  void setValue(double value_);
  void setIntValue(int value);
  void sliderMoving(int value);
  void sliderMoving(void);
  void setStep(double step_);
  void actionTriggering(int action);

 signals:
  void valueChanged(double value_);
  void sliderMoved(double value_);

 private: 
  double _minValue;
  double _maxValue;
  double _value;
  double _lineStep;
  double _pageStep;
  double _step;
  
  QScrollBar *bar;
  

};

#include "myscrollbar.hpp"

#endif // MYSCROLLBAR_H
// EOF
