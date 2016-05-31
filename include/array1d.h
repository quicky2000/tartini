/***************************************************************************
                          array1d.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ARRAY1D_H
#define ARRAY1D_H

#include <iostream>
#include <memory>
//#define NDEBUG //removes the asserts
#include "myassert.h"
#include <stdlib.h>
#include "useful.h"

/**
   Warning!: Use only for arrays of basic types or pointers
   Constructors are not called on creation of elements
   Destructors are not called on the elements in the array when removed
*/


// 1 dimentional array
template<class T> class Array1d
{
 public:

  /**
     Construct an empty Array1d
  */
  Array1d(void);

  /**
     Construct an Array1d of size length. The values are uninitialised
     @param length The size of the new Array1d
  */
  Array1d(int length);

  /**
     Construct an Array1d of size length. The values are all initialied to val
     @param length The size of the new Array1d
     @param val Values will initialied to this
  */
  Array1d(int length, const T &val);

  /**
     Construct an Array1d of size length
     Initalise the values from (copying) an exsiting array
     @param src The array to copy from
     @param length The amount of data to copy and the new size of this array.
  */
  Array1d(const T *src, int length);

  /**
     Copy constructor
     @param r The Array1d to (deep) copy
  */
  Array1d(Array1d<T> const &r);

  /**
     Assignment operator. Loses old values and copys from r
     @param r The Array1d to (deep) copy
  */
  Array1d<T> &operator = (Array1d<T> const &r);

  ~Array1d(void);

  T & operator()(int x);

  T & operator[](int x);

  const T & operator[](int x) const;

  T & at(int x);

  const T & at(int x) const;

  int size(void) const;

  T * begin(void) const;

  T * end(void) const;

  bool isEmpty(void) const;

  T & front(void) const;

  T & back(void) const;

  int capacity(void) const;

  int getIndex(T *element) const;

  /**
     Resizes this to newSize, making all data uninitized
     If newSize == size() then it dosn't do anything
  */
  void resize_raw(int newSize);

   /**
      Resize this, any new values will be uninitized
   */
  void resize(int newSize);

   /**
      Resize this, any new values will be initized to val
   */
  void resize(int newSize, const T &val);

  /**
     Resizes to newSize copying data from array
     @param src The array to copy from
     @param length The amount of data to copy and the new size of this array.
  */
  void resize_copy(const T *src, int length);

  /**
     Grow the array by one, putting val on the end
     @param val The value to add to the end of the array
  */
  void push_back(const T &val);

  /**
     Shrink the array by one
     Note: Use back() to get the value before pop_back() if needed
     Note: The array should not be empty. I.e no checking is done
  */
  void pop_back(void);

  /**
     Copies data from src to this, assuming this is large enough to hold it
     @param src The array to copy from
     @param length The amount of data to copy
  */
  void copy_raw(const T *src);

  /**
     Empites the array back to size 0
  */
  void clear(void);

  /**
     Fill the whole array with val
  */
  void fill(const T &val);

  /**
     Multiply each element by val
  */
  void operator*=(const T &val);

  /**
     Divide each element by val
  */
  void operator/=(const T &val);

  /**
     Shift all values to the left by n.
     losing the first n values and not initialising the last n
  */
  void shift_left(int n);

  /**
     Shift all values to the right by n.
     losing the last n values and not initialising the first n
  */
  void shift_right(int n);

 private:
  int dataSize;
  int allocatedSize;
  T* data;
};

template<class T>
std::ostream& operator<<(std::ostream &o, Array1d<T> &a);

#include "array1d.hpp"

#endif // ARRAY1D_H
//EOF
