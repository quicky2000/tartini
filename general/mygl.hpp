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
MyGLfloat2d::MyGLfloat2d( GLfloat p_x
                        , GLfloat p_y
                        )
: m_x(p_x)
, m_y(p_y)
{
}

//------------------------------------------------------------------------------
void MyGLfloat2d::set( GLfloat p_x
                     , GLfloat p_y
                     )
{
    m_x = p_x;
    m_y = p_y;
}


//------------------------------------------------------------------------------
void mygl_line( GLfloat p_x1
              , GLfloat p_y1
              , GLfloat p_x2
              , GLfloat p_y2
              )
{
    glBegin(GL_LINES);
    glVertex2f(p_x1, p_y1);
    glVertex2f(p_x2, p_y2);
    glEnd();
}

//------------------------------------------------------------------------------
void mygl_rect( GLfloat p_x
              , GLfloat p_y
              , GLfloat p_width
              , GLfloat p_height
              )
{
    glBegin(GL_QUADS);
    glVertex2f(p_x, p_y);
    glVertex2f(p_x + p_width, p_y);
    glVertex2f(p_x + p_width, p_y + p_height);
    glVertex2f(p_x, p_y + p_height);
    glEnd();
}

//------------------------------------------------------------------------------
void mygl_rect( const MyGLfloat2d & p_v1
              , const MyGLfloat2d & p_v2
              )
{
    glBegin(GL_QUADS);
    glVertex2f(p_v1.m_x, p_v1.m_y);
    glVertex2f(p_v2.m_x, p_v1.m_y);
    glVertex2f(p_v2.m_x, p_v2.m_y);
    glVertex2f(p_v1.m_x, p_v2.m_y);
    glEnd();
}

//------------------------------------------------------------------------------
void mygl_resize2d( int p_width
                  , int p_height
                  )
{
    glViewport(0, 0, (GLint)p_width, (GLint)p_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, p_width, p_height, 0);
}

//------------------------------------------------------------------------------
GLushort my_wrap_left( GLushort p_x
                     , int p_offset
                     )
{
    p_offset %= 16;
    return (p_x << p_offset) + (p_x >> (16 - p_offset));
}

//------------------------------------------------------------------------------
GLushort my_wrap_right( GLushort p_x
                      , int p_offset
                      )
{
    p_offset %= 16;
    return (p_x >> p_offset) + (p_x << (16 - p_offset));
}
//EOF
