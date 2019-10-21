/***************************************************************************
                          notedata.cpp  -  description
                             -------------------
    begin                : Mon May 23 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <vector>

#include "notedata.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "musicnotes.h"

//------------------------------------------------------------------------------
NoteData::NoteData(const Channel & p_channel)
: m_maxima(new Array1d<int>())
, m_minima(new Array1d<int>())
, m_channel(&p_channel)
, m_max_log_RMS(gdata->dBFloor())
, m_num_periods(0)
, m_period_octave_estimate(1.0f)
{
}

//------------------------------------------------------------------------------
NoteData::NoteData( const Channel & p_channel
                  , int p_start_chunk
                  , const AnalysisData & p_analysis_data
                  )
: m_nsdf_aggregate_roof(0.0)
, m_first_nsdf_period(0.0f)
, m_current_nsdf_period(0.0f)
, m_maxima(new Array1d<int>())
, m_minima(new Array1d<int>())
, m_channel(&p_channel)
, m_start_chunk(p_start_chunk)
, m_end_chunk(p_start_chunk + 1)
, m_max_log_RMS(p_analysis_data.getLogRms())
, m_max_intensity_DB(p_analysis_data.getMaxIntensityDB())
, m_max_correlation(p_analysis_data.getCorrelation())
, m_max_purity(p_analysis_data.getVolumeValue(*gdata))
, m_num_periods(0.0f) //periods;
, m_period_octave_estimate(1.0f)
, m_volume(0.0f)
, m_avg_pitch(0.0f)
, m_loop_step(m_channel->rate() / 1000)  // stepsize = 0.001 seconds
, m_loop_start(m_start_chunk * m_channel->framesPerChunk() + m_loop_step)
, m_prev_extremum_time(-1)
, m_prev_extremum_pitch(-1)
, m_prev_extremum(NONE)
{
    m_nsdf_aggregate_data.resize(p_channel.get_nsdf_data().size(), 0.0f);
    m_nsdf_aggregate_data_scaled.resize(p_channel.get_nsdf_data().size(), 0.0f);
}

//------------------------------------------------------------------------------
NoteData::~NoteData(void)
{
}

//------------------------------------------------------------------------------
void NoteData::resetData(void)
{
    m_num_periods = 0;
}

//------------------------------------------------------------------------------
void NoteData::addData( const AnalysisData & p_analysis_data
                      , float p_periods
                      )
{
    m_max_log_RMS = MAX(m_max_log_RMS, p_analysis_data.getLogRms());
    m_max_intensity_DB = MAX(m_max_intensity_DB, p_analysis_data.getMaxIntensityDB());
    m_max_correlation = MAX(m_max_correlation, p_analysis_data.getCorrelation());
    m_max_purity = MAX(m_max_purity, p_analysis_data.getVolumeValue(*gdata));
    m_volume = MAX(m_volume, dB2Normalised(p_analysis_data.getLogRms(), *gdata));
    m_num_periods += p_periods; //sum up the periods
    m_avg_pitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}

//------------------------------------------------------------------------------
double NoteData::noteLength(void) const
{
    return double(numChunks()) * double(m_channel->framesPerChunk()) / double(m_channel->rate());
}

//------------------------------------------------------------------------------
float NoteData::avgPitch(void) const
{
    return m_avg_pitch;
}

//------------------------------------------------------------------------------
void NoteData::addVibratoData(int p_chunk)
{
    if ((m_channel->doingDetailedPitch()) && (m_channel->get_pitch_lookup_smoothed().size() > 0))
    {
        // Detailed pitch information available, calculate maxima and minima
        int l_loop_limit = ((p_chunk + 1) * m_channel->framesPerChunk()) - m_loop_step;
        for(int l_current_time = m_loop_start; l_current_time < l_loop_limit; l_current_time += m_loop_step)
        {
            myassert(l_current_time + m_loop_step < (int)m_channel->get_pitch_lookup_smoothed().size());
            myassert(l_current_time - m_loop_step >= 0);
            float l_prev_pitch = m_channel->get_pitch_lookup_smoothed().at(l_current_time - m_loop_step);
            float l_current_pitch = m_channel->get_pitch_lookup_smoothed().at(l_current_time);
            float l_next_pitch = m_channel->get_pitch_lookup_smoothed().at(l_current_time + m_loop_step);

            if((l_prev_pitch < l_current_pitch) && (l_current_pitch >= l_next_pitch))
            {
                // Maximum
                if(m_prev_extremum == NONE)
                {
                    m_maxima->push_back(l_current_time);
                    m_prev_extremum_time = l_current_time;
                    m_prev_extremum_pitch = l_current_pitch;
                    m_prev_extremum = FIRST_MAXIMUM;
                    continue;
                }
                if((m_prev_extremum == FIRST_MAXIMUM) || (m_prev_extremum == MAXIMUM))
                {
                    if(l_current_pitch >= m_prev_extremum_pitch)
                    {
                        myassert(!m_maxima->isEmpty());
                        m_maxima->pop_back();
                        m_maxima->push_back(l_current_time);
                        m_prev_extremum_time = l_current_time;
                        m_prev_extremum_pitch = l_current_pitch;
                    }
                    continue;
                }
                if((fabs(l_current_pitch - m_prev_extremum_pitch) > 0.04) &&
                   (l_current_time - m_prev_extremum_time > 42 * m_loop_step)
                  )
                {
                    if(m_prev_extremum == MINIMUM)
                    {
                        m_maxima->push_back(l_current_time);
                        m_prev_extremum_time = l_current_time;
                        m_prev_extremum_pitch = l_current_pitch;
                        m_prev_extremum = MAXIMUM;
                        continue;
                    }
                    else
                    {
                        if(l_current_time - m_minima->at(0) < m_loop_step * 500)
                        {
                            // Good
                            m_maxima->push_back(l_current_time);
                            m_prev_extremum_time = l_current_time;
                            m_prev_extremum_pitch = l_current_pitch;
                            m_prev_extremum = MAXIMUM;
                            continue;
                        }
                        else
                        {
                            // Not good
                            myassert(!m_minima->isEmpty());
                            m_minima->pop_back();
                            m_maxima->push_back(l_current_time);
                            m_prev_extremum_time = l_current_time;
                            m_prev_extremum_pitch = l_current_pitch;
                            m_prev_extremum = FIRST_MAXIMUM;
                            continue;
                        }
                    }
                }
            }
            else if((l_prev_pitch > l_current_pitch) && (l_current_pitch <= l_next_pitch))
            {
                // Minimum
                if(m_prev_extremum == NONE)
                {
                    m_minima->push_back(l_current_time);
                    m_prev_extremum_time = l_current_time;
                    m_prev_extremum_pitch = l_current_pitch;
                    m_prev_extremum = FIRST_MINIMUM;
                    continue;
                }
                if((m_prev_extremum == FIRST_MINIMUM) || (m_prev_extremum == MINIMUM))
                {
                    if(l_current_pitch <= m_prev_extremum_pitch)
                    {
                        myassert(!m_minima->isEmpty());
                        m_minima->pop_back();
                        m_minima->push_back(l_current_time);
                        m_prev_extremum_time = l_current_time;
                        m_prev_extremum_pitch = l_current_pitch;
                    }
                    continue;
                }
                if((fabs(l_current_pitch - m_prev_extremum_pitch) > 0.04) &&
                   (l_current_time - m_prev_extremum_time > 42 * m_loop_step)
                  )
                {
                    if(m_prev_extremum == MAXIMUM)
                    {
                        m_minima->push_back(l_current_time);
                        m_prev_extremum_time = l_current_time;
                        m_prev_extremum_pitch = l_current_pitch;
                        m_prev_extremum = MINIMUM;
                        continue;
                    }
                    else
                    {
                        if(l_current_time - m_maxima->at(0) < m_loop_step * 500)
                        {
                            // Good
                            m_minima->push_back(l_current_time);
                            m_prev_extremum_time = l_current_time;
                            m_prev_extremum_pitch = l_current_pitch;
                            m_prev_extremum = MINIMUM;
                            continue;
                        }
                        else
                        {
                            // Not good
                            myassert(!m_maxima->isEmpty());
                            m_maxima->pop_back();
                            m_minima->push_back(l_current_time);
                            m_prev_extremum_time = l_current_time;
                            m_prev_extremum_pitch = l_current_pitch;
                            m_prev_extremum = FIRST_MINIMUM;
                            continue;
                        }
                    }
                }
            }
        }
        // Calculate start of next loop
        m_loop_start = m_loop_start + ((l_loop_limit - m_loop_start) / m_loop_step) * m_loop_step + m_loop_step;
    }
}

//------------------------------------------------------------------------------
void NoteData::recalcAvgPitch(void)
{
    m_num_periods = 0.0f;
    for(int l_j = startChunk(); l_j < endChunk(); l_j++)
    {
        m_num_periods += float(m_channel->framesPerChunk()) / float(m_channel->dataAtChunk(l_j)->getPeriod());
    }
    m_avg_pitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}

// EOF
