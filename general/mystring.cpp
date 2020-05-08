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

// return the index of the first occurance of char c in string s.
// or -1 if not found
//------------------------------------------------------------------------------
int find_pos( const char *p_s
            , const char p_c
            )
{
    int l_len = strlen(p_s);
    for(int l_j = 0; l_j < l_len; l_j++)
    {
        if(p_s[l_j] == p_c)
        {
            return l_j;
        }
    }
    return -1;
}

// returns a pointer to a copy of the string s
// allocating the memory required
//------------------------------------------------------------------------------
char * copy_string(const char * p_s)
{
    if(!p_s)
    {
        return NULL;
    }
    char * l_t = static_cast<char *>(malloc(strlen(p_s) + 1));
    if(nullptr == l_t)
    {
        throw std::bad_alloc();
    }
    strcpy(l_t, p_s);
    return l_t;
}

//------------------------------------------------------------------------------
int str_case_cmp( const char * p_s1
                , const char * p_s2
                )
{
#ifdef WINDOWS
    return _stricmp(p_s1, p_s2);
#else // WINDOWS
    return strcasecmp(p_s1, p_s2);
#endif // WINDOWS
}

// converts string to lower case
// returns the string
//------------------------------------------------------------------------------
char * to_lower_case(char * p_s)
{
    for(char * l_a = p_s; * l_a != '\0'; l_a++)
    {
        *l_a = tolower(*l_a);
    }
    return p_s;
}

// modifies the string, removing any leading or trailing spaces
// returns a pointer to the string
//------------------------------------------------------------------------------
char * strip_space(char * p_s)
{
  if(p_s[0] == ' ')
  {
      int l_pos = 0;
      while(p_s[l_pos] == ' ')
      {
          l_pos++;
      }
      memmove(p_s, p_s + l_pos, strlen(p_s) + 1 - l_pos);
  }
  int l_len = strlen(p_s);
  for(int l_j = l_len - 1; l_j >= 0; l_j--)
  {
      if(p_s[l_j] == ' ')
      {
          p_s[l_j] = '\0';
      }
      else
      {
          break;
      }
  }
  return p_s;
}

// returns a pointer to the file extention part or NULL
// if no file extention found
//------------------------------------------------------------------------------
const char * getFileExtension(const char * p_filename)
{
    const char * l_ext;
    if(p_filename && (l_ext = strrchr(p_filename, '.')) != NULL)
    {
        return l_ext + 1;
    }
    else
    {
        return NULL;
    }
}

//returns a pointer to the filename part of a full path name
//or NULL if no filename is found
//------------------------------------------------------------------------------
const char * getFilenamePart(const char * p_filename)
{
    const char *l_ext;
    if(!p_filename)
    {
        return NULL;
    }
    if((l_ext = strrchr(p_filename, '/')) != NULL)
    {
        return l_ext + 1;
    }
    if((l_ext = strrchr(p_filename, '\\')) != NULL)
    {
        return l_ext + 1;
    }
    return p_filename;
}

// a static string used by getFolderPart and withTrailingSlash
char g_mystring_folder[2048];

// retuns a string which contains only the folder name part of the string.
// if no folder name is found it return "./"
// NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
//------------------------------------------------------------------------------
const char * getFolderPart(const char * p_filename)
{
    if(!p_filename)
    {
        strcpy(g_mystring_folder, "./");
        return g_mystring_folder;
    }
    int l_len = strlen(p_filename);
    for(; l_len > 0; l_len--)
    {
        if(p_filename[l_len - 1] == '/' || p_filename[l_len - 1] == '\\')
        {
            break;
        }
    }
    if(l_len == 0)
    {
        strcpy(g_mystring_folder, "./");
    }
    else
    {
        if(l_len > 2047)
        {
            l_len = 2047;
        }
        memcpy(g_mystring_folder, p_filename, sizeof(char) * l_len);
        g_mystring_folder[l_len] = '\0';
    }
    return g_mystring_folder;
}

// If filename ends in a slash it is returned
// otherwise a pointer to static string is returned which contains a trailing slash
// NOTE: The return value is static so the next call to getFolderPart or withTrailingSlash will overwrite it
//------------------------------------------------------------------------------
const char * withTrailingSlash(const char * p_filename)
{
    if(!p_filename)
    {
        strcpy(g_mystring_folder, "/");
        return g_mystring_folder;
    }
    int l_len = strlen(p_filename);
    if(p_filename[l_len - 1] == '/')
    {
        return p_filename;
    }
    else
    {
        if(l_len > 2046)
        {
            l_len = 2046;
        }
        memcpy(g_mystring_folder, p_filename, sizeof(char) * l_len);
        g_mystring_folder[l_len] = '/';
        g_mystring_folder[l_len + 1] = '\0';
        return g_mystring_folder;
    }
}

//EOF
