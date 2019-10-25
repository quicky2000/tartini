/***************************************************************************
                          Filter.h  -  description
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
#ifndef FILTER_H
#define FILTER_H

typedef unsigned int uint;

class Filter
{
  public:
    virtual void filter( const float * p_input
                       , float * p_output
                       , int p_n
                       ) = 0;
    virtual void reset(void) = 0;
    virtual int delay(void) const { return 0; }
    virtual inline ~Filter(void) { }
};

#endif // FILTER_H
// EOF
