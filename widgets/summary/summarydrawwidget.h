/***************************************************************************
                          summarydrawwidget.h  -  description
                             -------------------
    begin                : Fri Dec 10 2004
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

#ifndef SUMMARYDRAWWIDGET_H
#define SUMMARYDRAWWIDGET_H

#include "drawwidget.h"
#include "useful.h"

#include <QPixmap>
#include <QEvent>
#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>

#include <map>

class SummaryDrawWidget: public DrawWidget
{
  Q_OBJECT

  public:

    SummaryDrawWidget(QWidget * p_parent);
    virtual ~SummaryDrawWidget(void);

    void paintEvent(QPaintEvent *);

    void mousePressEvent(QMouseEvent * p_event);
    void mouseMoveEvent(QMouseEvent * p_event);
    void mouseReleaseEvent(QMouseEvent * p_event);
    void wheelEvent(QWheelEvent *p_event);

 private:

    QPixmap * m_buffer;

    bool m_mouse_down;

    int m_mouse_X;
    int m_mouse_Y;
    double m_click_time;
    double m_click_note;
    double m_click_current_time_diff;
    double m_click_view_bottom_diff;
    double m_scaler, m_down_scaler;
    int m_click_mode;
};
#endif // SUMMARYDRAWWIDGET_H
// EOF
