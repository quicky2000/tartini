/***************************************************************************
                          myio.cpp  -  low level io stuff
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
#include <stdio.h>
#include "myio.h"

/**
   igetw:
   Reads a 16 bit word from a file, using intel byte ordering.
*/
int igetw(FILE *p_file)
{
    int l_byte_1;
    int l_byte_2;

    if((l_byte_1 = fgetc(p_file)) != EOF)
    {
        if((l_byte_2 = fgetc(p_file)) != EOF)
        {
            return ((l_byte_2 << 8) | l_byte_1);
        }
    }
    return EOF;
}



/**
   igetl:
   Reads a 32 bit long from a file, using intel byte ordering.
*/
long igetl(FILE *p_file)
{
    int l_byte_1;
    int l_byte_2;
    int l_byte_3;
    int l_byte_4;

    if((l_byte_1 = fgetc(p_file)) != EOF)
    {
        if((l_byte_2 = fgetc(p_file)) != EOF)
        {
            if((l_byte_3 = fgetc(p_file)) != EOF)
            {
                if((l_byte_4 = fgetc(p_file)) != EOF)
                {
                    return ((static_cast<long>(l_byte_4) << 24) | (static_cast<long>(l_byte_3) << 16) |
                            (static_cast<long>(l_byte_2) << 8) | static_cast<long>(l_byte_1)
                           );
                }
            }
        }
    }
    return EOF;
}



/**
   iputw:
   Writes a 16 bit int to a file, using intel byte ordering.
*/
int iputw( int p_word
         , FILE * p_file
         )
{
    int l_byte_1;
    int l_byte_2;

    l_byte_1 = (p_word & 0xFF00) >> 8;
    l_byte_2 = p_word & 0x00FF;

    if (fputc(l_byte_2, p_file) == l_byte_2)
    {
        if (fputc(l_byte_1, p_file) == l_byte_1)
        {
            return p_word;
        }
    }
    return EOF;
}



/**
   iputw:
   Writes a 32 bit long to a file, using intel byte ordering.
*/
long iputl( long p_long
          , FILE * p_file
          )
{
    int l_byte_1;
    int l_byte_2;
    int l_byte_3;
    int l_byte_4;

    l_byte_1 = static_cast<int>((p_long & 0xFF000000L) >> 24);
    l_byte_2 = static_cast<int>((p_long & 0x00FF0000L) >> 16);
    l_byte_3 = static_cast<int>((p_long & 0x0000FF00L) >> 8);
    l_byte_4 = static_cast<int>(p_long & 0x00FF);

    if(fputc(l_byte_4, p_file) == l_byte_4)
    {
        if(fputc(l_byte_3, p_file) == l_byte_3)
        {
            if(fputc(l_byte_2, p_file) == l_byte_2)
            {
                if(fputc(l_byte_1, p_file) == l_byte_1)
                {
                    return p_long;
                }
            }
        }
    }
    return EOF;
}



/**
   mgetw:
   Reads a 16 bit int from a file, using motorola byte-ordering.
*/
int mgetw(FILE * p_file)
{
    int l_byte_1;
    int l_byte_2;

    if((l_byte_1 = fgetc(p_file)) != EOF)
    {
        if((l_byte_2 = fgetc(p_file)) != EOF)
        {
            return ((l_byte_1 << 8) | l_byte_2);
        }
    }
    return EOF;
}



/**
   mgetl:
   Reads a 32 bit long from a file, using motorola byte-ordering.
*/
long mgetl(FILE * p_file)
{
    int l_byte_1;
    int l_byte_2;
    int l_byte_3;
    int l_byte_4;

    if((l_byte_1 = fgetc(p_file)) != EOF)
    {
        if((l_byte_2 = fgetc(p_file)) != EOF)
        {
            if((l_byte_3 = fgetc(p_file)) != EOF)
            {
                if((l_byte_4 = fgetc(p_file)) != EOF)
                {
                    return ((static_cast<long>(l_byte_1) << 24) | (static_cast<long>(l_byte_2) << 16) |
                            (static_cast<long>(l_byte_3) << 8) | static_cast<long>(l_byte_4)
                           );
                }
            }
        }
    }
    return EOF;
}



/**
   mputw:
   Writes a 16 bit int to a file, using motorola byte-ordering.
*/
int mputw( int p_word
         , FILE * p_file
         )
{
    int l_byte_1;
    int l_byte_2;

    l_byte_1 = (p_word & 0xFF00) >> 8;
    l_byte_2 = p_word & 0x00FF;

    if(fputc(l_byte_1, p_file) == l_byte_1)
    {
        if(fputc(l_byte_2, p_file) == l_byte_2)
        {
            return p_word;
        }
    }
    return EOF;
}



/**
   mputl:
   Writes a 32 bit long to a file, using motorola byte-ordering.
*/
long mputl( long p_long
          , FILE * p_file
          )
{
    int l_byte_1;
    int l_byte_2;
    int l_byte_3;
    int l_byte_4;

    l_byte_1 = static_cast<int>((p_long & 0xFF000000L) >> 24);
    l_byte_2 = static_cast<int>((p_long & 0x00FF0000L) >> 16);
    l_byte_3 = static_cast<int>((p_long & 0x0000FF00L) >> 8);
    l_byte_4 = static_cast<int>(p_long & 0x00FF);

    if(fputc(l_byte_1, p_file) == l_byte_1)
    {
        if(fputc(l_byte_2, p_file) == l_byte_2)
        {
            if(fputc(l_byte_3, p_file) == l_byte_3)
            {
                if(fputc(l_byte_4, p_file) == l_byte_4)
                {
                    return p_long;
                }
            }
        }
    }
    return EOF;
}

//EOF
