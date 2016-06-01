/***************************************************************************
                          myalgo.cpp  -  Generic useful functions
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "myalgo.h"

//------------------------------------------------------------------------------
void bresenham1d(const Array1d<float> & input, Array1d<float> & output)
{
  //Use Bresenham's algorithm in 1d to choose the points to draw
  int w = output.size();
  int intStep = int(input.size() / w);
  int remainderStep = input.size() - (intStep * w);
  int pos = 0;
  int remainder = 0;
  for(int j = 0; j < w; j++, pos += intStep, remainder += remainderStep)
    {
      if(remainder >= w)
	{
	  pos++;
	  remainder -= w;
	}
      myassert(pos < input.size());
      output[j] = input[pos];
    }
}

//------------------------------------------------------------------------------
void maxAbsDecimate1d(const Array1d<float> & input, Array1d<float> & output)
{
  if(input.size() > output.size())
    {
      //Use Bresenham's algorithm in 1d to choose the points to draw
      int w = output.size();
      if(w <= 0)
	{
	  return;
	}
      int intStep = int(input.size() / w);
      int remainderStep = input.size() - (intStep * w);
      int pos = 0;
      int remainder = 0;
      int prev = 0;
      int j=0;

      do
	{
	  pos += intStep;
	  remainder += remainderStep;
	  if(remainder >= w)
	    {
	      pos++;
	      remainder -= w;
	    }
	  myassert(pos <= input.size());
	  output[j] = *std::max_element(input.begin() + prev, input.begin() + pos, absoluteLess<float>());
	  prev = pos;
	}
      while(++j < w);
    }
  else if(input.size() == output.size())
    {
      std::copy(input.begin(), input.end(), output.begin());
    }
  else
    {
      bresenham1d(input, output);
    }
}
//EOF
