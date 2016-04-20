
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

#include <assert.h>

//------------------------------------------------------------------------------
float AnalysisData::getValue(size_t p_index)const
{
  assert(p_index < NUM_AMP_MODES);
  return values[p_index];
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriod(void)const
{
  return period;
}

//------------------------------------------------------------------------------
void AnalysisData::setPeriod(float p_period)
{
  period = p_period;
}

//------------------------------------------------------------------------------
float AnalysisData::getFundamentalFreq(void)const
{
  return fundamentalFreq;
}

//------------------------------------------------------------------------------
void AnalysisData::setFundamentalFreq(float p_fundamental_freq)
{
  fundamentalFreq = p_fundamental_freq;
}

//------------------------------------------------------------------------------
float AnalysisData::getFreqCentroid(void)const
{
  return _freqCentroid;
}

//------------------------------------------------------------------------------
float AnalysisData::getPitch(void)const
{
  return pitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitch(float p_pitch)
{
  pitch = p_pitch;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getPitchSum(void)const
{
  return pitchSum;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitchSum(const double & p_pitch_sum)
{
  pitchSum = p_pitch_sum;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getPitch2Sum(void)const
{
  return pitch2Sum;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitch2Sum(const double & p_pitch_2_sum)
{
  pitch2Sum = p_pitch_2_sum;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getShortTermMean(void)const
{
  return shortTermMean;
}

//------------------------------------------------------------------------------
void AnalysisData::setShortTermMean(const double & p_short_term_mean)
{
  shortTermMean = p_short_term_mean;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getShortTermDeviation(void)const
{
  return shortTermDeviation;
}

//------------------------------------------------------------------------------
void AnalysisData::setShortTermDeviation(const double & p_short_term_deviation)
{
  shortTermDeviation = p_short_term_deviation;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getLongTermMean(void)const
{
  return longTermMean;
}

//------------------------------------------------------------------------------
void AnalysisData::setLongTermMean(const double & p_long_term_mean)
{
  longTermMean = p_long_term_mean;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getLongTermDeviation(void)const
{
  return longTermDeviation;
}

//------------------------------------------------------------------------------
void AnalysisData::setLongTermDeviation(const double & p_long_term_deviation)
{
  longTermDeviation = p_long_term_deviation;
}

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

//------------------------------------------------------------------------------
size_t AnalysisData::getPeriodEstimatesAmpSize(void)const
{
  return periodEstimatesAmp.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodEstimatesAmpAt(size_t p_index)const
{
#ifdef MYDEBUG
  return periodEstimatesAmp.at(p_index);
#else
  return periodEstimatesAmp[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::clearPeriodEstimatesAmp(void)
{
  periodEstimatesAmp.clear();
}

//------------------------------------------------------------------------------
void AnalysisData::addPeriodEstimatesAmp(float p_value)
{
  periodEstimatesAmp.push_back(p_value);
}

//------------------------------------------------------------------------------
int AnalysisData::getPeriodEstimatesAmpMaxElementIndex(void)const
{
  return int(std::max_element(periodEstimatesAmp.begin(), periodEstimatesAmp.end()) - periodEstimatesAmp.begin());
}

//------------------------------------------------------------------------------
bool lessFundametalFreq::operator()(const AnalysisData & x, const AnalysisData & y)
{
  return x.getFundamentalFreq() < y.getFundamentalFreq();
}

//------------------------------------------------------------------------------
bool greaterFundametalFreq::operator()(const AnalysisData &x, const AnalysisData &y)
{
  return x.getFundamentalFreq() > y.getFundamentalFreq();
}

//------------------------------------------------------------------------------
bool lessPitch::operator()(const AnalysisData &x, const AnalysisData &y)
{
  return x.getPitch() < y.getPitch();
}

//------------------------------------------------------------------------------
bool greaterPitch::operator()(const AnalysisData &x, const AnalysisData &y)
{
  return x.getPitch() > y.getPitch();
}

//------------------------------------------------------------------------------
lessValue::lessValue(int v_)
{
  v = v_;
}

//------------------------------------------------------------------------------
bool lessValue::operator()(const AnalysisData &x, const AnalysisData &y)
{
  return x.getValue(v) < y.getValue(v);
}

//------------------------------------------------------------------------------
greaterValue::greaterValue(int v_)
{
  v = v_;
}

//------------------------------------------------------------------------------
bool greaterValue::operator()(const AnalysisData &x, const AnalysisData &y)
{
  return x.getValue(v) > y.getValue(v);
}

//EOF
