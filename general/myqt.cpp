/***************************************************************************
                          myqt.cpp
                             -------------------
    begin                : Unknown
    copyright            : Unknown
    email                : Unknown

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "myqt.h"

/**
   Gives a color between a and b, using ratio (a number between 0 and 1)
*/
QColor colorBetween(QColor a, QColor b, double ratio)
{
  //myassert(ratio >= 0.0 && ratio <= 1.0);
  if(ratio < 0.0)
    {
      ratio = 0.0;
    }
  if(ratio > 1.0)
    {
      ratio = 1.0;
    }
  return QColor(
		a.red()+int(ratio*float(b.red()-a.red())),
		a.green()+int(ratio*float(b.green()-a.green())),
		a.blue()+int(ratio*float(b.blue()-a.blue()))
		);
}

//EOF
