/***************************************************************************
                          mymatrix.cpp
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
#include "mymatrix.h"
#include "stdio.h"
#include "cmath"
#include "algorithm"

#define MY_EPSILON 0.000001

//------------------------------------------------------------------------------
void print2x2matrix(const double * p_x)
{
  printf("[%8.5lf %8.5lf]\n", p_x[0], p_x[1]);
  printf("[%8.5lf %8.5lf]\n", p_x[2], p_x[3]);
}

//------------------------------------------------------------------------------
void print3x3matrix(const double * p_x)
{
  printf("[%8.5lf %8.5lf %8.5lf]\n", p_x[0], p_x[1], p_x[2]);
  printf("[%8.5lf %8.5lf %8.5lf]\n", p_x[3], p_x[4], p_x[5]);
  printf("[%8.5lf %8.5lf %8.5lf]\n", p_x[6], p_x[7], p_x[8]);
}

//------------------------------------------------------------------------------
bool invert2x2matrix(const double * p_input, double * p_output)
{
  double l_det = determinant2x2(p_input[0], p_input[1], p_input[2], p_input[3]);
  if(fabs(l_det) < MY_EPSILON)
    {
      return false;
    }
    p_output[0] = p_input[3] / l_det;
    p_output[1] = -p_input[1] / l_det;
    p_output[2] = -p_input[2] / l_det;
    p_output[3] = p_input[0] / l_det;
  return true;
}

//------------------------------------------------------------------------------
bool invert3x3matrix(const double * p_x, double * p_y)
{
  double l_det = determinant3x3(p_x);
  if(fabs(l_det) < MY_EPSILON)
    {
      return false;
    }
    p_y[0] = determinant2x2(p_x[4], p_x[5], p_x[7], p_x[8]) / l_det;
    p_y[1] = determinant2x2(p_x[2], p_x[1], p_x[8], p_x[7]) / l_det;
    p_y[2] = determinant2x2(p_x[1], p_x[2], p_x[4], p_x[5]) / l_det;
    p_y[3] = determinant2x2(p_x[5], p_x[3], p_x[8], p_x[6]) / l_det;
    p_y[4] = determinant2x2(p_x[0], p_x[2], p_x[6], p_x[8]) / l_det;
    p_y[5] = determinant2x2(p_x[2], p_x[0], p_x[5], p_x[3]) / l_det;
    p_y[6] = determinant2x2(p_x[3], p_x[4], p_x[6], p_x[7]) / l_det;
    p_y[7] = determinant2x2(p_x[1], p_x[0], p_x[7], p_x[6]) / l_det;
    p_y[8] = determinant2x2(p_x[0], p_x[1], p_x[3], p_x[4]) / l_det;
  return true;
}

//------------------------------------------------------------------------------
bool pinv(const float * p_x1, const float * p_y, int p_n, double * p_alpha)
{
  double l_sum_x1_y = 0.0;
  double l_sum_x1_x1 = 0.0;
  for(int l_j = 0; l_j < p_n; l_j++)
    {
        l_sum_x1_y += p_x1[l_j] * p_y[l_j];
        l_sum_x1_x1 += p_x1[l_j] * p_x1[l_j];
    }
  if(fabs(l_sum_x1_x1) < MY_EPSILON)
    {
      return false;
    }
    p_alpha[0] = l_sum_x1_y / l_sum_x1_x1;
  return true;
}

//------------------------------------------------------------------------------
bool pinv(const float * p_x1, const float * p_x2, const float * p_y, int p_n, double * p_alpha)
{
  double l_s[4];
  double l_s_inv[4];
  double l_sum_x1_y = 0.0;
  double l_sum_x2_y = 0.0;
    l_s[0] = l_s[1] = l_s[3] = 0.0;
  if(p_x1)
    {
      for(int l_j = 0; l_j < p_n; l_j++)
	{
        l_s[0] += p_x1[l_j] * p_x1[l_j];
        l_s[1] += p_x1[l_j] * p_x2[l_j];
        l_s[3] += p_x2[l_j] * p_x2[l_j];
        l_sum_x1_y += p_x1[l_j] * p_y[l_j];
        l_sum_x2_y += p_x2[l_j] * p_y[l_j];
	}
    }
  else
    {
        l_s[0] = p_n;
      for(int l_j = 0; l_j < p_n; l_j++)
	{
        l_s[1] += p_x2[l_j];
        l_s[3] += p_x2[l_j] * p_x2[l_j];
        l_sum_x1_y += p_y[l_j];
        l_sum_x2_y += p_x2[l_j] * p_y[l_j];
	}
    }
    l_s[2] = l_s[1];
  if(!invert2x2matrix(l_s, l_s_inv))
    {
      return false;
    }

    p_alpha[0] = l_s_inv[0] * l_sum_x1_y + l_s_inv[1] * l_sum_x2_y;
    p_alpha[1] = l_s_inv[2] * l_sum_x1_y + l_s_inv[3] * l_sum_x2_y;
  return true;
}

//------------------------------------------------------------------------------
bool pinv(const float * p_x1, const float * p_x2, const float * p_x3, const float * p_y, int p_n, double * p_alpha)
{
  double l_s[9];
  double l_s_inv[9];
  double l_sum_x1_y = 0.0;
  double l_sum_x2_y=0.0;
  double l_sum_x3_y=0.0;
  std::fill(l_s, l_s + 9, 0.0);
  if(p_x1)
    {
      for(int l_j = 0; l_j < p_n; l_j++)
	{
        l_s[0] += p_x1[l_j] * p_x1[l_j];
        l_s[1] += p_x1[l_j] * p_x2[l_j];
        l_s[2] += p_x1[l_j] * p_x3[l_j];
        l_s[4] += p_x2[l_j] * p_x2[l_j];
        l_s[5] += p_x2[l_j] * p_x3[l_j];
        l_s[8] += p_x3[l_j] * p_x3[l_j];
        l_sum_x1_y += p_x1[l_j] * p_y[l_j];
        l_sum_x2_y += p_x2[l_j] * p_y[l_j];
        l_sum_x3_y += p_x3[l_j] * p_y[l_j];
    }
  }
  else
    {
        l_s[0] = p_n;
      for(int l_j = 0; l_j < p_n; l_j++)
	{
        l_s[1] += p_x2[l_j];
        l_s[2] += p_x3[l_j];
        l_s[4] += p_x2[l_j] * p_x2[l_j];
        l_s[5] += p_x2[l_j] * p_x3[l_j];
        l_s[8] += p_x3[l_j] * p_x3[l_j];
        l_sum_x1_y += p_y[l_j];
        l_sum_x2_y += p_x2[l_j] * p_y[l_j];
        l_sum_x3_y += p_x3[l_j] * p_y[l_j];
	}
    }
    l_s[3] = l_s[1];
    l_s[6] = l_s[2];
    l_s[7] = l_s[5];
  if(!invert3x3matrix(l_s, l_s_inv))
    {
      return false;
    }

    p_alpha[0] = l_s_inv[0] * l_sum_x1_y + l_s_inv[1] * l_sum_x2_y + l_s_inv[2] * l_sum_x3_y;
    p_alpha[1] = l_s_inv[3] * l_sum_x1_y + l_s_inv[4] * l_sum_x2_y + l_s_inv[5] * l_sum_x3_y;
    p_alpha[2] = l_s_inv[6] * l_sum_x1_y + l_s_inv[7] * l_sum_x2_y + l_s_inv[8] * l_sum_x3_y;
  return true;
}

//EOF
