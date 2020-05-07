/***************************************************************************
                          notedata.h  -  description
                             -------------------
    begin                : Mon May 23 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/

#ifndef NOTEDATA_H
#define NOTEDATA_H

#include "analysisdata.h"
#include <memory>
#include "array1d.h"
#include <vector>


class Channel;

/**
   @author Philip McLeod
*/
class NoteData
{
  public:
    inline NoteData();
    NoteData(const Channel & p_channel);
    NoteData( const Channel  & p_channel
            , int p_start_chunk
            , const AnalysisData & p_analysis_data
            );
    ~NoteData();


    void resetData();
    inline bool isValid() const;
    inline void setStartChunk(int p_start_chunk);
    inline void setEndChunk(int p_end_chunk);
    inline int startChunk() const;
    inline int endChunk() const;
    void addData( const AnalysisData & p_analysis_data
                , float p_periods
                );
    inline int numChunks() const;

    /**
       @return The length of the note (in seconds)

    */
    double noteLength() const;

    inline float numPeriods() const;

    /**
       @return in Hertz
    */
    inline double avgFreq() const;

    /**
       @return The average of this note, in fractions of semi-tones.

    */
    float avgPitch() const;
    inline void setPeriodOctaveEstimate(float p_period_octave_estimate);
    inline float periodOctaveEstimate() const;
    void addVibratoData(int p_chunk);
    inline float volume() const;
    void recalcAvgPitch();

    inline const std::shared_ptr<Array1d<int> > get_maxima() const;
    inline const std::shared_ptr<Array1d<int> > get_minima() const;

    inline void set_nsdf_aggregate_roof(const double & p_value);
    inline void add_nsdf_aggregate_roof(const double & p_value);
    inline const double & get_nsdf_aggregate_roof()const;

    inline void set_first_nsdf_period(float p_period);
    inline float get_first_nsdf_period()const;

    inline void set_current_nsdf_period(float p_period);
    inline void add_current_nsdf_period(float p_period_diff);
    inline float get_current_nsdf_period() const;

    inline const Array1d<float> & get_nsdf_aggregate_data()const;
    inline const Array1d<float> & get_nsdf_aggregate_data_scaled()const;

  private:
    Array1d<float> m_nsdf_aggregate_data;
    Array1d<float> m_nsdf_aggregate_data_scaled;

    /**
       keeps the sum of scalers. i.e. The highest possible aggregate value
    */
    double m_nsdf_aggregate_roof;

    float m_first_nsdf_period;
    float m_current_nsdf_period;

    std::shared_ptr<Array1d<int> > m_maxima;
    std::shared_ptr<Array1d<int> > m_minima;

    const Channel * m_channel;

    /**
       the chunk at which this note starts on

    */
    int m_start_chunk;

    /**
       the chunk after the last one in the note

    */
    int m_end_chunk;

    /**
       The maximum RMS volume during the note

    */
    float m_max_log_RMS;

    /**
       The maximum intensity volume during the note
    */
    float m_max_intensity_DB;
    float m_max_correlation;
    float m_max_purity;
    float m_num_periods;

    /**
       This is in terms of the periodRatio at the beginning of the note
    */
    float m_period_octave_estimate;

    /**
       A normalised volume between 0 and 1
    */
    float m_volume;

    float m_avg_pitch;

    /**
       In the vibrato-analysis, search pitchLookupSmoothed with steps of the size of loopStep
    */
    int m_loop_step;

    /**
       Remembers where the previous vibrato-analysis broke off
    */
    int m_loop_start;
    int m_prev_extremum_time;
    float m_prev_extremum_pitch;

    typedef enum class PrevExtremum
    { NONE
    , FIRST_MAXIMUM
    , FIRST_MINIMUM
    , MAXIMUM
    , MINIMUM
    } t_prev_extremum;

    t_prev_extremum m_prev_extremum;

};

#include "notedata.hpp"

#endif // NOTEDATA_H
// EOF
