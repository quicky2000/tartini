/***************************************************************************
                          wavewidget.h  -  description
                             -------------------
    begin                : Mon Mar 14 2005
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
#ifndef WAVEWIDGET_H
#define WAVEWIDGET_H

#include "drawwidget.h"
#include <QPolygon>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;

class WaveWidget : public DrawWidget
{
  Q_OBJECT

  public:
    WaveWidget(QWidget *parent);

    virtual ~WaveWidget(void);

    void paintEvent(QPaintEvent *);

    QSize sizeHint(void) const;
    const double & zoomY(void) const;

  signals:
    void zoomYChanged(double p_zoom_Y);

  public slots:
    void setZoomY(const double & p_zoom_Y);

  private:
    QPolygon m_point_array;
    double m_zoom_Y;
};
#endif // WAVEWIDGET_H
// EOF
