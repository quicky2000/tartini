/***************************************************************************
                          timeaxis.h  -  description
                             -------------------
    begin                : Mon Dec 13 2004
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

#ifndef TIMEAXIS_H
#define TIMEAXIS_H

#include <qpixmap.h>
#include <QPaintEvent>

#include "drawwidget.h"
#include "gdata.h"

class QFont;

class TimeAxis : public DrawWidget
{
  Q_OBJECT

public:
  TimeAxis(QWidget *parent, bool numbersOnTop_=true);
  TimeAxis(QWidget *parent, double leftTime_, double rightTime_, bool numbersOnTop_=true);
  ~TimeAxis(void);

  QSize sizeHint(void) const
  {
    return QSize(700, 20);
  }

  void init(void);
  double leftTime(void)
  {
    return gdata->getView().viewLeft();
  }
  double rightTime(void)
  {
    return gdata->getView().viewRight();
  }
  double timeWidth(void)
  {
    return gdata->getView().viewTotalTime();
  }

  void setFontSize(int fontSize);
  
  void paintEvent(QPaintEvent *);

  public slots:
  void setLeftTime(double time)
  {
    _leftTime = time;
  }
  void setRightTime(double time)
  {
    _rightTime = time;
  }
  void setRange(double leftTime_, double rightTime_)
  {
    _leftTime = leftTime_;
    _rightTime = rightTime_;
  }

 private:
  double _leftTime, _rightTime;
  QPixmap *buffer;
  bool _numbersOnTop;
  QFont _font;
  int _fontSize;
    
};

#endif // TIMEAXIS_H
// EOF
