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
Array1d<T>::Array1d()
: m_data_size(0)
, m_allocated_size(0)
, m_data(NULL)
{
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(int p_length)
: m_data_size(p_length)
, m_allocated_size(nextPowerOf2(m_data_size))
, m_data(static_cast<T *>(malloc(m_allocated_size * sizeof(T))))
{
    myassert(m_data != NULL);
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d( int p_length
                   , const T & p_val
                   )
: m_data_size(p_length)
, m_allocated_size(nextPowerOf2(m_data_size))
, m_data(static_cast<T *>(malloc(m_allocated_size * sizeof(T))))
{
    myassert(m_data != NULL);
    fill(p_val);
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d( const T * p_src
                   , int p_length
                   )
: m_data_size(p_length)
, m_allocated_size(nextPowerOf2(m_data_size))
, m_data(static_cast<T *>(malloc(m_allocated_size * sizeof(T))))
{
    myassert(m_data != NULL);
    for(T * l_p = m_data; l_p != end();)
    {
        *l_p++ = *p_src++;
    }
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::Array1d(Array1d<T> const & p_r)
: m_data_size(p_r.size())
, m_allocated_size(nextPowerOf2(m_data_size))
, m_data(static_cast<T *>(malloc(m_allocated_size * sizeof(T))))
{
    myassert(m_data != NULL);
    copy_raw(p_r.begin());
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T> & Array1d<T>::operator =(Array1d<T> const & p_r)
{
    resize_copy(p_r.begin(), p_r.size());
    return *this;
}

//------------------------------------------------------------------------------
template<class T>
Array1d<T>::~Array1d()
{
    if(m_data)
    {
        free(m_data);
    }
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::operator()(int p_x)
{
    return at(p_x);
}

//------------------------------------------------------------------------------
template<class T>
const T & Array1d<T>::operator()(int p_x) const
{
    return at(p_x);
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::operator[](int p_x)
{
    return at(p_x);
}

//------------------------------------------------------------------------------
template<class T>
const T& Array1d<T>::operator[](int p_x) const
{
    return at(p_x);
}

//------------------------------------------------------------------------------
template<class T>
T& Array1d<T>::at(int p_x)
{
    myassert(m_data != NULL);
    myassert(p_x >= 0 && p_x < size());
    return m_data[p_x];
}

//------------------------------------------------------------------------------
template<class T>
const T& Array1d<T>::at(int p_x) const
{
    myassert(m_data != NULL);
    myassert(p_x >= 0 && p_x < size());
    return m_data[p_x];
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::size() const
{
    return m_data_size;
}

//------------------------------------------------------------------------------
template<class T>
T * Array1d<T>::begin() const
{
    return m_data;
}

//------------------------------------------------------------------------------
template<class T>
T * Array1d<T>::end() const
{
    return m_data + m_data_size;
}

//------------------------------------------------------------------------------
template<class T>
bool Array1d<T>::isEmpty() const
{
    return (m_data == NULL);
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::front() const
{
    return *m_data;
}

//------------------------------------------------------------------------------
template<class T>
T & Array1d<T>::back() const
{
    return m_data[m_data_size - 1];
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::capacity() const
{
    return m_allocated_size;
}

//------------------------------------------------------------------------------
template<class T>
int Array1d<T>::getIndex(T * p_element) const
{
    return p_element - m_data;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize_raw(int p_new_size)
{
    if(p_new_size == m_data_size)
    {
        return;
    }
    if(p_new_size > m_allocated_size)
    {
        if(m_data)
        {
            free(m_data);
        }
        m_allocated_size = nextPowerOf2(p_new_size);
        m_data = static_cast<T *>(malloc(m_allocated_size * sizeof(T))); //I think this is faster than realloc
    }
    m_data_size = p_new_size;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize(int p_new_size)
{
    if(p_new_size > m_allocated_size)
    {
        m_allocated_size = nextPowerOf2(p_new_size);
        T * l_new_data_ptr = static_cast<T *>(realloc(m_data, m_allocated_size * sizeof(T)));
        if(nullptr == l_new_data_ptr)
        {
            throw std::bad_alloc();
        }
        m_data = l_new_data_ptr;
    }
    m_data_size = p_new_size;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize( int p_new_size
                       , const T & p_val
                       )
{
    if(p_new_size > m_allocated_size)
    {
        m_allocated_size = nextPowerOf2(p_new_size);
        T * l_new_data_ptr = static_cast<T *>(realloc(m_data, m_allocated_size * sizeof(T)));
        if(nullptr == l_new_data_ptr)
        {
            throw std::bad_alloc();
        }
        m_data = l_new_data_ptr;
    }
    if(p_new_size > m_data_size)
    {
        T * l_end = m_data + p_new_size;
        for(T * l_p = m_data + m_data_size; l_p < l_end;)
        {
            *l_p++ = p_val;
        }
    }
    m_data_size = p_new_size;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::resize_copy( const T * p_src
                            , int p_length
                            )
{
    resize_raw(p_length);
    copy_raw(p_src);
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::push_back(const T & p_val)
{
    if(++m_data_size > m_allocated_size)
    {
        m_allocated_size = nextPowerOf2(m_data_size);
        T * l_new_data_ptr = static_cast<T *>(realloc(m_data, m_allocated_size * sizeof(T)));
        if(nullptr == l_new_data_ptr)
        {
            throw std::bad_alloc();
        }
        m_data = l_new_data_ptr;
    }
    back() = p_val;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::pop_back()
{
    m_data_size--;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::copy_raw(const T * p_src)
{
    T * l_end = end();
    for(T * l_p = begin(); l_p != l_end;)
    {
        *l_p++ = *p_src++;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::clear()
{
    if(m_data)
    {
        free(m_data);
        m_data = NULL;
    }
    m_data_size = 0;
    m_allocated_size = 0;
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::fill(const T & p_val)
{
    T * l_end = end();
    for(T * l_p = begin(); l_p != l_end;)
    {
        *l_p++ = p_val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::operator*=(const T & p_val)
{
    T * l_end = end();
    for(T * l_p = begin(); l_p != l_end;)
    {
        *l_p++ *= p_val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::operator/=(const T & p_val)
{
    T * l_end = end();
    for(T * l_p = begin(); l_p != l_end;)
    {
        *l_p++ /= p_val;
    }
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::shift_left(int p_n)
{
    if(p_n < 1 || p_n >= size())
    {
        return;
    }
    memmove(begin(), begin() + p_n, (size() - p_n) * sizeof(T));
}

//------------------------------------------------------------------------------
template<class T>
void Array1d<T>::shift_right(int p_n)
{
    if(p_n < 1 || p_n >= size())
    {
        return;
    }
    memmove(begin() + p_n, begin(), (size() - p_n) * sizeof(T));
}

//------------------------------------------------------------------------------
template<class T>
std::ostream& operator<<( std::ostream & p_ostream
                        , Array1d<T> & p_array
                        )
{
    p_ostream << '[';
    for(int l_j=0; l_j < p_array.size(); l_j++)
    {
        p_ostream << p_array(l_j);
        if(l_j != p_array.size() - 1)
        {
            p_ostream << ' ';
        }
    }
    p_ostream << ']' << std::endl;
    return p_ostream;
}
//EOF
