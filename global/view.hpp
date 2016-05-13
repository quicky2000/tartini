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
double View::viewWidth(void)
{
  return double(_pixelWidth) * _zoomX;
}

//------------------------------------------------------------------------------
double View::viewHeight(void)
{
  return double(_pixelHeight) / exp(_logZoomY);
}

//------------------------------------------------------------------------------
double View::viewOffset(void)
{
  return _viewOffset;
}

//------------------------------------------------------------------------------
double View::viewBottom(void)
{
  return _viewBottom;
}

//------------------------------------------------------------------------------
double View::viewLeft(void)
{
  return currentTime() - viewOffset();
}

//------------------------------------------------------------------------------
double View::viewRight(void)
{
  return viewLeft() + viewWidth();
}

//------------------------------------------------------------------------------
double View::viewTotalTime(void)
{
  return viewWidth();
}

//------------------------------------------------------------------------------
double View::viewTop(void)
{
  return viewBottom() + viewHeight();
}

//------------------------------------------------------------------------------
int View::screenPixelX(double t)
{
  return toInt((t - viewLeft()) / zoomX());
}

//------------------------------------------------------------------------------
double View::zoomX(void)
{
  return _zoomX;
}

//------------------------------------------------------------------------------
double View::zoomY(void)
{
  return _zoomY;
}

//------------------------------------------------------------------------------
double View::logZoomX(void)
{
  return _logZoomX;
}

//------------------------------------------------------------------------------
double View::logZoomY(void)
{
  return _logZoomY;
}

//------------------------------------------------------------------------------
void View::setLogZoomYRaw(double y)
{
  setLogZoomY(y);
}

//EOF
