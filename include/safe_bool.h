/***************************************************************************
                          safe_bool.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SAFE_BOOL_H
#define SAFE_BOOL_H

/**
 * Macros for safe-bool idiom.
 * This is to help stop wrong use of the operator bool ()
 */
#ifdef VCL_BORLAND
#define SPTR_SAFE_BOOL_TRUE true
#define SPTR_SAFE_BOOL_DEFINE typedef bool safe_bool
#else // VCL_BORLAND
# define SPTR_SAFE_BOOL_TRUE (&safe_bool_dummy::dummy)
# define SPTR_SAFE_BOOL_DEFINE                   \
   struct safe_bool_dummy { void dummy() {} };   \
   typedef void (safe_bool_dummy::* safe_bool)()
#endif // VCL_BORLAND

#endif // SAFE_BOOL_H
// EOF
