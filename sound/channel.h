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

  Channel(SoundFile *parent_, int size_, int k_=0);
  virtual ~Channel(void);

  inline float * begin(void);
  inline float * end(void);
  inline int size(void) const;
  inline const float & at(int pos) const;
  inline int rate(void) const;

  virtual void resize(int newSize, int k_=0);
  virtual void shift_left(int n);

  inline int framesPerChunk(void) const;
  inline void setParent(SoundFile *parent_);
  inline SoundFile* getParent(void) const;
  inline void setPitchMethod(int pitch_method);
  inline int pitchMethod(void) const;

  void calc_last_n_coefficients(int n);

  /**
     Analysis the current data and add it to the end of the lookup table
     Note: The Channel shoud be locked before calling this.
  */
  void processNewChunk(FilterState *filterState);

  /**
     Analysis the current data and put it at chunk position in the lookup table
     Note: The Channel shoud be locked before calling this.
     @param chunk The chunk number to store the data at
  */
  void processChunk(int chunk);
  inline bool isVisible(void) const;
  inline void setVisible(bool state=true);
  void reset(void);
  inline double timePerChunk(void) const;
  inline double startTime(void) const;
  inline void setStartTime(double newStartTime);
  inline int totalChunks(void) const;
  inline double finishTime(void) const;
  inline double totalTime(void) const;
  inline void jumpToTime(double t);
  inline int chunkAtTime(double t) const;
  inline double chunkFractionAtTime(double t) const;
  inline int chunkAtCurrentTime(void) const;
  inline int currentChunk(void) const;
  inline double timeAtChunk(int chunk) const;

  inline const AnalysisData *dataAtChunk(int chunk) const;
  inline AnalysisData *dataAtChunk(int chunk);
  inline const AnalysisData *dataAtCurrentChunk(void) const;
  inline AnalysisData *dataAtCurrentChunk(void);
  inline const AnalysisData *dataAtTime(double t) const;
  inline large_vector<AnalysisData>::iterator dataIteratorAtChunk(int chunk);
  static const AnalysisData * getActiveChannelCurrentChunkData(void);
  
  inline bool hasAnalysisData(void) const;
  inline bool isValidChunk(int chunk) const;
  inline bool isValidTime(double t) const;
  inline bool isValidCurrentTime(void) const;
  
  /**
     Returns the average pitch for a given channel between two frames.
     It weights using a -ve cos shaped window
     @param begin the starting frame number.
     @param end   the ending frame number.
     @return the average pitch, or -1 if there were no valid pitches.
  */
  float averagePitch(int begin, int end) const;
  float averageMaxCorrelation(int begin, int end) const;

  inline float threshold(void) const;
  inline void setIntThreshold(int thresholdPercentage);
  void resetIntThreshold(int thresholdPercentage);
  inline void setColor(QColor c);

  inline bool isNotePlaying(void) const;

  /**
     @param noteIndex_ the index of the note to inquire about
     @return true if the loudest part of the note is above the noiseThreshold
  */
  bool isVisibleNote(int noteIndex_) const;
  inline bool isVisibleChunk(int chunk_) const;
  bool isVisibleChunk(const AnalysisData *data) const;
  bool isChangingChunk(AnalysisData *data) const;

  /**
     If the current note has shifted far enough away from the mean of the current Note
     then the note is changing.
  */
  bool isNoteChanging(int chunk);

  /**
     @param noteIndex_ the index of the note to inquire about
     @return true if the note is long enough
  */
  bool isLabelNote(int noteIndex_) const;
  void clearFreqLookup(void);
  void clearAmplitudeLookup(void);
  void recalcScoreThresholds(void);

  QString getUniqueFilename(void) const;

  const NoteData * getLastNote(void) const;
  NoteData * getLastNote(void);

  const NoteData * getCurrentNote(void) const;
  const NoteData * getNote(int noteIndex) const;
  inline int getCurrentNoteIndex(void) const;
  void backTrackNoteChange(int chunk);
  void processNoteDecisions(int chunk, float periodDiff);
  void noteBeginning(int chunk);
  void noteEnding(int chunk);

  /**
     Uses the nsdfAggregateData to get an estimate of
     the period, over a whole note so far.
     This can be used to help determine the correct octave throughout the note
     @return the estimated note period, or -1.0 if none found
  */
  float calcOctaveEstimate(void);
  void recalcNotePitches(int chunk);

  /**
     Choose the correlation index (with no starting octave estimate)
     For use with at the start of the note
  */
  void chooseCorrelationIndex1(int chunk);

  /**
     This uses an octave extimate to help chose the correct correlation index
     Returns true if the new correlation index is different from the old one
  */
  bool chooseCorrelationIndex(int chunk, float periodOctaveEstimate);
  void calcDeviation(int chunk);
  bool isFirstChunkInNote(int chunk) const;
  void resetNSDFAggregate(float period);
  void addToNSDFAggregate(const float scaler, float periodDiff);

  /**
     Calculate (the middle half of) pitches within the current window of the input
     Calculates pitches for positions 1/4 of size() to 3/4 of size()
     e.g. for size() == 1024, does indexs 256 through < 768
     @param period The period estimate
     @return The change in period size
  */
  float calcDetailedPitch(float *input, double period, int chunk);
  inline bool firstTimeThrough(void) const;
  inline bool doingDetailedPitch(void) const;

  void calcVibratoData(int chunk);
  float periodOctaveEstimate(int chunk) const; /*< A estimate from over the whole duration of the note, to help get the correct octave */

  void exportChannel(int type, QString typeString) const;
  void doPronyFit(int chunk);
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
  inline void apply_highpass_filter(const float *input, float *output, int n);
  inline const Filter & get_pitch_small_smoothing_filter(void)const;
  inline const Filter & get_pitch_big_smoothing_filter(void)const;
  inline const double & get_rms_floor(void)const;
  inline void set_rms_floor(const double &);
  inline const double & get_rms_ceiling(void)const;
  inline void set_rms_ceiling(const double &);
  inline ZoomLookup & get_summary_zoom_lookup(void);

 private:
  SoundFile *parent;
  float freq; /**< Channel's frequency */
  int _pitch_method;
  bool visible;
  bool noteIsPlaying;
  large_vector<AnalysisData> lookup;
  float _threshold;
  QMutex *mutex;
  bool isLocked;
  int pronyWindowSize;
  Array1d<float> pronyData;
  fast_smooth *fastSmooth;
  large_vector<float> pitchLookup;
  large_vector<float> pitchLookupSmoothed;
  QColor color;
  Array1d<float> directInput;
  Array1d<float> filteredInput;
  Array1d<float> coefficients_table;
  Array1d<float> nsdfData;
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;

  /**
     Keeps the sum of scalers. i.e. The highest possible aggregate value
  */
  double nsdfAggregateRoof;

  Array1d<float> fftData1;
  Array1d<float> fftData2;
  Array1d<float> cepstrumData;
  Array1d<float> detailedPitchData;
  Array1d<float> detailedPitchDataSmoothed;
  large_vector<NoteData> noteData;
  Filter *highPassFilter;
  Filter *pitchSmallSmoothingFilter;
  Filter *pitchBigSmoothingFilter;
  double rmsFloor; //in dB
  double rmsCeiling; //in dB

  ZoomLookup summaryZoomLookup;

 public:
  ZoomLookup normalZoomLookup;
  ZoomLookup amplitudeZoomLookup;

};

/**
   Create a ChannelLocker on the stack, the channel will be freed automaticly when
   the ChannelLocker goes out of scope
*/
class ChannelLocker
{
  Channel *channel;
  
public:
  inline ChannelLocker(Channel *channel_);
  inline ~ChannelLocker(void);
};

#include "channel.hpp"

#endif
