/***************************************************************************
   bspline.cpp  - 
    Functions for interpolating between values in an array.
    Both linear and cubic b-spline functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "bspline.h"
#include <math.h>

//a linear interpolation
//quicker than b-spline
float interpolate_linear(int len, float *array, float x)
{
  int x0 = int(floor(x));
  if(x0 < 0 || x0 > len - 2) return 0; //{ printf("find_interpolate error\n"); exit(1); } //exit with an error
  float y0 = array[x0];
  float y1 = array[x0+1];
  x -= float(x0);
  return (y0 + x*(y1-y0));
}

//a cubic b-spline interpolation
//NOTE: the values don't always pass through the in points
//      but it gives a nice smooth curve
//slower than linear
float interpolate_b_spline(int len, float *array, float x)
{
  int x0 = int(ceil(x));
  if((x0 < 0) || (x0 > len)) return 0; //{ printf("find_interpolate error\n"); exit(1); } //exit with an error
  float y_2, y_1, y0, y1;
  //check the end cases
  y_2 = (x0 <= 1) ? array[0] : array[x0-2];
  y_1 = (x0 == 0) ? array[0] : array[x0-1];
  y0 = (x0 >= len) ? y_1 : array[x0];
  y1 = (x0 >= len-1) ? y0 : array[x0+1];
  
  //if x is an integer value we can do very quick
  if(float(x0) == x) return (y_1 + 4*y0 + y1) / 6.0f;
  
  x = float(x0)-x;
  float xx = x*x;
  float xxx = xx*x;
  
  return (xxx*y_2 +
	  (1 + 3*(x + xx - xxx))*y_1 +
	  (4 - 6*xx + 3*xxx)*y0 +
	  (1 - 3*(x - xx) - xxx)*y1) / 6.0f;
}


//Uses a Hermite curve
float interpolate_cubic(int len, float *data, double x)
{
	int inpos = int(x);
	float finpos = float(x - double(inpos));
	float xm1;
	//take care of boundary cases on the left
	if(inpos < 1) {
		if(inpos < 0)	return data[0]; //just return the left most value
		else
			xm1 = data[inpos];
	} else
		 xm1 = data[inpos - 1];

	float x0;
	float x1;
	float x2;
	//take care of boundary cases on the right
	if(inpos > len-3) {
		if(inpos > len-2) {
			if(inpos > len-1)	return data[len-1];
			else {
				x0 = x1 = x2 = data[inpos];
			}
		} else {
			x0 = data[inpos];
			x1 = x2 = data[inpos+1];
		}
	} else {
		x0 = data[inpos];
		x1 = data[inpos+1];
		x2 = data[inpos+2];
	}

	float a = (3 * (x0-x1) - xm1 + x2) / 2;
	float b = 2*x1 + xm1 - (5*x0 + x2) / 2;
	float c = (x1 - xm1) / 2;
	return (((a * finpos) + b) * finpos + c) * finpos + x0;
}


//NOTE: this could be made more efficient
//in_len & *in are the array of floats for input
//out_len & *out are the array where the (resampled) stretched array output is stored
//start = where the to start resampling from the *in array
//len = the length of the resampling from the *in array
//type = LINEAR or BSPLINE
void stretch_array(int in_len, float *in, int out_len, float *out, float start, float len, int type)
{
  float step = len / float(out_len);
  float x = start;
  
  if(type == LINEAR) {
    for(int j=0; j<out_len; j++) {
      out[j] = interpolate_linear(in_len, in, x);
      x += step;
    }
  } else if(type == BSPLINE) {
      for(int j=0; j<out_len; j++) {
	  out[j] = interpolate_b_spline(in_len, in, x);
	  x += step;
      }
  } else {
      for(int j=0; j<out_len; j++) {
	  out[j] = interpolate_cubic(in_len, in, x);
	  x += step;
      }
  }
}
