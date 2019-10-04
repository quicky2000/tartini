/***************************************************************************
                          debugwidget.h  -  description
                             -------------------
    begin                : Wed Feb 22 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include "drawwidget.h"
#include <QPaintEvent>

class DebugWidget : public DrawWidget
{
  Q_OBJECT

  public:
  DebugWidget(QWidget *p_parent);
  virtual ~DebugWidget(void);

  void paintEvent( QPaintEvent * );
  void printString(const QString &p_string);

  QSize sizeHint(void) const;

 private:
  int m_text_Y;
  int m_font_height;
};
#endif // DEBUGWIDGET_H
// EOF

