/***************************************************************************
                          ledindicator.h  -  description
                             -------------------
    begin                : Tue Jan 11 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include "drawwidget.h"
#include <QPaintEvent>

class QColor;
class QPainter;
class QPixmap;

class LEDIndicator : public QWidget
{
  Q_OBJECT

  public:
    LEDIndicator(QPixmap * p_buffer
                ,QWidget * p_parent = 0
                ,const char *p_name = 0
                ,const QColor & p_on = Qt::red
                ,const QColor & p_off = Qt::darkRed
                );

    virtual ~LEDIndicator(void);

    void setOn(bool p_on);
    void toggle(void);

    bool lit(void);

    void paintEvent(QPaintEvent *);

    QSize sizeHint(void) const;

  private:
    QColor m_on;
    QColor m_off;
    bool m_active;
};

#endif // LEDINDICATOR_H
// EOF
