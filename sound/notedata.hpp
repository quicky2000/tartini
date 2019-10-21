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
  m_channel(NULL)
{
}

//------------------------------------------------------------------------------
bool NoteData::isValid(void) const
{
  return (numChunks() > 2);
}

//------------------------------------------------------------------------------
void NoteData::setStartChunk(int p_start_chunk)
{
    m_start_chunk = p_start_chunk;
}

//------------------------------------------------------------------------------
void NoteData::setEndChunk(int p_end_chunk)
{
    m_end_chunk = p_end_chunk;
}

//------------------------------------------------------------------------------
int NoteData::startChunk(void) const
{
  return m_start_chunk;
}

//------------------------------------------------------------------------------
int NoteData::endChunk(void) const
{
  return m_end_chunk;
}

//------------------------------------------------------------------------------
int NoteData::numChunks(void) const
{
  return m_end_chunk - m_start_chunk;
}

//------------------------------------------------------------------------------
float NoteData::numPeriods(void) const
{
  return m_num_periods;
}

//------------------------------------------------------------------------------
void NoteData::setPeriodOctaveEstimate(float p_period_octave_estimate)
{
    m_period_octave_estimate = p_period_octave_estimate;
}

//------------------------------------------------------------------------------
float NoteData::periodOctaveEstimate(void) const
{
  return m_period_octave_estimate;
}

//------------------------------------------------------------------------------
float NoteData::volume(void) const
{
  return m_volume;
}

//------------------------------------------------------------------------------
double NoteData::avgFreq(void) const
{
  return numPeriods() / noteLength();
}

//------------------------------------------------------------------------------
const SmartPtr<Array1d<int> > NoteData::get_maxima(void) const
{
  return m_maxima;
}

//------------------------------------------------------------------------------
const SmartPtr<Array1d<int> > NoteData::get_minima(void) const
{
  return m_minima;
}

//------------------------------------------------------------------------------
void NoteData::set_first_nsdf_period(float p_period)
{
  m_first_nsdf_period = p_period;
}

//------------------------------------------------------------------------------
float NoteData::get_first_nsdf_period(void)const
{
  return m_first_nsdf_period;
}

//------------------------------------------------------------------------------
void NoteData::set_current_nsdf_period(float p_period)
{
  m_current_nsdf_period = p_period;
}

//------------------------------------------------------------------------------
void NoteData::add_current_nsdf_period(float p_period_diff)
{
  m_current_nsdf_period += p_period_diff;
}

//------------------------------------------------------------------------------
float NoteData::get_current_nsdf_period(void) const
{
  return m_current_nsdf_period;
}

//------------------------------------------------------------------------------
void NoteData::set_nsdf_aggregate_roof(const double & p_value)
{
    m_nsdf_aggregate_roof = p_value;
}

//------------------------------------------------------------------------------
void NoteData::add_nsdf_aggregate_roof(const double & p_value)
{
    m_nsdf_aggregate_roof += p_value;
}

//------------------------------------------------------------------------------
const double & NoteData::get_nsdf_aggregate_roof(void)const
{
  return m_nsdf_aggregate_roof;
}

//------------------------------------------------------------------------------
const Array1d<float> & NoteData::get_nsdf_aggregate_data(void)const
{
  return m_nsdf_aggregate_data;
}

//------------------------------------------------------------------------------
const Array1d<float> & NoteData::get_nsdf_aggregate_data_scaled(void)const
{
  return m_nsdf_aggregate_data_scaled;
}
//EOF
