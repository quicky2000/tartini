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
MyGLChar::MyGLChar(int width, int height, GLubyte *theData, int width_raw):
  _w(width),
  _h(height),
  _w_raw(width_raw),
  _data(theData)
{
}

//------------------------------------------------------------------------------
const GLubyte * MyGLChar::data(void) const
{
  return _data;
}

//------------------------------------------------------------------------------
int MyGLChar::w(void) const
{
  return _w;
}

//------------------------------------------------------------------------------
int MyGLChar::h(void) const
{
  return _h;
}

//------------------------------------------------------------------------------
int MyGLChar::w_raw(void) const
{
  return _w_raw;
}

//------------------------------------------------------------------------------
MyGLFont::MyGLFont(void):
  beenInit(false)
{
}

//------------------------------------------------------------------------------
MyGLFont::MyGLFont(const QFont f)
{
  init(f);
}

//EOF
