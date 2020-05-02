/***************************************************************************
                          tunerwidget.h  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#ifndef TUNERWIDGET_H
#define TUNERWIDGET_H

#include "drawwidget.h"
#include <QPixmap>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;

class TunerWidget : public DrawWidget
{
  Q_OBJECT

  public:
    TunerWidget(QWidget *p_parent);
    virtual ~TunerWidget();

    void paintEvent(QPaintEvent *);
    QSize minimumSizeHint() const;

    /**
       Sets the value of the widget.
       
       @param p_value the value to represent (in cents). Accepts values between -60 and +60 inclusive.
       @param p_intensity How sure of the note you are
    */
    void setValue(float p_value
                 ,float p_intensity
                 );
    double value();

  private:
    float m_value;
    float m_intensity;
};
#endif // TUNERWIDGET_H
// EOF
