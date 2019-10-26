/***************************************************************************
                          conversions.cpp  -  
                             -------------------
    begin                : 16/01/2006
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <math.h>
#include "gdata.h"

//------------------------------------------------------------------------------
double linear2dB( const double & p_x
                , const GData & p_data
                )
{
  return (p_x > 0.0) ? bound((log10(p_x) * 20.0), p_data.dBFloor(), 0.0) : p_data.dBFloor();
}

//------------------------------------------------------------------------------
double dB2Linear(const double & p_x)
{
  return pow10(p_x / 20.0);
}

//------------------------------------------------------------------------------
double dB2Normalised( const double & p_x
                    , const GData & p_data
                    )
{
  return bound(1.0 - (p_x / p_data.dBFloor()), 0.0, 1.0);
}

//------------------------------------------------------------------------------
double normalised2dB( const double & p_x
                    , const GData & p_data
                    )
{
  return (1.0 - p_x) * p_data.dBFloor();
}
//------------------------------------------------------------------------------
double dB2ViewVal(const double & p_x)
{
  return pow10(1.0 + p_x / 40.0);
}

//------------------------------------------------------------------------------
double same( const double & p_x
           , const GData & p_data
           )
{
  p_data.do_nothing();
 return p_x;
}

//------------------------------------------------------------------------------
double oneMinus( const double & p_x
               , const GData & p_data
               )
{
  p_data.do_nothing();
 return 1.0 - p_x;
}

//------------------------------------------------------------------------------
double dB2Normalised( const double & p_x
                    , const double & p_ceiling
                    , const double & p_floor
                    )
{
  return bound(1.0 + ((p_x - p_ceiling) / (p_ceiling - p_floor)), 0.0, 1.0);
}
//EOF
