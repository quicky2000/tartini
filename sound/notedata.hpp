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
NoteData::NoteData(void)
{
}

//------------------------------------------------------------------------------
bool NoteData::isValid(void)
{
  return (numChunks() > 2);
}

//------------------------------------------------------------------------------
void NoteData::setStartChunk(int startChunk_)
{
  _startChunk = startChunk_;
}

//------------------------------------------------------------------------------
void NoteData::setEndChunk(int endChunk_)
{
  _endChunk = endChunk_;
}

//------------------------------------------------------------------------------
int NoteData::startChunk(void)
{
  return _startChunk;
}

//------------------------------------------------------------------------------
int NoteData::endChunk(void)
{
  return _endChunk;
}

//------------------------------------------------------------------------------
int NoteData::numChunks(void)
{
  return _endChunk - _startChunk;
}

//------------------------------------------------------------------------------
float NoteData::numPeriods(void)
{
  return _numPeriods;
}

//------------------------------------------------------------------------------
void NoteData::setPeriodOctaveEstimate(float periodOctaveEstimate_)
{
  _periodOctaveEstimate = periodOctaveEstimate_;
}

//------------------------------------------------------------------------------
float NoteData::periodOctaveEstimate(void)
{
  return _periodOctaveEstimate;
}

//------------------------------------------------------------------------------
float NoteData::volume(void)
{
  return _volume;
}

//------------------------------------------------------------------------------
double NoteData::avgFreq(void)
{
  return numPeriods() / noteLength();
}

//EOF
