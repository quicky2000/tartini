/***************************************************************************
   bspline.cpp  - 
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

#include "bspline.h"
#include <math.h>
#include "useful.h"
#include "myassert.h"

//------------------------------------------------------------------------------
inline float interpolate_linear( int p_len
                               , const float * p_array
                               , float p_x
                               )
{
    int l_x0 = static_cast<int>(p_x);
    if(l_x0 >= 0 && l_x0 < p_len - 1)
    {
        float l_y0 = p_array[l_x0];
        float l_y1 = p_array[l_x0 + 1];
        p_x -= float(l_x0);
        return (l_y0 + p_x * (l_y1 - l_y0));
    }
    else
    {
        if(l_x0 < 0)
        {
            return p_array[0];
        }
        else
        {
            return p_array[p_len - 1];
        }
    }
}

//------------------------------------------------------------------------------
float interpolate_b_spline( int p_len
                          , const float * p_array
                          , float p_x
                          )
{
    int l_x0 = int(ceil(p_x));
    if((l_x0 < 0) || (l_x0 > p_len))
    {
        return 0; //{ printf("find_interpolate error\n"); exit(1); } //exit with an error
    }

    //check the end cases
    float l_y_2 = (l_x0 <= 1) ? p_array[0] : p_array[l_x0 - 2];
    float l_y_1 = (l_x0 == 0) ? p_array[0] : p_array[l_x0 - 1];
    float l_y0 = (l_x0 >= p_len) ? l_y_1 : p_array[l_x0];
    float l_y1 = (l_x0 >= p_len - 1) ? l_y0 : p_array[l_x0 + 1];
  
    //if p_x is an integer value we can do very quick
    if(float(l_x0) == p_x)
    {
        return (l_y_1 + 4 * l_y0 + l_y1) / 6.0f;
    }

    p_x = float(l_x0) - p_x;
    float l_xx = p_x * p_x;
    float l_xxx = l_xx * p_x;
  
    return (l_xxx * l_y_2 +
            (1 + 3 * (p_x + l_xx - l_xxx)) * l_y_1 +
            (4 - 6 * l_xx + 3 * l_xxx) * l_y0 +
            (1 - 3 * (p_x - l_xx) - l_xxx) * l_y1
           ) / 6.0f;
}


//------------------------------------------------------------------------------
float interpolate_cubic( int p_len
                       , const float * p_data
                       , const double & p_x
                       )
{
    int l_inpos = int(p_x);
    float l_finpos = float(p_x - double(l_inpos));
    float l_xm1;
    //take care of boundary cases on the left
    if(l_inpos < 1)
    {
        if(l_inpos < 0)
        {
            return p_data[0]; //just return the left most value
        }
        else
        {
            l_xm1 = p_data[l_inpos];
        }
    }
    else
    {
        l_xm1 = p_data[l_inpos - 1];
    }

    float l_x0;
    float l_x1;
    float l_x2;
    //take care of boundary cases on the right
    if(l_inpos > p_len - 3)
    {
        if(l_inpos > p_len - 2)
        {
            if(l_inpos > p_len - 1)
            {
                return p_data[p_len - 1];
            }
            else
            {
                l_x0 = l_x1 = l_x2 = p_data[l_inpos];
            }
        }
        else
        {
            l_x0 = p_data[l_inpos];
            l_x1 = l_x2 = p_data[l_inpos + 1];
        }
    }
    else
    {
        l_x0 = p_data[l_inpos];
        l_x1 = p_data[l_inpos + 1];
        l_x2 = p_data[l_inpos + 2];
    }

    float l_a = (3 * (l_x0 - l_x1) - l_xm1 + l_x2) / 2;
    float l_b = 2 * l_x1 + l_xm1 - (5 * l_x0 + l_x2) / 2;
    float l_c = (l_x1 - l_xm1) / 2;
    return (((l_a * l_finpos) + l_b) * l_finpos + l_c) * l_finpos + l_x0;
}


//------------------------------------------------------------------------------
void stretch_array( int p_in_len
                  , const float * p_in
                  , int p_out_len
                  , float * p_out
                  , float p_start
                  , float p_len
                  , t_spline_type p_type
                  )
{
    float l_x = p_start;
    float l_step = p_len / float(p_out_len);
  
    if(p_type == t_spline_type::LINEAR)
    {
        for(int l_j = 0; l_j < p_out_len; l_j++)
        {
            p_out[l_j] = interpolate_linear(p_in_len, p_in, l_x);
            l_x += l_step;
        }
    }
    else if(p_type == t_spline_type::BSPLINE)
    {
        for(int l_j = 0; l_j < p_out_len; l_j++)
        {
            p_out[l_j] = interpolate_b_spline(p_in_len, p_in, l_x);
            l_x += l_step;
        }
    }
    else if(p_type == t_spline_type::HERMITE_CUBIC)
    {
        for(int l_j = 0; l_j < p_out_len; l_j++)
        {
            p_out[l_j] = interpolate_cubic(p_in_len, p_in, l_x);
            l_x += l_step;
        }
    }
    else
    {
        myassert(false);
    }
}

//EOF
