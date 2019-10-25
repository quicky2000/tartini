/***************************************************************************
                          FastSmoothAveragingFilter.cpp  -  description
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
#include <iostream>
#include "FastSmoothedAveragingFilter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

//------------------------------------------------------------------------------
FastSmoothedAveragingFilter::FastSmoothedAveragingFilter(int p_size)
{
  init(p_size);
}

//------------------------------------------------------------------------------
void FastSmoothedAveragingFilter::init(int p_size)
{
    m_size = p_size;
    m_size_left = m_size / 2;
    m_size_right = m_size - m_size_left;
    m_angle = -2 * M_PI / double(p_size + 1);
    m_sin_angle = sin(m_angle);
    m_cos_angle = cos(m_angle);
    m_sum = 0;
    for(int l_j = 0; l_j < p_size; l_j++)
    {
        m_sum += 1.0 - cos((l_j + 1) * m_angle);
    }

    m_cos_sum = m_sin_sum = m_total_sum = 0.0;
    m_x.resize(p_size);
    reset();
}

#include "fast_smooth.h"

//------------------------------------------------------------------------------
void FastSmoothedAveragingFilter::filter( const float * p_input
                                        , float * p_output
                                        , int p_n
                                        )
{
    //blur stays centered if odd
    int l_j;
    if(p_n > m_size)
    {
        for(l_j = 0; l_j < m_size; l_j++)
        {
            m_cos_sum += p_input[l_j];
            fast_complex_rotate(m_sin_sum, m_cos_sum, m_sin_angle, m_cos_angle);
            m_cos_sum -= m_x[l_j];
            m_total_sum += p_input[l_j] - m_x[l_j];
            p_output[l_j] = (m_total_sum - m_cos_sum) / m_sum;
        }
        for(l_j = m_size; l_j < p_n; l_j++)
        {
            m_cos_sum += p_input[l_j];
            fast_complex_rotate(m_sin_sum, m_cos_sum, m_sin_angle, m_cos_angle);
            m_cos_sum -= p_input[l_j - m_size];
            m_total_sum += p_input[l_j] - p_input[l_j - m_size];
            p_output[l_j] = (m_total_sum - m_cos_sum) / m_sum;
        }
        std::copy(p_input + p_n - m_size, p_input + p_n, m_x.begin());
    }
    else
    {
        for(l_j = 0; l_j < p_n; l_j++)
        {
            m_cos_sum += p_input[l_j];
            fast_complex_rotate(m_sin_sum, m_cos_sum, m_sin_angle, m_cos_angle);
            m_cos_sum -= m_x[l_j];
            m_total_sum += p_input[l_j] - m_x[l_j];
            p_output[l_j] = (m_total_sum - m_cos_sum) / m_sum;
        }
        m_x.shift_left(p_n);
        std::copy(p_input, p_input + p_n, m_x.end() - p_n);
    }
}

//------------------------------------------------------------------------------
void FastSmoothedAveragingFilter::reset(void)
{
    m_cos_sum = m_sin_sum = m_total_sum = 0.0;
    m_x.fill(0.0f);
}

//EOF
