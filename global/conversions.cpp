/***************************************************************************
                          conversions.cpp  -  
                             -------------------
    begin                : 16/01/2006
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <math.h>
#include "gdata.h"

//------------------------------------------------------------------------------
double linear2dB(const double & x)
{
  return (x > 0.0) ? bound((log10(x) * 20.0), gdata->dBFloor(), 0.0) : gdata->dBFloor();
}

//------------------------------------------------------------------------------
double dB2Linear(const double & x)
{
  return pow10(x / 20.0);
}

//------------------------------------------------------------------------------
double dB2Normalised(const double & x)
{
  return bound(1.0 - (x / gdata->dBFloor()), 0.0, 1.0);
}

//------------------------------------------------------------------------------
double normalised2dB(const double & x)
{
  return (1.0 - x) * gdata->dBFloor();
}
//------------------------------------------------------------------------------
double dB2ViewVal(const double & x)
{
  return pow10(1.0 + x / 40.0);
}

//------------------------------------------------------------------------------
double same(const double & x)
{
 return x;
}

//------------------------------------------------------------------------------
double oneMinus(const double & x)
{
 return 1.0 - x;
}

//------------------------------------------------------------------------------
double dB2Normalised(const double & x,const double & theCeiling, const double & theFloor)
{
  return bound(1.0 + ((x-theCeiling) / (theCeiling-theFloor)), 0.0, 1.0);
}
