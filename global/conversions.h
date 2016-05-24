/***************************************************************************
                          conversions.h  -  
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

/**
   x is between 0 and 1. 1 becomes 0dB and 0 becomes dBFloor (-ve)
*/
double linear2dB(const double & x);
double dB2Linear(const double & x);
double dB2Normalised(const double & x);
double normalised2dB(const double & x);
double dB2ViewVal(const double & x);
double same(const double & x);
double oneMinus(const double & x);

double dB2Normalised(const double & x, const double & theCeiling, const double & theFloor);
