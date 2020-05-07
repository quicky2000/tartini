/***************************************************************************
                          large_vector.h  -  description
                             -------------------
    begin                : Wed Jun 1 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/

#ifndef LARGE_VECTOR_H
#define LARGE_VECTOR_H

#include <vector>
#include "array1d.h"
#include <memory>
#include <iterator>

/**
   large_vector works like vector.
   Same:
   It has constant time push_back
   It has constant time access []
   Different:
   Pointers to elements always stay valid during a push_back
   Elements are not guaranteed to be consecutive in memory
  
   Storage:
   It is stored as a one layer tree.
   ie. there is one parent node with as many children buffers as needed.
   The size of the buffers can be specified with a second parameter to the constructor
   operator= does a shallow copy. i.e points to the same array
*/
template<typename T>
class large_vector
{
  public:

  class iterator: public std::iterator<std::bidirectional_iterator_tag, T, unsigned int>
    {
      public:

        iterator(large_vector<T> * p_parent
                , int p_pos
                );
        iterator(const iterator & p_iter) = default;
        iterator & operator=(const iterator &) = default;

        unsigned int pos() const;
        iterator & operator++();
        iterator & operator++(int);
        iterator & operator--();
        iterator & operator--(int);
        T & operator*();
        const T & operator*() const;
        T* operator->();
        const T * operator->() const;
        bool operator!=(const iterator & p_iter) const;
        bool operator==(const iterator & p_iter) const;
        bool operator<(const iterator & p_iter) const;
        bool operator>(const iterator & p_iter) const;
        //iterator& operator=(const iterator &it) { return (*this); }

      private:
        large_vector<T> * m_parent;
        unsigned int m_pos;

    };

    large_vector( unsigned int p_size = 0
                , unsigned int p_buffer_size = 2048
                );
    ~large_vector();
  
    T & operator[](unsigned int p_pos);
    const T & operator[](unsigned int p_pos) const;

    T & at(unsigned int p_pos);
    const T & at(unsigned int p_pos) const;
    T & front();
    const T & front() const;
    T & back();
    const T & back() const;
    unsigned int size() const;
    bool empty() const;
    void push_back(const T & p_new_element);
    T pop_back();
    void push_back(const T * p_src, unsigned int p_length);
    void increase_size(unsigned int p_num);
    void clear();
    iterator begin();
    iterator end();
    iterator iterator_at(unsigned int p_pos);
  
    unsigned int bufferSize() const;
    int numBuffers();
    std::vector<T> & getBuffer(unsigned int p_buffer_num);

    /**
       efficient copy to a single block of memory (ie array or vector)
    */
    void copyTo( T * p_dest
               , unsigned int p_start
               , unsigned int p_length
               );

    /**
       efficient copy from a single block of memory (ie array or vector)
    */
    void copyFrom( const T * p_src
                 , unsigned int p_start
                 , unsigned int p_length
                 );

  private:

    Array1d<std::vector<T> *> & buf_ptrs();
    const Array1d<std::vector<T> *> & buf_ptrs() const;

    void addBuffer(unsigned int p_num = 0);
    void removeBuffer();

    unsigned int m_buffer_size;
    //std::vector<std::vector<T> *> buf_ptrs();
    std::shared_ptr<Array1d<std::vector<T> *> > m_buf_ptrs;

};


/*
  void testLargeVectorFunc()
  {
  int j, k;
  int count = 0;
  int x[1024];
  large_vector<int> v;
  printf("begin test\n");
  for(j=0; j<6000; j++) {
  for(k=0; k<1024; k++) x[k] = count++;
  v.push_back(x, 1024);
  v.copyTo(x, j*1024, 1024);
  v.copyFrom(x, j*1024, 1024);
  }
  for(j=0; j<6000*1024; j++) if(v[j] != j) printf("%d, %d\n", v[j], j);

  printf("\n");
  large_vector<int> u;
  u.push_back(10);
  u.push_back(11);
  printf("%d\n", u.back());
  u.pop_back();
  printf("%d\n", u.back());
  u.pop_back();

  printf("done test\n");
  exit(0);
  }
*/

#include "large_vector.hpp"

#endif // LARGE_VECTOR_H
// EOF
