/***************************************************************************
                          RingBuffer.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
// RingBuffer.h: interface for the RingBuffer class.
// A fast efficient queue (FIFO)
// Wraping around inside preset max buffer size
//
//////////////////////////////////////////////////////////////////////

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

//#include <assert.h>
#include <algorithm>
#include "myassert.h"
#include "useful.h"

template<class T>
class RingBuffer
{
  public:

	RingBuffer();

	RingBuffer(int p_buf_size);

	virtual ~RingBuffer();

	void resize_clear(int p_buf_size = 0);

	int size();

	int capacity();

	int available();

	void setAutoGrow(bool p_value);

	int nextGrowSize();

	T & operator[](int p_x);

	/**
	 * Empty all the element from the buffer
	 */
	void clear();

	/**
	 * ignore - read past num elements without returning them
	 * @param p_num
	 * @return
	 */
	int ignore(int p_num);

	/**
     * put item on the back of the queue
     * @return false in no room on the queue
     */
	bool put(const T & p_item);

	/**
	 * Put a number of items on the queue
	 * @return the number of items successfully put in
	 */
	int put( const T * p_items
	       , int p_num
	       );

	/**
	 * always put item, may lose the oldest item from the buffer
	 * @param p_item
	 */
	void force_put(const T & p_item);

	/**
	 * always put items, may lose some older items from the buffer
	 * @param p_items
	 * @param num
	 */
	void force_put( const T * p_items
	              , int num
	              );
	/**
	 * retrieves 1 item without removing it
	 * @param p_item
	 * @return
	 */
	bool peek(T * p_item);

	/**
	 * returns the number of items returned, max of num items
	 * peek doesn't remove the item from the list like get does
	 */
	int peek( T * p_items
	        , int p_num
	        );

	bool get(T * p_item);

	/**
	 * returns the number of items returned, max of num items
	 * @param p_items
	 * @param p_num
	 * @return
	 */
	int get( T * p_items
	       , int p_num
	       );

	/**
	 * keeps the current data intacted
	 * Note: if resizing smaller, items will be lost from the front
	 */
	void resize(int p_new_size);

  protected:
    T * m_bufffer;
    int m_buffer_size;
    int m_current_size;
    int m_start;
    bool m_auto_grow;

    int end();

};

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::RingBuffer()
{
    m_bufffer = NULL;
    m_buffer_size = 0;
    m_auto_grow = false;
    clear();
}

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::RingBuffer(int p_buf_size)
{
    m_bufffer = NULL;
    m_buffer_size = 0;
    m_auto_grow = false;
    resize_clear(p_buf_size);
}

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::~RingBuffer()
{
    if(m_bufffer)
    {
        delete m_bufffer;
    }
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::resize_clear(int p_buf_size)
{
    if(m_bufffer)
    {
        delete m_bufffer;
    }

    m_buffer_size = std::max(p_buf_size, 0);
    m_bufffer = new T[m_buffer_size];
    clear();
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::size()
{
    return m_current_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::capacity()
{
    return m_buffer_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::available()
{
    return m_buffer_size - m_current_size;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::setAutoGrow(bool p_value)
{
    m_auto_grow = p_value;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::nextGrowSize()
{
    return std::max(2, m_current_size * 2);
}

//-----------------------------------------------------------------------------
template<class T>
T & RingBuffer<T>::operator[](int p_x)
{
    myassert(p_x >= 0 && p_x < m_current_size);
    return m_bufffer[(m_start + p_x) % m_buffer_size];
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::clear()
{
    m_current_size = m_start = 0;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::ignore(int p_num)
{
    p_num = bound(p_num, 0, m_current_size);
    m_start = (m_start + p_num) % m_buffer_size;
    m_current_size -= p_num;
    return p_num;
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::put(const T &p_item)
{
    if(m_current_size == m_buffer_size)
    {
        if(m_auto_grow)
        {
            resize(nextGrowSize());
        }
        else
        {
            return false;
        }
    }
    m_bufffer[end()] = p_item;
    m_current_size++;
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::put( const T * p_items
                      , int p_num
       )
{
    if(p_num <= 0)
    {
        return 0;
    }
    if(p_num > available())
    {
        if(m_auto_grow)
        {
            resize(nextPowerOf2(m_current_size + p_num));
        }
        else
        {
            p_num = available();
        }
    }
    if(end() + p_num <= m_buffer_size)
    {
        //do in one go
        //memcpy((void *)(m_bufffer+end()), (void *)p_items, p_num * sizeof(T));
        std::copy(p_items, p_items + p_num, m_bufffer + end());
    }
    else
    {
        //split into two bits
        int l_chunk_1 = m_buffer_size - end();
        //int chunk2 = p_num - l_chunk_1;
        //memcpy((void *)(m_bufffer+end()), (void *)p_items, l_chunk_1 * sizeof(T));
        std::copy(p_items, p_items + l_chunk_1, m_bufffer + end());
        //memcpy((void *)m_bufffer, (void *)(p_items+l_chunk_1), chunk2 * sizeof(T));
        std::copy(p_items + l_chunk_1, p_items + p_num, m_bufffer);
    }

    m_current_size += p_num;
    return p_num;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::force_put(const T & p_item)
{
    if(m_auto_grow || m_current_size < m_buffer_size)
    {
        put(p_item);
    }
    else
    {
        m_bufffer[end()] = p_item;
        m_start = (m_start + 1) % m_buffer_size;
    }
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::force_put( const T * p_items
                             , int num
                             )
{
    if(m_auto_grow || num < available())
    {
        put(p_items, num);
    }
    if(num > m_buffer_size)
    {
        //just fill the whole buffer with the last m_buffer_size elements
        //memcpy((void *)m_bufffer, (void *)(p_items+num-m_buffer_size), m_buffer_size * sizeof(T));
        std::copy(p_items + num - m_buffer_size, p_items + num, m_bufffer);
        m_current_size = m_buffer_size;
        m_start = 0;
    }
    else
    {
        int l_overlap = num - available();
        if(l_overlap > 0)
        {
            ignore(l_overlap);
        }
        put(p_items, num);
    }
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::peek(T * p_item)
{
    if(m_current_size < 1)
    {
        return false;
    }
    *p_item = m_bufffer[m_start];
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::peek( T * p_items
                       , int p_num
                       )
{
    p_num = bound(p_num, 0, m_current_size);
    if(m_start + p_num < m_buffer_size)
    {
        //do in one go
        //memcpy((void *)p_items, (void *)(m_bufffer+m_start), p_num * sizeof(T));
        std::copy(m_bufffer + m_start, m_bufffer + m_start + p_num, p_items);
    }
    else
    {
        //split into two bits
        int l_chunk_1 = m_buffer_size - m_start;
        int l_chunk_2 = p_num - l_chunk_1;
        //memcpy((void *)p_items, (void *)(m_bufffer+m_start), l_chunk_1 * sizeof(T));
        std::copy(m_bufffer + m_start, m_bufffer + m_buffer_size, p_items);
        //memcpy((void *)(p_items + l_chunk_1), (void *)m_bufffer, l_chunk_2 * sizeof(T));
        std::copy(m_bufffer, m_bufffer + l_chunk_2, p_items + l_chunk_1);
    }
    return p_num;
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::get(T * p_item)
{
    if(m_current_size < 1)
    {
        return false;
    }
    *p_item = m_bufffer[m_start];
    m_start = (m_start + 1) % m_buffer_size;
    m_current_size--;
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::get( T * p_items
                      , int p_num
                      )
{
    p_num = peek(p_items, p_num);
    m_start = (m_start + p_num) % m_buffer_size;
    m_current_size -= p_num;
    return p_num;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::resize(int p_new_size)
{
    if(m_current_size == 0)
    {
        resize_clear(p_new_size);
        return;
    }
    if(p_new_size < m_buffer_size)
    {
        ignore(m_buffer_size - p_new_size);
    }
    T * l_new_buffer = new T[p_new_size];
    if(m_start + m_current_size > m_buffer_size)
    {
        //do in one go
        std::copy(m_bufffer + m_start, m_bufffer + m_start + m_current_size, l_new_buffer);
    }
    else
    {
        int l_chunk_1 = m_buffer_size - m_start;
        int l_chunk_2 = m_current_size - l_chunk_1;
        std::copy(m_bufffer + m_start, m_bufffer + m_buffer_size, l_new_buffer);
        std::copy(m_bufffer, m_bufffer + l_chunk_2, l_new_buffer + l_chunk_1);
    }
    m_start = 0;
    if(m_bufffer)
    {
        delete m_bufffer;
    }
    m_bufffer = l_new_buffer;
    m_buffer_size = p_new_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::end()
{
    return (m_start + m_current_size) % m_buffer_size;
}

#endif // RING_BUFFER_H
// EOF