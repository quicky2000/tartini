/***************************************************************************
                          vibratoview.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
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
  VibratoView(int viewID_, QWidget * parent = NULL);
  virtual ~VibratoView(void);

  QSize sizeHint(void) const;

  std::vector<LEDIndicator*> leds;

 private:
  VibratoSpeedWidget * vibratoSpeedWidget;
  VibratoPeriodWidget * vibratoPeriodWidget;
  VibratoCircleWidget * vibratoCircleWidget;
  VibratoTimeAxis * vibratoTimeAxis;
  VibratoWidget * vibratoWidget;

};
#endif // VIBRATOVIEW_H
// EOF
