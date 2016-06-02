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

struct MyGLfloat2d
{
  GLfloat x;
  GLfloat y;
  inline MyGLfloat2d(GLfloat _x, GLfloat _y);
  inline void set(GLfloat _x, GLfloat _y);
};

inline void mygl_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
inline void mygl_rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h);
inline void mygl_rect(const MyGLfloat2d & v1, const MyGLfloat2d & v2);
inline void mygl_resize2d(int w, int h);
inline GLushort my_wrap_left(GLushort x, int offset);
inline GLushort my_wrap_right(GLushort x, int offset);

#include "mygl.hpp"

#endif // MYGL_H
// EOF
