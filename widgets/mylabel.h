/***************************************************************************
                          mylabel.h  -  description
                             -------------------
    begin                : 29/6/2005
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

#ifndef MYLABEL_H
#define MYLABEL_H

#include "drawwidget.h"
#include <qstring.h>
#include <QPaintEvent>

/**
   MyLabel uses double buffering to draw simple text.
   So is faster (espicially on the Mac) than using QLabel.
   It only supports 1 line of text.
*/
class MyLabel : public DrawWidget
{
public:
  MyLabel(const QString & text_, QWidget * parent, const char * name = 0);
  QString text(void)
  {
    return _text;
  }
  void setText(const QString & text_)
  {
    _text = text_;
    update();
  }
  
  void paintEvent( QPaintEvent * );
  QSize sizeHint(void) const
  {
    return QSize(_textWidth + 8, _fontHeight + 4);
  }

 private:
  QString _text;
  QFont _font;
  int _fontHeight;
  int _textWidth;

};

#endif // MYLABEL_H
// EOF
