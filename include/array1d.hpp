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
template<class T>
Array1d<T>::Array1d(void):
  dataSize(0),
  allocatedSize(0),
  data(NULL)
{
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(int length):
  dataSize(length),
  allocatedSize(nextPowerOf2(dataSize)),
  data((T*)malloc(allocatedSize * sizeof(T)))
{
  myassert(data != NULL);
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(int length, const T &val):
  dataSize(length),
  allocatedSize(nextPowerOf2(dataSize)),
  data((T*)malloc(allocatedSize * sizeof(T)))
{
  myassert(data != NULL);
  fill(val);
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(const T *src, int length):
  dataSize(length),
  allocatedSize(nextPowerOf2(dataSize)),
  data((T*)malloc(allocatedSize * sizeof(T)))
{
  myassert(data != NULL);
  for(T *p = data; p != end();)
    {
      *p++ = *src++;
    }
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(Array1d<T> const &r):
  dataSize(r.size()),
  allocatedSize(nextPowerOf2(dataSize)),
  data((T*)malloc(allocatedSize * sizeof(T)))
{
  myassert(data != NULL);
  copy_raw(r.begin());
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T> & Array1d<T>::operator = (Array1d<T> const &r)
{
  resize_copy(r.begin(), r.size());
  return *this;
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::~Array1d(void)
{
  if(data)
    {
      free(data);
    }
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::operator()(int x)
{
  return at(x);
}

//------------------------------------------------------------------------------
template<class T>
const T & Array1d<T>::operator()(int x) const
{
  return at(x);
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::operator[](int x)
{
  return at(x);
}

//------------------------------------------------------------------------------
template<class T>
const T& Array1d<T>::operator[](int x) const
{
  return at(x);
}

//------------------------------------------------------------------------------
template<class T>
T& Array1d<T>::at(int x)
{
  myassert(data != NULL);
  myassert(x >= 0 && x < size());
  return data[x];
}

//------------------------------------------------------------------------------
template<class T>
const T& Array1d<T>::at(int x) const
{
  myassert(data != NULL);
  myassert(x >= 0 && x < size());
  return data[x];
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::size(void) const
{
  return dataSize;
}

//------------------------------------------------------------------------------
template<class T>
T * Array1d<T>::begin(void) const
{
  return data;
}

//------------------------------------------------------------------------------
template<class T>
T * Array1d<T>::end(void) const
{
  return data+dataSize;
}

//------------------------------------------------------------------------------
template<class T>
bool Array1d<T>::isEmpty(void) const
{
  return (data==NULL);
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::front(void) const
{
  return *data;
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::back(void) const
{
  return data[dataSize - 1];
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::capacity(void) const
{
  return allocatedSize;
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::getIndex(T *element) const
{
  return element - data;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize_raw(int newSize)
{
  if(newSize == dataSize)
    {
      return;
    }
  if(newSize > allocatedSize)
    {
      if(data)
	{
	  free(data);
	}
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)malloc(allocatedSize * sizeof(T)); //I think this is faster than realloc
    }
    dataSize = newSize;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize(int newSize)
{
  if(newSize > allocatedSize)
    {
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
  dataSize = newSize;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize(int newSize, const T &val)
{
  if(newSize > allocatedSize)
    {
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
  if(newSize > dataSize)
    {
      T *theEnd = data+newSize;
      for(T* p=data+dataSize; p<theEnd;)
	{
	  *p++ = val;
	}
    }
  dataSize = newSize;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize_copy(const T *src, int length)
{
  resize_raw(length);
  copy_raw(src);
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::push_back(const T &val)
{
  if(++dataSize > allocatedSize)
    {
      allocatedSize = nextPowerOf2(dataSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
  back() = val;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::pop_back(void)
{
  dataSize--;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::copy_raw(const T *src)
{
  T *theEnd = end();
  for(T *p = begin(); p != theEnd;)
    {
      *p++ = *src++;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::clear(void)
{
  if(data)
    {
      free(data);
      data = NULL; 
    }
  dataSize = 0;
  allocatedSize = 0;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::fill(const T &val)
{
  T *theEnd = end();
  for(T *p = begin(); p != theEnd;)
    {
      *p++ = val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::operator*=(const T &val)
{
  T *theEnd = end();
  for(T *p = begin(); p != theEnd;)
    {
      *p++ *= val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::operator/=(const T &val)
{
  T *theEnd = end();
  for(T *p = begin(); p != theEnd;)
    {
      *p++ /= val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::shift_left(int n)
{
  if(n < 1 || n >= size())
    {
      return;
    }
  memmove(begin(), begin()+n, (size()-n)*sizeof(T));
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::shift_right(int n)
{
  if(n < 1 || n >= size())
    {
      return;
    }
  memmove(begin()+n, begin(), (size()-n)*sizeof(T));
}

//------------------------------------------------------------------------------
template<class T>
std::ostream& operator<<(std::ostream &o, Array1d<T> &a)
{
  o << '[';
  for(int j=0; j<a.size(); j++)
    {
      o << a(j);
      if(j != a.size()-1)
	{
	  o << ' ';
	}
    }
  o << ']' << std::endl;
  return o;
}
//EOF
