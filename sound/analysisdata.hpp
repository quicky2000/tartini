
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
int AnalysisData::get_no_note()
{
    return m_no_note;
}
//------------------------------------------------------------------------------
float AnalysisData::getValue(t_amplitude_modes p_index)const
{
    assert(static_cast<int>(p_index) <= static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE));
    return m_values[static_cast<int>(p_index)];
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriod()const
{
    return m_period;
}

//------------------------------------------------------------------------------
void AnalysisData::setPeriod(float p_period)
{
    m_period = p_period;
}

//------------------------------------------------------------------------------
float AnalysisData::getFundamentalFreq()const
{
    return m_fundamental_freq;
}

//------------------------------------------------------------------------------
void AnalysisData::setFundamentalFreq(float p_fundamental_freq)
{
    m_fundamental_freq = p_fundamental_freq;
}

//------------------------------------------------------------------------------
float AnalysisData::getFreqCentroid()const
{
    return m_freq_centroid;
}

//------------------------------------------------------------------------------
void AnalysisData::setFreqCentroid(float p_value)
{
    m_freq_centroid = p_value;
}

//------------------------------------------------------------------------------
float AnalysisData::getPitch()const
{
    return m_pitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitch(float p_pitch)
{
    m_pitch = p_pitch;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getPitchSum()const
{
    return m_pitch_sum;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitchSum(const double & p_pitch_sum)
{
    m_pitch_sum = p_pitch_sum;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getPitch2Sum()const
{
    return m_pitch_squared_sum;
}

//------------------------------------------------------------------------------
void AnalysisData::setPitch2Sum(const double & p_pitch_2_sum)
{
    m_pitch_squared_sum = p_pitch_2_sum;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getShortTermMean()const
{
    return m_short_term_mean;
}

//------------------------------------------------------------------------------
void AnalysisData::setShortTermMean(const double & p_short_term_mean)
{
    m_short_term_mean = p_short_term_mean;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getShortTermDeviation()const
{
    return m_short_term_deviation;
}

//------------------------------------------------------------------------------
void AnalysisData::setShortTermDeviation(const double & p_short_term_deviation)
{
    m_short_term_deviation = p_short_term_deviation;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getLongTermMean()const
{
    return m_long_term_mean;
}

//------------------------------------------------------------------------------
void AnalysisData::setLongTermMean(const double & p_long_term_mean)
{
    m_long_term_mean = p_long_term_mean;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getLongTermDeviation()const
{
    return m_long_term_deviation;
}

//------------------------------------------------------------------------------
void AnalysisData::setLongTermDeviation(const double & p_long_term_deviation)
{
    m_long_term_deviation = p_long_term_deviation;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getSpread()const
{
    return m_spread;
}

//------------------------------------------------------------------------------
void AnalysisData::setSpread(const double & p_spread)
{
    m_spread = p_spread;
}

//------------------------------------------------------------------------------
const double & AnalysisData::getSpread2()const
{
    return m_spread_2;
}

//------------------------------------------------------------------------------
void AnalysisData::setSpread2(const double & p_spread2)
{
    m_spread_2 = p_spread2;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoPitch()const
{
    return m_vibrato_pitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoPitch(float p_vibrato_pitch)
{
    m_vibrato_pitch = p_vibrato_pitch;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoWidth()const
{
    return m_vibrato_width;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoWidth(float p_vibrato_width)
{
    m_vibrato_width = p_vibrato_width;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoWidthAdjust()const
{
    return m_vibrato_width_adjust;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoWidthAdjust(float p_vibrato_width_adjust)
{
    m_vibrato_width_adjust = p_vibrato_width_adjust;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoSpeed()const
{
    return m_vibrato_speed;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoSpeed(float p_vibrato_speed)
{
    m_vibrato_speed = p_vibrato_speed;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoPhase()const
{
    return m_vibrato_phase;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoPhase(float p_vibrato_phase)
{
    m_vibrato_phase = p_vibrato_phase;
}

//------------------------------------------------------------------------------
float AnalysisData::getVibratoError()const
{
    return m_vibrato_error;
}

//------------------------------------------------------------------------------
void AnalysisData::setVibratoError(float p_vibrato_error)
{
    m_vibrato_error = p_vibrato_error;
}

//------------------------------------------------------------------------------
int AnalysisData::getReason()const
{
    return m_reason;
}

//------------------------------------------------------------------------------
void AnalysisData::setReason(int p_reason)
{
    m_reason = p_reason;
}

//------------------------------------------------------------------------------
int AnalysisData::getHighestCorrelationIndex()const
{
    return m_highest_correlation_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setHighestCorrelationIndex(int p_index)
{
    m_highest_correlation_index = p_index;
}

//------------------------------------------------------------------------------
int AnalysisData::getChosenCorrelationIndex()const
{
    return m_chosen_correlation_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setChosenCorrelationIndex(int p_index)
{
    m_chosen_correlation_index = p_index;
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodRatio()const
{
    return m_period_ratio;
}

//------------------------------------------------------------------------------
void AnalysisData::setPeriodRatio(float p_period_ratio)
{
    m_period_ratio = p_period_ratio;
}

//------------------------------------------------------------------------------
int AnalysisData::getCepstrumIndex()const
{
    return m_cepstrum_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setCepstrumIndex(int p_index)
{
    m_cepstrum_index = p_index;
}

//------------------------------------------------------------------------------
float AnalysisData::getCepstrumPitch()const
{
    return m_cepstrum_pitch;
}

//------------------------------------------------------------------------------
void AnalysisData::setCepstrumPitch(float p_cepstrum_pitch)
{
    m_cepstrum_pitch = p_cepstrum_pitch;
}

//------------------------------------------------------------------------------
bool AnalysisData::isPeriodEstimatesEmpty()const
{
    return m_period_estimates.empty();
}

//------------------------------------------------------------------------------
size_t AnalysisData::getPeriodEstimatesSize()const
{
    return m_period_estimates.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodEstimatesAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_period_estimates.at(p_index);
#else // MYDEBUG
    return m_period_estimates[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::clearPeriodEstimates()
{
    m_period_estimates.clear();
}

//------------------------------------------------------------------------------
void AnalysisData::addPeriodEstimates(float p_value)
{
    m_period_estimates.push_back(p_value);
}

//------------------------------------------------------------------------------
float AnalysisData::searchClosestPeriodEstimates(const float & p_value)const
{
    std::vector<float>::const_iterator l_closest_iter = binary_search_closest(m_period_estimates.begin(), m_period_estimates.end(), p_value);
    return *l_closest_iter;
}

//------------------------------------------------------------------------------
size_t AnalysisData::getPeriodEstimatesAmpSize()const
{
    return m_period_estimates_amp.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getPeriodEstimatesAmpAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_period_estimates_amp.at(p_index);
#else // MYDEBUG
    return m_period_estimates_amp[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::clearPeriodEstimatesAmp()
{
    m_period_estimates_amp.clear();
}

//------------------------------------------------------------------------------
void AnalysisData::addPeriodEstimatesAmp(float p_value)
{
    m_period_estimates_amp.push_back(p_value);
}

//------------------------------------------------------------------------------
int AnalysisData::getPeriodEstimatesAmpMaxElementIndex()const
{
    return int(std::max_element(m_period_estimates_amp.begin(), m_period_estimates_amp.end()) - m_period_estimates_amp.begin());
}

//------------------------------------------------------------------------------
size_t AnalysisData::getHarmonicAmpNoCutOffSize()const
{
    return m_harmonic_amp_no_cut_off.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpNoCutOffAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_harmonic_amp_no_cut_off.at(p_index);
#else // MYDEBUG
    return m_harmonic_amp_no_cut_off[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicAmpNoCutOff(size_t p_size)
{
    m_harmonic_amp_no_cut_off.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicAmpNoCutOffAt( size_t p_index
                                           , float p_value
                                           )
{
#ifdef MYDEBUG
    m_harmonic_amp_no_cut_off.at(p_index) = p_value;
#else // MYDEBUG
    m_harmonic_amp_no_cut_off[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpRelativeAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_harmonic_amp_relative.at(p_index);
#else // MYDEBUG
    return m_harmonic_amp_relative[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
size_t AnalysisData::getHarmonicAmpRelativeSize()const
{
    return m_harmonic_amp_relative.size();
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicAmpAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_harmonic_amp.at(p_index);
#else // MYDEBUG
    return m_harmonic_amp[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicAmp(size_t p_size)
{
    m_harmonic_amp.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicAmpAt( size_t p_index
                                   , float p_value
                                   )
{
#ifdef MYDEBUG
    m_harmonic_amp.at(p_index) = p_value;
#else // MYDEBUG
    m_harmonic_amp[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicFreqAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_harmonic_freq.at(p_index);
#else // MYDEBUG
    return m_harmonic_freq[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
size_t AnalysisData::getHarmonicFreqSize()const
{
    return m_harmonic_freq.size();
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicFreq(size_t p_size)
{
    m_harmonic_freq.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicFreqAt( size_t p_index
                                    , float p_value
                                    )
{
#ifdef MYDEBUG
    m_harmonic_freq.at(p_index) = p_value;
#else // MYDEBUG
    m_harmonic_freq[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
float AnalysisData::getHarmonicNoiseAt(size_t p_index)const
{
#ifdef MYDEBUG
    return m_harmonic_noise.at(p_index);
#else // MYDEBUG
    return m_harmonic_noise[p_index];
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::resizeHarmonicNoise(size_t p_size)
{
    m_harmonic_noise.resize(p_size);
}

//------------------------------------------------------------------------------
void AnalysisData::setHarmonicNoiseAt( size_t p_index
                                     , float p_value
                                     )
{
#ifdef MYDEBUG
    m_harmonic_noise.at(p_index) = p_value;
#else // MYDEBUG
    m_harmonic_noise[p_index] = p_value;
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void AnalysisData::setFilterState(const FilterState & p_filter_state)
{
    m_filter_state = p_filter_state;
}

//------------------------------------------------------------------------------
int AnalysisData::getNoteIndex()const
{
    return m_note_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setNoteIndex(int p_node_index)
{
    m_note_index = p_node_index;
}

//------------------------------------------------------------------------------
void AnalysisData::setNotePlaying(bool p_note_playing)
{
    m_note_playing = p_note_playing;
}

//------------------------------------------------------------------------------
bool AnalysisData::isNotePlaying()const
{
    return m_note_playing;
}

//------------------------------------------------------------------------------
void AnalysisData::setDone(bool p_done)
{
    m_done = p_done;
}

//------------------------------------------------------------------------------
bool AnalysisData::isDone()const
{
    return m_done;
}

//------------------------------------------------------------------------------
float AnalysisData::getLogRms()const
{
    return m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)];
}

//------------------------------------------------------------------------------
void AnalysisData::setLogRms(float p_log_rms)
{
    m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)] = p_log_rms;
}

//------------------------------------------------------------------------------
float AnalysisData::getMaxIntensityDB()const
{
    return m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_MAX_INTENSITY)];
}

//------------------------------------------------------------------------------
void AnalysisData::setMaxIntensityDB(float p_value)
{
    m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_MAX_INTENSITY)] = p_value;
}

//------------------------------------------------------------------------------
float AnalysisData::getCorrelation()const
{
    return m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)];
}

//------------------------------------------------------------------------------
void AnalysisData::setCorrelation(float p_value)
{
    m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)] = p_value;
}

//------------------------------------------------------------------------------
void AnalysisData::setChangeness(float p_value)
{
    m_values[static_cast<int>(t_amplitude_modes::FREQ_CHANGENESS)] = p_value;
}

//------------------------------------------------------------------------------
void AnalysisData::setDeltaFreqCentroid(float p_value)
{
    m_values[static_cast<int>(t_amplitude_modes::DELTA_FREQ_CENTROID)] = p_value;
}

//------------------------------------------------------------------------------
float AnalysisData::getVolumeValue(const GData & p_data)const
{
    return (dB2Normalised(m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)], p_data) + m_values[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)] - 1.0f) * 0.2;
}

//------------------------------------------------------------------------------
float AnalysisData::getNoteScore()const
{
    return m_values[static_cast<int>(t_amplitude_modes::NOTE_SCORE)];
}

//------------------------------------------------------------------------------
float AnalysisData::getNoteChangeScore()const
{
    return m_values[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE)];
}

//------------------------------------------------------------------------------
bool lessFundametalFreq::operator()( const AnalysisData & p_x
                                   , const AnalysisData & p_y
                                   )
{
    return p_x.getFundamentalFreq() < p_y.getFundamentalFreq();
}

//------------------------------------------------------------------------------
bool greaterFundametalFreq::operator()( const AnalysisData & p_x
                                      , const AnalysisData & p_y
                                      )
{
    return p_x.getFundamentalFreq() > p_y.getFundamentalFreq();
}

//------------------------------------------------------------------------------
bool lessPitch::operator()( const AnalysisData & p_x
                          , const AnalysisData & p_y
                          )
{
    return p_x.getPitch() < p_y.getPitch();
}

//------------------------------------------------------------------------------
bool greaterPitch::operator()( const AnalysisData & p_x
                             , const AnalysisData & p_y
                             )
{
    return p_x.getPitch() > p_y.getPitch();
}

//------------------------------------------------------------------------------
lessValue::lessValue(t_amplitude_modes p_value)
{
    m_value = p_value;
}

//------------------------------------------------------------------------------
bool lessValue::operator()( const AnalysisData & p_x
                          , const AnalysisData & p_y
                          )
{
    return p_x.getValue(m_value) < p_y.getValue(m_value);
}

//------------------------------------------------------------------------------
greaterValue::greaterValue(t_amplitude_modes p_value)
{
    m_value = p_value;
}

//------------------------------------------------------------------------------
bool greaterValue::operator()( const AnalysisData & p_x
                             , const AnalysisData & p_y
                             )
{
    return p_x.getValue(m_value) > p_y.getValue(m_value);
}

//EOF
