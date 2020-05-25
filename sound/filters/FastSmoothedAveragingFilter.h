/***************************************************************************
                          FastSmoothAveragingFilter.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

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
    FastSmoothedAveragingFilter() { }
    /**
       Construct a FastSmoothedAverageFilter
       @param p_size The total width of the hanning window. To keep data centered use an odd size
    */
    FastSmoothedAveragingFilter(int p_size);
    void init(int p_size);
    void filter( const float * p_input
               , float * p_output
               , int p_n
               ) override;
    void reset() override;
    inline int delay() const override { return m_size / 2; }

  private:
    int m_size;
    int m_size_left;
    int m_size_right;
    double m_angle;
    double m_cos_angle;
    double m_sin_angle;
    double m_sum;

    /**
     the last size input values
    */
    Array1d<float> m_x;

    double m_cos_sum;
    double m_sin_sum;
    double m_total_sum;
};

#endif // FAST_SMOOTHED_AVERAGING_FILTER_H
// EOF
