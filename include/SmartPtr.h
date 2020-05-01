#ifndef SMART_PTR_H
#define SMART_PTR_H

#include "safe_bool.h"

/** Smart Pointer class
 *  This can be used in place of normal pointers.
 *  It performs reference counting, and deletes objects automatically when done
 *  Note: One map per type is used to store the number of references
 *
 *  Authors: Philip McLeod, Koen Herms
 *  Created: 13 Feb 2006
 *
 *  Example Use:
 *
 *  SmartPtr<TestClass> p1 = new TestClass();
 *  SmartPtr<TestClass> p2 = p1;
 *  p1->x = 4; //or (*p1).x = 4;
 *  p1 = NULL; //Here the object is not deleted, because it still has a reference
 *  cout << p2->x; //output: 4
 *  p2 = NULL; //Here the object is deleted
 *
 *  Note: You don't normally need to set to NULL, as they are destroyed (if not needed) when they go out of scope
 */ 

#include <map>

template <class T>
class SmartPtr
{
    SPTR_SAFE_BOOL_DEFINE;

  public:
    typedef T                                  value_type;
    typedef std::map<const T*, unsigned int>   map_type;
    typedef T*                                 pointer_type;
    typedef unsigned int                       map_data_type;

    /**
     * Default constructor
     * Speed: Constant time
     */
    SmartPtr();

    /**
     * Construct from pointer
     * Speed: Logarithmic to the size of the map
     */
    SmartPtr(T * p_ptr);

    /**
     * Construct from SmartPtr
     * Speed: Constant time
     */
    SmartPtr(SmartPtr<T> const & p_r);

    /**
     * Destructor
     * Speed: Logarithmic if last reference, else Constant time
     */
    ~SmartPtr();

    /**
     * Assignment of a smart pointer (Shallow copy)
     * Note: Creates the new pointer before deleting the old pointer
     * Speed: Constant time, unless previous value was a last reference, then logarithmic
    */
    SmartPtr<T> & operator =(SmartPtr<T> const & p_r);

    /**
     * Assignment of a raw pointer (Shallow copy)
     * Speed: Logarithmic to the size of the map
     */
    SmartPtr<T> & operator =(T * p_p);
    
    /**
     * @return Return a (deep) copy of this object
     * Speed: Logarithmic
     */
    SmartPtr<T> copy();

    /**
     * boolean (cast) operator. e.g. if(smartPtr) ...
     */
    operator safe_bool () const;

    /**
     * boolean not operator. e.g. if(!smartPtr) ...
     */
    bool operator !() const;

    /**
     * Dereferencing the pointer
     */
    T & operator * () const;

    /**
     * Return the raw/dumb pointer.
     */
    T * operator -> () const;

    /**
     * Return the raw/dumb pointer.
     */
    T *ptr() const;

    //operator T *() const { return m_ptr; } //People should explicitly get the pointer using ptr()

    /**
     * Do they point to the same object. (shallow equality)
     */
    bool operator ==(T const * p_p) const;
 
    /**
     * Do the smart pointers not point to the same object. (shallow inequality)
     */
    bool operator !=(T const * p_p) const;
 
    /**
     * Do they point to the same object. (shallow equality)
     */
    bool operator ==(SmartPtr<T> const & p_r) const;

    /**
     * Do the smart pointers not point to the same object. (shallow inequality)
     */
    bool operator !=(SmartPtr<T> const & p_r) const;

    bool operator <(SmartPtr<T> const & p_r) const;

    bool operator >(SmartPtr<T> const & p_r) const;

    bool operator <=(SmartPtr<T> const & p_r) const;

    bool operator >=(SmartPtr<T> const & p_r) const;

    /**
     * Returns the number of references to this object
     * Speed: Constant time
     */
    unsigned int getNumRef();

    /**
     * This allows read access to the map, to see what memory is being used
     * Mainly for debugging purposes
     */
    static const map_type & getMap();

  protected:

    unsigned int * getRefCounter() const;

  private:

    /**
     * Pointer to the actual object, or NULL
     */
    T * m_ptr;

    /**
     * Pointer to the reference counter in the map
     */
    map_data_type * m_ref_counter;

    static map_type m_smart_ptrs;

};

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T>::SmartPtr()
: m_ptr(NULL)
, m_ref_counter(NULL)
{
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T>::SmartPtr(T * p_ptr)
: m_ptr(p_ptr)
{
    if(m_ptr)
    {
        m_ref_counter = &(++(m_smart_ptrs[m_ptr]));
    }
    else
    {
        m_ref_counter = NULL;
    }
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T>::SmartPtr(SmartPtr<T> const & p_r)
: m_ptr(p_r.ptr())
{

    if(m_ptr)
    {
        ++(*(m_ref_counter = p_r.getRefCounter())); //no need to search the map here
    }
    else
    {
        m_ref_counter = NULL;
    }
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T>::~SmartPtr()
{
    // The clang static analyzer gets confused by Smart Pointers.
    // It incorrectly reports that memory might be used after it is freed.
    // Because it doesn't track the reference counts, the static analyzer assumes that *any* time
    // ~SmartPtr() is called the object could be deleted.
    // Suppress these warnings by hiding the code in ~SmartPtr() from the static analyzer.
#ifndef __clang_analyzer__
    if(m_ptr)
    {
        if(--(*m_ref_counter) == 0)
        {
            m_smart_ptrs.erase(m_ptr); //we need to search the map here to erase the element
            delete m_ptr;
            m_ptr = NULL;
            m_ref_counter = NULL;
        }
    }
#endif // __clang_analyzer__
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T> & SmartPtr<T>::operator =(SmartPtr<T> const & p_r)
{
    if(m_ptr != p_r.ptr())
    {
        T * l_old_ptr = m_ptr;
        map_data_type * l_old_ref_counter = m_ref_counter;

        if(p_r.ptr())
        {
            m_ptr = p_r.ptr();
            ++(*(m_ref_counter = p_r.getRefCounter()));
        }
        else
        {
            m_ptr = NULL;
            m_ref_counter = NULL;
        }
        if(l_old_ptr)
        {
            if(--(*l_old_ref_counter) == 0)
            {
                m_smart_ptrs.erase(l_old_ptr);
                delete l_old_ptr;
            }
        }
    }
    return *this;
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T> & SmartPtr<T>::operator =(T * p_p)
{
    if(m_ptr != p_p)
    {
        T * l_old_ptr = m_ptr;
        map_data_type * l_old_ref_counter = m_ref_counter;

        if(p_p)
        {
            m_ref_counter = &(++(m_smart_ptrs[p_p])); //this is the magic!
            m_ptr = p_p;
        }
        else
        {
            m_ptr = NULL;
            m_ref_counter = NULL;
        }
        if(l_old_ptr)
        {
            if(--(*l_old_ref_counter) == 0)
            {
                m_smart_ptrs.erase(l_old_ptr);
                delete l_old_ptr;
            }
        }
    }
    return *this;
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T> SmartPtr<T>::copy()
{
    T * l_t = new T();
    *l_t = *m_ptr;
    return SmartPtr<T>(l_t);
}

//-----------------------------------------------------------------------------
template <class T>
SmartPtr<T>::operator safe_bool () const
{
    return (m_ptr != (T*)NULL) ? SPTR_SAFE_BOOL_TRUE : false;
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator !() const
{
    return (m_ptr != (T*)NULL) ? false : true;
}

//-----------------------------------------------------------------------------
template <class T>
T & SmartPtr<T>::operator * () const
{
    return *m_ptr;
}

//-----------------------------------------------------------------------------
template <class T>
T * SmartPtr<T>::operator -> () const
{
    return m_ptr;
}

//-----------------------------------------------------------------------------
template <class T>
T * SmartPtr<T>::ptr() const
{
    return m_ptr;
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator ==(T const * p_p) const
{
    return m_ptr == p_p;
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator !=(T const * p_p) const
{
    return m_ptr != p_p;
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator ==(SmartPtr<T> const & p_r) const
{
    return m_ptr == p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator !=(SmartPtr<T> const & p_r) const
{
    return m_ptr != p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator <(SmartPtr<T> const & p_r) const
{
    return m_ptr < p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator >(SmartPtr<T> const & p_r) const
{
    return m_ptr > p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator <=(SmartPtr<T> const & p_r) const
{
    return m_ptr <= p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
bool SmartPtr<T>::operator >=(SmartPtr<T> const & p_r) const
{
    return m_ptr >= p_r.ptr();
}

//-----------------------------------------------------------------------------
template <class T>
unsigned int SmartPtr<T>::getNumRef()
{
    return (m_ptr) ? *m_ref_counter : 0;
}

//-----------------------------------------------------------------------------
template <class T>
const typename SmartPtr<T>::map_type & SmartPtr<T>::getMap()
{
    return m_smart_ptrs;
}

//-----------------------------------------------------------------------------
template <class T>
unsigned int * SmartPtr<T>::getRefCounter() const
{
    return m_ref_counter;
}

/**
 * Is p and r pointing to the same object
 */
//: Comparison of pointer with smart-pointer (cannot be a member function)
template <class T>
inline bool operator ==( T const * p_p
                       , SmartPtr<T> const & p_r
                       )
{
    return p_r.ptr() == p_p;
}

/**
 * Is p and r pointing to the same object
 */
template <class T>
inline bool operator!=( T const * p_p
                      , SmartPtr<T> const& p_r
                      )
{
    return p_r.ptr() != p_p;
}

/**
 * This inforces instatiation of the static variable _smartPtrs
 */
template <class T> std::map<const T *, unsigned int> SmartPtr<T>::m_smart_ptrs = std::map<const T *, unsigned int>();

/**
 * Test whether a pointer has been turn into a smart pointer
 * Speed: Logarithmic
 */
template <class T>
inline bool isSmart(const T* p_ptr)
{
    return (p_ptr) ? SmartPtr<T>::getMap().count(p_ptr) : false;
}

#endif // SMART_PTR_H
// EOF
