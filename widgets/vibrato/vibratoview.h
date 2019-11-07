/***************************************************************************
                          vibratoview.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOVIEW_H
#define VIBRATOVIEW_H

#include "viewwidget.h"

class VibratoTunerWidget;
class VibratoSpeedWidget;
class VibratoPeriodWidget;
class VibratoCircleWidget;
class VibratoTimeAxis;
class VibratoWidget;
class LEDIndicator;

class VibratoView: public ViewWidget
{
  Q_OBJECT

  public:
    VibratoView(int p_view_ID
               ,QWidget * p_parent = NULL
               );

    virtual ~VibratoView();

    QSize sizeHint() const;

    std::vector<LEDIndicator*> m_leds;

  private:
    VibratoSpeedWidget * m_vibrato_speed_widget;
    VibratoPeriodWidget * m_vibrato_period_widget;
    VibratoCircleWidget * m_vibrato_circle_widget;
    VibratoTimeAxis * m_vibrato_time_axis;
    VibratoWidget * m_vibrato_widget;

};
#endif // VIBRATOVIEW_H
// EOF
