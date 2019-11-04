/***************************************************************************
                           fast_smooth.cpp
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
#include <math.h>
#include "fast_smooth.h"
#include <numeric>

//------------------------------------------------------------------------------
fast_smooth::fast_smooth(int p_size):
        m_size(p_size),
        m_size_left(m_size / 2),
        m_size_right(m_size - m_size_left),
        m_angle(-2 * M_PI / double(p_size + 1)),
        m_cos_angle(cos(m_angle)),
        m_sin_angle(sin(m_angle)),
        m_sum(0)
{
  for(int l_j = 0; l_j < p_size; l_j++)
    {
        m_sum += 1.0 - cos((l_j + 1) * m_angle);
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothA(float *p_source, float *p_dest, int p_length, int p_step)
{
  if(p_step == 1)
    {
      fast_smoothA(p_source, p_dest, p_length);
      return;
    }
  //blur stays centered if odd
  double l_cos_sum = 0;
  double l_sin_sum = 0;
  double l_total_sum = 0;
  int l_j;
  for(l_j = 0; l_j < m_size_right; l_j++)
    {
        l_cos_sum += p_source[l_j * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[l_j * p_step];
    }
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right) * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[(l_j + m_size_right) * p_step];
    }
  for(l_j = m_size_left; l_j < p_length - m_size_left - 1; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right) * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left) * p_step];
        l_total_sum += p_source[(l_j + m_size_right) * p_step] - p_source[(l_j - m_size_left) * p_step];
  }
  for(l_j = p_length - m_size_left - 1; l_j < p_length; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left) * p_step];
        l_total_sum -= p_source[(l_j - m_size_left) * p_step];
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothA(float *p_source, float *p_dest, int p_length)
{
  //blur stays centered if odd
  double l_cos_sum = 0;
  double l_sin_sum = 0;
  double l_total_sum = 0;
  int l_j;
  for(l_j = 0; l_j < m_size_right; l_j++)
    {
        l_cos_sum += p_source[l_j];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[l_j];
    }
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right)];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[(l_j + m_size_right)];
    }
  for(l_j = m_size_left; l_j < p_length - m_size_left - 1; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right)];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left)];
        l_total_sum += p_source[(l_j + m_size_right)] - p_source[(l_j - m_size_left)];
    }
  for(l_j = p_length - m_size_left - 1; l_j < p_length; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left)];
        l_total_sum -= p_source[(l_j - m_size_left)];
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothB(float *p_source, float *p_dest, int p_length, int p_step)
{
  if(p_step == 1)
    {
      fast_smoothB(p_source, p_dest, p_length);
      return;
    }
  //blur should be odd
  double l_cos_sum = 0;
  double l_sin_sum = 0;
  double l_total_sum = 0;
  int l_j;
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        l_cos_sum += p_source[0 * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[0 * p_step];
    }
  for(l_j = 0; l_j < m_size_right; l_j++)
    {
        l_cos_sum += p_source[l_j * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[l_j * p_step];
    }
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right) * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[0 * p_step];
        l_total_sum += p_source[(l_j + m_size_right) * p_step] - p_source[0 * p_step];
    }
  for(l_j = m_size_left; l_j < p_length - m_size_left - 1; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right) * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left) * p_step];
        l_total_sum += p_source[(l_j + m_size_right) * p_step] - p_source[(l_j - m_size_left) * p_step];
    }
  for(l_j = p_length - m_size_left - 1; l_j < p_length; l_j++)
    {
        p_dest[l_j * p_step] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(p_length - 1) * p_step];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left) * p_step];
        l_total_sum += p_source[(p_length - 1) * p_step] - p_source[(l_j - m_size_left) * p_step];
    }
}

#include <stdio.h>

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothB(float *p_source, float *p_dest, int p_length)
{
  //blur should be odd
  double l_cos_sum = 0;
  double l_sin_sum = 0;
  double l_total_sum = 0;
  int l_j;
  if(p_length < m_size)
    {
      float l_avg = std::accumulate(p_source, p_source + p_length, 0.0) / float(p_length);
      std::fill(p_dest, p_dest + p_length, l_avg);
      return;
    }
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        l_cos_sum += p_source[0];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[0];
    }
  //FIXME: Doesn't deal with a blur bigger than the p_length
  for(l_j = 0; l_j < m_size_right; l_j++)
    {
        l_cos_sum += p_source[l_j];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_total_sum += p_source[l_j];
    }
  for(l_j = 0; l_j < m_size_left; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right)];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[0];
        l_total_sum += p_source[(l_j + m_size_right)] - p_source[0];
    }
  for(l_j = m_size_left; l_j < p_length - m_size_left - 1; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(l_j + m_size_right)];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left)];
        l_total_sum += p_source[(l_j + m_size_right)] - p_source[(l_j - m_size_left)];
    }
  for(l_j = p_length - m_size_left - 1; l_j < p_length; l_j++)
    {
        p_dest[l_j] = (l_total_sum - l_cos_sum) / m_sum;
        l_cos_sum += p_source[(p_length - 1)];
      fast_complex_rotate(l_sin_sum, l_cos_sum, m_sin_angle, m_cos_angle);
        l_cos_sum -= p_source[(l_j - m_size_left)];
        l_total_sum += p_source[(p_length - 1)] - p_source[(l_j - m_size_left)];
    }
}

//EOF
