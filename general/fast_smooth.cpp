#include <math.h>
#include "fast_smooth.h"
#include <numeric>

//------------------------------------------------------------------------------
fast_smooth::fast_smooth(int size)
{
  _size = size;
  _size_left = _size / 2;
  _size_right = _size - _size_left;
  _angle = -2*M_PI/double(size + 1); 
  _sin_angle = sin(_angle);
  _cos_angle = cos(_angle);
  _sum = 0;
  int j;
  for(j = 0; j < size; j++)
    {
      _sum += 1.0 - cos((j + 1) * _angle);
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothA(float *source, float *dest, int length, int step)
{
  if(step == 1)
    {
      fast_smoothA(source, dest, length);
      return;
    }
  //blur stays centered if odd
  double cos_sum = 0;
  double sin_sum = 0;
  double total_sum = 0;
  int j;
  for(j = 0; j < _size_right; j++)
    {
      cos_sum += source[j * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[j * step];
    }
  for(j = 0; j < _size_left; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right) * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[(j + _size_right) * step];
    }
  for(j = _size_left; j < length - _size_left - 1; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right) * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left) * step];
      total_sum += source[(j + _size_right) * step] - source[(j - _size_left) * step];
  }
  for(j = length - _size_left - 1; j < length; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left) * step];
      total_sum -= source[(j - _size_left) * step];
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothA(float *source, float *dest, int length)
{
  //blur stays centered if odd
  double cos_sum = 0;
  double sin_sum = 0;
  double total_sum = 0;
  int j;
  for(j = 0; j < _size_right; j++)
    {
      cos_sum += source[j];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[j];
    }
  for(j = 0; j < _size_left; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right)];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[(j + _size_right)];
    }
  for(j = _size_left; j < length - _size_left - 1; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right)];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left)];
      total_sum += source[(j + _size_right)] - source[(j - _size_left)];
    }
  for(j = length - _size_left - 1; j < length; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left)];
      total_sum -= source[(j - _size_left)];
    }
}

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothB(float *source, float *dest, int length, int step)
{
  if(step == 1)
    {
      fast_smoothB(source, dest, length);
      return;
    }
  //blur should be odd
  double cos_sum = 0;
  double sin_sum = 0;
  double total_sum = 0;
  int j;
  for(j = 0; j < _size_left; j++)
    {
      cos_sum += source[0* step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[0* step];
    }
  for(j = 0; j<_size_right; j++)
    {
      cos_sum += source[j * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[j * step];
    }
  for(j = 0; j<_size_left; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right) * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[0* step];
      total_sum += source[(j + _size_right) * step] - source[0* step];
    }
  for(j = _size_left; j < length - _size_left-1; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right) * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left) * step];
      total_sum += source[(j + _size_right) * step] - source[(j - _size_left) * step];
    }
  for(j = length - _size_left - 1; j < length; j++)
    {
      dest[j * step] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(length-1) * step];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left) * step];
      total_sum += source[(length-1) * step] - source[(j - _size_left) * step];
    }
}

#include <stdio.h>

//------------------------------------------------------------------------------
void fast_smooth::fast_smoothB(float *source, float *dest, int length)
{
  //blur should be odd
  double cos_sum = 0;
  double sin_sum = 0;
  double total_sum = 0;
  int j;
  if(length < _size)
    {
      float avg = std::accumulate(source, source+length, 0.0) / float(length);
      std::fill(dest, dest+length, avg);
      return;
    }
  for(j = 0; j<_size_left; j++)
    {
      cos_sum += source[0];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[0];
    }
  //FIXME: Doesn't deal with a blur bigger than the length
  for(j = 0; j<_size_right; j++)
    {
      cos_sum += source[j];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      total_sum += source[j];
    }
  for(j = 0; j<_size_left; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right)];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[0];
      total_sum += source[(j + _size_right)] - source[0];
    }
  for(j = _size_left; j < length - _size_left-1; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(j + _size_right)];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left)];
      total_sum += source[(j + _size_right)] - source[(j - _size_left)];
    }
  for(j = length - _size_left-1; j < length; j++)
    {
      dest[j] = (total_sum - cos_sum) / _sum;
      cos_sum += source[(length-1)];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= source[(j - _size_left)];
      total_sum += source[(length-1)] - source[(j - _size_left)];
    }
}

//EOF
