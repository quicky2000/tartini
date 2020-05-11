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
#include <string>

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

//------------------------------------------------------------------------------
std::string getFileExtension(const std::string & p_filename)
{
    size_t l_pos = p_filename.rfind('.');
    if(std::string::npos != l_pos && l_pos < p_filename.size() - 1)
    {
        return p_filename.substr(l_pos + 1);
    }
    return "";
}

//------------------------------------------------------------------------------
std::string getFilenamePart(const std::string & p_filename)
{
    size_t l_pos = p_filename.rfind('/');
    if(std::string::npos != l_pos && l_pos < p_filename.size() - 1)
    {
        return p_filename.substr(l_pos + 1);
    }
    l_pos = p_filename.rfind('\\');
    if(std::string::npos != l_pos && l_pos < p_filename.size() - 1)
    {
        return p_filename.substr(l_pos + 1);
    }
    return p_filename;
}

//EOF
