/***************************************************************************
                          large_vector.h  -  description
                             -------------------
    begin                : Wed Jun 1 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef LARGE_VECTOR_H
#define LARGE_VECTOR_H

#include <vector>

/** large_vector works like vector.
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
*/
template<typename T>
class large_vector
{
private:
  uint _buffer_size;
  std::vector<std::vector<T> *> _buf_ptrs;
  
  void addBuffer(uint num=0) {
    _buf_ptrs.push_back(new std::vector<T>(num));
    _buf_ptrs.back()->reserve(_buffer_size);
  }

public:
  class iterator
  {
    large_vector<T> *_parent;
    uint _pos;
    
   public:
    iterator(large_vector<T> *parent, int pos) : _parent(parent), _pos(pos) { }
    iterator(const iterator &it) : _parent(it._parent), _pos(it._pos) { }
    uint pos() const { return _pos; }
    iterator& operator++() { ++_pos; return (*this); }
    iterator& operator++(int) { _pos++; return (*this); }
    iterator& operator--() { --_pos; return (*this); }
    iterator& operator--(int) { _pos--; return (*this); }
    T& operator*() { return (*_parent)[_pos]; }
    const T& operator*() const { return (*_parent)[_pos]; }
    T* operator->() { return &(operator*()); }
    const T* operator->() const { return &(operator*()); }
    bool operator!=(const iterator &it) const { return _pos != it.pos(); }
    bool operator==(const iterator &it) const { return _pos == it.pos(); }
    bool operator<(const iterator &it) const { return _pos < it.pos(); }
    bool operator>(const iterator &it) const { return _pos > it.pos(); }
    //iterator& operator=(const iterator &it) { return (*this); }
  };

  large_vector(uint size=0, uint buffer_size=2048) {
    _buffer_size = buffer_size;
    while(size > _buffer_size) {
      _buf_ptrs.push_back(new std::vector<T>(_buffer_size));
      size-=_buffer_size;
    }
    //_buf_ptrs.push_back(new std::vector<T>(size));
    //_buf_ptrs.back()->reserve(_buffer_size);
    addBuffer(size);
  }
  ~large_vector() {
    for(uint j=0; j<_buf_ptrs.size();j++) delete _buf_ptrs[j];
  }
  
  T& operator[](uint pos) {
    //myassert(empty() || pos < size());
    return (*_buf_ptrs[pos / _buffer_size])[pos % _buffer_size];
  }
  T& at(uint pos) {
    //myassert(empty() || pos < size());
    return (*_buf_ptrs[pos / _buffer_size])[pos % _buffer_size];
  }
  T &front() { return at(0); }
  T &back() { return at(size()-1); }
  uint size() { return (_buf_ptrs.size()-1)*_buffer_size + _buf_ptrs.back()->size(); }
  bool empty() { return (_buf_ptrs.size()==1) ? _buf_ptrs.back()->empty() : false; }
  void push_back(const T &new_element) {
    _buf_ptrs.back()->push_back(new_element);
    if(_buf_ptrs.back()->size() == _buffer_size) {
      //_buf_ptrs.push_back(new std::vector<T>(0));
      //_buf_ptrs.back()->reserve(_buffer_size);
      addBuffer();
    }
  }
  void push_back(const T *src, uint length) {
    uint sizeBefore = size();
    increase_size(length);
    copyFrom(src, sizeBefore, length);
  }
  void increase_size(uint num) {
    if(num < bufferSize() - _buf_ptrs.back()->size()) {
      _buf_ptrs.back()->resize(_buf_ptrs.back()->size() + num);
    } else {
      num -= bufferSize() - _buf_ptrs.back()->size();
      _buf_ptrs.back()->resize(bufferSize());
      addBuffer();
      while(num >= bufferSize()) {
        num -= bufferSize();
        _buf_ptrs.back()->resize(bufferSize());
        addBuffer();
      }
      _buf_ptrs.back()->resize(num);
    }
  }
  void clear() {
    for(uint j=0; j<_buf_ptrs.size();j++) delete _buf_ptrs[j];
    _buf_ptrs.clear();
    //_buf_ptrs.push_back(new std::vector<T>(0));
    //_buf_ptrs.back()->reserve(_buffer_size);
    addBuffer();
  }
  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
  iterator iterator_at(uint pos) { return iterator(this, pos); }
  
  uint bufferSize() const { return _buffer_size; }
  std::vector<T> &getBuffer(uint bufferNum) { return *_buf_ptrs[bufferNum]; }
  //efficient copy to a single block of memory (ie array or vector)
  void copyTo(T *dest, uint start, uint length) {
    myassert(start+length <= size());
    T* ending = dest+length;
    uint curBuf = start / bufferSize();
    uint offset = start % bufferSize();
    if(length <= bufferSize() - offset) {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).begin()+offset+length, dest);
    } else {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).end(), dest);
      dest += bufferSize() - offset;
      curBuf++;
      while(uint(ending - dest) > bufferSize()) {
        std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).end(), dest);
        dest += bufferSize();
        curBuf++;
      }
      std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).begin()+(ending-dest), dest);
    }
  }
  //efficient copy from a single block of memory (ie array or vector)
  void copyFrom(const T *src, uint start, uint length) {
    myassert(start+length <= size());
    const T* ending = src+length;
    uint curBuf = start / bufferSize();
    uint offset = start % bufferSize();
    if(length <= bufferSize() - offset) {
      std::copy(src, src+length, getBuffer(curBuf).begin()+offset);
    } else {
      std::copy(src, src+(bufferSize()-offset), getBuffer(curBuf).begin()+offset);
      src += bufferSize() - offset;
      curBuf++;
      while(uint(ending - src) > bufferSize()) {
        std::copy(src, src+bufferSize(), getBuffer(curBuf).begin());
        src += bufferSize();
        curBuf++;
      }
      std::copy(src, ending, getBuffer(curBuf).begin());
    }
  }
};

#endif
