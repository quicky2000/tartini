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

	RingBuffer(int buf_size_);

	virtual ~RingBuffer();

	void resize_clear(int buf_size_ = 0);

	int size();

	int capacity();

	int available();

	void setAutoGrow(bool value);

	int nextGrowSize();

	T & operator[](int x);

	/**
	 * Empty all the element from the buffer
	 */
	void clear();

	/**
	 * ignore - read past num elements without returning them
	 * @param num
	 * @return
	 */
	int ignore(int num);

	/**
     * put item on the back of the queue
     * @return false in no room on the queue
     */
	bool put(const T &item);

	/**
	 * Put a number of items on the queue
	 * @return the number of items successfully put in
	 */
	int put( const T * items
	       , int num
	       );

	/**
	 * always put item, may lose the oldest item from the buffer
	 * @param item
	 */
	void force_put(const T & item);

	/**
	 * always put items, may lose some older items from the buffer
	 * @param items
	 * @param num
	 */
	void force_put( const T * items
	              , int num
	              );
	/**
	 * retrieves 1 item without removing it
	 * @param item
	 * @return
	 */
	bool peek(T * item);

	/**
	 * returns the number of items returned, max of num items
	 * peek doesn't remove the item from the list like get does
	 */
	int peek( T * items
	        , int num
	        );

	bool get(T * item);

	/**
	 * returns the number of items returned, max of num items
	 * @param items
	 * @param num
	 * @return
	 */
	int get( T *items
	       , int num
	       );

	/**
	 * keeps the current data intacted
	 * Note: if resizing smaller, items will be lost from the front
	 */
	void resize(int new_size);

  protected:
    T * buf;
    int buf_size;
    int cur_size;
    int start;
    bool autoGrow;

    int end();

};

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::RingBuffer()
{
    buf = NULL;
    buf_size = 0;
    autoGrow = false;
    clear();
}

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::RingBuffer(int buf_size_)
{
    buf = NULL;
    buf_size = 0;
    autoGrow = false;
    resize_clear(buf_size_);
}

//-----------------------------------------------------------------------------
template<class T>
RingBuffer<T>::~RingBuffer()
{
    if(buf)
    {
        delete buf;
    }
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::resize_clear(int buf_size_)
{
    if(buf)
    {
        delete buf;
    }

    buf_size = std::max(buf_size_, 0);
    buf = new T[buf_size];
    clear();
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::size()
{
    return cur_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::capacity()
{
    return buf_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::available()
{
    return buf_size - cur_size;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::setAutoGrow(bool value)
{
    autoGrow = value;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::nextGrowSize()
{
    return std::max(2, cur_size * 2);
}

//-----------------------------------------------------------------------------
template<class T>
T & RingBuffer<T>::operator[](int x)
{
    myassert(x >= 0 && x < cur_size);
    return buf[(start + x) % buf_size];
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::clear()
{
    cur_size = start = 0;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::ignore(int num)
{
    num = bound(num, 0, cur_size);
    start = (start + num) % buf_size;
    cur_size -= num;
    return num;
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::put(const T &item)
{
    if(cur_size == buf_size)
    {
        if(autoGrow)
        {
            resize(nextGrowSize());
        }
        else
        {
            return false;
        }
    }
    buf[end()] = item;
    cur_size++;
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::put( const T * items
                      , int num
       )
{
    if(num <= 0)
    {
        return 0;
    }
    if(num > available())
    {
        if(autoGrow)
        {
            resize(nextPowerOf2(cur_size + num));
        }
        else
        {
            num = available();
        }
    }
    if(end() + num <= buf_size)
    {
        //do in one go
        //memcpy((void *)(buf+end()), (void *)items, num * sizeof(T));
        std::copy(items, items + num, buf + end());
    }
    else
    {
        //split into two bits
        int chunk1 = buf_size - end();
        //int chunk2 = num - chunk1;
        //memcpy((void *)(buf+end()), (void *)items, chunk1 * sizeof(T));
        std::copy(items, items + chunk1, buf + end());
        //memcpy((void *)buf, (void *)(items+chunk1), chunk2 * sizeof(T));
        std::copy(items + chunk1, items + num, buf);
    }

    cur_size += num;
    return num;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::force_put(const T & item)
{
    if(autoGrow || cur_size < buf_size)
    {
        put(item);
    }
    else
    {
        buf[end()] = item;
        start = (start + 1) % buf_size;
    }
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::force_put( const T * items
                             , int num
                             )
{
    if(autoGrow || num < available())
    {
        put(items, num);
    }
    if(num > buf_size)
    {
        //just fill the whole buffer with the last buf_size elements
        //memcpy((void *)buf, (void *)(items+num-buf_size), buf_size * sizeof(T));
        std::copy(items + num - buf_size, items + num, buf);
        cur_size = buf_size;
        start = 0;
    }
    else
    {
        int overlap = num - available();
        if(overlap > 0)
        {
            ignore(overlap);
        }
        put(items, num);
    }
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::peek(T * item)
{
    if(cur_size < 1)
    {
        return false;
    }
    *item = buf[start];
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::peek( T * items
                       , int num
                       )
{
    num = bound(num, 0, cur_size);
    if(start + num < buf_size)
    {
        //do in one go
        //memcpy((void *)items, (void *)(buf+start), num * sizeof(T));
        std::copy(buf + start, buf + start + num, items);
    }
    else
    {
        //split into two bits
        int chunk1 = buf_size - start;
        int chunk2 = num - chunk1;
        //memcpy((void *)items, (void *)(buf+start), chunk1 * sizeof(T));
        std::copy(buf + start, buf + buf_size, items);
        //memcpy((void *)(items + chunk1), (void *)buf, chunk2 * sizeof(T));
        std::copy(buf, buf+chunk2, items+chunk1);
    }
    return num;
}

//-----------------------------------------------------------------------------
template<class T>
bool RingBuffer<T>::get(T * item)
{
    if(cur_size < 1)
    {
        return false;
    }
    *item = buf[start];
    start = (start + 1) % buf_size;
    cur_size--;
    return true;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::get( T * items
                      , int num
                      )
{
    num = peek(items, num);
    start = (start + num) % buf_size;
    cur_size -= num;
    return num;
}

//-----------------------------------------------------------------------------
template<class T>
void RingBuffer<T>::resize(int new_size)
{
    if(cur_size == 0)
    {
        resize_clear(new_size);
        return;
    }
    if(new_size < buf_size)
    {
        ignore(buf_size - new_size);
    }
    T * new_buf = new T[new_size];
    if(start + cur_size > buf_size)
    {
        //do in one go
        std::copy(buf + start, buf + start + cur_size, new_buf);
    }
    else
    {
        int chunk1 = buf_size - start;
        int chunk2 = cur_size - chunk1;
        std::copy(buf + start, buf + buf_size, new_buf);
        std::copy(buf, buf + chunk2, new_buf + chunk1);
    }
    start = 0;
    if(buf)
    {
        delete buf;
    }
    buf = new_buf;
    buf_size = new_size;
}

//-----------------------------------------------------------------------------
template<class T>
int RingBuffer<T>::end()
{
    return (start + cur_size) % buf_size;
}

#endif // RING_BUFFER_H
// EOF