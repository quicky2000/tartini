/***************************************************************************
                          channel.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
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
//#include "myqmutex.h"
#include "analysisdata.h"
#include "zoomlookup.h"
#include "soundfile.h"
#include "notedata.h"
#include "large_vector.h"
#include "Filter.h"

class Channel/* : public Array1d<float>*/
{
private:
  SoundFile *parent;
  float freq; /**< Channel's frequency */
  //float estimate;
  //int frame_num;
  int _pitch_method;
  bool visible;
  bool noteIsPlaying;
  //double timeOffset; /**< Where the file starts in absolute time (in seconds) */
  //std::vector<AnalysisData> lookup;
  large_vector<AnalysisData> lookup;
  float _threshold;
  QMutex *mutex;
  bool isLocked;
  int pronyWindowSize;
  Array1d<float> pronyData;
  fast_smooth *fastSmooth;

public:
  large_vector<float> pitchLookup;
  large_vector<float> pitchLookupSmoothed;
  QColor color;
  //large_vector<float> filteredData;
  Array1d<float> directInput;
  Array1d<float> filteredInput;
  Array1d<float> coefficients_table;
  Array1d<float> nsdfData;
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;
  double nsdfAggregateRoof; //keeps the sum of scalers. i.e. The highest possible aggregate value
  Array1d<float> fftData1;
  Array1d<float> fftData2;
  Array1d<float> fftData3;
  Array1d<float> cepstrumData;
  Array1d<float> detailedPitchData;
  Array1d<float> detailedPitchDataSmoothed;
  //std::vector<NoteData> noteData;
  large_vector<NoteData> noteData;
  Filter *highPassFilter;
  Filter *pitchSmallSmoothingFilter;
  Filter *pitchBigSmoothingFilter;
  //double filterStateX1, filterStateX2;
  //double filterStateY1, filterStateY2;
  double rmsFloor; //in dB
  double rmsCeiling; //in dB
  
  ZoomLookup summaryZoomLookup;
  ZoomLookup normalZoomLookup;
  ZoomLookup amplitudeZoomLookup;
  
  inline void lock(void);
  inline void unlock(void);
  inline bool locked(void) const; // For same thread testing of asserts only

  //Channel();
  Channel(SoundFile *parent_, int size_, int k_=0);
  virtual ~Channel();

  inline float * begin();
  inline float * end();
  inline int size();
  inline float & at(int pos);
  inline int rate();

  virtual void resize(int newSize, int k_=0);
  virtual void shift_left(int n);

  inline int framesPerChunk();
  inline void setParent(SoundFile *parent_);
  inline SoundFile* getParent();
  inline void setPitchMethod(int pitch_method);
  inline int pitchMethod();

  void calc_last_n_coefficients(int n);
  void processNewChunk(FilterState *filterState);
  void processChunk(int chunk);
  inline bool isVisible();
  inline void setVisible(bool state=true);
  void reset();
  inline double timePerChunk();
  inline double startTime();
  inline void setStartTime(double newStartTime);
  //int chunkNum() { return parent->chunkNum(); }
  inline int totalChunks();
  inline double finishTime();
  //double totalTime() { return double(lookup.size()) * timePerChunk(); }
  inline double totalTime();
  inline void jumpToTime(double t);
  inline int chunkAtTime(double t);
  inline double chunkFractionAtTime(double t);
  inline int chunkAtCurrentTime();
  //int chunkOffset() { return parent->chunkOffset(); }
  //int currentChunk() { return chunkNum() - chunkOffset(); } //this one should be use to retrieve current info
  inline int currentChunk();
  inline double timeAtChunk(int chunk);

  //AnalysisData &dataAtChunk(int chunk) { myassert(chunk >= 0 && chunk < int(lookup.size())); return lookup[chunk]; }
  //AnalysisData &dataAtCurrentTime() { return dataAtChunk(chunkAtCurrentTime()); }

  inline AnalysisData *dataAtChunk(int chunk);
  inline AnalysisData *dataAtCurrentChunk();
  inline AnalysisData *dataAtTime(double t);
  inline large_vector<AnalysisData>::iterator dataIteratorAtChunk(int chunk);
  static AnalysisData *getActiveChannelCurrentChunkData();
  
  inline bool hasAnalysisData();
  inline bool isValidChunk(int chunk);
  inline bool isValidTime(double t);
  inline bool isValidCurrentTime();
  
  float averagePitch(int begin, int end);
  float averageMaxCorrelation(int begin, int end);

  inline float threshold();
  //void setThreshold(float threshold) { _threshold = threshold; }
  inline void setIntThreshold(int thresholdPercentage);
  void resetIntThreshold(int thresholdPercentage);
  inline void setColor(QColor c);
  //static QColor getNextColour();

  inline bool isNotePlaying();
  bool isVisibleNote(int noteIndex_);
  inline bool isVisibleChunk(int chunk_);
  bool isVisibleChunk(AnalysisData *data);
  bool isChangingChunk(AnalysisData *data);
  bool isNoteChanging(int chunk);
  bool isLabelNote(int noteIndex_);
  void clearFreqLookup();
  void clearAmplitudeLookup();
  void recalcScoreThresholds();

  QString getUniqueFilename();

  NoteData *getLastNote();
  NoteData *getCurrentNote();
  NoteData *getNote(int noteIndex);
  inline int getCurrentNoteIndex();
  void backTrackNoteChange(int chunk);
  void processNoteDecisions(int chunk, float periodDiff);
  void noteBeginning(int chunk);
  void noteEnding(int chunk);
  float calcOctaveEstimate();
  void recalcNotePitches(int chunk);
  void chooseCorrelationIndex1(int chunk);
  bool chooseCorrelationIndex(int chunk, float periodOctaveEstimate);
  void calcDeviation(int chunk);
  bool isFirstChunkInNote(int chunk);
  void resetNSDFAggregate(float period);
  void addToNSDFAggregate(const float scaler, float periodDiff);
  float calcDetailedPitch(float *input, double period, int chunk);
  inline bool firstTimeThrough();
  inline bool doingDetailedPitch();

  void calcVibratoData(int chunk);
  float periodOctaveEstimate(int chunk); /*< A estimate from over the whole duration of the note, to help get the correct octave */

  void exportChannel(int type, QString typeString);
  void doPronyFit(int chunk);
  inline int pronyDelay();
};

/** Create a ChannelLocker on the stack, the channel will be freed automaticly when
  the ChannelLocker goes out of scope
*/
class ChannelLocker
{
  Channel *channel;
  
public:
  inline ChannelLocker(Channel *channel_);
  inline ~ChannelLocker();
};

#include "channel.hpp"

#endif
