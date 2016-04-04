/***************************************************************************
                          gdata.h  -  
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef GDATA_H
#define GDATA_H

#define TARTINI_VERSION_STR  "1.0"
#define TARTINI_VERSION       1.0

#include <qapplication.h>
#include <qobject.h>

#include <qmutex.h>

#include <vector>
//#include "sound_file_stream.h"
//#include "audio_stream.h"
#include "audio_thread.h"
//#include "chirp_xform.h"
#include "settings.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif
#include <qcolor.h>
#include "array2d.h"
#include "useful.h"
#include "view.h"

#ifndef WINDOWS
//for multi-threaded profiling
extern struct itimerval profiler_value;
extern struct itimerval profiler_ovalue;
#endif

#define STREAM_STOP     0
#define STREAM_FORWARD  1
#define STREAM_PAUSE    2
#define STREAM_UPDATE   3

#define SOUND_PLAY      0x01
#define SOUND_REC       0x02
#define SOUND_PLAY_REC  0x03

enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, AMPLITUDE_PURITY };

#define NUM_WIN_SIZES 5
extern int frame_window_sizes[NUM_WIN_SIZES];
extern const char *frame_window_strings[NUM_WIN_SIZES];

#define NUM_STEP_SIZES 6
extern float step_sizes[NUM_STEP_SIZES];
extern const char *step_size_strings[NUM_STEP_SIZES];

#define NUM_PITCH_METHODS 8
extern const char *pitch_method_strings[NUM_PITCH_METHODS];

#define NUM_INTERPOLATING_TYPES 3
extern const char *interpolating_type_strings[NUM_INTERPOLATING_TYPES];

class FBuffer;
class SoundStream;
class SoundFileStream;
class AudioStream;
class FWinFunc;
class Filter;
class SoundFile;
class Channel;

typedef struct
{
  unsigned char    red;
  unsigned char    green;
  unsigned char    blue;
} FrameRGB;


class GData : public QObject
{
  Q_OBJECT

public:
  //double tempStuff;
  
  GData(/*int buffer_size_, int winfunc_, float step_size_*/);
  virtual ~GData();

  Settings settings;

  //std::vector<float> coefficients_table;
  //Array2d<float> coefficients_table;

  //QMutex bufferMutex;
  //SoundFileStream *sound_file_stream;
  //FWinFunc *fwinfunc;
  //FWinFunc *loudnessFunc;
  //SoundStream *input_stream;
  //SoundStream *output_stream;
  int soundMode;

  AudioStream *audio_stream;
  bool need_update;
  //int buffer_size;
  //int winfunc;
  //float step_size;
  //int harmonicNum;

  std::vector<Filter*> filter_hp; //highpass filter
  std::vector<Filter*> filter_lp; //lowpass filter

  //bool equalLoudness;
  //bool useMasking;
  //bool useRidgeFile; //output a file with ridges found
  double cur_note;
  float peakThreshold;
  float correlationThreshold;

  bool doingStuff;
  AudioThread audioThread;
  int sync_flag;

  QMutex frameCounterMutex;
  int frameCounter;

  //char *inputFile;

  //int in_channels;
  //int process_channels;
  //int out_channels;
  //int pitch_method[2]; //a pitch method for each channel
  int interpolating_type;
  int bisection_steps;
  int fast_correlation_repeats;
  int running;
  bool using_coefficients_table;
  //chirp_xform fct;QColor myLineColor1(32, 32, 32);

  //float *fct_in_data;
  //float *fct_out_data;
  //FrameRGB *fct_draw_data;

  std::vector<SoundFile*> soundFiles;
  std::vector<Channel*> channels;
  std::vector<QColor> lineColor;
  int nextColorIndex;

  View *view;

  void setActiveChannel(Channel *toActive);
  Channel* getActiveChannel() { return activeChannel; }
  SoundFile* getActiveSoundFile();

private:
  Channel *activeChannel; /**< Pointer to the active channel */ 
  bool _doingHarmonicAnalysis;
  bool _doingFreqAnalysis;
  bool _doingEqualLoudness;
  int _doingActiveAnalysis;
  int _doingActiveFFT;
  int _doingActiveCepstrum;
  int _fastUpdateSpeed;
  int _slowUpdateSpeed;
  double _noiseThreshold;
  double _noiseThresholdDB;
  int _amplitudeMode;
  int _pitchContourMode;
  int _analysisType;

  QPixmap *_drawingBuffer;
  QColor _backgroundColor;
  QColor _shading1Color;
  QColor _shading2Color; 
  
  double _leftTime; /**< The lower bound of the start times of all channels */
  double _rightTime; /**< The upper bound of the finish times of all channels */
  double _topNote; /**< The highest possible note allowed (lowest possible is 0) */

public:
  QPixmap* drawingBuffer() { return _drawingBuffer; }
  void setLeftTime(double x); /**< Allows you to specify the leftmost time a file starts */
  void setRightTime(double x); /**< Allows you to specify the rightmost time a file starts */
  double leftTime() { return _leftTime; } /**< Returns the leftmost time a file starts */
  double rightTime() { return _rightTime; }/**< Returns the rightmost time a file stops */
  double totalTime() { return _rightTime - _leftTime; } /**< Returns the total number of seconds the files take up */
  double topNote() { return _topNote; } /**< Returns the top note the programme allows */
  void setTopNote(double y); /**< Allows you to specify the top note the programme should allow */
  void setDoingActiveAnalysis(bool x) { _doingActiveAnalysis += (x) ? 1 : -1; }
  void setDoingActiveFFT(bool x) { _doingActiveFFT += (x) ? 1 : -1; }
  void setDoingActiveCepstrum(bool x) { _doingActiveCepstrum += (x) ? 1 : -1; setDoingActiveFFT(x); }
  QColor getNextColor();

  int getAnalysisBufferSize(int rate);
  int getAnalysisStepSize(int rate);
  bool doingHarmonicAnalysis() { return _doingHarmonicAnalysis; }
  bool doingEqualLoudness() { return _doingEqualLoudness; }
  bool doingFreqAnalysis() { return _doingFreqAnalysis; }
  bool doingActiveAnalysis() { return _doingActiveAnalysis; }
  bool doingActiveFFT() { return _doingActiveFFT; }
  bool doingActiveCepstrum() { return _doingActiveCepstrum; }
  bool doingActive() { return (doingActiveAnalysis() || doingActiveFFT() || doingActiveCepstrum()); }
  int amplitudeMode() { return _amplitudeMode; }
  int pitchContourMode() { return _pitchContourMode; }
  int fastUpdateSpeed() { return _fastUpdateSpeed; }
  int slowUpdateSpeed() { return _slowUpdateSpeed; }
  double noiseThreshold() { return _noiseThreshold; }
  double noiseThresholdDB() { return _noiseThresholdDB; }
  //void setNoiseThreshold(double noiseThreshold_);
  void setNoiseThresholdDB(double noiseThresholdDB_);
  int analysisType() { return _analysisType; }
  
  QColor backgroundColor() { return _backgroundColor; }
  QColor shading1Color() { return _shading1Color; }
  QColor shading2Color() { return _shading2Color; }
  QString getFilenameString();
  void addFileToList(SoundFile *s);
  void removeFileFromList(SoundFile *s);
  void clearFreqLookup();
  void clearAmplitudeLookup();
  int getActiveIntThreshold();

signals:
  void activeChannelChanged(Channel *active);
  void activeIntThresholdChanged(int thresholdPercentage);
  void leftTimeChanged(double x);
  void rightTimeChanged(double x);
  void timeRangeChanged(double leftTime_, double rightTime_);
  void channelsChanged();

public slots:
  //void setBuffers(int freq, int channels);
  //void setFrameWindowSize(int index);
  //void setWinFunc(int index);
  //void setPitchMethod(int channel, int index) { pitch_method[channel] = index; }
  void setInterpolatingType(int type) { interpolating_type = type; }
  void setBisectionSteps(int num_steps) { bisection_steps = num_steps; }
  void setFastRepeats(int num_repeats) { fast_correlation_repeats = num_repeats; }
  void setAmplitudeMode(int amplitudeMode);
  void setPitchContourMode(int pitchContourMode);
  //void setStepSize(int index);
  void pauseSound();
  //void openMicrophone();
  bool openRecord(SoundFile *s);
  bool playSound(SoundFile *s);
  //void jump_forward(int frames);
  void updateViewLeftRightTimes();
  void updateActiveChunkTime(double t);
  void updateQuickRefSettings();
  
  void beginning();
  void rewind();
  bool play();
  void stop();
  void end();
  void fastforward();

  bool closeAllFiles();
  void saveActiveFile();
  void closeActiveFile();
  QString saveFileAsk(QString oldFilename);
  int saveFile(SoundFile *s, QString newFilename);
  int closeFile(SoundFile *s, bool ask=true);
  void resetActiveIntThreshold(int thresholdPercentage);
};

extern GData *gdata;

#endif
