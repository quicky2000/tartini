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
int ScoreWidget::getStaveHeight(void)
{
  return toInt(_scaleY * (_showBaseClef ? 16 : 8));
}

//------------------------------------------------------------------------------
int ScoreWidget::getStaveCenterY(void)
{
  return toInt(_scaleY * (_showBaseClef ? 8 : 7));
}

//------------------------------------------------------------------------------
QSize ScoreWidget::sizeHint(void) const
{
  return QSize(300, 200);
}
 
//------------------------------------------------------------------------------
double ScoreWidget::scaleX(void)
{
  return _scaleX;
}

//------------------------------------------------------------------------------
double ScoreWidget::scaleY(void)
{
  return _scaleY;
}

//------------------------------------------------------------------------------
void ScoreWidget::setScaleX(double x)
{
  _scaleX = x;
}

//------------------------------------------------------------------------------
void ScoreWidget::setScaleY(double y)
{
  _scaleY = y;
}

//------------------------------------------------------------------------------
void ScoreWidget::setSharpsMode(int mode)
{
  _useFlats = (mode!=0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setNotesMode(int mode)
{
  _showNotes = (mode==0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setClefMode(int mode)
{
  _showBaseClef = (mode==0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setOpaqueMode(int mode)
{
  _showOpaqueNotes = (mode!=0);
}

//------------------------------------------------------------------------------
void ScoreWidget::setTransposeLevel(int index)
{
  _pitchOffset = (index - 2) * -12;
}

//------------------------------------------------------------------------------
void ScoreWidget::setShowAllMode(int mode)
{
  _showAllMode = (mode!=0);
}
// EOF
