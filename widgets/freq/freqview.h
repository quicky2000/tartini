/***************************************************************************
                          freqview.h  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef FREQVIEW_H
#define FREQVIEW_H

#include "viewwidget.h"

class FreqDrawWidget;
class FreqWidgetGL;
class AmplitudeWidget;
class MyScrollBar;
class TimeAxis;
class QwtWheel;

class FreqView: public ViewWidget
{
  Q_OBJECT

  public:
    FreqView( int p_view_id
            , QWidget *p_parent = NULL
            );
    virtual ~FreqView(void);

    QSize sizeHint(void) const;

  public slots:
    void zoomIn(void);
    void zoomOut(void);
    void setAmplitudeZoom(double);

  private:
    FreqWidgetGL * m_freq_widget_GL;
    AmplitudeWidget * m_amplitude_widget;
    MyScrollBar * m_freq_scroll_bar;
    MyScrollBar * m_amplitude_scroll_bar;
    QwtWheel * m_freq_wheel_Y;
    QwtWheel * m_amplitude_wheel_Y;
    QwtWheel * m_freq_wheel_X;

    TimeAxis * m_time_axis;

};
#endif // FREQVIEW_H
// EOF
