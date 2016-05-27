/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
template<typename T>
Array1d<std::vector<T> *> & large_vector<T>::buf_ptrs(void)
{
  return *_buf_ptrs;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::addBuffer(uint num)
{
  buf_ptrs().push_back(new std::vector<T>(num));
  buf_ptrs().back()->reserve(_buffer_size);
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::removeBuffer(void)
{
  delete buf_ptrs().back();
  buf_ptrs().pop_back();
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::copyTo(T *dest, uint start, uint length)
{
  myassert(start+length <= size());
  T* ending = dest+length;
  uint curBuf = start / bufferSize();
  uint offset = start % bufferSize();
  if(length <= bufferSize() - offset)
    {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).begin()+offset+length, dest);
    }
  else
    {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).end(), dest);
      dest += bufferSize() - offset;
      curBuf++;
      while(uint(ending - dest) > bufferSize())
	{
	  std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).end(), dest);
	  dest += bufferSize();
	  curBuf++;
	}
      std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).begin()+(ending-dest), dest);
    }
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::copyFrom(const T *src, uint start, uint length)
{
  myassert(start+length <= size());
  const T* ending = src+length;
  uint curBuf = start / bufferSize();
  uint offset = start % bufferSize();
  if(length <= bufferSize() - offset)
    {
      std::copy(src, src+length, getBuffer(curBuf).begin()+offset);
    }
  else
    {
      std::copy(src, src+(bufferSize()-offset), getBuffer(curBuf).begin()+offset);
      src += bufferSize() - offset;
      curBuf++;
      while(uint(ending - src) > bufferSize())
	{
	  std::copy(src, src+bufferSize(), getBuffer(curBuf).begin());
	  src += bufferSize();
	  curBuf++;
	}
      std::copy(src, ending, getBuffer(curBuf).begin());
    }
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::clear(void)
{
  for(int j=0; j<buf_ptrs().size();j++)
    {
      delete buf_ptrs()[j];
    }
  buf_ptrs().clear();
  //buf_ptrs().push_back(new std::vector<T>(0));
  //buf_ptrs().back()->reserve(_buffer_size);
  addBuffer();
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator large_vector<T>::begin(void)
{
  return iterator(this, 0);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator large_vector<T>::end(void)
{
  return iterator(this, size());
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator large_vector<T>::iterator_at(uint pos)
{
  return iterator(this, pos);
}
  
//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::bufferSize(void) const
{
  return _buffer_size;
}

//------------------------------------------------------------------------------
template<typename T>
int large_vector<T>::numBuffers(void)
{
  return buf_ptrs().size();
}

//------------------------------------------------------------------------------
template<typename T>
std::vector<T> & large_vector<T>::getBuffer(uint bufferNum)
{
  return *buf_ptrs()[bufferNum];
}

//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::at(uint pos)
{
  myassert(empty() || pos < size());
  return (*buf_ptrs()[pos / _buffer_size])[pos % _buffer_size];
}

//------------------------------------------------------------------------------
template<typename T>
T & large_vector<T>::front(void)
{
  return at(0);
}

//------------------------------------------------------------------------------
template<typename T>
T & large_vector<T>::back(void)
{
  return at(size() - 1);
}

//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::size(void)
{
  return (buf_ptrs().size() - 1) * _buffer_size + buf_ptrs().back()->size();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::empty(void)
{
  return (buf_ptrs().size()==1) ? buf_ptrs().back()->empty() : false;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::push_back(const T &new_element)
{
  buf_ptrs().back()->push_back(new_element);
  if(buf_ptrs().back()->size() == _buffer_size)
    {
      //buf_ptrs().push_back(new std::vector<T>(0));
      //buf_ptrs().back()->reserve(_buffer_size);
      addBuffer();
    }
}

//------------------------------------------------------------------------------
template<typename T>
T large_vector<T>::pop_back(void)
{
  if(buf_ptrs().back()->empty())
    {
      if(numBuffers() == 1) return T();
      else removeBuffer();
    }
  T temp = buf_ptrs().back()->back();
  buf_ptrs().back()->pop_back();
  return temp;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::push_back(const T *src, uint length)
{
  uint sizeBefore = size();
  increase_size(length);
  copyFrom(src, sizeBefore, length);
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::increase_size(uint num)
{
  if(num < bufferSize() - buf_ptrs().back()->size())
    {
      buf_ptrs().back()->resize(buf_ptrs().back()->size() + num);
    }
  else
    {
      num -= bufferSize() - buf_ptrs().back()->size();
      buf_ptrs().back()->resize(bufferSize());
      addBuffer();
      while(num >= bufferSize())
	{
	  num -= bufferSize();
	  buf_ptrs().back()->resize(bufferSize());
	  addBuffer();
	}
      buf_ptrs().back()->resize(num);
    }
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::large_vector(uint size, uint buffer_size)
{
  _buf_ptrs = new Array1d<std::vector<T> *>();
  _buffer_size = buffer_size;
  while(size > _buffer_size)
    {
      buf_ptrs().push_back(new std::vector<T>(_buffer_size));
      size-=_buffer_size;
    }
  //buf_ptrs().push_back(new std::vector<T>(size));
  //buf_ptrs().back()->reserve(_buffer_size);
  addBuffer(size);
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::~large_vector(void)
{
  if(_buf_ptrs.getNumRef() == 1)
    {
      for(int j=0; j<buf_ptrs().size();j++)
	{
	  delete buf_ptrs()[j];
	}
    }
}
  
//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::operator[](uint pos)
{
  //myassert(empty() || pos < size());
  return (*buf_ptrs()[pos / _buffer_size])[pos % _buffer_size];
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::iterator::iterator(large_vector<T> *parent, int pos):
  _parent(parent),
  _pos(pos)
{
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::iterator::iterator(const iterator &it):
  _parent(it._parent),
  _pos(it._pos)
{
}

//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::iterator::pos(void) const
{
  return _pos;
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator++()
{
  ++_pos;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator++(int)
{
  _pos++;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator--()
{
  --_pos;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator--(int)
{
  _pos--;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::iterator::operator*()
{
  return (*_parent)[_pos];
}

//------------------------------------------------------------------------------
template<typename T>
const T& large_vector<T>::iterator::operator*() const
{
  return (*_parent)[_pos];
}

//------------------------------------------------------------------------------
template<typename T>
T* large_vector<T>::iterator::operator->()
{
  return &(operator*());
}

//------------------------------------------------------------------------------
template<typename T>
const T* large_vector<T>::iterator::operator->() const
{
  return &(operator*());
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator!=(const iterator &it) const
{
  return _pos != it.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator==(const iterator &it) const
{
  return _pos == it.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator<(const iterator &it) const
{
  return _pos < it.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator>(const iterator &it) const
{
  return _pos > it.pos();
}
//EOF
