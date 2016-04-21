
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
void AnalysisData::setFreqCentroid(float p_value)
{
  _freqCentroid = p_value;
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
const double & AnalysisData::getSpread(void)const
{
  return spread;
}

//------------------------------------------------------------------------------
void AnalysisData::setSpread(const double & p_spread)
{
  spread = p_spread;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getSpread2(void)const
{
  return spread2;
}

//------------------------------------------------------------------------------
void AnalysisData::setSpread2(const double & p_spread2)
{
  spread2 = p_spread2;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoPitch(void)const
{
  return vibratoPitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoPitch(float p_vibrato_pitch)
{
  vibratoPitch = p_vibrato_pitch;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoWidth(void)const
{
  return vibratoWidth;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoWidth(float p_vibrato_width)
{
  vibratoWidth = p_vibrato_width;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoWidthAdjust(void)const
{
  return vibratoWidthAdjust;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoWidthAdjust(float p_vibrato_width_adjust)
{
  vibratoWidthAdjust = p_vibrato_width_adjust;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoSpeed(void)const
{
  return vibratoSpeed;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoSpeed(float p_vibrato_speed)
{
  vibratoSpeed = p_vibrato_speed;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoPhase(void)const
{
  return vibratoPhase;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoPhase(float p_vibrato_phase)
{
  vibratoPhase = p_vibrato_phase;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoError(void)const
{
  return vibratoError;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoError(float p_vibrato_error)
{
  vibratoError = p_vibrato_error;
}

//------------------------------------------------------------------------------
int AnalysisData::getReason(void)const
{
  return reason;
}

//------------------------------------------------------------------------------
void AnalysisData::setReason(int p_reason)
{
  reason = p_reason;
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
int AnalysisData::getChosenCorrelationIndex(void)const
{
  return chosenCorrelationIndex;
}

//------------------------------------------------------------------------------
void AnalysisData::setChosenCorrelationIndex(int p_index)
{
  chosenCorrelationIndex = p_index;
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodRatio(void)const
{
  return periodRatio;
}

//------------------------------------------------------------------------------
void AnalysisData::setPeriodRatio(float p_period_ratio)
{
  periodRatio = p_period_ratio;
}

//------------------------------------------------------------------------------
int AnalysisData::getCepstrumIndex(void)const
{
  return cepstrumIndex;
}

//------------------------------------------------------------------------------
void AnalysisData::setCepstrumIndex(int p_index)
{
  cepstrumIndex = p_index;
}

//------------------------------------------------------------------------------
float AnalysisData::getCepstrumPitch(void)const
{
  return cepstrumPitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setCepstrumPitch(float p_cepstrum_pitch)
{
  cepstrumPitch = p_cepstrum_pitch;
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
size_t AnalysisData::getHarmonicAmpNoCutOffSize(void)const
{
  return harmonicAmpNoCutOff.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpNoCutOffAt(size_t p_index)const
{
#ifdef MYDEBUG
  return harmonicAmpNoCutOff.at(p_index);
#else
  return harmonicAmpNoCutOff[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicAmpNoCutOff(size_t p_size)
{
  harmonicAmpNoCutOff.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicAmpNoCutOffAt(size_t p_index, float p_value)
{
#ifdef MYDEBUG
  harmonicAmpNoCutOff.at(p_index) = p_value;
#else
  harmonicAmpNoCutOff[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpRelativeAt(size_t p_index)const
{
#ifdef MYDEBUG
  return harmonicAmpRelative.at(p_index);
#else
  return harmonicAmpRelative[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
size_t AnalysisData::getHarmonicAmpRelativeSize(void)const
{
  return harmonicAmpRelative.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpAt(size_t p_index)const
{
#ifdef MYDEBUG
  return harmonicAmp.at(p_index);
#else
  return harmonicAmp[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicAmp(size_t p_size)
{
  harmonicAmp.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicAmpAt(size_t p_index, float p_value)
{
#ifdef MYDEBUG
  harmonicAmp.at(p_index) = p_value;
#else
  harmonicAmp[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicFreqAt(size_t p_index)const
{
#ifdef MYDEBUG
  return harmonicFreq.at(p_index);
#else
  return harmonicFreq[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
size_t AnalysisData::getHarmonicFreqSize(void)const
{
  return harmonicFreq.size();
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicFreq(size_t p_size)
{
  harmonicFreq.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicFreqAt(size_t p_index, float p_value)
{
#ifdef MYDEBUG
  harmonicFreq.at(p_index) = p_value;
#else
  harmonicFreq[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicNoiseAt(size_t p_index)const
{
#ifdef MYDEBUG
  return harmonicNoise.at(p_index);
#else
  return harmonicNoise[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicNoise(size_t p_size)
{
  harmonicNoise.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicNoiseAt(size_t p_index, float p_value)
{
#ifdef MYDEBUG
  harmonicNoise.at(p_index) = p_value;
#else
  harmonicNoise[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::setFilterState(const FilterState & p_filter_state)
{
  filterState = p_filter_state;
}

//------------------------------------------------------------------------------
int AnalysisData::getNoteIndex(void)const
{
  return noteIndex;
}

//------------------------------------------------------------------------------
void AnalysisData::setNoteIndex(int p_node_index)
{
  noteIndex = p_node_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setNotePlaying(bool p_note_playing)
{
  notePlaying = p_note_playing;
}

//------------------------------------------------------------------------------
bool AnalysisData::isNotePlaying(void)const
{
  return notePlaying;
}

//------------------------------------------------------------------------------
void AnalysisData::setDone(bool p_done)
{
  done = p_done;
}

//------------------------------------------------------------------------------
bool AnalysisData::isDone(void)const
{
  return done;
}

//------------------------------------------------------------------------------
float AnalysisData::getLogRms(void)const
{
  return values[AMPLITUDE_RMS];
}

//------------------------------------------------------------------------------
void AnalysisData::setLogRms(float p_log_rms)
{
  values[AMPLITUDE_RMS] = p_log_rms;
}

//------------------------------------------------------------------------------
float AnalysisData::getMaxIntensityDB(void)const
{
  return values[AMPLITUDE_MAX_INTENSITY];
}

//------------------------------------------------------------------------------
void AnalysisData::setMaxIntensityDB(float p_value)
{
  values[AMPLITUDE_MAX_INTENSITY] = p_value;
}

//------------------------------------------------------------------------------
float AnalysisData::getCorrelation(void)const
{
  return values[AMPLITUDE_CORRELATION];
}

//------------------------------------------------------------------------------
void AnalysisData::setCorrelation(float p_value)
{
  values[AMPLITUDE_CORRELATION] = p_value;
}

//------------------------------------------------------------------------------
void AnalysisData::setChangeness(float p_value)
{
  values[FREQ_CHANGENESS] = p_value;
}

//------------------------------------------------------------------------------
void AnalysisData::setDeltaFreqCentroid(float p_value)
{
  values[DELTA_FREQ_CENTROID] = p_value;
}

//------------------------------------------------------------------------------
float AnalysisData::getVolumeValue(void)const
{
  return (dB2Normalised(values[AMPLITUDE_RMS]) + values[AMPLITUDE_CORRELATION] - 1.0f) * 0.2;
}

//------------------------------------------------------------------------------
float AnalysisData::getNoteScore(void)const
{
  return values[NOTE_SCORE];
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
