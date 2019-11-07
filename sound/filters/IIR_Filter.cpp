/***************************************************************************
                          IIR_Filter.cpp  -  description
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
#include "IIR_Filter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

//------------------------------------------------------------------------------
IIR_Filter::IIR_Filter( double * p_b
                      , double * p_a
                      , int p_n
                      , int p_m
                      )
{
    init(p_b, p_a, p_n, p_m);
}

//------------------------------------------------------------------------------
void IIR_Filter::init( double * p_b
                     , double * p_a
                     , int p_n
                     , int p_m
                     )
{
    //same number of terms in p_a and p_b
    if(p_m == -1)
    {
        p_m = p_n;
    }

    m_b.resize_copy(p_b, p_n);
    m_a.resize_copy(p_a + 1, p_m - 1);

    if(p_a[0] != 1.0)
    {
        //normalize
        m_a /= p_a[0];
        m_b /= p_a[0];
    }
  
    m_x.resize(m_b.size() - 1);
    m_y.resize(m_a.size());
    reset();
}

//------------------------------------------------------------------------------
void IIR_Filter::reset()
{
    m_x.fill(0.0);
    m_y.fill(0.0);
}

//------------------------------------------------------------------------------
void IIR_Filter::print()
{
    int l_j;
    for(l_j = 0; l_j < m_b.size(); ++l_j)
    {
        std::cout << "b[" << l_j + 1 << "] = " << m_b[l_j] << std::endl;
    }
    for(l_j = 0; l_j < m_a.size(); ++l_j)
    {
        std::cout << "a[1.0 " << l_j << "] = " << m_a[l_j] << std::endl;
    }
}

//------------------------------------------------------------------------------
void IIR_Filter::filter(const float *p_input, float *p_output, int p_n)
{
    int l_size_x = m_x.size();
    int l_size_y = m_y.size();
    m_buf_x.resize_raw(l_size_x + p_n);
    m_buf_y.resize_raw(l_size_y + p_n);

    for(int l_j = 0; l_j < l_size_x; l_j++)
    {
        m_buf_x[l_j] = m_x[l_j];
    }
    for(int l_j = 0; l_j < l_size_y; l_j++)
    {
        m_buf_y[l_j] = m_y[l_j];
    }
    for(int l_j = 0; l_j < p_n; l_j++)
    {
        m_buf_x[l_size_x + l_j] = p_input[l_j];
    }

    double * l_y = m_buf_y.begin() + l_size_y;
    double * l_x = m_buf_x.begin() + l_size_x;
    float * l_outy = p_output;

    // do the special common case of 2, 2 more efficiently
    if(l_size_x == 2 && l_size_y == 2)
    {
        for(int l_j = 0; l_j < p_n; l_j++)
        {
            double l_b0 = m_b[0];
            double l_b1 = m_b[1];
            double l_b2 = m_b[2];
            double l_a0 = m_a[0];
            double l_a1 = m_a[1];
            *l_y = l_b0 * l_x[0] + l_b1 * l_x[-1] + l_b2 * l_x[-2] - l_a0 * l_y[-1] - l_a1 * l_y[-2];
            *l_outy++ = *l_y;
            l_y++;
            l_x++;
        }
        m_x[0] = m_buf_x[p_n];
        m_x[1] = m_buf_x[p_n + 1];
        m_y[0] = m_buf_y[p_n];
        m_y[1] = m_buf_y[p_n + 1];
    }
    else
    {
        // General case
        for(int l_j = 0; l_j < p_n; l_j++)
        {
            *l_y = 0.0;
            for(int l_k = 0; l_k < l_size_x + 1; l_k++)
            {
                *l_y += m_b[l_k] * l_x[-l_k];
            }
            for(int l_k = 0; l_k < l_size_y; l_k++)
            {
                *l_y -= m_a[l_k] * l_y[-l_k - 1];
            }
            *l_outy++ = *l_y;
            l_y++;
            l_x++;
        }
        for(int l_k = 0; l_k < l_size_x; l_k++)
        {
            m_x[l_k] = m_buf_x[p_n + l_k];

        }
        for(int l_k = 0; l_k < l_size_y; l_k++)
        {
            m_y[l_k] = m_buf_y[p_n + l_k];
        }
    }
}

//------------------------------------------------------------------------------
void IIR_Filter::getState(FilterState * p_filter_state) const
{
    p_filter_state->set_x(m_x);
    p_filter_state->set_y(m_y);
}

//------------------------------------------------------------------------------
void IIR_Filter::setState(const FilterState *p_filter_state)
{
    m_x = p_filter_state->get_x();
    m_y = p_filter_state->get_y();
}

//------------------------------------------------------------------------------
void FilterState::set_x(const Array1d<double> & p_x)
{
    m_x = p_x;
}

//------------------------------------------------------------------------------
void FilterState::set_y(const Array1d<double> & p_y)
{
    m_y = p_y;
}

//------------------------------------------------------------------------------
const Array1d<double> &
FilterState::get_x() const
{
    return m_x;
}

//------------------------------------------------------------------------------
const Array1d<double> &
FilterState::get_y() const
{
    return m_y;
}

// EOF
