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
inline void fast_complex_rotate(double &p_x, double &p_y, const double & p_sin_angle, const double & p_cos_angle);

/**
   A class to perform smoothing/bluring on data using a hanning (cos shaped) window.
   This uses and fast internal rotation algorithm
*/
class fast_smooth
{
    int m_size, m_size_left, m_size_right;
    double m_angle;
    double m_cos_angle, m_sin_angle;
    double m_sum;

 public:
    /**
        the total width of the hanning window. To keep data centered use an odd size
    */
    fast_smooth(int p_size);
    void fast_smoothA(float *p_source, float *p_dest, int p_length, int p_step);
    void fast_smoothA(float *p_source, float *p_dest, int p_length);

    /**
       treats all values off either end the same as the value at end
    */
    void fast_smoothB(float *p_source, float *p_dest, int p_length, int p_step);

    /**
       treats all values off either end the same as the value at end
    */
    void fast_smoothB(float *p_source, float *p_dest, int p_length);
};

#include "fast_smooth.hpp"

#endif // _FAST_SMOOTH_H
//EOF
