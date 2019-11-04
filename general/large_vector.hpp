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
  return *m_buf_ptrs;
}

//------------------------------------------------------------------------------
template<typename T>
const Array1d<std::vector<T> *> & large_vector<T>::buf_ptrs(void) const
{
  return *m_buf_ptrs;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::addBuffer(uint p_num)
{
  buf_ptrs().push_back(new std::vector<T>(p_num));
  buf_ptrs().back()->reserve(m_buffer_size);
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
void large_vector<T>::copyTo(T *p_dest, uint p_start, uint p_length)
{
  myassert(p_start + p_length <= size());
  T * l_ending = p_dest + p_length;
  uint l_cur_buf = p_start / bufferSize();
  uint l_offset = p_start % bufferSize();
  if(p_length <= bufferSize() - l_offset)
    {
      std::copy(getBuffer(l_cur_buf).begin() + l_offset, getBuffer(l_cur_buf).begin() + l_offset + p_length, p_dest);
    }
  else
    {
      std::copy(getBuffer(l_cur_buf).begin() + l_offset, getBuffer(l_cur_buf).end(), p_dest);
        p_dest += bufferSize() - l_offset;
      l_cur_buf++;
      while(uint(l_ending - p_dest) > bufferSize())
	{
	  std::copy(getBuffer(l_cur_buf).begin(), getBuffer(l_cur_buf).end(), p_dest);
        p_dest += bufferSize();
	  l_cur_buf++;
	}
      std::copy(getBuffer(l_cur_buf).begin(), getBuffer(l_cur_buf).begin() + (l_ending - p_dest), p_dest);
    }
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::copyFrom(const T *p_src, uint p_start, uint p_length)
{
  myassert(p_start + p_length <= size());
  const T * l_ending = p_src + p_length;
  uint l_cur_buf = p_start / bufferSize();
  uint l_offset = p_start % bufferSize();
  if(p_length <= bufferSize() - l_offset)
    {
      std::copy(p_src, p_src + p_length, getBuffer(l_cur_buf).begin() + l_offset);
    }
  else
    {
      std::copy(p_src, p_src + (bufferSize() - l_offset), getBuffer(l_cur_buf).begin() + l_offset);
        p_src += bufferSize() - l_offset;
      l_cur_buf++;
      while(uint(l_ending - p_src) > bufferSize())
	{
	  std::copy(p_src, p_src + bufferSize(), getBuffer(l_cur_buf).begin());
        p_src += bufferSize();
	  l_cur_buf++;
	}
      std::copy(p_src, l_ending, getBuffer(l_cur_buf).begin());
    }
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::clear(void)
{
  for(int l_j = 0; l_j < buf_ptrs().size(); l_j++)
    {
      delete buf_ptrs()[l_j];
    }
  buf_ptrs().clear();
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
typename large_vector<T>::iterator large_vector<T>::iterator_at(uint p_pos)
{
  return iterator(this, p_pos);
}
  
//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::bufferSize(void) const
{
  return m_buffer_size;
}

//------------------------------------------------------------------------------
template<typename T>
int large_vector<T>::numBuffers(void)
{
  return buf_ptrs().size();
}

//------------------------------------------------------------------------------
template<typename T>
std::vector<T> & large_vector<T>::getBuffer(uint p_buffer_num)
{
  return *buf_ptrs()[p_buffer_num];
}

//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::at(uint p_pos)
{
  myassert(empty() || p_pos < size());
  return (*buf_ptrs()[p_pos / m_buffer_size])[p_pos % m_buffer_size];
}

//------------------------------------------------------------------------------
template<typename T>
const T & large_vector<T>::at(uint p_pos) const
{
  myassert(empty() || p_pos < size());
  return (*buf_ptrs()[p_pos / m_buffer_size])[p_pos % m_buffer_size];
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
const T & large_vector<T>::front(void) const
{
  return at(0);
}

//------------------------------------------------------------------------------
template<typename T>
const T & large_vector<T>::back(void) const
{
  return at(size() - 1);
}

//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::size(void) const
{
  return (buf_ptrs().size() - 1) * m_buffer_size + buf_ptrs().back()->size();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::empty(void) const
{
  return (buf_ptrs().size() == 1) ? buf_ptrs().back()->empty() : false;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::push_back(const T &p_new_element)
{
  buf_ptrs().back()->push_back(p_new_element);
  if(buf_ptrs().back()->size() == m_buffer_size)
    {
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
  T l_temp = buf_ptrs().back()->back();
  buf_ptrs().back()->pop_back();
  return l_temp;
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::push_back(const T *p_src, uint p_length)
{
  uint l_size_before = size();
  increase_size(p_length);
  copyFrom(p_src, l_size_before, p_length);
}

//------------------------------------------------------------------------------
template<typename T>
void large_vector<T>::increase_size(uint p_num)
{
  if(p_num < bufferSize() - buf_ptrs().back()->size())
    {
      buf_ptrs().back()->resize(buf_ptrs().back()->size() + p_num);
    }
  else
    {
        p_num -= bufferSize() - buf_ptrs().back()->size();
      buf_ptrs().back()->resize(bufferSize());
      addBuffer();
      while(p_num >= bufferSize())
	{
        p_num -= bufferSize();
	  buf_ptrs().back()->resize(bufferSize());
	  addBuffer();
	}
      buf_ptrs().back()->resize(p_num);
    }
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::large_vector(uint p_size, uint p_buffer_size)
{
    m_buf_ptrs = new Array1d<std::vector<T> *>();
    m_buffer_size = p_buffer_size;
  while(p_size > m_buffer_size)
    {
      buf_ptrs().push_back(new std::vector<T>(m_buffer_size));
        p_size-=m_buffer_size;
    }
  addBuffer(p_size);
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::~large_vector(void)
{
  if(m_buf_ptrs.getNumRef() == 1)
    {
      for(int l_j = 0; l_j < buf_ptrs().size(); l_j++)
	{
	  delete buf_ptrs()[l_j];
	}
    }
}
  
//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::operator[](uint p_pos)
{
  return (*buf_ptrs()[p_pos / m_buffer_size])[p_pos % m_buffer_size];
}

//------------------------------------------------------------------------------
template<typename T>
const T & large_vector<T>::operator[](uint p_pos) const
{
  return (*buf_ptrs()[p_pos / m_buffer_size])[p_pos % m_buffer_size];
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::iterator::iterator(large_vector<T> *p_parent, int p_pos):
  m_parent(p_parent),
  m_pos(p_pos)
{
}

//------------------------------------------------------------------------------
template<typename T>
large_vector<T>::iterator::iterator(const iterator &p_iter):
  m_parent(p_iter.m_parent),
  m_pos(p_iter.m_pos)
{
}

//------------------------------------------------------------------------------
template<typename T>
uint large_vector<T>::iterator::pos(void) const
{
  return m_pos;
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator++()
{
  ++m_pos;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator++(int)
{
  m_pos++;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator--()
{
  --m_pos;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
typename large_vector<T>::iterator & large_vector<T>::iterator::operator--(int)
{
  m_pos--;
  return (*this);
}

//------------------------------------------------------------------------------
template<typename T>
T& large_vector<T>::iterator::operator*()
{
  return (*m_parent)[m_pos];
}

//------------------------------------------------------------------------------
template<typename T>
const T& large_vector<T>::iterator::operator*() const
{
  return (*m_parent)[m_pos];
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
bool large_vector<T>::iterator::operator!=(const iterator &p_iter) const
{
  return m_pos != p_iter.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator==(const iterator &p_iter) const
{
  return m_pos == p_iter.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator<(const iterator &p_iter) const
{
  return m_pos < p_iter.pos();
}

//------------------------------------------------------------------------------
template<typename T>
bool large_vector<T>::iterator::operator>(const iterator &p_iter) const
{
  return m_pos > p_iter.pos();
}
//EOF
