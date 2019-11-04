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
QColor colorBetween( QColor p_color_a
                   , QColor p_color_b
                   , double p_ratio
                   )
{
    //myassert(p_ratio >= 0.0 && p_ratio <= 1.0);
    if(p_ratio < 0.0)
    {
        p_ratio = 0.0;
    }
    if(p_ratio > 1.0)
    {
        p_ratio = 1.0;
    }
    return QColor( p_color_a.red() + int(p_ratio * float(p_color_b.red() - p_color_a.red()))
                 , p_color_a.green() + int(p_ratio * float(p_color_b.green() - p_color_a.green()))
                 , p_color_a.blue() + int(p_ratio * float(p_color_b.blue() - p_color_a.blue()))
                 );
}

//EOF
