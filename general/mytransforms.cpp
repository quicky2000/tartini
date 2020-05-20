/***************************************************************************
                          mytransforms.cpp  -  description
                             -------------------
    begin                : Sat Dec 11 2004
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

#include "mytransforms.h"
#include "myassert.h"
#include "array1d.h"
#include "equalloudness.h"
#include "gdata.h"
#include "bspline.h"
#include "channel.h"
#include "Filter.h"
#include "conversions.h"
#include "music_note.h"

#include "useful.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <float.h>

#include <complex>

//------------------------------------------------------------------------------
MyTransforms::MyTransforms()
{
    m_been_init = false;
}
  
//------------------------------------------------------------------------------
MyTransforms::~MyTransforms()
{
    uninit();
}

//------------------------------------------------------------------------------
void MyTransforms::init( int p_n
                       , int p_k
                       , double p_rate
                       , bool p_equal_loudness
                       , int p_num_harmonics
                       )
{
    const int l_my_FFT_mode = FFTW_ESTIMATE;
    uninit();
    if(p_k == 0)
    {
        p_k = (p_n + 1) / 2;
    }
  
    m_n = p_n;
    m_k = p_k;
    m_size = m_n + m_k;
    m_rate = p_rate;
    m_equal_loudness = p_equal_loudness;
    m_num_harmonics = p_num_harmonics;

    m_data_temp = static_cast<float*>(fftwf_malloc(sizeof(float) * m_n));
    m_data_time = static_cast<float*>(fftwf_malloc(sizeof(float) * m_n));
    m_data_FFT  =  static_cast<float*>(fftwf_malloc(sizeof(float) * m_n));
    m_autocorr_time = static_cast<float*>(fftwf_malloc(sizeof(float) * m_size));
    m_autocorr_FFT  = static_cast<float*>(fftwf_malloc(sizeof(float) * m_size));
    m_hanning_coeff  = static_cast<float*>(fftwf_malloc(sizeof(float) * m_n));

    m_plan_autocorr_time_2_FFT = fftwf_plan_r2r_1d(m_size, m_autocorr_time, m_autocorr_FFT, FFTW_R2HC, l_my_FFT_mode);
    m_plan_autocorr_FFT_2_time = fftwf_plan_r2r_1d(m_size, m_autocorr_FFT, m_autocorr_time, FFTW_HC2R, l_my_FFT_mode);

    m_plan_data_time_2_FFT = fftwf_plan_r2r_1d(m_n, m_data_time, m_data_FFT, FFTW_R2HC, l_my_FFT_mode);
    m_plan_data_FFT_2_time = fftwf_plan_r2r_1d(m_n, m_data_FFT, m_data_time, FFTW_HC2R, l_my_FFT_mode); //???

    m_harmonics_amp_left = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));
    m_harmonics_phase_left = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));
    m_harmonics_amp_center = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));
    m_harmonics_phase_center = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));
    m_harmonics_amp_right = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));
    m_harmonics_phase_right = static_cast<float*>(fftwf_malloc(m_num_harmonics * sizeof(float)));

    m_freq_per_bin = m_rate / double(m_size);
    //init m_hanning_coeff. The hanning windowing function
    m_hanning_scalar = 0;
    for(int l_j = 0; l_j < m_n; l_j++)
    {
        m_hanning_coeff[l_j] = (1.0 - cos(double(l_j + 1) / double(m_n + 1) * twoPI)) / 2.0;
        m_hanning_scalar += m_hanning_coeff[l_j];
    }

    //to normalise the FFT coefficients we divide by the sum of the hanning window / 2
    m_hanning_scalar /= 2;

    m_fast_smooth = new fast_smooth(m_n / 8);
#ifdef DEBUG_PRINTF
    printf("m_fast_smooth size = %d\n", m_n / 8);
#endif // DEBUG_PRINTF
    m_been_init = true;
}

//------------------------------------------------------------------------------
void MyTransforms::uninit()
{
    if(m_been_init)
    {
        fftwf_free(m_harmonics_amp_left);
        fftwf_free(m_harmonics_phase_left);
        fftwf_free(m_harmonics_amp_center);
        fftwf_free(m_harmonics_phase_center);
        fftwf_free(m_harmonics_amp_right);
        fftwf_free(m_harmonics_phase_right);
        fftwf_destroy_plan(m_plan_data_FFT_2_time);
        fftwf_destroy_plan(m_plan_data_time_2_FFT);
        fftwf_destroy_plan(m_plan_autocorr_FFT_2_time);
        fftwf_destroy_plan(m_plan_autocorr_time_2_FFT);
        fftwf_free(m_autocorr_FFT);
        fftwf_free(m_autocorr_time);
        fftwf_free(m_data_FFT);
        fftwf_free(m_data_time);
        fftwf_free(m_data_temp);
        delete m_fast_smooth;
        m_been_init = false;
    }
}

//------------------------------------------------------------------------------
double MyTransforms::autocorr( float * p_input
                             , float * p_output
                             )
{
    myassert(m_been_init);
    float l_fsize = float(m_size);
  
    //pack the data into an array which is zero padded by k elements
    std::copy(p_input, p_input + m_n, m_autocorr_time);
    std::fill(m_autocorr_time + m_n, m_autocorr_time + m_size, 0.0f);

    //Do a forward FFT
    fftwf_execute(m_plan_autocorr_time_2_FFT);

    //calculate the (real*real + ima*imag) for each coefficient
    //Note: The numbers are packed in half_complex form (refer fftw)
    //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
    for(int l_j = 1; l_j < m_size / 2; l_j++)
    {
        m_autocorr_FFT[l_j] = sq(m_autocorr_FFT[l_j]) + sq(m_autocorr_FFT[m_size - l_j]);
        m_autocorr_FFT[m_size - l_j] = 0.0f;
    }
    m_autocorr_FFT[0] = sq(m_autocorr_FFT[0]);
    m_autocorr_FFT[m_size / 2] = sq(m_autocorr_FFT[m_size / 2]);

    //Do an inverse FFT
    fftwf_execute(m_plan_autocorr_FFT_2_time);

    //extract the wanted elements out, and normalise
    for(float * l_p1 = p_output, *l_p2 = m_autocorr_time + 1; l_p1 < p_output + m_k;)
    {
        *l_p1++ = *l_p2++ / l_fsize;
    }

    return double(m_autocorr_time[0]) / double(m_size);
}

//------------------------------------------------------------------------------
double MyTransforms::autoLogCorr( float * p_input
                                , float * p_output
                                )
{
    myassert(m_been_init);
    float l_fsize = float(m_size);
  
    //pack the data into an array which is zero padded by k elements
    std::copy(p_input, p_input + m_n, m_autocorr_time);
    std::fill(m_autocorr_time + m_n, m_autocorr_time + m_size, 0.0f);

    //Do a forward FFT
    fftwf_execute(m_plan_autocorr_time_2_FFT);

    //calculate the (real*real + ima*imag) for each coefficient
    //Note: The numbers are packed in half_complex form (refer fftw)
    //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
    for(int l_j = 1; l_j < m_size / 2; l_j++)
    {
        m_autocorr_FFT[l_j] = (sq(m_autocorr_FFT[l_j]) + sq(m_autocorr_FFT[m_size - l_j]));
        m_autocorr_FFT[m_size - l_j] = 0.0f;
    }
    m_autocorr_FFT[0] = (sq(m_autocorr_FFT[0]));
    m_autocorr_FFT[m_size / 2] = (sq(m_autocorr_FFT[m_size / 2]));

    //Do an inverse FFT
    fftwf_execute(m_plan_autocorr_FFT_2_time);

    //extract the wanted elements out, and normalise
    for(float *l_p1=p_output, *l_p2= m_autocorr_time + 1; l_p1 < p_output + m_k;)
    {
        *l_p1++ = *l_p2++ / l_fsize;
    }
    
    return double(m_autocorr_time[0]) / double(m_size);
}

//------------------------------------------------------------------------------
double MyTransforms::asdf( float * p_input
                         , float * p_output
                         )
{
    myassert(m_been_init);

    //Do an autocorrelation and return the sum of squares of the input
    double l_sum_sq = autocorr(p_input, p_output);
    double l_sum_right_sq = l_sum_sq;
    double l_sum_left_sq = l_sum_sq;
    for(int l_j = 0; l_j < m_k; l_j++)
    {
        l_sum_left_sq  -= sq(p_input[m_n - 1 - l_j]);
        l_sum_right_sq -= sq(p_input[l_j]);
        p_output[l_j] = l_sum_left_sq + l_sum_right_sq - 2 * p_output[l_j];
    }
    return l_sum_sq;
}

//------------------------------------------------------------------------------
double MyTransforms::nsdf( float * p_input
                         , float * p_output
                         )
{
    myassert(m_been_init);

    //the sum of squares of the input
    double l_sum_sq = autocorr(p_input, p_output);

    double l_total_sum_sq = l_sum_sq * 2.0;
    if(GData::getUniqueInstance().analysisType() == AnalysisModes::MPM || GData::getUniqueInstance().analysisType() == AnalysisModes::MPM_MODIFIED_CEPSTRUM)
    {
        //nsdf
        for(int l_j = 0; l_j < m_k; l_j++)
        {
            l_total_sum_sq  -= sq(p_input[m_n - 1 - l_j]) + sq(p_input[l_j]);
            //dividing by zero is very slow, so deal with it seperately
            if(l_total_sum_sq > 0.0)
            {
                p_output[l_j] *= 2.0 / l_total_sum_sq;
            }
            else
            {
                p_output[l_j] = 0.0;
            }
        }
    }
    else
    {
        //autocorr
        for(int l_j = 0; l_j < m_k; l_j++)
        {
            //dividing by zero is very slow, so deal with it seperately
            if(l_total_sum_sq > 0.0)
            {
                p_output[l_j] /= l_sum_sq;
            }
            else
            {
                p_output[l_j] = 0.0;
            }
        }
    }
    return l_sum_sq;
}

//------------------------------------------------------------------------------
int MyTransforms::findNSDFMaxima( float * p_input
                                , int p_len
                                , std::vector<int> & p_max_positions
                                )
{
    int l_pos = 0;
    int l_cur_max_pos = 0;
    int l_overall_max_index = 0;

    //find the first negitive zero crossing
    while(l_pos < (p_len - 1) / 3 && p_input[l_pos] > 0.0f)
    {
        l_pos++;
    }

    //loop over all the values below zero
    while(l_pos < p_len - 1 && p_input[l_pos] <= 0.0f)
    {
        l_pos++;
    }

    // can happen if output[0] is NAN
    if(l_pos == 0)
    {
        l_pos = 1;
    }

    while(l_pos < p_len - 1)
    {
        //don't assert on NAN
        myassert(!(p_input[l_pos] < 0));
        if(p_input[l_pos] > p_input[l_pos - 1] && p_input[l_pos] >= p_input[l_pos + 1])
        {
            //a local maxima
            if(l_cur_max_pos == 0)
            {
                //the first maxima (between zero crossings)
                l_cur_max_pos = l_pos;
            }
            else if(p_input[l_pos] > p_input[l_cur_max_pos])
            {
                //a higher maxima (between the zero crossings)
                l_cur_max_pos = l_pos;
            }
        }
        l_pos++;
        if(l_pos < p_len - 1 && p_input[l_pos] <= 0.0f)
        {
            //a negative zero crossing
            if(l_cur_max_pos > 0)
            {
                //if there was a maximum
                //add it to the vector of maxima
                p_max_positions.push_back(l_cur_max_pos);
                if(l_overall_max_index == 0)
                {
                    l_overall_max_index = l_cur_max_pos;
                }
                else if(p_input[l_cur_max_pos] > p_input[l_overall_max_index])
                {
                    l_overall_max_index = l_cur_max_pos;
                }
                l_cur_max_pos = 0; //clear the maximum position, so we start looking for a new ones
            }
            //loop over all the values below zero
            while(l_pos < p_len - 1 && p_input[l_pos] <= 0.0f)
            {
                l_pos++;
            }
        }
    }

    if(l_cur_max_pos > 0)
    {
        //if there was a maximum in the last part
        //add it to the vector of maxima
        p_max_positions.push_back(l_cur_max_pos);
        if(l_overall_max_index == 0)
        {
            l_overall_max_index = l_cur_max_pos;
        }
        else if(p_input[l_cur_max_pos] > p_input[l_overall_max_index])
        {
            l_overall_max_index = l_cur_max_pos;
        }
        // clear the maximum position, so we start looking for a new ones
        l_cur_max_pos = 0;
    }
    return l_overall_max_index;
}

//------------------------------------------------------------------------------
int MyTransforms::findNSDFsubMaximum( float * p_input
                                    , int p_len
                                    , float p_threshold
                                    )
{
    std::vector<int> l_indices;
    int l_overall_max_index = findNSDFMaxima(p_input, p_len, l_indices);
    p_threshold += (1.0 - p_threshold) * (1.0 - p_input[l_overall_max_index]);
    float l_cutoff = p_input[l_overall_max_index] * p_threshold;
    for(unsigned int l_j = 0; l_j < l_indices.size(); l_j++)
    {
        if(p_input[l_indices[l_j]] >= l_cutoff)
        {
            return l_indices[l_j];
        }
    }
    //should never get here
    myassert(0);

    //stop the compiler warning
    return 0;
}

//------------------------------------------------------------------------------
void MyTransforms::calculateAnalysisData(/*float *input, */
                                          int p_chunk
                                        , Channel * p_channel
                                        /*, float threshold*/
                                        )
{
    myassert(p_channel);
    myassert(p_channel->dataAtChunk(p_chunk));
    AnalysisData & l_analysis_data = *p_channel->dataAtChunk(p_chunk);
    AnalysisData * l_prev_analysis_data = p_channel->dataAtChunk(p_chunk - 1);
    float * l_output = p_channel->get_nsdf_data().begin();
    float * l_current_input = (m_equal_loudness) ? p_channel->get_filtered_input().begin() : p_channel->get_direct_input().begin();

    std::vector<int> l_nsdf_max_positions;
    l_analysis_data.setMaxIntensityDB(linear2dB(fabs(*std::max_element(l_current_input, l_current_input + m_n, absoluteLess<float>())), GData::getUniqueInstance()));
  
    doChannelDataFFT(p_channel, l_current_input, p_chunk);
    std::copy(l_current_input, l_current_input + m_n, m_data_time);
  
    if(GData::getUniqueInstance().doingFreqAnalysis() && (p_channel->firstTimeThrough() || GData::getUniqueInstance().doingActiveAnalysis()))
    {
        //calculate the Normalised Square Difference Function
        double l_log_rms = linear2dB(nsdf(m_data_time, p_channel->get_nsdf_data().begin()) / double(m_n), GData::getUniqueInstance()); // Do the NSDF calculation
        l_analysis_data.setLogRms(l_log_rms);
        if(GData::getUniqueInstance().doingAutoNoiseFloor() && !l_analysis_data.isDone())
        {
            //do it for g_data. this is only here for old code. remove some stage
            if(p_chunk == 0)
            {
                GData::getUniqueInstance().set_rms_floor(0.0);
                GData::getUniqueInstance().set_rms_ceiling(GData::getUniqueInstance().dBFloor());
            }
            if(l_log_rms + 15 < GData::getUniqueInstance().rmsFloor())
            {
                GData::getUniqueInstance().set_rms_floor(l_log_rms + 15);
            }
            if(l_log_rms > GData::getUniqueInstance().rmsCeiling())
            {
                GData::getUniqueInstance().set_rms_ceiling(l_log_rms);
            }

            //do it for the channel
            if(p_chunk == 0)
            {

                p_channel->set_rms_floor(0.0);
                p_channel->set_rms_ceiling(GData::getUniqueInstance().dBFloor());
            }
            if(l_log_rms + 15 < p_channel->get_rms_floor())
            {
                p_channel->set_rms_floor(l_log_rms + 15);
            }
            if(l_log_rms > p_channel->get_rms_ceiling())
            {
                p_channel->set_rms_ceiling(l_log_rms);
            }
        }

        l_analysis_data.setFreqCentroid(calcFreqCentroidFromLogMagnitudes(p_channel->get_fft_data1().begin(), p_channel->get_fft_data1().size()));
        if(l_prev_analysis_data)
        {
            l_analysis_data.setDeltaFreqCentroid(bound(fabs(l_analysis_data.getFreqCentroid() - l_prev_analysis_data->getFreqCentroid()) * 20.0, 0.0, 1.0));
        }
        else
        {
            l_analysis_data.setDeltaFreqCentroid(0.0);
        }
    
        findNSDFMaxima(p_channel->get_nsdf_data().begin(), m_k, l_nsdf_max_positions);
        if(!l_analysis_data.isDone())
        {
        }

        //store some of the best period estimates
        l_analysis_data.clearPeriodEstimates();
        l_analysis_data.clearPeriodEstimatesAmp();
        float l_small_cutoff = 0.4f;
        for(std::vector<int>::iterator l_iter = l_nsdf_max_positions.begin(); l_iter < l_nsdf_max_positions.end(); l_iter++)
        {
            if(l_output[*l_iter] >= l_small_cutoff)
            {
                float l_x, l_y;
                //do a parabola fit to find the maximum
                parabolaTurningPoint2(l_output[*l_iter - 1], l_output[*l_iter], l_output[*l_iter + 1], float(*l_iter + 1), &l_x, &l_y);
                l_y = bound(l_y, -1.0f, 1.0f);
                l_analysis_data.addPeriodEstimates(l_x);
                l_analysis_data.addPeriodEstimatesAmp(l_y);
            }
        }

        float l_period_diff = 0.0f;
        if(l_analysis_data.isPeriodEstimatesEmpty())
        {
            //no period found
            l_analysis_data.calcScores();
            l_analysis_data.setDone(true);
        }
        else
        {
            //calc the l_period_diff
            if(p_chunk > 0)
            {
                myassert(l_prev_analysis_data);
                float l_prev_period = l_prev_analysis_data->getHighestCorrelationIndex() != -1 ? l_prev_analysis_data->getPeriodEstimatesAt(l_prev_analysis_data->getHighestCorrelationIndex()) : 0;
                l_period_diff = l_analysis_data.searchClosestPeriodEstimates(l_prev_period) - l_prev_period;
                if(absolute(l_period_diff) > 8.0f)
                {
                    l_period_diff = 0.0f;
                }
            }

            int l_nsdf_max_index = l_analysis_data.getPeriodEstimatesAmpMaxElementIndex();
            l_analysis_data.setHighestCorrelationIndex(l_nsdf_max_index);

            if(!l_analysis_data.isDone())
            {
                if(GData::getUniqueInstance().analysisType() == AnalysisModes::MPM_MODIFIED_CEPSTRUM)
                {
                    //calculate pitch
                    p_channel->chooseCorrelationIndex(p_chunk, float(l_analysis_data.getCepstrumIndex()));
                }
                else
                {
                    if(p_channel->isNotePlaying() && p_chunk > 0)
                    {
                        //calculate pitch
                        p_channel->chooseCorrelationIndex(p_chunk, p_channel->periodOctaveEstimate(p_chunk - 1));
                    }
                    else
                    {
                        //calculate pitch
                        p_channel->chooseCorrelationIndex1(p_chunk);
                    }
                }
                p_channel->calcDeviation(p_chunk);

                //calculate m_vibrato_pitch, m_vibrato_width, m_vibrato_speed
                p_channel->doPronyFit(p_chunk);
            }

            l_analysis_data.setChangeness(0.0f);

            if(GData::getUniqueInstance().doingHarmonicAnalysis())
            {
                std::copy(m_data_time, m_data_time + m_n, m_data_temp);
                if(l_analysis_data.getChosenCorrelationIndex() >= 0)
                {
                    doHarmonicAnalysis(m_data_temp, l_analysis_data, l_analysis_data.getPeriodEstimatesAt(l_analysis_data.getChosenCorrelationIndex())/*period*/);
                }
            }
        }

        if(GData::getUniqueInstance().doingFreqAnalysis() && p_channel->doingDetailedPitch() && p_channel->firstTimeThrough())
        {
            float l_period_diff_2 = p_channel->calcDetailedPitch(l_current_input, l_analysis_data.getPeriod(), p_chunk);
            l_period_diff = l_period_diff_2;

            p_channel->get_pitch_lookup().push_back(p_channel->get_detailed_pitch_data().begin(), p_channel->get_detailed_pitch_data().size());
            p_channel->get_pitch_lookup_smoothed().push_back(p_channel->get_detailed_pitch_data_smoothed().begin(), p_channel->get_detailed_pitch_data_smoothed().size());
        }

        if(!l_analysis_data.isDone())
        {
            l_analysis_data.calcScores();
            p_channel->processNoteDecisions(p_chunk, l_period_diff);
            l_analysis_data.setDone(true);
        }

        if(GData::getUniqueInstance().doingFreqAnalysis() && p_channel->doingDetailedPitch() && p_channel->firstTimeThrough())
        {
            p_channel->calcVibratoData(p_chunk);
        }
    }

    if(GData::getUniqueInstance().doingFreqAnalysis() && p_channel->doingDetailedPitch() && (!p_channel->firstTimeThrough()))
    {
        p_channel->get_pitch_lookup().copyTo(p_channel->get_detailed_pitch_data().begin(), p_chunk * p_channel->get_detailed_pitch_data().size(), p_channel->get_detailed_pitch_data().size());
        p_channel->get_pitch_lookup_smoothed().copyTo(p_channel->get_detailed_pitch_data_smoothed().begin(), p_chunk * p_channel->get_detailed_pitch_data_smoothed().size(), p_channel->get_detailed_pitch_data_smoothed().size());
    }

    if(!l_analysis_data.isDone())
    {
        int l_j;
        //calc l_rms by hand
        double l_rms = 0.0;
        for(l_j = 0; l_j < m_n; l_j++)
        {
            l_rms += sq(m_data_time[l_j]);
        }
        l_analysis_data.setLogRms(linear2dB(l_rms / float(m_n), GData::getUniqueInstance()));
        l_analysis_data.calcScores();
        l_analysis_data.setDone(true);
    }
}

//------------------------------------------------------------------------------
float MyTransforms::get_fine_clarity_measure(double p_period)
{
    int l_temp_N = m_n - int(ceil(p_period));
    float * l_temp_data = new float[l_temp_N];
    float l_big_sum = 0;
    float l_corr_sum = 0;
    float l_match_val;
    float l_match_min = 1.0;
    //create some data points at the fractional period delay into l_temp_data
    stretch_array(m_n, m_data_time, l_temp_N, l_temp_data, p_period, l_temp_N, t_spline_type::LINEAR);
    //l_temp_N / 4;
    int l_dN = int(floor(p_period));
    for(int l_j = 0; l_j < l_dN; l_j++)
    {
        l_big_sum += sq(m_data_time[l_j]) + sq(l_temp_data[l_j]);
        l_corr_sum += m_data_time[l_j] * l_temp_data[l_j];
    }
    l_match_min = (2.0 * l_corr_sum) / l_big_sum;
#ifdef DEBUG_PRINTF
    printf("%f, ", l_match_min);
#endif // DEBUG_PRINTF
    for(int l_j = 0; l_j < l_temp_N - l_dN; l_j++)
    {
        l_big_sum -= sq(m_data_time[l_j]) + sq(l_temp_data[l_j]);
        l_corr_sum -= m_data_time[l_j] * l_temp_data[l_j];
        l_big_sum += sq(m_data_time[l_j + l_dN]) + sq(l_temp_data[l_j + l_dN]);
        l_corr_sum += m_data_time[l_j + l_dN] * l_temp_data[l_j + l_dN];
        l_match_val = (2.0 * l_corr_sum) / l_big_sum;
        if(l_match_val < l_match_min)
        {
            l_match_min = l_match_val;
        }
    }
#ifdef DEBUG_PRINTF
    printf("%f\n", l_match_min);
#endif // DEBUG_PRINTF
    delete[] l_temp_data;

    return l_match_min;
}

//------------------------------------------------------------------------------
double MyTransforms::get_max_note_change( float * p_input
                                        , double p_period
                                        )
{
    //TODO
    /*matlab code
      l = length(s);
      m = floor(l / 4); % m is the maximum size sub-window to use
      % w is the sub-window size
      if p < m
      w = p * floor(m / p);
      else
      w = p;
      end

      n = -4:4;
      ln = length(n);
      left = cell(1, ln);
      right = cell(1, ln);
      left_pow = zeros(1, ln);
      right_pow = zeros(1, ln);
      pow = zeros(1, ln);
      err = zeros(1, ln);

      for i = 1:ln
      left{i} = s(1:(w-n(i))); % a smaller delay period has a larger window size, to ensure only the same data is used
      right{i} = s(1+p+n(i):w+p);
      left_pow(i) = sum(left{i}.^2);
      right_pow(i) = sum(right{i}.^2);
      err(i) = sum((left{i} - right{i}) .^ 2);
      end
    */
    int l_i;
    int l_j;
    int l_j2;
    int l_max_subwindow = m_n / 4;
    int l_subwindow_size;
    if(p_period < 1.0)
    {
        //period too small
        return 0.0;
    }
    if(p_period > m_n / 2)
    {
        printf("period = %lf\n", p_period);
        return 0.0;
    }
    int l_i_period = int(floor(p_period));
    if(p_period < double(l_max_subwindow))
    {
        l_subwindow_size = int(floor(p_period * (double(l_max_subwindow) / p_period)));
    }
    else
    {
        l_subwindow_size = int(floor(p_period));
    }
    int l_num = m_n - l_subwindow_size - l_i_period;

    std::vector<int> l_offsets;
    for(l_j = -4; l_j <= 4; l_j++)
    {
        //do a total of 9 subwindows at once. 4 either side.
        l_offsets.push_back(l_j);
    }
    int l_ln = l_offsets.size();

    std::vector<float> l_left(l_ln);
    std::vector<float> l_right(l_ln);
    std::vector<float> l_left_pow(l_ln);
    std::vector<float> l_right_pow(l_ln);
    std::vector<float> l_pow(l_ln);
    std::vector<float> l_err(l_ln);
    std::vector<float> l_result(l_ln);
    std::vector<float> l_unsmoothed(l_num);
    std::vector<float> l_smoothed(l_num);
    std::vector<float> l_smoothed_diff(l_num);
    //calc the values of pow and err for the first in each row.
    for(l_i = 0; l_i < l_ln; l_i++)
    {
        l_left_pow[l_i] = l_right_pow[l_i] = l_pow[l_i] = l_err[l_i] = 0;
        for(l_j = 0, l_j2 = l_i_period + l_offsets[l_i]; l_j < l_subwindow_size - l_offsets[l_i]; l_j++, l_j2++)
        {
            l_left_pow[l_i] += sq(p_input[l_j]);
            l_right_pow[l_i] += sq(p_input[l_j2]);
            l_err[l_i] += sq(p_input[l_j] - p_input[l_j2]);
        }
    }
#ifdef DEBUG_PRINTF
    printf("l_subwindow_size=%d, l_num=%d, p_period=%lf\n", l_subwindow_size, l_num, p_period);
#endif // DEBUG_PRINTF
    /*matlab code
      for j = 1:(num-1)
        for i = 1:ln
          pow(i) = (left_pow(i) + right_pow(i));
          resulte(i, j) = err(i);
          resultp(i, j) = pow(i);
          result(i, j) = 1 - (err(i) / pow(i));

          %err = err - (s(j) - s(j+p)).^2 + (s(j+w) - s(j+w+p)).^2;
          err(i) = err(i) - ((s(j) - s(j+p+n(i))).^2) + (s(j+w-n(i)) - s(j+w+p)).^2;
          left_pow(i) = left_pow(i) - s(j).^2 + s(j+w-n(i)).^2;
          right_pow(i) = right_pow(i) - s(j+p+n(i)).^2 + s(j+p+w).^2;
        end
      end

      for i = 1:ln
        pow(i) = (left_pow(i) + right_pow(i));
        result(i, num) = 1 - (err(i) / pow(i));
      end
    */
    //TODO: speed up this for loop
    for(l_j = 0; l_j < l_num - 1; l_j++)
    {
        for(l_i = 0; l_i < l_ln; l_i++)
        {
            l_pow[l_i] = l_left_pow[l_i] + l_right_pow[l_i];
            l_result[l_i] = 1.0 - (l_err[l_i] / l_pow[l_i]);

            l_err[l_i] += -sq(p_input[l_j] - p_input[l_j + l_i_period + l_offsets[l_i]]) + sq(p_input[l_j + l_subwindow_size - l_offsets[l_i]] - p_input[l_j + l_subwindow_size + l_i_period]);
            l_left_pow[l_i] += -sq(p_input[l_j]) + sq(p_input[l_j + l_subwindow_size - l_offsets[l_i]]);
            l_right_pow[l_i] += -sq(p_input[l_j + l_i_period + l_offsets[l_i]]) + sq(p_input[l_j + l_i_period + l_subwindow_size]);
        }
        /*matlab code
          for j = 1:num
	      [dud pos] = max(result(:,j));
	      if pos > 1 && pos < ln
                [period(j) dummy] = parabolaPeak(result(pos-1, j), result(pos, j), result(pos+1, j), p+n(pos));
	      else
                period(j) = p+n(pos);
	      end
	      period_int(j) = p+n(pos);
	      end
         */
        int l_pos = int(std::max_element(l_result.begin(), l_result.begin() + l_ln) - l_result.begin());
        if(l_pos > 0 && l_pos < l_ln-1)
        {
            l_unsmoothed[l_j] = double(l_i_period + l_offsets[l_pos]) + parabolaTurningPoint(l_result[l_pos - 1], l_result[l_pos], l_result[l_pos + 1]);
        }
        else
        {
            l_unsmoothed[l_j] = double(l_i_period + l_offsets[l_pos]);
        }
    }
    m_fast_smooth->fast_smoothB(&(l_unsmoothed[0]), &(l_smoothed[0]), l_num - 1);
    int l_max_pos = 0;
    for(l_j = 0; l_j < l_num - 2; l_j++)
    {
        l_smoothed_diff[l_j] = fabs(l_smoothed[l_j + 1] - l_smoothed[l_j]);
#ifdef DEBUG_PRINTF
        printf("%f ", l_smoothed[l_j]);
#endif // DEBUG_PRINTF
        if(l_smoothed_diff[l_j] > l_smoothed_diff[l_max_pos])
        {
            l_max_pos = l_j;
        }
    }
#ifdef DEBUG_PRINTF
    printf("\l_smoothed_diff=%f\n", l_smoothed_diff[l_max_pos]);
#endif // DEBUG_PRINTF
    double l_ret = l_smoothed_diff[l_max_pos] / p_period * double(m_rate) * double(l_subwindow_size) / 10000.0;
    return l_ret;
}

/**
   Find the index of the first maxima above the threshold times the overall maximum.
   @param p_threshold A value between 0.0 and 1.0
   @return The index of the first subMaxima
*/
int findFirstSubMaximum( float * p_data
                       , int p_length
                       , float p_threshold
                       )
{
    float l_max_value = *std::max_element(p_data, p_data + p_length);
    float l_cutoff_value = l_max_value * p_threshold;
    for(int l_j = 0; l_j < p_length; l_j++)
    {
        if(p_data[l_j] >= l_cutoff_value)
        {
            return l_j;
        }
    }
    myassert(0); //shouldn't get here.
    return p_length;
}

/**
   Given cepstrum input finds the first maxima above the threshold
   @param p_threshold The fraction of the 'overall maximum value' the 'cepstrum maximum' must be over.
   A value between 0.0 and 1.0.
*/
int findCepstrumMaximum( float * p_data
                       , int p_length
                       , float p_threshold
                       )
{
    int l_pos = 0;
    //loop until the first negative value in p_data
    while(l_pos < p_length - 1 && p_data[l_pos] > 0.0f)
    {
        l_pos++;
    }
    return l_pos + findFirstSubMaximum(p_data + l_pos, p_length - l_pos, p_threshold);
}

//------------------------------------------------------------------------------
void MyTransforms::doChannelDataFFT( Channel * p_channel
                                   , float * p_current_input
                                   , int p_chunk
                                   )
{
    std::copy(p_current_input, p_current_input + m_n, m_data_time);
    applyHanningWindow(m_data_time);
    fftwf_execute(m_plan_data_time_2_FFT);
    int l_n_div_2 = m_n / 2;
    //LOG RULES: log(sqrt(x)) = log(x) / 2.0
    //LOG RULES: log(a * b) = log(a) + log(b)
    myassert(p_channel->get_fft_data1().size() == l_n_div_2);
    double l_log_size = log10(double(p_channel->get_fft_data1().size())); //0.0
    //Adjust the coefficents, both real and imaginary part by same amount
    double l_sq_value;
    const double l_log_base = 100.0;
    for(int l_j = 1; l_j < l_n_div_2; l_j++)
    {
        l_sq_value = sq(m_data_FFT[l_j]) + sq(m_data_FFT[m_n - l_j]);
        p_channel->get_fft_data2()[l_j] = logBaseN(l_log_base, 1.0 + 2.0 * sqrt(l_sq_value) / double(l_n_div_2) * (l_log_base - 1.0));
        if(l_sq_value > 0.0)
        {
            p_channel->get_fft_data1()[l_j] = bound(log10(l_sq_value) / 2.0 - l_log_size, GData::getUniqueInstance().dBFloor(), 0.0);
        }
        else
        {
            p_channel->get_fft_data1()[l_j] = GData::getUniqueInstance().dBFloor();
        }
    }
    l_sq_value = sq(m_data_FFT[0]) + sq(m_data_FFT[l_n_div_2]);
    p_channel->get_fft_data2()[0] = logBaseN(l_log_base, 1.0 + 2.0 * sqrt(l_sq_value) / double(l_n_div_2) * (l_log_base - 1.0));
    if(l_sq_value > 0.0)
    {
        p_channel->get_fft_data1()[0] = bound(log10(l_sq_value) / 2.0 - l_log_size, GData::getUniqueInstance().dBFloor(), 0.0);
    }
    else
    {
        p_channel->get_fft_data1()[0] = GData::getUniqueInstance().dBFloor();
    }

#ifdef DEBUG_PRINTF
    printf("n = %d, fff = %f\n", l_n_div_2, *std::max_element(p_channel->get_fft_data2().begin(), p_channel->get_fft_data2().end()));
#endif // DEBUG_PRINTF

    if(GData::getUniqueInstance().analysisType() == AnalysisModes::MPM_MODIFIED_CEPSTRUM)
    {
        for(int l_j = 1; l_j < l_n_div_2; l_j++)
        {
            m_data_FFT[l_j] = p_channel->get_fft_data2()[l_j];
            m_data_FFT[m_n - l_j] = 0.0;
        }
        m_data_FFT[0] = p_channel->get_fft_data2()[0];
        m_data_FFT[l_n_div_2] = 0.0;
        fftwf_execute(m_plan_data_FFT_2_time);

        for(int l_j = 1; l_j < m_n; l_j++)
        {
            m_data_time[l_j] /= m_data_time[0];
        }
        m_data_time[0] = 1.0;
        for(int l_j = 0; l_j < l_n_div_2; l_j++)
        {
            p_channel->get_cepstrum_data()[l_j] = m_data_time[l_j];
        }
        AnalysisData &l_analysis_data = *p_channel->dataAtChunk(p_chunk);
        l_analysis_data.setCepstrumIndex(findNSDFsubMaximum(m_data_time, l_n_div_2, 0.6f));
        l_analysis_data.setCepstrumPitch(MusicNote::freq2pitch(double(l_analysis_data.getCepstrumIndex()) / p_channel->rate()));
    }
}

//------------------------------------------------------------------------------
void MyTransforms::calcHarmonicAmpPhase( float * p_harmonic_amp
                                       , float * p_harmonic_phase
                                       , int p_bins_per_harmonic
                                       )
{
    int l_harmonic;
    for(int l_j = 0; l_j < m_num_harmonics; l_j++)
    {
        l_harmonic = (l_j + 1) * p_bins_per_harmonic;
        if(l_harmonic < m_n)
        {
            p_harmonic_amp[l_j] = sqrt(sq(m_data_FFT[l_harmonic]) + sq(m_data_FFT[m_n - l_harmonic]));
            p_harmonic_phase[l_j] = atan2f(m_data_FFT[m_n - l_harmonic], m_data_FFT[l_harmonic]);
        }
        else
        {
            p_harmonic_amp[l_j] = 0.0;
            p_harmonic_phase[l_j] = 0.0;
        }
    }
}

//------------------------------------------------------------------------------
void MyTransforms::doHarmonicAnalysis( float * p_input
                                     , AnalysisData & p_analysis_data
                                     , double p_period
                                     )
{
    double l_num_periods_fit = floor(double(m_n) / p_period); //2.0
    double l_num_periods_use = l_num_periods_fit - 1.0;
    int l_int_num_periods_use = int(l_num_periods_use);
    double l_center_X = float(m_n) / 2.0;
#ifdef DEBUG_PRINTF
    printf("l_int_num_periods_use = %d\n", l_int_num_periods_use);
#endif // DEBUG_PRINTF
    //do left
    double l_start = l_center_X - (l_num_periods_fit / 2.0) * p_period;
    double l_length = (l_num_periods_use) * p_period;
    stretch_array(m_n, p_input, m_n, m_data_time, l_start, l_length, t_spline_type::LINEAR);
    applyHanningWindow(m_data_time);
    fftwf_execute(m_plan_data_time_2_FFT);
    calcHarmonicAmpPhase(m_harmonics_amp_left, m_harmonics_phase_left, l_int_num_periods_use);
#ifdef DEBUG_PRINTF
    printf("left\n");
    for(int jj = 0; jj < 6; jj++)
    {
        printf("[%d %lf %lf]", jj, m_harmonics_amp_left[jj], m_harmonics_phase_left[jj]);
    }
    printf("\n");
#endif // DEBUG_PRINTF
  
    //do center
    l_start += p_period / 2.0;
    stretch_array(m_n, p_input, m_n, m_data_time, l_start, l_length, t_spline_type::LINEAR);
    applyHanningWindow(m_data_time);
    fftwf_execute(m_plan_data_time_2_FFT);
    calcHarmonicAmpPhase(m_harmonics_amp_center, m_harmonics_phase_center, l_int_num_periods_use);
#ifdef DEBUG_PRINTF
    printf("centre\n");
    for(int jj = 0; jj < 6; jj++)
    {
        printf("[%d %lf %lf]", jj, m_harmonics_amp_center[jj], m_harmonics_phase_center[jj]);
    }
    printf("\n");
#endif // DEBUG_PRINTF
  
    //do right
    l_start += p_period / 2.0;
    stretch_array(m_n, p_input, m_n, m_data_time, l_start, l_length, t_spline_type::LINEAR);
    applyHanningWindow(m_data_time);
    fftwf_execute(m_plan_data_time_2_FFT);
    calcHarmonicAmpPhase(m_harmonics_amp_right, m_harmonics_phase_right, l_int_num_periods_use);
#ifdef DEBUG_PRINTF
    printf("right\n");
    for(int jj = 0; jj < 6; jj++)
    {
        printf("[%d %lf %lf]", jj, m_harmonics_amp_right[jj], m_harmonics_phase_right[jj]);
    }
    printf("\n");
#endif // DEBUG_PRINTF
  
    double l_freq = m_rate / p_period;
  
    p_analysis_data.resizeHarmonicAmpNoCutOff(m_num_harmonics);
    p_analysis_data.resizeHarmonicAmp(m_num_harmonics);
    p_analysis_data.resizeHarmonicFreq(m_num_harmonics);
    p_analysis_data.resizeHarmonicNoise(m_num_harmonics);

    for(int l_j = 0; l_j < m_num_harmonics; l_j++)
    {
        p_analysis_data.setHarmonicAmpAt(l_j, log10(m_harmonics_amp_center[l_j] / m_hanning_scalar) * 20);
        p_analysis_data.setHarmonicAmpNoCutOffAt(l_j, p_analysis_data.getHarmonicAmpAt(l_j));
        p_analysis_data.setHarmonicAmpAt(l_j, 1.0 - (p_analysis_data.getHarmonicAmpAt(l_j) / GData::getUniqueInstance().ampThreshold(t_amplitude_modes::AMPLITUDE_RMS, 0)));
        if(p_analysis_data.getHarmonicAmpAt(l_j) < 0.0)
        {
            p_analysis_data.setHarmonicAmpAt(l_j, 0.0);
        }
        //should be 1 whole period between left and right. i.e. the same freq give 0 phase difference
        double l_diff_angle = (m_harmonics_phase_right[l_j] - m_harmonics_phase_left[l_j]) / twoPI;
        l_diff_angle = cycle(l_diff_angle + 0.5, 1.0) - 0.5;
        double l_diff_angle_2 = (m_harmonics_phase_center[l_j] - m_harmonics_phase_left[l_j]) / twoPI;
        //if an odd harmonic the phase will be 180 degrees out. So correct for this
        if(l_j % 2 == 0)
        {
            l_diff_angle_2 += 0.5;
        }
        l_diff_angle_2 = cycle(l_diff_angle_2 + 0.5, 1.0) - 0.5;
        p_analysis_data.setHarmonicNoiseAt(l_j, fabs(l_diff_angle_2 - l_diff_angle));
        p_analysis_data.setHarmonicFreqAt(l_j, float(l_freq * double(l_j + 1)) + (l_freq * l_diff_angle));
    }
}

//------------------------------------------------------------------------------
void MyTransforms::applyHanningWindow(float * p_d)
{
    for(int l_j = 0; l_j < m_n; l_j++)
    {
        p_d[l_j] *= m_hanning_coeff[l_j];
    }
}

//------------------------------------------------------------------------------
double MyTransforms::calcFreqCentroid( float * p_buffer
                                     , int p_len
                                     )
{
    double l_centroid = 0.0;
    double l_total_weight = 0.0;
    double l_abs_value;
    for(int l_j = 1; l_j < p_len / 2; l_j++)
    {
        //ignore the end freq bins, ie l_j=0
        //calculate l_centroid
        l_abs_value = sqrt(sq(p_buffer[l_j]) + sq(p_buffer[p_len - l_j]));
        l_centroid += double(l_j) * l_abs_value;
        l_total_weight += l_abs_value;
    }
    l_centroid /= l_total_weight * double(p_len / 2);
    return l_centroid;
}

//------------------------------------------------------------------------------
double MyTransforms::calcFreqCentroidFromLogMagnitudes( float * p_buffer
                                                      , int p_len
                                                      )
{
    double l_centroid = 0.0;
    double l_total_weight = 0.0;
    for(int l_j = 1; l_j < p_len; l_j++)
    {
        //ignore the end freq bins, ie l_j=0
        //calculate l_centroid
        l_centroid += double(l_j) * p_buffer[l_j];
        l_total_weight += p_buffer[l_j];
    }
    return l_centroid;
}
//EOF
