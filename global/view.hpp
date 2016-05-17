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
void View::setLogZoomX(double x)
{
  _logZoomX = x;
  _zoomX = 1.0 / exp(_logZoomX);
}

//------------------------------------------------------------------------------
void View::setLogZoomY(double y)
{
  _logZoomY = bound(y, 0.6, 6.0);
  _zoomY = 1.0 / exp(_logZoomY);
}

//------------------------------------------------------------------------------
const double & View::currentTime(void)const
{
  return _currentTime;
}

//------------------------------------------------------------------------------
double View::viewWidth(void)const
{
  return double(_pixelWidth) * _zoomX;
}

//------------------------------------------------------------------------------
double View::viewHeight(void)const
{
  return double(_pixelHeight) / exp(_logZoomY);
}

//------------------------------------------------------------------------------
const double & View::viewOffset(void)const
{
  return _viewOffset;
}

//------------------------------------------------------------------------------
const double & View::viewBottom(void)const
{
  return _viewBottom;
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
int View::screenPixelX(double t)const
{
  return toInt((t - viewLeft()) / zoomX());
}

//------------------------------------------------------------------------------
const double & View::zoomX(void)const
{
  return _zoomX;
}

//------------------------------------------------------------------------------
const double & View::zoomY(void)const
{
  return _zoomY;
}

//------------------------------------------------------------------------------
const double & View::logZoomX(void)const
{
  return _logZoomX;
}

//------------------------------------------------------------------------------
const double & View::logZoomY(void)const
{
  return _logZoomY;
}

//------------------------------------------------------------------------------
void View::setLogZoomYRaw(double y)
{
  setLogZoomY(y);
}

//------------------------------------------------------------------------------
bool View::autoFollow(void)const
{
  return _autoFollow;
}

//------------------------------------------------------------------------------
bool View::backgroundShading(void)const
{
  return _backgroundShading;
}

//EOF
