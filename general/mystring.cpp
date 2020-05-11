/***************************************************************************
                          mystring.cpp  -  description
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
#include "mystring.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef WINDOWS
#include <string.h>
#else
#include <strings.h>
#endif
#include <new>

//------------------------------------------------------------------------------
int str_case_cmp( const std::string & p_s1
                , const std::string & p_s2
                )
{
#ifdef WINDOWS
    return _stricmp(p_s1.c_str(), p_s2.c_str());
#else // WINDOWS
    return strcasecmp(p_s1.c_str(), p_s2.c_str());
#endif // WINDOWS
}

// returns a pointer to the file extention part or NULL
// if no file extention found
//------------------------------------------------------------------------------
std::string getFileExtension(const std::string & p_filename)
{
    const char * l_ext;
    if((l_ext = strrchr(p_filename.c_str(), '.')) != nullptr)
    {
        return l_ext + 1;
    }
    else
    {
        return "";
    }
}

//returns a pointer to the filename part of a full path name
//or NULL if no filename is found
//------------------------------------------------------------------------------
std::string getFilenamePart(const std::string & p_filename)
{
    const char *l_ext;
    if((l_ext = strrchr(p_filename.c_str(), '/')) != nullptr)
    {
        return l_ext + 1;
    }
    if((l_ext = strrchr(p_filename.c_str(), '\\')) != nullptr)
    {
        return l_ext + 1;
    }
    return p_filename;
}

//EOF
