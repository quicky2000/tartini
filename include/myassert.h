/***************************************************************************
                          myassert.h  -  Cause a crash in debug mode
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

#ifndef MYASSERT_H
#define MYASSERT_H

#ifdef MYDEBUG // Cause a crash in debug mode so you can trace it with your debugger

#include <cstdlib>

#define myassert(expr) if(!(expr))                                                                              \
  {                                                                                                             \
      fprintf(stderr, "Assert Failed: %s in %s line %u.\nCausing a trace crash.\n", #expr, __FILE__, __LINE__); \
      fflush(stderr);                                                                                           \
      abort();                                                                                                  \
  } //cause a crash

#else // Do nothing when not in debug mode
#define myassert(expr) ;
#endif // MYDEBUG

#define myAlwaysAssert(expr) if(!(expr))                                                                        \
  {                                                                                                             \
      fprintf(stderr, "Assert Failed: %s in %s line %u.\nCausing a trace crash.\n", #expr, __FILE__, __LINE__); \
      fflush(stderr);                                                                                           \
      abort();                                                                                                  \
  } //cause a crash

#endif //MYASSERT_H
// EOF