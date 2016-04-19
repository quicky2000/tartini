
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
int AnalysisData::getHighestCorrelationIndex(void)const
{
  return highestCorrelationIndex;
}

//------------------------------------------------------------------------------
void AnalysisData::setHighestCorrelationIndex(int p_index)
{
  highestCorrelationIndex = p_index;
}

//------------------------------------------------------------------------------
bool AnalysisData::isPeriodEstimatesEmpty(void)const
{
  return periodEstimates.empty();
}

//------------------------------------------------------------------------------
size_t AnalysisData::getPeriodEstimatesSize(void)const
{
  return periodEstimates.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodEstimatesAt(size_t p_index)const
{
#ifdef MYDEBUG
  return periodEstimates.at(p_index);
#else
  return periodEstimates[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::clearPeriodEstimates(void)
{
  periodEstimates.clear();
}

//------------------------------------------------------------------------------
void AnalysisData::addPeriodEstimates(float p_value)
{
  periodEstimates.push_back(p_value);
}

//------------------------------------------------------------------------------
float AnalysisData::searchClosestPeriodEstimates(const float & p_value)const
{
  std::vector<float>::const_iterator l_closest_iter = binary_search_closest(periodEstimates.begin(), periodEstimates.end(), p_value);
  return *l_closest_iter;
}

//EOF
