/***************************************************************************
                          myqmutex.h  -  description
                             -------------------
    begin                : Mon Nov 29 2004
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
#ifndef MYQMUTEX_H
#define MYQMUTEX_H
 
#if (QT_VERSION > 230)
#include <qmutex.h>
#else // (QT_VERSION > 230)
#include <qthread.h>

//class Q_EXPORT QMutexLocker
class QMutexLocker
{
  public:
    QMutexLocker( QMutex * );
    ~QMutexLocker();

    QMutex *mutex() const;

  private:
    QMutex * m_mutex;

#if defined(Q_DISABLE_COPY)
    QMutexLocker( const QMutexLocker & );
    QMutexLocker &operator=( const QMutexLocker & );
#endif // defined(Q_DISABLE_COPY)
};

//-----------------------------------------------------------------------------
inline QMutexLocker::QMutexLocker( QMutex * p_mutex)
: m_mutex(p_mutex)
{
    if( m_mutex )
    {
        m_mutex->lock();
    }
}

//-----------------------------------------------------------------------------
inline QMutexLocker::~QMutexLocker()
{
    if(m_mutex)
    {
        m_mutex->unlock();
    }
}

//-----------------------------------------------------------------------------
inline QMutex * QMutexLocker::mutex() const
{
    return m_mutex;
}

#endif // (QT_VERSION > 230)
#endif // MYQMUTEX_H
// EOF
