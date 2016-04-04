/***************************************************************************
                          filter.cpp  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <iostream>
#include "filter.h"
#include <algorithm>

void Filter::make_FIR(double *b, uint n)
{
	_b.resize(n);
	copy(b, b+n, _b.begin());
	_a.clear();
  clear();
	gain = 1.0;
}

//crate IIR Filter with n coefficients
//the coeff's a direct form II transposed structure
//if a(0) is not 1.0 then all the coefficients are normalized be dividing by a(0)
void Filter::make_IIR(double *b, double *a, int n, int m)
{
  if(m == -1) m = n; //same number of terms in a and b
	_b.resize(n);
	copy(b, b+n, _b.begin());
	_a.resize(m-1);
	copy(a+1, a+m, _a.begin());
  if(a[0] != 1.0) { //normalize
    std::vector<double>::iterator it;
    for(it = _a.begin(); it != _a.end(); it++) *it /= a[0];
    for(it = _b.begin(); it != _b.end(); it++) *it /= a[0];
  }
  
  _x.resize(_a.size());
  _y.resize(_b.size());
  clear();
	gain = 1.0;
}

void Filter::clear()
{
  fill(_x.begin(), _x.end(), 0.0);
  fill(_y.begin(), _y.end(), 0.0);
}

void Filter::print()
{
	uint j;
	for(j=0; j<_b.size(); ++j)
		std::cout << "b[" << j+1 << "] = " << _b[j] << std::endl;
  for(j=0; j<_a.size(); ++j)
    std::cout << "a[1.0 " << j << "] = " << _a[j] << std::endl;
}

double Filter::apply(double input)
{
	double result = 0.0;
  uint j;

	_x.push_front(input);
	for(j=0; j<_b.size(); j++) result += _b[j] * _x[j];
	//print_deque(x);
	_x.pop_back();

	for(j=0; j<_a.size(); j++) result -= _a[j] * _y[j];
	//print_deque(y);
	_y.push_front(result);
	_y.pop_back();
	return result;
}

void Filter::getState(FilterState *filterState) const
{
  filterState->_x = _x;
  filterState->_y = _y;
}

void Filter::setState(const FilterState *filterState)
{
  _x = filterState->_x;
  _y = filterState->_y;
}
