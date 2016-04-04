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
#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <deque>

typedef unsigned int uint;

class FilterState;

class Filter
{
public:
	std::vector<double> _a; //coefficients for the top line (use with outputs) a[0] to a[n-1]
	std::vector<double> _b; //coefficients for the bottom line (use with inputs) b[1] to b[m]
	std::deque<double> _x; //last n inputs
	std::deque<double> _y; //last m outputs
	double gain;

	Filter() { }
	void make_FIR(double *b, uint n); //crate FIR filter with n coefficients
	void make_IIR(double *b, double *a, int n, int m=-1); //crate IIR filter with n coefficients
	void print();
	double apply(double input);
	void clear();
  void getState(FilterState *filterState) const;
  void setState(const FilterState *filterState);
};

class FilterState
{
public:
  std::deque<double> _x;
  std::deque<double> _y;
  //void operator=(const FilterState &fs) { _x = fs._x; _y = fs._y; }
};

#endif
