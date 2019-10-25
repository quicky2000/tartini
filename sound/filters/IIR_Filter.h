/***************************************************************************
                          IIR_Filter.h  -  description
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
#ifndef IIR_FILTER_H
#define IIR_FILTER_H

#include "Filter.h"
#include "array1d.h"

class FilterState;

/**
   Infinite Impulse Response filter
*/
class IIR_Filter : public Filter
{

  public:

    inline IIR_Filter(void) { }

    /**
       Create an IIR Filter from the coeff's a direct form II transposed structure
       if a[0] is not 1.0 then all the coefficients are normalized be dividing by a[0]
    */
    IIR_Filter( double * p_b
              , double * p_a
              , int p_n
              , int p_m = -1
              );
    inline ~IIR_Filter(void) { }


    /**
       Initialise the IIR Filter from the coeff's a direct form II transposed structure
       if a[0] is not 1.0 then all the coefficients are normalized be dividing by a[0]
    */
    void init( double * p_b
             , double * p_a
             , int p_n
             , int p_m = -1
             );
    void print(void);

    /**
       Apply the filter to a block of data
       @param p_input The data to be filtered
       @param p_output Where the filtered result is stored. Note: The output is delayed by the filter size.
       @param p_n The size of data
    */
    void filter( const float * p_input
               , float * p_output
               , int p_n
               );

    /**
       Resets the filter back to an initial state of zeros
    */
    void reset(void);
    void getState(FilterState * p_filter_state) const;
    void setState(const FilterState * p_filter_state);

  private:
    /**
       tempery buffer storage
    */
    Array1d<double> m_buf_x;
    Array1d<double> m_buf_y;

    /**
       The filter coefficient's
    */
    Array1d<double> m_a;
    Array1d<double> m_b;

    /**
       The current filter state (last n states of input and output)
    */
    Array1d<double> m_x;
    Array1d<double> m_y;

};

class FilterState
{
  public:
    Array1d<double> m_x;
    Array1d<double> m_y;
};

#endif // IIR_FILTER_H
// EOF
