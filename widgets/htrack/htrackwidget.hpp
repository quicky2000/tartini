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
float HTrackWidget::peakThreshold(void)const
{
    return m_peak_threshold;
}

//------------------------------------------------------------------------------
const double & HTrackWidget::viewAngleHorizontal(void)const
{
    return m_view_angle_horizontal;
}

//------------------------------------------------------------------------------
const double & HTrackWidget::viewAngleVertical(void)const
{
    return m_view_angle_vertical;
}

//------------------------------------------------------------------------------
void HTrackWidget::setPeakThreshold(float p_peak_threshold)
{
    m_peak_threshold = p_peak_threshold;
}

//------------------------------------------------------------------------------
void HTrackWidget::setViewAngleHorizontal(const double & p_angle)
{
    if(p_angle != m_view_angle_horizontal)
    {
        m_view_angle_horizontal = p_angle;
        emit viewAngleHorizontalChanged(p_angle);
    }
}

//------------------------------------------------------------------------------
void HTrackWidget::setViewAngleVertical(const double & p_angle)
{
    if(p_angle != m_view_angle_vertical)
    {
        m_view_angle_vertical = p_angle;
        emit viewAngleVerticalChanged(p_angle);
    }
}

//------------------------------------------------------------------------------
void HTrackWidget::setDistanceAway(const double & p_distance)
{
    if(p_distance != m_distance_away)
    {
        m_distance_away = p_distance;
        emit distanceAwayChanged(p_distance);
    }
}

//EOF
