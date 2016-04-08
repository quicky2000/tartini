/***************************************************************************
                          fftwidget.h  -  description
                             -------------------
    begin                : May 18 2005
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
#ifndef FFTWIDGET_H
#define FFTWIDGET_H

#include "drawwidget.h"
#include <QPolygon>
#include <QPaintEvent>

class FFTWidget: public DrawWidget
{
  Q_OBJECT

  public:
    FFTWidget(QWidget * p_parent);
    virtual ~FFTWidget(void);

    void paintEvent( QPaintEvent * );

    QSize sizeHint(void) const;

  private:
    QPolygon m_point_array;
};

#endif // FFTWIDGET_H
// EOF

