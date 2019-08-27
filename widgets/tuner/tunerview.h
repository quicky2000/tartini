/***************************************************************************
                          tunerview.h  -  description
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
#ifndef TUNERVIEW_H
#define TUNERVIEW_H

#include <vector>
#include <QPixmap>
#include <QResizeEvent>
#include <QPaintEvent>

#include "viewwidget.h"
#include "vibratotunerwidget.h"

class QPixmap;
class TunerWidget;
class LEDIndicator;
class QwtSlider;
class Channel;

class TunerView : public ViewWidget
{
  Q_OBJECT

  public:
  TunerView(int p_view_iD_, QWidget *p_parent = NULL);
  virtual ~TunerView(void);

  void resizeEvent(QResizeEvent *);
  void paintEvent( QPaintEvent* );

  QSize sizeHint(void) const;

  public slots:
  void slotCurrentTimeChanged(double p_time);
  void setLed(int p_index, bool p_value);
  void doUpdate(void);

  private:
  void resetLeds(void);

  VibratoTunerWidget *m_tuner_widget;
  std::vector<LEDIndicator*> m_leds;
  QwtSlider * m_slider;
  QPixmap * m_led_buffer;

};
#endif // TUNERVIEW_H
// EOF
