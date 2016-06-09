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
NoteData::NoteData(void):
  channel(NULL)
{
}

//------------------------------------------------------------------------------
bool NoteData::isValid(void) const
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
int NoteData::startChunk(void) const
{
  return _startChunk;
}

//------------------------------------------------------------------------------
int NoteData::endChunk(void) const
{
  return _endChunk;
}

//------------------------------------------------------------------------------
int NoteData::numChunks(void) const
{
  return _endChunk - _startChunk;
}

//------------------------------------------------------------------------------
float NoteData::numPeriods(void) const
{
  return _numPeriods;
}

//------------------------------------------------------------------------------
void NoteData::setPeriodOctaveEstimate(float periodOctaveEstimate_)
{
  _periodOctaveEstimate = periodOctaveEstimate_;
}

//------------------------------------------------------------------------------
float NoteData::periodOctaveEstimate(void) const
{
  return _periodOctaveEstimate;
}

//------------------------------------------------------------------------------
float NoteData::volume(void) const
{
  return _volume;
}

//------------------------------------------------------------------------------
double NoteData::avgFreq(void)
{
  return numPeriods() / noteLength();
}

//------------------------------------------------------------------------------
const SmartPtr<Array1d<int> > NoteData::get_maxima(void) const
{
  return maxima;
}

//------------------------------------------------------------------------------
const SmartPtr<Array1d<int> > NoteData::get_minima(void) const
{
  return minima;
}

//------------------------------------------------------------------------------
void NoteData::set_first_nsdf_period(float p_period)
{
  firstNsdfPeriod = p_period;
}

//------------------------------------------------------------------------------
float NoteData::get_first_nsdf_period(void)const
{
  return firstNsdfPeriod;
}

//------------------------------------------------------------------------------
void NoteData::set_current_nsdf_period(float p_period)
{
  currentNsdfPeriod = p_period;
}

//------------------------------------------------------------------------------
void NoteData::add_current_nsdf_period(float p_period_diff)
{
  currentNsdfPeriod += p_period_diff;
}

//------------------------------------------------------------------------------
float NoteData::get_current_nsdf_period(void) const
{
  return currentNsdfPeriod;
}

//------------------------------------------------------------------------------
void NoteData::set_nsdf_aggregate_roof(const double & p_value)
{
  nsdfAggregateRoof = p_value;
}

//------------------------------------------------------------------------------
void NoteData::add_nsdf_aggregate_roof(const double & p_value)
{
  nsdfAggregateRoof += p_value;
}

//------------------------------------------------------------------------------
const double & NoteData::get_nsdf_aggregate_roof(void)const
{
  return nsdfAggregateRoof;
}

//------------------------------------------------------------------------------
const Array1d<float> & NoteData::get_nsdf_aggregate_data(void)const
{
  return nsdfAggregateData;
}

//------------------------------------------------------------------------------
const Array1d<float> & NoteData::get_nsdf_aggregate_data_scaled(void)const
{
  return nsdfAggregateDataScaled;
}
//EOF
