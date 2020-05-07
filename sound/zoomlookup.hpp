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
ZoomElement::ZoomElement()
: m_low(0.0)
, m_high(0.0)
, m_corr(0.0)
, m_note_index(0)
, m_mid_chunk(0)
, m_is_valid(false)
{
}

//------------------------------------------------------------------------------
float ZoomElement::low() const
{
    return m_low;
}

//------------------------------------------------------------------------------
float ZoomElement::high() const
{
    return m_high;
}

//------------------------------------------------------------------------------
float ZoomElement::corr() const
{
    return m_corr;
}

//------------------------------------------------------------------------------
const QColor & ZoomElement::color() const
{
    return m_color;
}

//------------------------------------------------------------------------------
int ZoomElement::noteIndex() const
{
    return m_note_index;
}

//------------------------------------------------------------------------------
int ZoomElement::midChunk() const
{
    return m_mid_chunk;
}

//------------------------------------------------------------------------------
bool ZoomElement::isValid() const
{
    return m_is_valid;
}

//------------------------------------------------------------------------------
void ZoomElement::set( float p_low
                     , float p_high
                     , float p_corr
                     , const QColor & p_color
                     , int p_note_index
                     , int p_mid_chunk
                     )
{
    m_low = p_low;
    m_high = p_high;
    m_corr = p_corr;
    m_color = p_color;
    m_note_index = p_note_index;
    m_mid_chunk = p_mid_chunk;
    m_is_valid = true;
}

//------------------------------------------------------------------------------
int ZoomLookup::size() const
{
    return m_size;
}

//------------------------------------------------------------------------------
ZoomElement & ZoomLookup::at(int p_x)
{
    myassert(p_x >= 0 && p_x < int(m_table.size()));
    return m_table[p_x];
}

// EOF
