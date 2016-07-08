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
ScoreSegmentIterator::ScoreSegmentIterator(ScoreWidget *scoreWidget, Channel *channel)
{
  reset(scoreWidget, channel);
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::isValid(void)
{
  return _isValid;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::leftTime(void)
{
  return _leftTime;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::rightTime(void)
{
  return _rightTime;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::leftX(void)
{
  return _leftX;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::lineCenterY(void)
{
  return _lineCenterY;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::staveTop(void)
{
  return _lineCenterY - halfStaveHeight;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::staveBottom(void)
{
  return staveTop() + staveHeight;
}

//------------------------------------------------------------------------------
double ScoreSegmentIterator::curTime(void)
{
  return _curTime;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::curPage(void)
{
  return _curPage;
}

//------------------------------------------------------------------------------
int ScoreSegmentIterator::numPages(void)
{
  return _numPages;
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::contains(double t)
{
  return (t >= _leftTime && t <= _rightTime);
}
// EOF
