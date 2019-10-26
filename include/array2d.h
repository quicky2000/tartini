/***************************************************************************
                          array2d.h  -  description
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

/*
I prefer this than the vector<vector> > approach
I believe this is faster
And array2d give you raw pointers to use rather than all that iterator crap
Note: You can still use alot of the c++ algorithm template stuff on array2d
Warning! Array2d uses malloc so no Constructors or Destructors are called
         on the elements in the array.

Example:
Array2d<GLfloat> myArray(10, 10);
myArray(5, 6) = 4;
for(GLfloat *j = myArray.begin(); j < myArray.end(); j++) *j = 0;
vector<int>::iterator it;
int *ptr = &*it;
*/

#ifndef ARRAY2D_H
#define ARRAY2D_H

//#include <algorithm>
#include <iostream>
//#define NDEBUG //removes the asserts
#include "myassert.h"

#ifndef MIN
#define MIN(x,y)    ((x)<(y) ? (x) : (y))
#endif // MIN
#ifndef MAX
#define MAX(x,y)    ((x)>(y) ? (x) : (y))
#endif // MAX

/**
 * 2 dimensional array
 */
template<class T>
class Array2d
{
  public:
    Array2d();

    Array2d( int w_
           , int h_
           );

    Array2d( int w_
           , int h_
           , T val
           );

    /*virtual*/ ~Array2d();

    T & operator()( int x
                  , int y
                  );

    T const & operator()( int x
                        , int y
                        ) const;

    T * operator[](int y);

    int w();

    int h();

    int size();

    T * begin();

    T * begin(int y);

    T * end();

    T * end(int y);

    /*virtual*/ bool isEmpty();

    void resize_raw( int w_
                   , int h_
                   );

    void resize( int w_
               , int h_
               );

    void resize( int w_
               , int h_
               , T val
               );

    void set_w(int w_);

    void set_h(int h_);

    void shift_left(int n);

  private:

    T * data;
    int width;
    int height;

};

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d()
{
    width = 0;
    height = 0;
    data = NULL;
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d( int w_
                   , int h_
                   )
{
    w_ = MAX(w_, 0);
    h_ = MAX(h_, 0);
    width = w_;
    height = h_;
    data = (T *)malloc(size() * sizeof(T));
    myassert(data);
    //std::uninitialized_fill(begin(), end(), T());
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d( int w_
                   , int h_
                   , T val
                   )
{
    //initialise fill with val
    w_ = MAX(w_, 0);
    h_ = MAX(h_, 0);
    width = w_;
    height = h_;
    data = (T *)malloc(size() * sizeof(T));
    myassert(data);
    //for(T *p = data; p != end();)
    //*p++ = val;
    std::uninitialized_fill(begin(), end(), val);
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::~Array2d()
{
    if(data)
    {
        free(data);
    }
}

//-----------------------------------------------------------------------------
template<class T>
T & Array2d<T>::operator()( int x
                          , int y
                          )
{
    myassert(x >= 0 && x < width);
    myassert(y >= 0 && y < height);
    return( *(data + (width * y + x)));
}

//-----------------------------------------------------------------------------
template<class T>
T const & Array2d<T>::operator()( int x
                                , int y
                                ) const
{
    myassert(x >= 0 && x < width);
    myassert(y >= 0 && y < height);
    return( *(data + (width * y + x)));
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::operator[](int y)
{
    myassert(y >= 0 && y < height);
    return(data + width * y);
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::w()
{
    return width;
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::h()
{
    return height;
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::size()
{
    return width * height;
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::begin()
{
    return data;
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::begin(int y)
{
    // same as []
    myassert(y >= 0 && y < height);
    return(data + width * y);
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::end()
{
    return data + size();
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::end(int y)
{
    return begin(y) + width;
}

//-----------------------------------------------------------------------------
template<class T>
bool Array2d<T>::isEmpty()
{
    return (data == NULL);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize_raw( int w_
                           , int h_
                           )
{
    //messes up data, but fast
    w_ = std::max(w_, 0);
    h_ = std::max(h_, 0);
    width = w_;
    height = h_;
    data = (T *)realloc(data, size() * sizeof(T));
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize( int w_
                       , int h_
                       )
{
    //keeps data in order, but doesn't initialise new data
    w_ = std::max(w_, 0);
    h_ = std::max(h_, 0);
    if(w_ < width)
    {
        for(int j = 1; j < std::min(h_, height); j++)
        {
            memmove(data + w_ * j, data + width * j, w_ * sizeof(T));
        }
        resize_raw(w_, h_);
    }
    else if(w_ > width)
    {
        int old_height = height;
        int old_width = width;
        resize_raw(w_, h_);
        for(int j = std::min(h_, old_height) - 1; j > 0; j--)
        {
            memmove(data + w_ * j, data + old_width * j, old_width * sizeof(T));
        }
    }
    else
    {
        resize_raw(w_, h_);
    }
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize( int w_
                       , int h_
                       , T val
                       )
{
    //keeps data in order and initialises new stuff to val
    w_ = std::max(w_, 0);
    h_ = std::max(h_, 0);
    int old_height = height;
    int old_width = width;
    resize(w_, h_);
    if(w_ > old_width)
    {
        for(int j = 0; j < std::min(h_, old_height); j++)
        {
            T * end = data + j * w_ + w_;
            for(T * p = data + w_ * j + old_width; p != end;)
            {
                *p++ = val;
            }
        }
    }
    if(h_ > old_height)
    {
        for(int j = old_height; j < h_; j++)
        {
            T * end = data + w_ * j + w_;
            for(T * p = data + w_ * j; p!=end;)
            {
                *p++ = val;
            }
        }
    }
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::set_w(int w_)
{
    resize(w_, height);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::set_h(int h_)
{
    resize(width, h_);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::shift_left(int n)
{
    //shift all cols to the left by n. losing the first n cols and not initialising the last n
    if(n < 1 || n >= width)
    {
        return;
    }
    for(int k = 0; k < height; k++)
    {
        memmove(begin(k), begin(k) + n, (width - n) * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
template<class T>
std::ostream & operator<<( std::ostream & o
                         , Array2d<T> & a
                        )
{
    if(a.w() == 0 && a.h() == 0)
    {
        o << "[]" << std::endl;
    }
    for(int k = 0; k < a.h(); k++)
    {
        o << '[';
        for(int j = 0; j < a.w(); j++)
        {
            o << a(j, k);
            if(j != a.w()-1)
            {
                o << '\t';
            }
        }
        o << ']' << std::endl;
    }
    return o;
}

#endif // ARRAY2D_H
// EOF