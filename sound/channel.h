/***************************************************************************
                          channel.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "array1d.h"
#include "array2d.h"
#include "qcolor.h"
#include <vector>
#include "analysisdata.h"
#include "zoomlookup.h"
#include "soundfile.h"
#include "notedata.h"
#include "large_vector.h"
#include "Filter.h"

class Channel
{
 public:
  inline void lock(void);
  inline void unlock(void);
  inline bool locked(void) const; // For same thread testing of asserts only

  Channel(SoundFile *p_parent, int p_size, int p_k=0);
  virtual ~Channel(void);

  inline float * begin(void);
  inline float * end(void);
  inline int size(void) const;
  inline const float & at(int p_pos) const;
  inline int rate(void) const;

  virtual void resize(int p_new_size, int p_k=0);
  virtual void shift_left(int p_n);

  inline int framesPerChunk(void) const;
  inline void setParent(SoundFile *p_parent);
  inline SoundFile* getParent(void) const;
  inline void setPitchMethod(int p_pitch_method);
  inline int pitchMethod(void) const;

  void calc_last_n_coefficients(int p_n);

  /**
     Analysis the current data and add it to the end of the lookup table
     Note: The Channel shoud be locked before calling this.
  */
  void processNewChunk(FilterState *p_filter_state);

  /**
     Analysis the current data and put it at chunk position in the lookup table
     Note: The Channel shoud be locked before calling this.
     @param p_chunk The chunk number to store the data at
  */
  void processChunk(int p_chunk);
  inline bool isVisible(void) const;
  inline void setVisible(bool p_state=true);
  void reset(void);
  inline double timePerChunk(void) const;
  inline double startTime(void) const;
  inline void setStartTime(double p_new_start_time);
  inline int totalChunks(void) const;
  inline double finishTime(void) const;
  inline double totalTime(void) const;
  inline void jumpToTime(double p_time);
  inline int chunkAtTime(double p_time) const;
  inline double chunkFractionAtTime(double p_time) const;
  inline int chunkAtCurrentTime(void) const;
  inline int currentChunk(void) const;
  inline double timeAtChunk(int p_chunk) const;

  inline const AnalysisData *dataAtChunk(int p_chunk) const;
  inline AnalysisData *dataAtChunk(int p_chunk);
  inline const AnalysisData *dataAtCurrentChunk(void) const;
  inline AnalysisData *dataAtCurrentChunk(void);
  inline const AnalysisData *dataAtTime(double p_time) const;
  inline large_vector<AnalysisData>::iterator dataIteratorAtChunk(int p_chunk);
  static const AnalysisData * getActiveChannelCurrentChunkData(void);
  
  inline bool hasAnalysisData(void) const;
  inline bool isValidChunk(int p_chunk) const;
  inline bool isValidTime(double p_time) const;
  inline bool isValidCurrentTime(void) const;
  
  /**
     Returns the average pitch for a given channel between two frames.
     It weights using a -ve cos shaped window
     @param begin the starting frame number.
     @param end   the ending frame number.
     @return the average pitch, or -1 if there were no valid pitches.
  */
  float averagePitch(int p_begin, int p_end) const;
  float averageMaxCorrelation(int p_begin, int p_end) const;

  inline float threshold(void) const;
  inline void setIntThreshold(int p_threshold_percentage);
  void resetIntThreshold(int p_threshold_percentage);
  inline void setColor(QColor p_color);

  inline bool isNotePlaying(void) const;

  /**
     @param p_note_index the index of the note to inquire about
     @return true if the loudest part of the note is above the noiseThreshold
  */
  bool isVisibleNote(int p_note_index) const;
  inline bool isVisibleChunk(int p_chunk) const;
  bool isVisibleChunk(const AnalysisData *p_data) const;
  bool isChangingChunk(AnalysisData *p_data) const;

  /**
     If the current note has shifted far enough away from the mean of the current Note
     then the note is changing.
  */
  bool isNoteChanging(int p_chunk);

  /**
     @param p_note_index the index of the note to inquire about
     @return true if the note is long enough
  */
  bool isLabelNote(int p_note_index) const;
  void clearFreqLookup(void);
  void clearAmplitudeLookup(void);
  void recalcScoreThresholds(void);

  QString getUniqueFilename(void) const;

  const NoteData * getLastNote(void) const;
  NoteData * getLastNote(void);

  const NoteData * getCurrentNote(void) const;
  const NoteData * getNote(int p_note_index) const;
  inline int getCurrentNoteIndex(void) const;
  void backTrackNoteChange(int p_chunk);
  void processNoteDecisions(int p_chunk, float p_period_diff);
  void noteBeginning(int p_chunk);
  void noteEnding(int p_chunk);

  /**
     Uses the nsdfAggregateData to get an estimate of
     the period, over a whole note so far.
     This can be used to help determine the correct octave throughout the note
     @return the estimated note period, or -1.0 if none found
  */
  float calcOctaveEstimate(void);
  void recalcNotePitches(int p_chunk);

  /**
     Choose the correlation index (with no starting octave estimate)
     For use with at the start of the note
  */
  void chooseCorrelationIndex1(int p_chunk);

  /**
     This uses an octave extimate to help chose the correct correlation index
     Returns true if the new correlation index is different from the old one
  */
  bool chooseCorrelationIndex(int p_chunk, float p_period_octave_estimate);
  void calcDeviation(int p_chunk);
  bool isFirstChunkInNote(int p_chunk) const;
  void resetNSDFAggregate(float p_period);
  void addToNSDFAggregate(const float p_scaler, float p_period_diff);

  /**
     Calculate (the middle half of) pitches within the current window of the input
     Calculates pitches for positions 1/4 of size() to 3/4 of size()
     e.g. for size() == 1024, does indexs 256 through < 768
     @param period The period estimate
     @return The change in period size
  */
  float calcDetailedPitch(float *p_input, double p_period, int p_chunk);
  inline bool firstTimeThrough(void) const;
  inline bool doingDetailedPitch(void) const;

  void calcVibratoData(int p_chunk);
  float periodOctaveEstimate(int p_chunk) const; /*< A estimate from over the whole duration of the note, to help get the correct octave */

  void exportChannel(int p_type, QString p_type_string) const;
  void doPronyFit(int p_chunk);
  inline int pronyDelay(void) const;

  inline const large_vector<float> & get_pitch_lookup(void) const;
  inline large_vector<float> & get_pitch_lookup(void);
  inline const large_vector<float> & get_pitch_lookup_smoothed(void) const;
  inline large_vector<float> & get_pitch_lookup_smoothed(void);
  inline const QColor & get_color(void) const;
  inline const Array1d<float> & get_direct_input(void) const;
  inline const Array1d<float> & get_filtered_input(void) const;
  inline const Array1d<float> & get_nsdf_data(void) const;
  inline const Array1d<float> & get_fft_data1(void) const;
  inline Array1d<float> & get_fft_data1(void);
  inline const Array1d<float> & get_fft_data2(void) const;
  inline Array1d<float> & get_fft_data2(void);

  inline Array1d<float> & get_cepstrum_data(void);
  inline const Array1d<float> & get_detailed_pitch_data(void) const;
  inline const Array1d<float> & get_detailed_pitch_data_smoothed(void) const;
  inline const large_vector<NoteData> & get_note_data(void) const;
  inline void apply_highpass_filter(const float *p_input, float *p_output, int p_n);
  inline const Filter & get_pitch_small_smoothing_filter(void)const;
  inline const Filter & get_pitch_big_smoothing_filter(void)const;
  inline const double & get_rms_floor(void)const;
  inline void set_rms_floor(const double &);
  inline const double & get_rms_ceiling(void)const;
  inline void set_rms_ceiling(const double &);
  inline ZoomLookup & get_summary_zoom_lookup(void);
  inline ZoomLookup & get_normal_zoom_lookup(void);
  inline ZoomLookup & get_amplitude_zoom_lookup(void);

 private:
  SoundFile *m_parent;
  float m_freq; /**< Channel's frequency */
  int m_pitch_method;
  bool m_visible;
  bool m_note_is_playing;
  large_vector<AnalysisData> m_lookup;
  float m_threshold;
  QMutex *m_mutex;
  bool m_is_locked;
  int m_prony_window_size;
  Array1d<float> m_prony_data;
  fast_smooth *m_fast_smooth;
  large_vector<float> m_pitch_lookup;
  large_vector<float> m_pitch_lookup_smoothed;
  QColor m_color;
  Array1d<float> m_direct_input;
  Array1d<float> m_filtered_input;
  Array1d<float> m_coefficients_table;
  Array1d<float> m_nsdf_data;
  Array1d<float> m_nsdf_aggregate_data;
  Array1d<float> m_nsdf_aggregate_data_scaled;

  /**
     Keeps the sum of scalers. i.e. The highest possible aggregate value
  */
  double m_nsdf_aggregate_roof;

  Array1d<float> m_fft_data1;
  Array1d<float> m_fft_data2;
  Array1d<float> m_cepstrum_data;
  Array1d<float> m_detailed_pitch_data;
  Array1d<float> m_detailed_pitch_data_smoothed;
  large_vector<NoteData> m_note_data;
  Filter *m_high_pass_filter;
  Filter *m_pitch_small_smoothing_filter;
  Filter *m_pitch_big_smoothing_filter;
  double m_rms_floor; //in dB
  double m_rms_ceiling; //in dB

  ZoomLookup m_summary_zoom_lookup;
  ZoomLookup m_normal_zoom_lookup;
  ZoomLookup m_amplitude_zoom_lookup;

};

/**
   Create a ChannelLocker on the stack, the channel will be freed automaticly when
   the ChannelLocker goes out of scope
*/
class ChannelLocker
{
  Channel * m_channel;
  
public:
  inline ChannelLocker(Channel *p_channel);
  inline ~ChannelLocker(void);
};

#include "channel.hpp"

#endif
