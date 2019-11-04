/***************************************************************************
                          mytransforms.h  -  description
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

#ifndef MYTRANSFORMS_H
#define MYTRANSFORMS_H

#include <fftw3.h>
#include "analysisdata.h"
#include "fast_smooth.h"

class Channel;
class Filter;

class MyTransforms
{  
  public:

    MyTransforms(void);
    virtual ~MyTransforms(void);

    /**
       init() Initalises the parameters of a class instance. This must be called before use
       @param p_n The size of the data windows to be processed
       @param p_k The number of outputs wanted (autocorr size = n_ + k_). Set k_ = 0, to get default n_/2
       @param p_rate The sampling rate of the incoming signal to process
       @param threshold_ The ratio of highest peak to the first peak allowed to be chosen
    */
    void init( int p_n
             , int p_k
             , double p_rate
             , bool p_equal_loudness = false
             , int p_num_harmonics = 40
             );

    void uninit(void);

    /**
        Performs an autocorrelation on the input
        @param p_input An array of length n, in which the autocorrelation is taken
        @param ouput This should be an array of length k.
        This is the correlation of the signal with itself
        for delays 1 to k (stored in elements 0 to k-1)
        @return The sum of squares of the input. (ie the zero delay correlation)
        Note: Use the init function to set values of n and k before calling this.
    */
    double autocorr( float * p_input
                   , float * p_output
                   );
    double autoLogCorr( float * p_input
                      , float * p_output
                      );

    /**
       The Average Square Difference Function.
       @param input. An array of length n, in which the ASDF is taken
       @param ouput. This should be an array of length k
    */
    double asdf( float * p_input
               , float * p_output
               );

    /**
       The Normalised Square Difference Function.
       @param input. An array of length n, in which the ASDF is taken
       @param ouput. This should be an array of length k
       @return The sum of square
    */
    double nsdf( float * p_input
               , float * p_output
               );

    /**
       Find the highest maxima between each pair of positive zero crossings.
       Including the highest maxima between the last +ve zero crossing and the end if any.
       Ignoring the first (which is at zero)
       In this diagram the disired maxima are marked with a *
     
                    *             *
      \      *     /\      *     /	\
      _\____/\____/__\/\__/\____/__
        \  /  \  /      \/  \  /
         \/    \/            \/

       @param p_input The array to look for maxima in
       @param p_len Then length of the input array
       @param p_max_positions The resulting maxima positions are pushed back to this vector
       @return The index of the overall maximum
    */
    static int findNSDFMaxima( float * p_input
                             , int p_len
                             , std::vector<int> & p_max_positions
                             );

    /**
       @return The index of the first sub maximum.
       This is now scaled from (threshold * overallMax) to 0.
    */
    static int findNSDFsubMaximum( float * p_input
                                 , int p_len
                                 , float p_threshold
                                 );

    /**
       Estimate the period or the fundamental frequency.
       Finds the first maximum of NSDF which past the first
       positive zero crossing and is over the threshold.
       If no maxima are over the threshold then the the highest maximum is returned.
       If no positive zero crossing is found, zero is returned.
       @param input. An array of length n.
       @param threshold. A number between 0 and 1 at which maxima above are acceped.
       @return The estimated period (in samples), or zero if none found.
    */
    void calculateAnalysisData(/*float *input, */
                                int p_chunk
                              , Channel * p_channel
                              /*, float threshold*/
                              );

    /**
       Do a Windowed FFT for use in the Active FFT data window
    */
    void doChannelDataFFT( Channel * p_channel
                         , float * p_current_input
                         , int p_chunk
                         );
    void calcHarmonicAmpPhase( float * p_harmonic_amp
                             , float * p_harmonic_phase
                             , int p_bins_per_harmonic
                             );
    void doHarmonicAnalysis( float * p_input
                           , AnalysisData & p_analysis_data
                           , double p_period
                           );

    /**
       using the given fractional period, constructs a smaller subwindow
       which slide through the window. The smallest clarity measure of this
       moving sub window is returned.
    */
    float get_fine_clarity_measure(double p_period);
    double get_max_note_change( float * p_input
                              , double p_period
                              );
    void applyHanningWindow(float * p_d);

    /**
       @param p_buffer The input buffer, as outputted from the fftw. ie 1st half real, 2nd half imaginary parts
       @param p_len The length of the buffer, including real and imaganary parts
       @return The normalised frequency centriod
    */
    static double calcFreqCentroid( float * p_buffer
                                  , int p_len
                                  );

    /**
       @param p_buffer The input buffer of logarithmic magnitudes
       @param p_len The length of the buffer
       @return The normalised frequency centriod
    */
    static double calcFreqCentroidFromLogMagnitudes( float * p_buffer
                                                   , int p_len
                                                   );

    inline bool getEqualLoudness(void) const;

  private:

    bool m_equal_loudness;

    /**
       m_n = size of data
    */
    int m_n;

    /**
       m_k = size of padding for autocorrelation
     */
    int m_k;

    /**
       size = n+k
    */
    int m_size;

    fftwf_plan m_plan_data_time_2_FFT;
    fftwf_plan m_plan_data_FFT_2_time;
    fftwf_plan m_plan_autocorr_time_2_FFT;
    fftwf_plan m_plan_autocorr_FFT_2_time;
    float * m_data_temp;
    float * m_data_time;
    float * m_data_FFT;
    float * m_autocorr_time;
    float * m_autocorr_FFT;
    float * m_hanning_coeff;
    float m_hanning_scalar;
    float * m_harmonics_amp_left;
    float * m_harmonics_phase_left;
    float * m_harmonics_amp_center;
    float * m_harmonics_phase_center;
    float * m_harmonics_amp_right;
    float * m_harmonics_phase_right;
    bool m_been_init;
    double m_freq_per_bin;
    double m_rate;
    int m_num_harmonics;
    fast_smooth * m_fast_smooth;

};

#include "mytransforms.hpp"
  
#endif // MYTRANSFORMS_H
//EOF
