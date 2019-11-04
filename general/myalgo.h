/***************************************************************************
                          useful.h  -  Generic useful functions
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
#ifndef MYALGO_H
#define MYALGO_H

#include "array1d.h"

/**
   Decimates input into the output, using a fast pick every n'th value idea.
*/
void bresenham1d( const Array1d<float> & p_input
                , Array1d<float> & p_output
                );

/**
   Decimates input into the output, using a 'find the max of every n values' idea.
*/
void maxAbsDecimate1d( const Array1d<float> & p_input
                     , Array1d<float> & p_output
                     );

#endif // MYALGO_H
//EOF
