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

    Array2d( int p_width
           , int p_height
           );

    Array2d( int p_width
           , int p_height
           , T p_value
           );

    /*virtual*/ ~Array2d();

    T & operator()( int p_x
                  , int p_y
                  );

    T const & operator()( int p_x
                        , int p_y
                        ) const;

    T * operator[](int p_y);

    int w() const;

    int h() const;

    int size() const;

    T * begin();

    T * begin(int p_y);

    T * end();

    T * end(int p_y);

    /*virtual*/ bool isEmpty() const;

    void resize_raw( int p_width
                   , int p_height
                   );

    void resize( int p_width
               , int p_height
               );

    void resize( int p_width
               , int p_height
               , T p_value
               );

    void set_w(int p_width);

    void set_h(int p_height);

    void shift_left(int p_n);

  private:

    T * m_data;
    int m_width;
    int m_height;

};

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d()
{
    m_width = 0;
    m_height = 0;
    m_data = NULL;
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d( int p_width
                   , int p_height
                   )
{
    p_width = MAX(p_width, 0);
    p_height = MAX(p_height, 0);
    m_width = p_width;
    m_height = p_height;
    m_data = (T *)malloc(size() * sizeof(T));
    myassert(m_data);
    //std::uninitialized_fill(begin(), end(), T());
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::Array2d( int p_width
                   , int p_height
                   , T p_value
                   )
{
    //initialise fill with p_value
    p_width = MAX(p_width, 0);
    p_height = MAX(p_height, 0);
    m_width = p_width;
    m_height = p_height;
    m_data = (T *)malloc(size() * sizeof(T));
    myassert(m_data);
    //for(T *p = data; p != end();)
    //*p++ = p_value;
    std::uninitialized_fill(begin(), end(), p_value);
}

//-----------------------------------------------------------------------------
template<class T>
Array2d<T>::~Array2d()
{
    if(m_data)
    {
        free(m_data);
    }
}

//-----------------------------------------------------------------------------
template<class T>
T & Array2d<T>::operator()( int p_x
                          , int p_y
                          )
{
    myassert(p_x >= 0 && p_x < m_width);
    myassert(p_y >= 0 && p_y < m_height);
    return( *(m_data + (m_width * p_y + p_x)));
}

//-----------------------------------------------------------------------------
template<class T>
T const & Array2d<T>::operator()( int p_x
                                , int p_y
                                ) const
{
    myassert(p_x >= 0 && p_x < m_width);
    myassert(p_y >= 0 && p_y < m_height);
    return( *(m_data + (m_width * p_y + p_x)));
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::operator[](int p_y)
{
    myassert(p_y >= 0 && p_y < m_height);
    return(m_data + m_width * p_y);
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::w() const
{
    return m_width;
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::h() const
{
    return m_height;
}

//-----------------------------------------------------------------------------
template<class T>
int Array2d<T>::size() const
{
    return m_width * m_height;
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::begin()
{
    return m_data;
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::begin(int p_y)
{
    // same as []
    myassert(p_y >= 0 && p_y < m_height);
    return(m_data + m_width * p_y);
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::end()
{
    return m_data + size();
}

//-----------------------------------------------------------------------------
template<class T>
T * Array2d<T>::end(int p_y)
{
    return begin(p_y) + m_width;
}

//-----------------------------------------------------------------------------
template<class T>
bool Array2d<T>::isEmpty() const
{
    return (m_data == NULL);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize_raw( int p_width
                           , int p_height
                           )
{
    //messes up data, but fast
    p_width = std::max(p_width, 0);
    p_height = std::max(p_height, 0);
    m_width = p_width;
    m_height = p_height;
    m_data = (T *)realloc(m_data, size() * sizeof(T));
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize( int p_width
                       , int p_height
                       )
{
    //keeps data in order, but doesn't initialise new data
    p_width = std::max(p_width, 0);
    p_height = std::max(p_height, 0);
    if(p_width < m_width)
    {
        for(int l_j = 1; l_j < std::min(p_height, m_height); l_j++)
        {
            memmove(m_data + p_width * l_j, m_data + m_width * l_j, p_width * sizeof(T));
        }
        resize_raw(p_width, p_height);
    }
    else if(p_width > m_width)
    {
        int l_old_height = m_height;
        int l_old_width = m_width;
        resize_raw(p_width, p_height);
        for(int l_j = std::min(p_height, l_old_height) - 1; l_j > 0; l_j--)
        {
            memmove(m_data + p_width * l_j, m_data + l_old_width * l_j, l_old_width * sizeof(T));
        }
    }
    else
    {
        resize_raw(p_width, p_height);
    }
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::resize( int p_width
                       , int p_height
                       , T p_value
                       )
{
    //keeps data in order and initialises new stuff to p_value
    p_width = std::max(p_width, 0);
    p_height = std::max(p_height, 0);
    int l_old_height = m_height;
    int l_old_width = m_width;
    resize(p_width, p_height);
    if(p_width > l_old_width)
    {
        for(int l_j = 0; l_j < std::min(p_height, l_old_height); l_j++)
        {
            T * l_end = m_data + l_j * p_width + p_width;
            for(T * l_p = m_data + p_width * l_j + l_old_width; l_p != l_end;)
            {
                *l_p++ = p_value;
            }
        }
    }
    if(p_height > l_old_height)
    {
        for(int l_j = l_old_height; l_j < p_height; l_j++)
        {
            T * l_end = m_data + p_width * l_j + p_width;
            for(T * l_p = m_data + p_width * l_j; l_p != l_end;)
            {
                *l_p++ = p_value;
            }
        }
    }
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::set_w(int p_width)
{
    resize(p_width, m_height);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::set_h(int p_height)
{
    resize(m_width, p_height);
}

//-----------------------------------------------------------------------------
template<class T>
void Array2d<T>::shift_left(int p_n)
{
    //shift all cols to the left by p_n. losing the first p_n cols and not initialising the last p_n
    if(p_n < 1 || p_n >= m_width)
    {
        return;
    }
    for(int l_k = 0; l_k < m_height; l_k++)
    {
        memmove(begin(l_k), begin(l_k) + p_n, (m_width - p_n) * sizeof(T));
    }
}

//-----------------------------------------------------------------------------
template<class T>
std::ostream & operator<<( std::ostream & p_ostream
                         , Array2d<T> & p_array
                         )
{
    if(p_array.w() == 0 && p_array.h() == 0)
    {
        p_ostream << "[]" << std::endl;
    }
    for(int l_k = 0; l_k < p_array.h(); l_k++)
    {
        p_ostream << '[';
        for(int l_j = 0; l_j < p_array.w(); l_j++)
        {
            p_ostream << p_array(l_j, l_k);
            if(l_j != p_array.w() - 1)
            {
                p_ostream << '\t';
            }
        }
        p_ostream << ']' << std::endl;
    }
    return p_ostream;
}

#endif // ARRAY2D_H
// EOF