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
QSize TimeAxis::sizeHint() const
{
    return QSize(700, 20);
}

//------------------------------------------------------------------------------
double TimeAxis::leftTime() const
{
    return g_data->getView().viewLeft();
}

//------------------------------------------------------------------------------
double TimeAxis::rightTime() const
{
    return g_data->getView().viewRight();
}

//------------------------------------------------------------------------------
double TimeAxis::timeWidth() const
{
    return g_data->getView().viewTotalTime();
}

//------------------------------------------------------------------------------
void TimeAxis::setLeftTime(const double & p_time)
{
    m_left_time = p_time;
}

//------------------------------------------------------------------------------
void TimeAxis::setRightTime(const double & p_time)
{
    m_right_time = p_time;
}

//------------------------------------------------------------------------------
void TimeAxis::setRange(const double & p_left_time
                       ,const double & p_right_time
                       )
{
    m_left_time = p_left_time;
    m_right_time = p_right_time;
}

// EOF
