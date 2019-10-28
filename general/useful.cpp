/***************************************************************************
                          useful.cpp  -  Generic useful functions
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
//#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "useful.h"
#include <stdio.h>
#include <errno.h>
#include "myassert.h"

//------------------------------------------------------------------------------
//return a pointer to a 2d array with each element of size size
void **malloc2d(const int p_row, const int p_col, const int p_size)
{
  void **l_ptr = (void **)malloc(sizeof(void *) * p_row);
  for(int l_j=0; l_j < p_row; l_j++)
    {
        l_ptr[l_j] = (void *)malloc(p_size * p_col);
    }
  return l_ptr;
}

//------------------------------------------------------------------------------
//like malloc2d but changes an existing 2d array
void **realloc2d(void **p_ptr, const int p_row, const int p_col, const int p_old_row, const int p_old_col, const int p_size)
{
  if(p_ptr)
    {
      int l_j;
      //shrink
      if(p_row < p_old_row)
	{
	  for(l_j=p_row; l_j < p_old_row; l_j++)
	    {
	      free(p_ptr[l_j]);
	    }
        p_ptr = (void **)realloc(p_ptr, sizeof(void *) * p_row);
	  if(p_col != p_old_col)
	    {
	      for(l_j=0; l_j < p_row; l_j++)
		{
            p_ptr[l_j] = (void *)realloc(p_ptr[l_j], p_size * p_col);
		}
	    }
	}
      //grow
      else if(p_row > p_old_row)
	{
        p_ptr = (void **)realloc(p_ptr, sizeof(void *) * p_row);
	  if(p_col != p_old_col)
	    {
	      for(l_j=0; l_j < p_old_row; l_j++)
		{
            p_ptr[l_j] = (void *)realloc(p_ptr[l_j], p_size * p_col);
		}
	    }
	  for(l_j=p_old_row; l_j < p_row; l_j++)
	    {
            p_ptr[l_j] = (void *)malloc(p_size * p_col);
	    }
	}
      else
	{ //same
	  if(p_col != p_old_col)
	    {
	      for(l_j=0; l_j < p_row; l_j++)
		{
            p_ptr[l_j] = (void *)realloc(p_ptr[l_j], p_size * p_col);
		}
	    }
	}
      return p_ptr;
    }
  else
    {
      return malloc2d(p_row, p_col, p_size);
    }
}

//------------------------------------------------------------------------------
void free2d(void **p_ptr, const int p_row)
{
  for(int l_j=0; l_j < p_row; l_j++)
    {
      free(p_ptr[l_j]);
    }
  free(p_ptr);
}

//------------------------------------------------------------------------------
double powi(const double p_x, const int p_y)
{
    if(p_y > 1)
      {
	if(p_y & 0x01)
	  {
	    return p_x * powi(p_x * p_x, p_y >> 1);
	  }
	else
	  {
	    return powi(p_x * p_x, p_y >> 1);
	  }
      }
    else if(p_y == 1)
      {
	return p_x;
      }
    else if(p_y == 0)
      {
	return 1;
      }
    else 
      {
	return 1.0 / powi(p_x, -p_y);
      }
}

//------------------------------------------------------------------------------
double nearestPowerOf2(double p_x)
{
  return pow2(myround(log2(p_x)));
}

//------------------------------------------------------------------------------
//sets max_pos to the position with the max value in the array
//sets max_value to the max value in the array
//max_pos and/or max_value can be NULL
void max_array(int p_n, float *p_data, int *p_max_pos, float *p_max_value)
{
  int l_pos = 0;
  float l_value = p_data[0];
  for(int l_j=1; l_j < p_n; l_j++)
    {
      if(p_data[l_j] > l_value)
	{
        l_value=p_data[l_j];
        l_pos=l_j;
	}
    }
  if(p_max_pos)
    {
      *p_max_pos = l_pos;
    }
  if(p_max_value)
    {
      *p_max_value = l_value;
    }
}

//------------------------------------------------------------------------------
MinMax minMax(float *p_begin, float *p_end)
{
  if (p_begin == p_end)
    {
      return MinMax();
    }
  
  MinMax l_result(*p_begin, *p_begin);

  for (float *l_p = p_begin + 1; l_p < p_end; l_p++)
    {
        l_result.m_min = MIN(l_result.m_min, *l_p);
        l_result.m_max = MAX(l_result.m_max, *l_p);
    }
  return l_result;
}

//------------------------------------------------------------------------------
MinMax minMax(float *p_begin, float *p_end, float p_low_bound, float p_high_bound)
{
  if (p_begin == p_end)
    {
      return MinMax();
    }

  float l_def = (*p_begin > p_low_bound && *p_begin < p_high_bound) ? *p_begin : 0;
  MinMax l_result(l_def, l_def);

  for (float *l_p = p_begin + 1; l_p < p_end; l_p++)
    {
      if (*l_p >= p_low_bound && *l_p <= p_high_bound)
	{
        l_result.m_min = MIN(l_result.m_min, *l_p);
        l_result.m_max = MAX(l_result.m_max, *l_p);
	}
    }
  return l_result;
}

//------------------------------------------------------------------------------
float average(float *p_begin, float *p_end)
{
  if (p_begin == p_end)
    {
      return 0;
    }

  float l_total = *p_begin;

  int l_count = 1;
  for (float *l_p = p_begin + 1; l_p < p_end; l_p++, l_count++)
    {
        l_total += *l_p;
    }
  return (l_total / l_count);
}

//------------------------------------------------------------------------------
float average(float *p_begin, float *p_end, float p_low_bound, float p_high_bound)
{
  if (p_begin == p_end)
    {
      return *p_begin;
    }

  float l_total = (*p_begin > p_low_bound && *p_begin < p_high_bound) ? *p_begin : 0;

  int l_count = 1;
  for (float *l_p = p_begin + 1; l_p < p_end; l_p++, l_count++)
    {
      if (*l_p >= p_low_bound && *l_p <= p_high_bound)
	{
        l_total += *l_p;
	}
    }
  return (l_total / l_count);
}

//------------------------------------------------------------------------------
int calcIndex(const double & p_frame_time, const double & p_base_X, int p_size)
{
  int l_result = (int) myround((floor(p_frame_time / p_base_X) * p_base_X));
  if (l_result < 0)
    {
      return 0;
    }
  if (l_result > p_size)
    {
      return p_size;
    }
  return l_result;
}

//------------------------------------------------------------------------------
bool copyFile(const char *p_src, const char *p_dest)
{
  char l_buffer[4096];
  size_t l_bytes_read;
  
  FILE *l_input_file = fopen(p_src, "rb");
  if(!l_input_file)
    {
      fprintf(stderr, "copyFile: Error opening %s for reading\n", p_src);
      return false;
    }
  
  FILE *l_output_file = fopen(p_dest, "wb");
  if(!l_output_file)
    {
      fclose(l_input_file);
      fprintf(stderr, "copyFile: Error opening %s for writing\n", p_dest);
      return false;
  }
  
  while((l_bytes_read = fread(l_buffer, 1, 4096, l_input_file)) != 0)
    {
      if(fwrite(l_buffer, 1, l_bytes_read, l_output_file) < l_bytes_read)
	{
	  fprintf(stderr, "copyFile: Error writing to %s. Possibly out of disk\n", p_dest);
	  fclose(l_input_file);
	  fclose(l_output_file);
	  return false;
	}
    }

  fclose(l_input_file);
  fclose(l_output_file);
  return true; //success
}

//------------------------------------------------------------------------------
bool moveFile(const char *p_src, const char *p_dest)
{
  if(rename(p_src, p_dest) == 0)
    {
      return true;
    }
  if(errno == EXDEV)
    { //on different file system
      if(copyFile(p_src, p_dest))
	{
	  if(remove(p_src))
	    {
	      fprintf(stderr, "moveFile: Copy to %s Successful. Error removing old file %s\n", p_dest, p_src);
	      return false;
	    }
	  else
	    {
	      return true; //success
	    }
	}
    }
  else
    {
      fprintf(stderr, "moveFile: Error moving '%s' to '%s'\n", p_src, p_dest);
    }
  return false;
}

//------------------------------------------------------------------------------
int nextPowerOf2(int p_x)
{
  myassert(p_x > 1 << 31);
  int l_y = 1;
  while(l_y < p_x)
    {
        l_y <<= 1;
    }
  return l_y;
}

//EOF
