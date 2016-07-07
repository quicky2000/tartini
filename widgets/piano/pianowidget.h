/***************************************************************************
                          paino.h  -  description
                             -------------------
    begin                : 17 Mar 2005
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

#ifndef PIANO_H
#define PIANO_H

#include <qwidget.h>
#include <qpixmap.h>
#include <QPaintEvent>
#include "drawwidget.h"

class PianoWidget:
public DrawWidget
{
public:
  PianoWidget(QWidget * parent);
  ~PianoWidget(void);

  void paintEvent(QPaintEvent *);
  
  QSize sizeHint(void) const
  {
    return QSize(165, 100);
  }

  int currentNote(void)
  {
    return _currentNote;
  }
  void setCurrentNote(int n, float amountPressed = 1.0);
  void setNoNote(void)
  {
    _currentNote = -1;
  }
  bool isNote(void)
  {
    return _currentNote >= 0;
  }
  float amountPressed(void)
  {
    return _amountPressed;
  }
  void setAmountPressed(float amountPressed)
  {
    _amountPressed = amountPressed;
  }
  
  QPixmap * buffer;

  //-1=no note, 0=C, 1=C#, 2=D ... 11=B
  int _currentNote;

  float _amountPressed;
};

#endif // PIANO_H
// EOF
