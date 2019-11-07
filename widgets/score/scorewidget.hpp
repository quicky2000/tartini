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
int ScoreWidget::getStaveHeight()
{
    return toInt(m_scale_Y * (m_show_base_clef ? 16 : 8));
}

//------------------------------------------------------------------------------
int ScoreWidget::getStaveCenterY()
{
    return toInt(m_scale_Y * (m_show_base_clef ? 8 : 7));
}

//------------------------------------------------------------------------------
QSize ScoreWidget::sizeHint() const
{
    return QSize(300, 200);
}
 
//------------------------------------------------------------------------------
double ScoreWidget::scaleX()
{
    return m_scale_X;
}

//------------------------------------------------------------------------------
double ScoreWidget::scaleY()
{
    return m_scale_Y;
}

//------------------------------------------------------------------------------
void ScoreWidget::setScaleX(double p_x)
{
    m_scale_X = p_x;
}

//------------------------------------------------------------------------------
void ScoreWidget::setScaleY(double p_y)
{
    m_scale_Y = p_y;
}

//------------------------------------------------------------------------------
void ScoreWidget::setSharpsMode(int p_mode)
{
    m_use_flats = (p_mode != 0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setNotesMode(int p_mode)
{
    m_show_notes = (p_mode == 0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setClefMode(int p_mode)
{
    m_show_base_clef = (p_mode == 0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setOpaqueMode(int p_mode)
{
    m_show_opaque_notes = (p_mode != 0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setTransposeLevel(int p_index)
{
    m_pitch_offset = (p_index - 2) * -12;
}

//------------------------------------------------------------------------------
void ScoreWidget::setShowAllMode(int p_mode)
{
  m_show_all_mode = (p_mode !=0 );
}
// EOF
