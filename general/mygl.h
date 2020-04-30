/***************************************************************************
                          mygl.h
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MYGL_H
#define MYGL_H

#include <QGLWidget>
#include <glu.h>
#include <gl.h>
#ifdef MACX
#include <GLKit/GLKMatrix4.h>
#endif // MACX

class MyGLfloat2d
{
  public:

    inline MyGLfloat2d( GLfloat p_x
                      , GLfloat p_y
                      );
    inline void set( GLfloat p_x
                   , GLfloat p_y
                   );

    inline GLfloat get_x() const;
    inline GLfloat get_y() const;

  private:
    GLfloat m_x;
    GLfloat m_y;
};

inline void mygl_line( GLfloat p_x1
                     , GLfloat p_y1
                     , GLfloat p_x2
                     , GLfloat p_y2
                     );
inline void mygl_rect( GLfloat p_x
                     , GLfloat p_y
                     , GLfloat p_width
                     , GLfloat p_height
                     );
inline void mygl_rect( const MyGLfloat2d & p_v1
                     , const MyGLfloat2d & p_v2
                     );
inline void mygl_resize2d( int p_width
                         , int p_height
                         );
inline GLushort my_wrap_left( GLushort p_x
                            , int p_offset
                            );
inline GLushort my_wrap_right( GLushort p_x
                             , int p_offset
                             );

#include "mygl.hpp"

#endif // MYGL_H
// EOF
