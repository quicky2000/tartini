/***************************************************************************
                          mystring.h  -  description
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
#ifndef MYSTRING_H
#define MYSTRING_H

/**
   works exactly like strcasecmp but also works on windows
*/
int str_case_cmp( const std::string & p_s1
                , const std::string & p_s2
                );

/**
   @return a pointer to the file extention part or NULL
   if no file extention found
*/
std::string getFileExtension(const std::string & p_filename);

/**
   @return a pointer to the filename part of a full path name
   or NULL if no filename is found
*/
std::string getFilenamePart(const std::string & p_filename);

#endif // MYSTRING_H
// EOF

