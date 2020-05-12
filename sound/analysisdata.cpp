/***************************************************************************
                          analysisdata.cpp  -  description
                             -------------------
    begin                : Fri Dec 17 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "analysisdata.h"
#include "gdata.h"

const std::string g_amp_mode_names[NUM_AMP_MODES] =
  { "RMS Amplitude (dB)"
  , "Max Amplitude (dB)"
  , "Amplitude Correlation"
  , "Frequency Changness"
  , "Delta Frequency Centroid"
  , "Note Score"
  , "Note Change Score"
  };

const std::string g_amp_display_string[NUM_AMP_MODES] =
  { "RMS Amp Threshold"
  , "Max Amp Threshold"
  , "Amp Corr Threshold"
  , "Freq Changeness Threshold"
  , "Delta Freq Centroid Threshold"
  , "Note Score Threshold"
  , "Note Change Score Threshold"
  };

double(*g_amp_mode_func[NUM_AMP_MODES])(const double &, const GData &) =
{ &dB2Normalised
, &dB2Normalised
, &same
, &oneMinus
, &same
, &same
, &same
};

double(*g_amp_mode_inv_func[NUM_AMP_MODES])(const double &, const GData &) =
{ &normalised2dB
, &normalised2dB
, &same
, &oneMinus
, &same
, &same
, &same
};

//------------------------------------------------------------------------------
AnalysisData::AnalysisData()
: m_period(0.0f)
, m_fundamental_freq(0.0f)
, m_pitch(0.0f)
, m_freq_centroid(0.0f)
, m_pitch_sum(0.0f)
, m_pitch_squared_sum(0.0)
, m_short_term_mean(0.0)
, m_short_term_deviation(0.0)
, m_long_term_mean(0.0)
, m_long_term_deviation(0.0)
, m_spread(0.0)
, m_spread_2(0.0)
, m_vibrato_pitch(0.0f)
, m_vibrato_width(0.0f)
, m_vibrato_width_adjust(0.0f)
, m_vibrato_speed(0.0f)
, m_vibrato_phase(0.0f)
, m_vibrato_error(0.0f)
, m_reason(0)
, m_highest_correlation_index(-1)
, m_chosen_correlation_index(-1)
, m_period_ratio(1.0f)
, m_cepstrum_index(-1)
, m_cepstrum_pitch(0.0f)
, m_note_index(NO_NOTE)
, m_note_playing(false)
, m_done(false)
{
    std::fill(m_values, m_values + NUM_AMP_MODES, 0.0f);
}

//------------------------------------------------------------------------------
void AnalysisData::calcScores()
{
    double l_a[NUM_AMP_MODES - 2];
    for(int l_j = 0; l_j < NUM_AMP_MODES - 2 ; l_j++)
    {
        l_a[l_j] = bound(((*g_amp_mode_func[l_j])(m_values[l_j], GData::getUniqueInstance()) - (*g_amp_mode_func[l_j])(GData::getUniqueInstance().ampThreshold(static_cast<t_amplitude_modes>(l_j), 0), GData::getUniqueInstance())) / ((*g_amp_mode_func[l_j])(GData::getUniqueInstance().ampThreshold(static_cast<t_amplitude_modes>(l_j), 1), GData::getUniqueInstance()) - (*g_amp_mode_func[l_j])(GData::getUniqueInstance().ampThreshold(static_cast<t_amplitude_modes>(l_j), 0), GData::getUniqueInstance())), 0.0, 1.0);
    }
    m_values[static_cast<int>(t_amplitude_modes::NOTE_SCORE)] = l_a[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)] * l_a[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)];
    m_values[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE)] = (1.0 - l_a[static_cast<int>(t_amplitude_modes::FREQ_CHANGENESS)]);
}
//EOF
