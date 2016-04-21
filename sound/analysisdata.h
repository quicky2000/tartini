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

//enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, AMPLITUDE_PURITY, FREQ_CHANGENESS };
enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, FREQ_CHANGENESS, DELTA_FREQ_CENTROID, NOTE_SCORE, NOTE_CHANGE_SCORE };

#define NUM_AMP_MODES 7
extern const char *amp_mode_names[NUM_AMP_MODES];
extern const char *amp_display_string[NUM_AMP_MODES];
extern double(*amp_mode_func[NUM_AMP_MODES])(double);
extern double(*amp_mode_inv_func[NUM_AMP_MODES])(double);

#define NO_NOTE -1

class AnalysisData
{
public: 
  inline float getValue(size_t p_index)const;
  inline float getPeriod(void)const;
  inline void setPeriod(float p_period);
  inline float getFundamentalFreq(void)const;
  inline void setFundamentalFreq(float p_fundamental_freq);
  inline float getPitch(void)const;
  inline void setPitch(float p_pitch);
  inline float getFreqCentroid(void)const;
  inline const double & getPitchSum(void)const;
  inline void setPitchSum(const double & p_pitch_sum);
  inline const double & getPitch2Sum(void)const;
  inline void setPitch2Sum(const double & p_pitch_2_sum);
  inline const double & getShortTermMean(void)const;
  inline void setShortTermMean(const double & p_short_term_mean);
  inline const double & getShortTermDeviation(void)const;
  inline void setShortTermDeviation(const double & p_short_term_deviation);
  inline const double & getLongTermMean(void)const;
  inline void setLongTermMean(const double & p_long_term_mean);
  inline const double & getLongTermDeviation(void)const;
  inline void setLongTermDeviation(const double & p_long_term_deviation);
  inline const double & getSpread(void)const;
  inline void setSpread(const double & p_spread);
  inline const double & getSpread2(void)const;
  inline void setSpread2(const double & p_spread2);
  inline float getVibratoPitch(void)const;
  inline void setVibratoPitch(float p_vibrato_pitch);
  inline float getVibratoWidth(void)const;
  inline void setVibratoWidth(float p_vibrato_width);
  inline float getVibratoWidthAdjust(void)const;
  inline void setVibratoWidthAdjust(float p_vibrato_width_adjust);
  inline float getVibratoSpeed(void)const;
  inline void setVibratoSpeed(float p_vibrato_speed);
  inline float getVibratoPhase(void)const;
  inline void setVibratoPhase(float p_vibrato_phase);
  inline float getVibratoError(void)const;
  inline void setVibratoError(float p_vibrato_error);
  inline int getReason(void)const;
  inline void setReason(int p_reason);
  inline int getHighestCorrelationIndex(void)const;
  inline void setHighestCorrelationIndex(int p_index);
  inline int getChosenCorrelationIndex(void)const;
  inline void setChosenCorrelationIndex(int p_index);
  inline float getPeriodRatio(void)const;
  inline void setPeriodRatio(float p_period_ratio);
  inline int getCepstrumIndex(void)const;
  inline void setCepstrumIndex(int p_index);
  inline float getCepstrumPitch(void)const;
  inline void setCepstrumPitch(float p_cepstrum_pitch);

  inline bool isPeriodEstimatesEmpty(void)const;
  inline size_t getPeriodEstimatesSize(void)const;
  inline float getPeriodEstimatesAt(size_t p_index)const;
  inline void clearPeriodEstimates(void);
  inline void addPeriodEstimates(float p_value);
  inline float searchClosestPeriodEstimates(const float & p_value)const;

  inline size_t getPeriodEstimatesAmpSize(void)const;
  inline float getPeriodEstimatesAmpAt(size_t p_index)const;
  inline void clearPeriodEstimatesAmp(void);
  inline void addPeriodEstimatesAmp(float p_value);
  inline int getPeriodEstimatesAmpMaxElementIndex(void)const;

  inline size_t getHarmonicAmpNoCutOffSize(void)const;
  inline float getHarmonicAmpNoCutOffAt(size_t p_index)const;
  inline void resizeHarmonicAmpNoCutOff(size_t p_size);
  inline void setHarmonicAmpNoCutOffAt(size_t p_index, float p_value);

  inline float getHarmonicAmpRelativeAt(size_t p_index)const;
  inline size_t getHarmonicAmpRelativeSize(void)const;

  inline float getHarmonicAmpAt(size_t p_index)const;
  inline void resizeHarmonicAmp(size_t p_size);
  inline void setHarmonicAmpAt(size_t p_index, float p_value);

  inline float getHarmonicFreqAt(size_t p_index)const;
  inline size_t getHarmonicFreqSize(void)const;
  inline void resizeHarmonicFreq(size_t p_size);
  inline void setHarmonicFreqAt(size_t p_index, float p_value);

  inline float getHarmonicNoiseAt(size_t p_index)const;
  inline void resizeHarmonicNoise(size_t p_size);
  inline void setHarmonicNoiseAt(size_t p_index, float p_value);

  inline void setFilterState(const FilterState & p_filter_state);
  inline int getNoteIndex(void)const;
  inline void setNoteIndex(int p_node_index);

  inline void setNotePlaying(bool p_note_playing);
  inline bool isNotePlaying(void)const;
  inline void setDone(bool p_done);
  inline bool isDone(void)const;
 private:
  float values[NUM_AMP_MODES];
  float period; /*< The period of the fundamental (in samples) */
  float fundamentalFreq; /*< The fundamental frequency in hertz */
  float pitch; /*< The pitch in semi-tones */
  float _freqCentroid;
  double pitchSum; /*< The sum of pitches so far in a note */
  double pitch2Sum; /*< The sum of pitches squared so far in a note */
  double shortTermMean; /*< The mean note so far */
  double shortTermDeviation; /*< The deviation of the note so far (based on standard deviation) */
  double longTermMean; /*> The mean note so far */
  double longTermDeviation; /*< The deviation of the note so far (based on standard deviation) */
  double spread;
  double spread2;
  float vibratoPitch;
  float vibratoWidth;
  float vibratoWidthAdjust;
  float vibratoSpeed;
  float vibratoPhase;
  float vibratoError;
  int reason; /*< The reason why there was a note change */
  int highestCorrelationIndex;
  int chosenCorrelationIndex;
  float periodRatio; /*< The ratio of the current period to the period at the beginning of the current note */
  int cepstrumIndex;
  float cepstrumPitch;
  std::vector<float> periodEstimates;
  std::vector<float> periodEstimatesAmp;
  std::vector<float> harmonicAmpNoCutOff;
  std::vector<float> harmonicAmpRelative;
  std::vector<float> harmonicAmp;
  std::vector<float> harmonicFreq;
  std::vector<float> harmonicNoise;
  FilterState filterState; //the state of the filter at the beginning of the chunk
  int noteIndex; //The index of the note in the noteData, or NO_NOTE
  bool notePlaying;
  bool done;
 public:
  //bool isValid();
  AnalysisData(void);
  void calcScores(void);

  float &logrms() { return values[AMPLITUDE_RMS]; }
  float &maxIntensityDB() { return values[AMPLITUDE_MAX_INTENSITY]; }
  float &correlation() { return values[AMPLITUDE_CORRELATION]; }
  float &changeness() { return values[FREQ_CHANGENESS]; }
  float &freqCentroid() { return _freqCentroid; }
  float &deltaFreqCentroid() { return values[DELTA_FREQ_CENTROID]; }
  float volumeValue() { return (dB2Normalised(values[AMPLITUDE_RMS]) + values[AMPLITUDE_CORRELATION]-1.0f) * 0.2; }
  float &noteScore() { return values[NOTE_SCORE]; }
  float &noteChangeScore() { return values[NOTE_CHANGE_SCORE]; }
};

struct lessFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  inline bool operator()(const AnalysisData & x, const AnalysisData & y);
};

struct greaterFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  inline bool operator()(const AnalysisData &x, const AnalysisData &y);
};

struct lessPitch : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  inline bool operator()(const AnalysisData &x, const AnalysisData &y);
};

struct greaterPitch : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  inline bool operator()(const AnalysisData &x, const AnalysisData &y);
};

struct lessValue : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  int v;
  inline lessValue(int v_);
  inline bool operator()(const AnalysisData &x, const AnalysisData &y);
};

struct greaterValue : public std::binary_function<AnalysisData &, AnalysisData &, bool>
{
  int v;
  inline greaterValue(int v_);
  inline bool operator()(const AnalysisData &x, const AnalysisData &y);
};

#include "analysisdata.hpp"

#endif // ANALYSIS_DAYA_H
//EOF
