/***************************************************************************
                          prony.cpp
                             -------------------
    begin                : Unknown
    copyright            : Unknown
    email                : Unknown

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#define _GLIBCXX_USE_C99 1
#define __WANT_EXTENSIONS__
#include <cmath>
#include <cstdio>
#include <complex>
#include <stdlib.h>

#include "prony.h"
#include "useful.h"
#include "mymatrix.h"
#include "array1d.h"

//------------------------------------------------------------------------------
bool pronyFit(PronyData *p_result, const float *p_x, int p_length, int p_gap, bool p_allow_offset)
{
    myassert(p_result != NULL);
    myassert(p_x != NULL);
    int l_j;
    double l_alpha[3];
    double l_omega;
    double l_error = 0.0;

    Array1d<float> l_x1(p_length);
    Array1d<float> l_x2(p_length);
    int l_gap2 = p_gap * 2;
    int l_n = p_length - l_gap2;
    for(l_j = 0; l_j < l_n; l_j++)
      {
          l_x1[l_j] = p_x[l_j] + p_x[l_j + l_gap2];
      }
    if(p_allow_offset)
      {
	if(!pinv(NULL, p_x + p_gap, l_x1.begin(), l_n, l_alpha))
	  {
	    return false;
	  }
          l_omega = acos(l_alpha[1] / 2) / p_gap;
      }
    else
      {
	if(!pinv(p_x + p_gap, l_x1.begin(), l_n, l_alpha))
	  {
	    return false;
	  }
          l_omega = acos(l_alpha[0] / 2) / p_gap;
      }
    if(std::isnan(l_omega))
      {
	return false;
      }

    p_result->m_omega = l_omega;
    for(l_j = 0; l_j < p_length; l_j++)
      {
          l_x1[l_j] = cos(l_j * l_omega);
          l_x2[l_j] = sin(l_j * l_omega);
      }
    if(p_allow_offset)
      {
	if(!pinv(NULL, l_x1.begin(), l_x2.begin(), p_x, p_length, l_alpha))
	  {
	    return false;
	  }
          p_result->m_y_offset = l_alpha[0];
          p_result->m_amp = hypot(l_alpha[1], l_alpha[2]);
          p_result->m_phase = (g_half_pi - atan2(l_alpha[2], l_alpha[1]));
      }
    else
      {
	if(!pinv(l_x1.begin(), l_x2.begin(), p_x, p_length, l_alpha))
	  {
	    return false;
	  }
          p_result->m_amp = hypot(l_alpha[0], l_alpha[1]);
          p_result->m_phase = (g_half_pi - atan2(l_alpha[1], l_alpha[0]));
      }

    //calculate the squared error
    for(l_j = 0; l_j < p_length; l_j++)
      {
	l_error += sq(p_result->m_amp * sin(l_j * l_omega + p_result->m_phase) + p_result->m_y_offset - p_x[l_j]);
      }
    p_result->m_error = l_error / p_length;

    return true;
}

//EOF
