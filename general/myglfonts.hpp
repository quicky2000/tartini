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
MyGLChar::MyGLChar(int p_width, int p_height, GLubyte *p_data, int p_width_raw):
        m_width(p_width),
        m_height(p_height),
        m_width_raw(p_width_raw),
        m_data(p_data)
{
}

//------------------------------------------------------------------------------
const GLubyte * MyGLChar::data(void) const
{
  return m_data;
}

//------------------------------------------------------------------------------
int MyGLChar::w(void) const
{
  return m_width;
}

//------------------------------------------------------------------------------
int MyGLChar::h(void) const
{
  return m_height;
}

//------------------------------------------------------------------------------
int MyGLChar::w_raw(void) const
{
  return m_width_raw;
}

//------------------------------------------------------------------------------
MyGLFont::MyGLFont(void):
        m_been_init(false)
{
}

//------------------------------------------------------------------------------
MyGLFont::MyGLFont(const QFont p_font)
{
  init(p_font);
}

//EOF
