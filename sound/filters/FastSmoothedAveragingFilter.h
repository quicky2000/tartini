/***************************************************************************
                          FastSmoothAveragingFilter.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef FAST_SMOOTHED_AVERAGING_FILTER_H
#define FAST_SMOOTHED_AVERAGING_FILTER_H

#include "Filter.h"
#include "array1d.h"

/**
   A class to perform smoothing/bluring on data using a hanning (cos shaped) window.
   It uses and fast internal rotation algorithm
   O(t) = n + size. Where n is the length of data, and size is the width of the filter
*/
class FastSmoothedAveragingFilter : public Filter
{
public:
  /**
     Construct a FastSmoothedAverageFilter
     @param size The total width of the hanning window. To keep data centered use an odd size
  */
  FastSmoothedAveragingFilter(void) { }
  FastSmoothedAveragingFilter(int size);
  void init(int size);
  void filter(const float *input, float *output, int n);
  void reset(void);
  inline int delay(void) const { return _size / 2; }

 private:
  int _size, _size_left, _size_right;
  double _angle;
  double _cos_angle, _sin_angle;
  double _sum;

  /**
     the last size input values
  */
  Array1d<float> _x;

  double cos_sum;
  double sin_sum;
  double total_sum;

};

#endif // FAST_SMOOTHED_AVERAGING_FILTER_H
// EOF
