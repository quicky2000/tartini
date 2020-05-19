/***************************************************************************
                          array1d.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
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

#ifndef ARRAY1D_H
#define ARRAY1D_H

#include <iostream>
#include <memory>
//#define NDEBUG //removes the asserts
#include "myassert.h"
#include <stdlib.h>
#include "useful.h"
#include <new>

/**
   Warning!: Use only for arrays of basic types or pointers
   Constructors are not called on creation of elements
   Destructors are not called on the elements in the array when removed
*/


// 1 dimentional array
template<class T>
class Array1d
{

  public:

    /**
       Construct an empty Array1d
    */
    Array1d();

    /**
       Construct an Array1d of size length. The values are uninitialised
       @param p_length The size of the new Array1d
    */
    Array1d(int p_length);

    /**
       Construct an Array1d of size length. The values are all initialied to val
       @param p_length The size of the new Array1d
       @param p_val Values will initialied to this
    */
    Array1d( int p_length
           , const T & p_val
           );

    /**
       Construct an Array1d of size length
       Initalise the values from (copying) an exsiting array
       @param p_src The array to copy from
       @param p_length The amount of data to copy and the new size of this array.
    */
    Array1d( const T * p_src
           , int p_length
           );

    /**
       Copy constructor
       @param p_r The Array1d to (deep) copy
    */
    Array1d(Array1d<T> const & p_r);

    /**
       Assignment operator. Loses old values and copys from r
       @param p_r The Array1d to (deep) copy
    */
    Array1d<T> &operator =(Array1d<T> const & p_r);

    ~Array1d();

    T & operator()(int p_x);

    const T & operator()(int p_x) const;

    T & operator[](int p_x);

    const T & operator[](int p_x) const;

    T & at(int p_x);

    const T & at(int p_x) const;

    int size() const;

    T * begin() const;

    T * end() const;

    bool isEmpty() const;

    T & front() const;

    T & back() const;

    int capacity() const;

    int getIndex(T * p_element) const;

    /**
       Resizes this to newSize, making all data uninitized
       If newSize == size() then it dosn't do anything
    */
    void resize_raw(int p_new_size);

    /**
      Resize this, any new values will be uninitized
    */
    void resize(int p_new_size);

    /**
       Resize this, any new values will be initized to val
    */
    void resize( int p_new_size
               , const T & p_val
               );

    /**
       Resizes to newSize copying data from array
       @param p_src The array to copy from
       @param p_length The amount of data to copy and the new size of this array.
    */
    void resize_copy( const T * p_src
                    , int p_length
                    );

    /**
       Grow the array by one, putting val on the end
       @param p_val The value to add to the end of the array
    */
    void push_back(const T & p_val);

    /**
       Shrink the array by one
       Note: Use back() to get the value before pop_back() if needed
       Note: The array should not be empty. I.e no checking is done
    */
    void pop_back();

    /**
       Copies data from src to this, assuming this is large enough to hold it
       @param p_src The array to copy from
    */
    void copy_raw(const T * p_src);

    /**
       Empites the array back to size 0
    */
    void clear();

    /**
       Fill the whole array with val
    */
    void fill(const T & p_val);

    /**
       Multiply each element by val
    */
    void operator*=(const T & p_val);

    /**
       Divide each element by val
    */
    void operator/=(const T & p_val);

    /**
       Shift all values to the left by n.
       losing the first n values and not initialising the last n
    */
    void shift_left(int p_n);

    /**
       Shift all values to the right by n.
       losing the last n values and not initialising the first n
    */
    void shift_right(int p_n);

  private:

    int m_data_size;
    int m_allocated_size;
    T * m_data;
};

template<class T>
std::ostream& operator<<( std::ostream & p_ostream
                        , Array1d<T> & p_array
                        );

#include "array1d.hpp"

#endif // ARRAY1D_H
//EOF
