/***************************************************************************
                          myio.h  -  low level io stuff
                             -------------------
    begin                : Unknown
    copyright            : Unknown
    email                : Unknown

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MY_IO_H
#define MY_IO_H

#include <stdio.h>

/**
   Reads a 16 bit word (intel byte ordering)
*/
int igetw(FILE * p_file);

/**
   Reads a 32 bit long (intel byte ordering)
*/
long igetl(FILE * p_file);

/**
   Writes a 16 bit int (intel byte ordering)
*/
int iputw( int p_word
         , FILE * p_file
         );

/**
   Writes a 32 bit long (intel byte ordering)
*/
long iputl( long p_long
          , FILE * p_file
          );

/**
   Reads a 16 bit int (motorola byte-ordering)
*/
int mgetw(FILE * p_file);

/**
   Reads a 32 bit long (motorola byte-ordering)
*/
long mgetl(FILE * p_file);

/**
   Writes a 16 bit int (motorola byte-ordering)
*/
int mputw( int p_word
         , FILE * p_file
         );

/**
   Writes a 32 bit long (motorola byte-ordering)
*/
long mputl( long p_long
          , FILE * p_file
          );

#endif // MY_IO_H

//EOF
