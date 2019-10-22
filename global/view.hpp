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
void View::setLogZoomX(double p_x)
{
    m_log_zoom_X = p_x;
    m_zoom_X = 1.0 / exp(m_log_zoom_X);
}

//------------------------------------------------------------------------------
void View::setLogZoomY(double p_y)
{
    m_log_zoom_Y = bound(p_y, 0.6, 6.0);
    m_zoom_Y = 1.0 / exp(m_log_zoom_Y);
}

//------------------------------------------------------------------------------
const double & View::currentTime(void)const
{
  return m_current_time;
}

//------------------------------------------------------------------------------
double View::viewWidth(void)const
{
  return double(m_pixel_width) * m_zoom_X;
}

//------------------------------------------------------------------------------
double View::viewHeight(void)const
{
  return double(m_pixel_height) / exp(m_log_zoom_Y);
}

//------------------------------------------------------------------------------
const double & View::viewOffset(void)const
{
  return m_view_offset;
}

//------------------------------------------------------------------------------
const double & View::viewBottom(void)const
{
  return m_view_bottom;
}

//------------------------------------------------------------------------------
double View::viewLeft(void)const
{
  return currentTime() - viewOffset();
}

//------------------------------------------------------------------------------
double View::viewRight(void)const
{
  return viewLeft() + viewWidth();
}

//------------------------------------------------------------------------------
double View::viewTotalTime(void)const
{
  return viewWidth();
}

//------------------------------------------------------------------------------
double View::viewTop(void)const
{
  return viewBottom() + viewHeight();
}

//------------------------------------------------------------------------------
int View::screenPixelX(double p_t)const
{
  return toInt((p_t - viewLeft()) / zoomX());
}

//------------------------------------------------------------------------------
const double & View::zoomX(void)const
{
  return m_zoom_X;
}

//------------------------------------------------------------------------------
const double & View::zoomY(void)const
{
  return m_zoom_Y;
}

//------------------------------------------------------------------------------
const double & View::logZoomX(void)const
{
  return m_log_zoom_X;
}

//------------------------------------------------------------------------------
const double & View::logZoomY(void)const
{
  return m_log_zoom_Y;
}

//------------------------------------------------------------------------------
void View::setLogZoomYRaw(double p_y)
{
  setLogZoomY(p_y);
}

//------------------------------------------------------------------------------
bool View::autoFollow(void)const
{
  return m_auto_follow;
}

//------------------------------------------------------------------------------
bool View::backgroundShading(void)const
{
  return m_background_shading;
}

//EOF
