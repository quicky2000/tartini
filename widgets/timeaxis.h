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
  TimeAxis(QWidget *parent,const double & leftTime_, const double & rightTime_, bool numbersOnTop_=true);
  ~TimeAxis(void);

  inline QSize sizeHint(void) const;

  void init(void);
  inline double leftTime(void) const;
  inline double rightTime(void) const;
  inline double timeWidth(void) const;

  void setFontSize(int fontSize);
  
  void paintEvent(QPaintEvent *);

  public slots:
  inline void setLeftTime(const double & time);
  inline void setRightTime(const double & time);
  inline void setRange(const double & leftTime_, const double & rightTime_);

 private:
  double _leftTime, _rightTime;
  bool _numbersOnTop;
  QFont _font;
  int _fontSize;
    
};

#include "timeaxis.hpp"

#endif // TIMEAXIS_H
// EOF
