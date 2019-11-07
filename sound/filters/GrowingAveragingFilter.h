/***************************************************************************
                          GrowingAveragingFilter.h  -  description
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
#ifndef GROWING_AVERAGING_FILTER_H
#define GROWING_AVERAGING_FILTER_H

#include "Filter.h"
#include "array1d.h"

/**
   An unwindowed (rectangular window) simple averaging filter with a fixed size
   It uses a simple incremental algorithm
*/
class GrowingAverageFilter : public Filter
{
  public:
    GrowingAverageFilter() { }
    GrowingAverageFilter(int p_size);
    void init(int p_size);
    void filter( const float * p_input
               , float * p_output
               , int p_n
               );
    void reset();
    inline int delay() const { return m_size / 2; }

  private:
    int m_size;
    int m_count;
    double m_total_sum;

    /**
       the last size input values
    */
    Array1d<float> m_x;
};

#endif // GROWING_AVERAGING_FILTER_H
// EOF
