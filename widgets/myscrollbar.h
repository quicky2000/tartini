/***************************************************************************
                          myscrollbar.h  -  description
                             -------------------
    begin                : Tue Jul 20 2004
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
#ifndef MYSCROLLBAR_H
#define MYSCROLLBAR_H
#include <QWidget>
#include <QResizeEvent>
#include "useful.h"

class QScrollBar;

class MyScrollBar : public QWidget
{
  Q_OBJECT

  public:
    MyScrollBar(Qt::Orientation p_orientation
               ,QWidget * p_parent
               ,const char * p_name = nullptr
               );

    MyScrollBar(double p_min_value
               ,double p_max_value
               ,double p_lines_per_page
               ,double p_page_step
               ,double p_value
               ,double p_step
               ,Qt::Orientation p_orientation
               ,QWidget * p_parent
               ,const char * p_name = nullptr
               );

    virtual ~MyScrollBar();

    QSize sizeHint() const;

    void resizeEvent(QResizeEvent * p_event);

    inline
    double minValue() const;

    inline
    double maxValue() const;

    inline
    double lineStep() const;

    inline
    double pageStep() const;

    inline
    double value() const;

    inline
    double step() const;

  public slots:

    void setMinValue(double p_min_value);

    void setMaxValue(double p_max_value_);

    void setRange(double p_min_value
                 ,double p_max_value
                 );

    void setLineStep(double p_line_step);

    void setPageStep(double p_page_step);

    void setValue(double p_value);

    void setIntValue(int p_value);

    void sliderMoving(int p_value);

    void sliderMoving();

    void setStep(double p_step);

    void actionTriggering(int p_action);

  signals:

    void valueChanged(double p_value);

    void sliderMoved(double p_value);

  private:

    double m_min_value;
    double m_max_value;
    double m_value;
    double m_line_step;
    double m_page_step;
    double m_step;

    QScrollBar * m_bar;
};

#include "myscrollbar.hpp"

#endif // MYSCROLLBAR_H
// EOF
