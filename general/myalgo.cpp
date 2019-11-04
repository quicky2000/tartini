/***************************************************************************
                          myalgo.cpp  -  Generic useful functions
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "myalgo.h"

//------------------------------------------------------------------------------
void bresenham1d( const Array1d<float> & p_input
                , Array1d<float> & p_output
                )
{
    //Use Bresenham's algorithm in 1d to choose the points to draw
    int l_w = p_output.size();
    int l_int_step = int(p_input.size() / l_w);
    int l_remainder_step = p_input.size() - (l_int_step * l_w);
    int l_pos = 0;
    int l_remainder = 0;
    for(int l_j = 0; l_j < l_w; l_j++, l_pos += l_int_step, l_remainder += l_remainder_step)
    {
        if(l_remainder >= l_w)
        {
            l_pos++;
            l_remainder -= l_w;
        }
        myassert(l_pos < p_input.size());
        p_output[l_j] = p_input[l_pos];
    }
}

//------------------------------------------------------------------------------
void maxAbsDecimate1d( const Array1d<float> & p_input
                     , Array1d<float> & p_output
                     )
{
    if(p_input.size() > p_output.size())
    {
        //Use Bresenham's algorithm in 1d to choose the points to draw
        int l_w = p_output.size();
        if(l_w <= 0)
        {
            return;
        }
        int l_int_step = int(p_input.size() / l_w);
        int l_remainder_step = p_input.size() - (l_int_step * l_w);
        int l_pos = 0;
        int l_remainder = 0;
        int l_prev = 0;
        int l_j = 0;

        do
        {
            l_pos += l_int_step;
            l_remainder += l_remainder_step;
            if(l_remainder >= l_w)
            {
                l_pos++;
                l_remainder -= l_w;
            }
            myassert(l_pos <= p_input.size());
            p_output[l_j] = *std::max_element(p_input.begin() + l_prev, p_input.begin() + l_pos, absoluteLess<float>());
            l_prev = l_pos;
        }
        while(++l_j < l_w);
    }
    else if(p_input.size() == p_output.size())
    {
        std::copy(p_input.begin(), p_input.end(), p_output.begin());
    }
    else
    {
        bresenham1d(p_input, p_output);
    }
}
//EOF
