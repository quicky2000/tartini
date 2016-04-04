/***************************************************************************
                          correlationwidget.h  -  description
                             -------------------
    begin                : May 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef CORRELATIONWIDGET_H
#define CORRELATIONWIDGET_H

#include "drawwidget.h"

// Forward declarations of classes the h file doesn't need to know specifics about
class QPointArray;

class CorrelationWidget : public DrawWidget {
  Q_OBJECT

  public:
    CorrelationWidget(QWidget *parent);
    virtual ~CorrelationWidget();

    void paintEvent( QPaintEvent * );

    QSize sizeHint() const { return QSize(500, 128); }

  private:
    QPointArray pointArray;
};


#endif
