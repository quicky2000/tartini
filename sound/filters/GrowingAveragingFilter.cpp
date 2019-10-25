/***************************************************************************
                          GrowingAveragingFilter.cpp  -  description
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
#include "GrowingAveragingFilter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

//------------------------------------------------------------------------------
GrowingAverageFilter::GrowingAverageFilter(int p_size)
{
  init(p_size);
}

//------------------------------------------------------------------------------
void GrowingAverageFilter::init(int p_size)
{
    m_size = p_size;
    m_count = 0;
    m_total_sum = 0;
  m_x.resize(p_size);
  reset();
}

//------------------------------------------------------------------------------
void GrowingAverageFilter::filter(const float *p_input, float *p_output, int p_n)
{
  int l_j;
  if(p_n > m_size)
    {
      for(l_j = 0; l_j < m_size; l_j++)
	{
	  if(m_count < m_size)
	    {
	      m_count++;
	    }
        m_total_sum += p_input[l_j] - m_x[l_j];
        p_output[l_j] = m_total_sum / m_count;
	}
      for(l_j = m_size; l_j < p_n; l_j++)
	{
	  if(m_count < m_size)
	    {
	      m_count++;
	    }
        m_total_sum += p_input[l_j] - p_input[l_j - m_size];
        p_output[l_j] = m_total_sum / m_count;
	}
      std::copy(p_input + p_n - m_size, p_input + p_n, m_x.begin());
    }
  else
    {
      for(l_j = 0; l_j < p_n; l_j++)
	{
	  if(m_count < m_size)
	    {
	      m_count++;
	    }
        m_total_sum += p_input[l_j] - m_x[l_j];
        p_output[l_j] = m_total_sum / m_count;
	}
      m_x.shift_left(p_n);
      std::copy(p_input, p_input + p_n, m_x.end() - p_n);
    }
}

//------------------------------------------------------------------------------
void GrowingAverageFilter::reset(void)
{
    m_count = 0;
    m_total_sum = 0.0;
  m_x.fill(0.0f);
}

// EOF
