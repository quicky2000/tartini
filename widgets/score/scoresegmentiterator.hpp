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
ScoreSegmentIterator::ScoreSegmentIterator(ScoreWidget *p_score_widget, Channel *p_channel)
{
  reset(p_score_widget, p_channel);
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::isValid(void)
{
  return m_is_valid;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::leftTime(void)
{
  return m_left_time;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::rightTime(void)
{
  return m_right_time;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::leftX(void)
{
  return m_left_X;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::lineCenterY(void)
{
  return m_line_center_Y;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::staveTop(void)
{
  return m_line_center_Y - m_half_stave_height;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::staveBottom(void)
{
  return staveTop() + m_stave_height;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::curTime(void)
{
  return m_cur_time;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::curPage(void)
{
  return m_cur_page;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::numPages(void)
{
  return m_num_pages;
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::contains(double p_t)
{
  return (p_t >= m_left_time && p_t <= m_right_time);
}
// EOF
