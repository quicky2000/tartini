/***************************************************************************
                          myqt.h
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
#ifndef MYQT_H
#define MYQT_H

#include <QColor>

/**
   Gives a color between a and b, using ratio (a number between 0 and 1)
*/
extern QColor colorBetween(QColor a, QColor b, double ratio);

#endif // MYQT_H
// EOF

