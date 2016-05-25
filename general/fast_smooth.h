/***************************************************************************
                           fast_smooth.h
                           -------------------
    begin                : Feb 2005
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
#ifndef _FAST_SMOOTH_H
#define _FAST_SMOOTH_H

/**
   rotates a the complex number (sin_sum, cos_sum) by an angle.
*/
inline void fast_complex_rotate(double &x, double &y, const double & sin_angle, const double & cos_angle);

/**
   A class to perform smoothing/bluring on data using a hanning (cos shaped) window.
   This uses and fast internal rotation algorithm
*/
class fast_smooth
{
    int _size, _size_left, _size_right;
    double _angle;
    double _cos_angle, _sin_angle;
    double _sum;

 public:
    /**
        the total width of the hanning window. To keep data centered use an odd size
    */
    fast_smooth(int size);
    void fast_smoothA(float *source, float *dest, int length, int step);
    void fast_smoothA(float *source, float *dest, int length);

    /**
       treats all values off either end the same as the value at end
    */
    void fast_smoothB(float *source, float *dest, int length, int step);

    /**
       treats all values off either end the same as the value at end
    */
    void fast_smoothB(float *source, float *dest, int length);
};

#include "fast_smooth.hpp"

#endif // _FAST_SMOOTH_H
//EOF
