/***************************************************************************
                          FixedAveragingFilter.h  -  description
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
#ifndef FIXED_AVERAGING_FILTER_H
#define FIXED_AVERAGING_FILTER_H

#include "Filter.h"
#include "array1d.h"

/**
   An unwindowed (rectangular window) simple averaging filter with a fixed size
   It uses a simple incremental algorithm
*/
class FixedAverageFilter : public Filter
{
  public:
    inline FixedAverageFilter(void) { }
    FixedAverageFilter(int p_size);
    void init(int p_size);
    void filter( const float * p_input
               , float * p_output
               , int p_n
               );
    void reset(void);
    int delay(void) const { return m_size / 2; }

  private:
    int m_size;
    double m_total_sum;
    Array1d<float> m_x; // the last size input values
};

#endif // FIXED_AVERAGING_FILTER_H
// EOF
