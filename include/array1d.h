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

//#include <algorithm>
#include <iostream>
//#define NDEBUG //removes the asserts
#include "myassert.h"
#include <stdlib.h>

//Warning!: Destructors are not called on the elements in the array when removed

// 1 dimentional array
template<class T> class Array1d
{
 private:
  T* data;
  int dataSize;
  
 public:
  Array1d() {
	  dataSize = 0;
	  data = NULL;
  }
  Array1d(int dataSize_) {
    dataSize = dataSize_;
    data = (T*)malloc(dataSize * sizeof(T));
    myassert(data != NULL);
  }
  Array1d(int dataSize_, T val) {
    dataSize = dataSize_;
    data = (T*)malloc(dataSize * sizeof(T));
    myassert(data != NULL);
  	for(T *p = data; p != end();)
      *p++ = val;
  }
  virtual ~Array1d() {
    if(data) free(data);
  }
  T& operator()(int x) { return at(x); }
  T& operator[](int x) { return at(x); }
  virtual T& at(int x) {
    myassert(data != NULL);
    myassert(x >= 0 && x < size());
  	return data[x];
  }
  virtual int size() { return dataSize; }
  virtual T *begin() { return data; }
  virtual T *end() { return data+size(); }
  virtual bool isEmpty() { return (data==NULL); }
  virtual void resize(int newSize) { //any new values will be uninitized
    data = (T*)realloc(data, newSize * sizeof(T));
    dataSize = newSize;
  }
  virtual void resize(int newSize, T val) { //and new values will be uninitized to val
    data = (T*)realloc(data, newSize * sizeof(T));
    if(newSize > dataSize) {
      for(T* p=data+dataSize; p<data+newSize;)
      *p++ = val;
    }
    dataSize = newSize;
  }
  virtual void clear() {
    if(data) { free(data); data = NULL; }
	  dataSize = 0;
  }
  virtual void shift_left(int n) { //shift all values to the left by n. losing the first n values and not initialising the last n
	  if(n < 1 || n >= size()) return;
    memmove(begin(), begin()+n, (size()-n)*sizeof(T));
  }
  virtual void shift_right(int n) { //shift all values to the right by n. losing the last n values and not initialising the first n
	  if(n < 1 || n >= size()) return;
    memmove(begin()+n, begin(), (size()-n)*sizeof(T));
  }
};

template<class T>
std::ostream& operator<<(std::ostream &o, Array1d<T> &a)
{
  o << '[';
  for(int j=0; j<a.size(); j++) {
    o << a(j);
    if(j != a.size()-1) o << ' ';
  }
  o << ']' << std::endl;
  return o;
}

#endif
