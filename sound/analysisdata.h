/***************************************************************************
                          analysisdata.h  -  description
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

#ifndef ANALYSIS_DAYA_H
#define ANALYSIS_DAYA_H

//#include "useful.h"
#include <functional>
#include <vector>
#include "Filter.h"
#include "IIR_Filter.h"

#include "conversions.h"
#include <string>

typedef enum class AmplitudeModes
        { AMPLITUDE_RMS = 0
        , AMPLITUDE_MAX_INTENSITY = 1
        , AMPLITUDE_CORRELATION = 2
        , FREQ_CHANGENESS = 3
        , DELTA_FREQ_CENTROID = 4
        , NOTE_SCORE = 5
        , NOTE_CHANGE_SCORE = 6
        } t_amplitude_modes;

class GData;

extern const std::string g_amp_mode_names[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1];
extern const std::string g_amp_display_string[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1];
extern double(* g_amp_mode_func[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1])( const double &
                                               , const GData &
                                               );
extern double(* g_amp_mode_inv_func[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1])( const double &
                                                   , const GData &
                                                   );

class AnalysisData
{
  public:
    AnalysisData();
    void calcScores();

    inline float getValue(t_amplitude_modes p_index)const;
    inline float getPeriod()const;
    inline void setPeriod(float p_period);
    inline float getFundamentalFreq()const;
    inline void setFundamentalFreq(float p_fundamental_freq);
    inline float getPitch()const;
    inline void setPitch(float p_pitch);
    inline float getFreqCentroid()const;
    inline void setFreqCentroid(float p_value);
    inline const double & getPitchSum()const;
    inline void setPitchSum(const double & p_pitch_sum);
    inline const double & getPitch2Sum()const;
    inline void setPitch2Sum(const double & p_pitch_2_sum);
    inline const double & getShortTermMean()const;
    inline void setShortTermMean(const double & p_short_term_mean);
    inline const double & getShortTermDeviation()const;
    inline void setShortTermDeviation(const double & p_short_term_deviation);
    inline const double & getLongTermMean()const;
    inline void setLongTermMean(const double & p_long_term_mean);
    inline const double & getLongTermDeviation()const;
    inline void setLongTermDeviation(const double & p_long_term_deviation);
    inline const double & getSpread()const;
    inline void setSpread(const double & p_spread);
    inline const double & getSpread2()const;
    inline void setSpread2(const double & p_spread2);
    inline float getVibratoPitch()const;
    inline void setVibratoPitch(float p_vibrato_pitch);
    inline float getVibratoWidth()const;
    inline void setVibratoWidth(float p_vibrato_width);
    inline float getVibratoWidthAdjust()const;
    inline void setVibratoWidthAdjust(float p_vibrato_width_adjust);
    inline float getVibratoSpeed()const;
    inline void setVibratoSpeed(float p_vibrato_speed);
    inline float getVibratoPhase()const;
    inline void setVibratoPhase(float p_vibrato_phase);
    inline float getVibratoError()const;
    inline void setVibratoError(float p_vibrato_error);
    inline int getReason()const;
    inline void setReason(int p_reason);
    inline int getHighestCorrelationIndex()const;
    inline void setHighestCorrelationIndex(int p_index);
    inline int getChosenCorrelationIndex()const;
    inline void setChosenCorrelationIndex(int p_index);
    inline float getPeriodRatio()const;
    inline void setPeriodRatio(float p_period_ratio);
    inline int getCepstrumIndex()const;
    inline void setCepstrumIndex(int p_index);
    inline float getCepstrumPitch()const;
    inline void setCepstrumPitch(float p_cepstrum_pitch);

    inline bool isPeriodEstimatesEmpty()const;
    inline size_t getPeriodEstimatesSize()const;
    inline float getPeriodEstimatesAt(size_t p_index)const;
    inline void clearPeriodEstimates();
    inline void addPeriodEstimates(float p_value);
    inline float searchClosestPeriodEstimates(const float & p_value)const;

    inline size_t getPeriodEstimatesAmpSize()const;
    inline float getPeriodEstimatesAmpAt(size_t p_index)const;
    inline void clearPeriodEstimatesAmp();
    inline void addPeriodEstimatesAmp(float p_value);
    inline int getPeriodEstimatesAmpMaxElementIndex()const;

    inline size_t getHarmonicAmpNoCutOffSize()const;
    inline float getHarmonicAmpNoCutOffAt(size_t p_index)const;
    inline void resizeHarmonicAmpNoCutOff(size_t p_size);
    inline void setHarmonicAmpNoCutOffAt( size_t p_index
                                        , float p_value
                                        );

    inline float getHarmonicAmpRelativeAt(size_t p_index)const;
    inline size_t getHarmonicAmpRelativeSize()const;

    inline float getHarmonicAmpAt(size_t p_index)const;
    inline void resizeHarmonicAmp(size_t p_size);
    inline void setHarmonicAmpAt( size_t p_index
                                , float p_value
                                );

    inline float getHarmonicFreqAt(size_t p_index)const;
    inline size_t getHarmonicFreqSize()const;
    inline void resizeHarmonicFreq(size_t p_size);
    inline void setHarmonicFreqAt( size_t p_index
                                 , float p_value
                                 );

    inline float getHarmonicNoiseAt(size_t p_index)const;
    inline void resizeHarmonicNoise(size_t p_size);
    inline void setHarmonicNoiseAt( size_t p_index
                                  , float p_value
                                  );

    inline void setFilterState(const FilterState & p_filter_state);
    inline int getNoteIndex()const;
    inline void setNoteIndex(int p_node_index);

    inline void setNotePlaying(bool p_note_playing);
    inline bool isNotePlaying()const;
    inline void setDone(bool p_done);
    inline bool isDone()const;

    inline float getLogRms()const;
    inline void setLogRms(float p_log_rms);

    inline float getMaxIntensityDB()const;
    inline void setMaxIntensityDB(float p_value);

    inline float getCorrelation()const;
    inline void setCorrelation(float p_value);

    inline void setChangeness(float p_value);

    inline void setDeltaFreqCentroid(float p_value);

    inline float getVolumeValue(const GData & p_data)const;

    inline float getNoteScore()const;

    inline float getNoteChangeScore()const;

    static inline
    int get_no_note();

  private:
    float m_values[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE) + 1];
    float m_period; /*< The period of the fundamental (in samples) */
    float m_fundamental_freq; /*< The fundamental frequency in hertz */
    float m_pitch; /*< The pitch in semi-tones */
    float m_freq_centroid;
    double m_pitch_sum; /*< The sum of pitches so far in a note */
    double m_pitch_squared_sum; /*< The sum of pitches squared so far in a note */
    double m_short_term_mean; /*< The mean note so far */
    double m_short_term_deviation; /*< The deviation of the note so far (based on standard deviation) */
    double m_long_term_mean; /*> The mean note so far */
    double m_long_term_deviation; /*< The deviation of the note so far (based on standard deviation) */
    double m_spread;
    double m_spread_2;
    float m_vibrato_pitch;
    float m_vibrato_width;
    float m_vibrato_width_adjust;
    float m_vibrato_speed;
    float m_vibrato_phase;
    float m_vibrato_error;
    int m_reason; /*< The reason why there was a note change */
    int m_highest_correlation_index;
    int m_chosen_correlation_index;
    float m_period_ratio; /*< The ratio of the current period to the period at the beginning of the current note */
    int m_cepstrum_index;
    float m_cepstrum_pitch;
    std::vector<float> m_period_estimates;
    std::vector<float> m_period_estimates_amp;
    std::vector<float> m_harmonic_amp_no_cut_off;
    std::vector<float> m_harmonic_amp_relative;
    std::vector<float> m_harmonic_amp;
    std::vector<float> m_harmonic_freq;
    std::vector<float> m_harmonic_noise;
    FilterState m_filter_state; //the state of the filter at the beginning of the chunk
    int m_note_index; //The index of the note in the m_note_data, or value of m_no_note
    bool m_note_playing;
    bool m_done;

    static const int m_no_note = -1;
};

struct lessFundametalFreq: public std::binary_function< AnalysisData &
                                                      , AnalysisData &
                                                      , bool
                                                      >
{
  inline bool operator()( const AnalysisData & p_x
                        , const AnalysisData & p_y
                        );
};

struct greaterFundametalFreq: public std::binary_function< AnalysisData &
                                                         , AnalysisData &
                                                         , bool
                                                         >
{
  inline bool operator()( const AnalysisData & p_x
                        , const AnalysisData & p_y
                        );
};

struct lessPitch: public std::binary_function< AnalysisData &
                                             , AnalysisData &
                                             , bool
                                             >
{
  inline bool operator()( const AnalysisData & p_x
                        , const AnalysisData & p_y
                        );
};

struct greaterPitch: public std::binary_function< AnalysisData &
                                                , AnalysisData &
                                                , bool
                                                >
{
  inline bool operator()( const AnalysisData & p_x
                        , const AnalysisData & p_y
                        );
};

class lessValue: public std::binary_function< AnalysisData &
                                            , AnalysisData &
                                            , bool
                                            >
{
  public:

    inline lessValue(t_amplitude_modes p_value);
    inline bool operator()( const AnalysisData & p_x
                          , const AnalysisData & p_y
                          );

  private:

    t_amplitude_modes m_value;

};

class greaterValue: public std::binary_function< AnalysisData &
                                               , AnalysisData &
                                               , bool
                                               >
{
  public:

    inline greaterValue(t_amplitude_modes p_value);
    inline bool operator()( const AnalysisData & p_x
                          , const AnalysisData & p_y
                          );

  private:

    t_amplitude_modes m_value;
};

#include "analysisdata.hpp"

#endif // ANALYSIS_DAYA_H
//EOF
