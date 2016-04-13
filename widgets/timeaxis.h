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

#include <QPixmap>
#include <QPaintEvent>

#include "drawwidget.h"
#include "gdata.h"

class QFont;

class TimeAxis : public DrawWidget
{
  Q_OBJECT

  public:
    TimeAxis(QWidget * p_parent
            ,bool p_numbers_on_top = true
            );

    TimeAxis(QWidget * p_parent
            ,const double & p_left_time
            ,const double & p_right_time
            ,bool p_numbers_on_top = true
            );

    ~TimeAxis(void);

    inline
    QSize sizeHint(void) const;

    void init(void);

    inline
    double leftTime(void) const;

    inline
    double rightTime(void) const;

    inline
    double timeWidth(void) const;

    void setFontSize(int p_font_size);

    void paintEvent(QPaintEvent *);

  public slots:

    inline
    void setLeftTime(const double & p_time);

    inline
    void setRightTime(const double & p_time);

    inline
    void setRange(const double & p_left_time
                 ,const double & p_right_time
                 );

  private:

    double m_left_time;
    double m_right_time;
    bool m_numbers_on_top;
    QFont m_font;
    int m_font_size;
};

#include "timeaxis.hpp"

#endif // TIMEAXIS_H
// EOF
