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
MyGLfloat2d::MyGLfloat2d(GLfloat _x, GLfloat _y):
  x(_x),
  y(_y)
{
}

//------------------------------------------------------------------------------
void MyGLfloat2d::set(GLfloat _x, GLfloat _y)
{
  x=_x;
  y=_y;
}


//------------------------------------------------------------------------------
void mygl_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

//------------------------------------------------------------------------------
void mygl_rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h)
{
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + w, y);
  glVertex2f(x + w, y + h);
  glVertex2f(x, y + h);
  glEnd();
}

//------------------------------------------------------------------------------
void mygl_rect(const MyGLfloat2d & v1, const MyGLfloat2d & v2)
{
  glBegin(GL_QUADS);
  glVertex2f(v1.x, v1.y);
  glVertex2f(v2.x, v1.y);
  glVertex2f(v2.x, v2.y);
  glVertex2f(v1.x, v2.y);
  glEnd();
}

//------------------------------------------------------------------------------
void mygl_resize2d(int w, int h)
{
  glViewport(0, 0, (GLint)w, (GLint)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
}

//------------------------------------------------------------------------------
GLushort my_wrap_left(GLushort x, int offset)
{
  offset %= 16;
  return (x << offset) + (x >> (16 - offset));
}

//------------------------------------------------------------------------------
GLushort my_wrap_right(GLushort x, int offset)
{
  offset %= 16;
  return (x >> offset) + (x << (16 - offset));
}
//EOF
