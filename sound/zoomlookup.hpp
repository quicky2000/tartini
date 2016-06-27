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
ZoomElement::ZoomElement(void)
{
  _isValid = false;
}

//------------------------------------------------------------------------------
float ZoomElement::low(void)
{
  return _low;
}

//------------------------------------------------------------------------------
float ZoomElement::high(void)
{
  return _high;
}

//------------------------------------------------------------------------------
float ZoomElement::corr(void)
{
  return _corr;
}

//------------------------------------------------------------------------------
QColor ZoomElement::color(void)
{
  return _color;
}

//------------------------------------------------------------------------------
int ZoomElement::noteIndex(void)
{
  return _noteIndex;
}

//------------------------------------------------------------------------------
int ZoomElement::midChunk(void)
{
  return _midChunk;
}

//------------------------------------------------------------------------------
bool ZoomElement::isValid(void)
{
  return _isValid;
}

//------------------------------------------------------------------------------
void ZoomElement::set(float low_,
		      float high_,
		      float corr_,
		      const QColor & color_,
		      int noteIndex_,
		      int midChunk_
		      )
{
  _low = low_;
  _high = high_;
  _corr = corr_;
  _color = color_;
  _noteIndex = noteIndex_;
  _midChunk = midChunk_;
  _isValid = true;
}

//------------------------------------------------------------------------------
int ZoomLookup::size(void)
{
  return _size;
}

//------------------------------------------------------------------------------
ZoomElement & ZoomLookup::at(int x)
{
  myassert(x >= 0 && x < int(_table.size()));
  return _table[x];
}

// EOF
