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
bool pronyFit(PronyData *result, const float *x, int length, int gap, bool allowOffset)
{
    myassert(result != NULL);
    myassert(x != NULL);
    int j;
    double alpha[3];
    double omega;
    double error = 0.0;

    Array1d<float> x1(length);
    Array1d<float> x2(length);
    int gap2 = gap*2;
    int n = length - gap2;
    for(j = 0; j < n; j++)
      {
	x1[j] = x[j] + x[j + gap2];
      }
    if(allowOffset)
      {
	if(!pinv(NULL, x + gap, x1.begin(), n, alpha))
	  {
	    return false;
	  }
	omega = acos(alpha[1] / 2) / gap;
      }
    else
      {
	if(!pinv(x + gap, x1.begin(), n, alpha))
	  {
	    return false;
	  }
	omega = acos(alpha[0] / 2) / gap;
      }
    if(std::isnan(omega))
      {
	return false;
      }

    result->omega = omega;
    for(j = 0; j < length; j++)
      {
	x1[j] = cos(j * omega);
	x2[j] = sin(j * omega);
      }
    if(allowOffset)
      {
	if(!pinv(NULL, x1.begin(), x2.begin(), x, length, alpha))
	  {
	    return false;
	  }
	result->yOffset = alpha[0];
	result->amp = hypot(alpha[1], alpha[2]);
	result->phase = (HalfPi - atan2(alpha[2], alpha[1]));
      }
    else
      {
	if(!pinv(x1.begin(), x2.begin(), x, length, alpha))
	  {
	    return false;
	  }
	result->amp = hypot(alpha[0], alpha[1]);
	result->phase = (HalfPi - atan2(alpha[1], alpha[0]));
      }

    //calculate the squared error
    for(j = 0; j < length; j++)
      {
	error += sq(result->amp * sin(j*omega + result->phase) + result->yOffset  -  x[j]);
      }
    result->error = error / length;

    return true;
}

//EOF
