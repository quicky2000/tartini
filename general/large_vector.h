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
#include "SmartPtr.h"

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
  class iterator
  {
    large_vector<T> *_parent;
    uint _pos;
    
  public:
    iterator(large_vector<T> *parent, int pos);
    iterator(const iterator &it);
    uint pos(void) const;
    iterator& operator++();
    iterator& operator++(int);
    iterator& operator--();
    iterator& operator--(int);
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;
    bool operator!=(const iterator &it) const;
    bool operator==(const iterator &it) const;
    bool operator<(const iterator &it) const;
    bool operator>(const iterator &it) const;
    //iterator& operator=(const iterator &it) { return (*this); }
  };

  large_vector(uint size=0, uint buffer_size=2048);
  ~large_vector(void);
  
  T& operator[](uint pos);
  const T & operator[](uint pos) const;

  T & at(uint pos);
  const T & at(uint pos) const;
  T & front(void);
  T & back(void);
  uint size(void) const;
  bool empty(void) const;
  void push_back(const T &new_element);
  T pop_back(void);
  void push_back(const T *src, uint length);
  void increase_size(uint num);
  void clear(void);
  iterator begin(void);
  iterator end(void);
  iterator iterator_at(uint pos);
  
  uint bufferSize(void) const;
  int numBuffers(void);
  std::vector<T> &getBuffer(uint bufferNum);

  /**
     efficient copy to a single block of memory (ie array or vector)
  */
  void copyTo(T *dest, uint start, uint length);

  /**
     efficient copy from a single block of memory (ie array or vector)
  */
  void copyFrom(const T *src, uint start, uint length);

 private:
  uint _buffer_size;
  //std::vector<std::vector<T> *> buf_ptrs();
  SmartPtr<Array1d<std::vector<T> *> > _buf_ptrs;

  Array1d<std::vector<T> *> & buf_ptrs(void);
  const Array1d<std::vector<T> *> & buf_ptrs(void) const;

  void addBuffer(uint num=0);
  void removeBuffer(void);


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
