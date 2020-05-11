/***************************************************************************
   bspline.h  - 
    Functions for interpolating between values in an array.
    Both linear and cubic b-spline functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef BSPLINE_H
#define BSPLINE_H

typedef enum class SplineType { LINEAR, BSPLINE, HERMITE_CUBIC } t_spline_type;

/**
   a linear interpolation
   quicker than b-spline
*/
float interpolate_linear( int p_len
                        , const float * p_array
                        , float p_x
                        );

/**
   a cubic b-spline interpolation
   NOTE: the values don't always pass through the in points
         but it gives a nice smooth curve
         slower than linear
*/
float interpolate_b_spline( int p_len
                          , const float * p_array
                          , float p_x
                          );

/**
   Uses a Hermite curve
*/
float interpolate_cubic( int p_len
                       , const float * p_data
                       , const double & p_x
                       );

/**
   NOTE: Only LINEAR has been optimised a little for speed. The others could be made more efficient
   @param p_in_len & *in are the array of floats for input
   @param p_out_len & *out are the array where the (resampled) stretched array output is stored
   @param p_start = where the to start resampling from the *in array
   @param p_len = the length of the resampling from the *in array
   @param p_type = LINEAR or BSPLINE
*/
//------------------------------------------------------------------------------
void stretch_array( int p_in_len
                  , const float * p_in
                  , int p_out_len
                  , float * p_out
                  , float p_start
                  , float p_len
                  , t_spline_type p_type
                  );

#endif // BSPLINE_H
//EOF
