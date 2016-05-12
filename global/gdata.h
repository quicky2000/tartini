/***************************************************************************
                          gdata.h  -  
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef GDATA_H
#define GDATA_H

#define TARTINI_NAME_STR      "Tartini1.2"
#define TARTINI_VERSION_STR   "1.2.0"
//#define TARTINI_VERSION       1.2

#include <Qt>
#include <qapplication.h>
#include <qobject.h>

#include <qmutex.h>
//Added by qt3to4:
#include <QPixmap>

#include <vector>
//#include "sound_file_stream.h"
//#include "audio_stream.h"
#include "audio_thread.h"
//#include "chirp_xform.h"
//#include "settings.h"
#include <QSettings>

#ifndef WINDOWS
#include <sys/time.h>
#endif
#include <qcolor.h>
#include "array2d.h"
#include "useful.h"
#include "view.h"
#include "analysisdata.h"
extern int gMusicKey;

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

enum AnalysisModes { MPM, AUTOCORRELATION, MPM_MODIFIED_CEPSTRUM };

#define NUM_WIN_SIZES 5
extern int frame_window_sizes[NUM_WIN_SIZES];
extern const char *frame_window_strings[NUM_WIN_SIZES];

#define NUM_STEP_SIZES 6
extern float step_sizes[NUM_STEP_SIZES];
extern const char *step_size_strings[NUM_STEP_SIZES];

#define NUM_PITCH_METHODS 8
extern const char *pitch_method_strings[NUM_PITCH_METHODS];

class FBuffer;
class SoundStream;
class SoundFileStream;
class AudioStream;
class FWinFunc;
class Filter;
class SoundFile;
class Channel;

class GData : public QObject
{
  Q_OBJECT

public:
  enum SavingModes { ALWAYS_ASK, NEVER_SAVE, ALWAYS_SAVE };

  GData(/*int buffer_size_, int winfunc_, float step_size_*/);
  virtual ~GData(void);

  inline bool settingsContains(const QString & p_key)const;
  inline int getSettingsValue(const QString & p_key,const int & p_default_value)const;
  inline bool getSettingsValue(const QString & p_key,const bool & p_default_value)const;
  inline QString getSettingsValue(const QString & p_key,const QString & p_default_value)const;
  inline QPoint getSettingsValue(const QString & p_key,const QPoint & p_default_value)const;
  inline QSize getSettingsValue(const QString & p_key,const QSize & p_default_value)const;
  inline void setSettingsValue(const QString & p_key,const QString & p_value);
  inline void setSettingsValue(const QString & p_key,const int & p_value);
  inline void setSettingsValue(const QString & p_key,const QPoint & p_value);
  inline void setSettingsValue(const QString & p_key,const QSize & p_value);
  inline void clearSettings(void);
  inline void syncSettings(void);
  inline int getSettingsIntValue(const QString & p_key)const;
  inline bool getSettingsBoolValue(const QString & p_key)const;
  inline QString getSettingsStringValue(const QString & p_key)const;

  inline int getSoundMode(void)const;
  inline void setSoundMode(const int & p_mode);

  inline AudioStream * getAudioStream(void)const;
  inline void setAudioStream(AudioStream * p_audio_stream);

  inline bool needUpdate(void)const;
  inline void setNeedUpdate(bool p_need_update);

  inline const AudioThread & getAudioThread(void)const;
  inline void stopAndWaitAudioThread(void);

  inline void setRunning(int p_running);
  inline int getRunning(void)const;

  inline size_t getChannelsSize(void)const;
  inline Channel* getChannelAt(size_t p_index)const;

  inline const View & getView(void)const;
  inline View & getView(void);
  inline void setView(View & p_view);

  void setActiveChannel(Channel *toActive);
  inline Channel* getActiveChannel(void);
  SoundFile* getActiveSoundFile(void);

  inline QPixmap* drawingBuffer(void);

  /**
     Allows you to specify the leftmost time a file starts
   */
  void setLeftTime(double x);

  /**
     Allows you to specify the rightmost time a file starts
   */
  void setRightTime(double x);

  /**
     Returns the leftmost time a file starts
  */
  inline const double & leftTime(void)const; 

  /**
     Returns the rightmost time a file stops
   */
  inline const double & rightTime(void)const;

  /**
     Returns the total number of seconds the files take up
  */
  inline double totalTime(void)const;

  /**
     Returns the top note pitch the programme allows
  */
  inline const double & topPitch(void)const;

  /**
     Allows you to specify the top note pitch the programme should allow
  */
  void setTopPitch(double y);

  inline void setDoingActiveAnalysis(bool x);
  inline void setDoingActiveFFT(bool x);
  inline void setDoingActiveCepstrum(bool x);
  inline void setDoingDetailedPitch(bool x);

  QColor getNextColor(void);

  int getAnalysisBufferSize(int rate);
  int getAnalysisStepSize(int rate);

  inline bool doingHarmonicAnalysis(void)const;
  inline bool doingAutoNoiseFloor(void)const;
  inline bool doingEqualLoudness(void)const;
  inline bool doingFreqAnalysis(void)const;
  inline bool doingActiveAnalysis(void)const;
  inline bool doingActiveFFT(void)const;
  inline bool doingActiveCepstrum(void)const;
  inline bool doingDetailedPitch(void)const;
  inline bool doingActive(void)const;
  inline bool vibratoSineStyle(void)const;
  inline int amplitudeMode(void)const;
  inline int pitchContourMode(void)const;
  inline int fastUpdateSpeed(void)const;
  inline int slowUpdateSpeed(void)const;
  inline bool mouseWheelZooms(void)const;

  void setAmpThreshold(int mode, int index, double value);
  double ampThreshold(int mode, int index);
  void setAmpWeight(int mode, double value);
  double ampWeight(int mode);

  inline int analysisType(void)const;
  inline bool polish(void)const;
  inline bool showMeanVarianceBars(void)const;
  inline int savingMode(void)const;

  inline const QColor & backgroundColor(void)const;
  inline const QColor & shading1Color(void)const;
  inline const QColor & shading2Color(void)const;

  QString getFilenameString(void);
  void addFileToList(SoundFile *s);
  void removeFileFromList(SoundFile *s);
  void clearFreqLookup(void);
  void clearAmplitudeLookup(void);
  void recalcScoreThresholds(void);
  int getActiveIntThreshold(void);

  inline const double & dBFloor(void)const;
  inline void setDBFloor(double dBFloor_);
  inline double & rmsFloor(void);
  inline double & rmsCeiling(void);

  inline int musicKey(void)const;
  inline int musicKeyType(void)const;
  inline int temperedType(void)const;
  inline const double & freqA(void)const;
  inline const double & semitoneOffset(void)const;

signals:
  void activeChannelChanged(Channel *active);
  void activeIntThresholdChanged(int thresholdPercentage);
  void leftTimeChanged(double x);
  void rightTimeChanged(double x);
  void timeRangeChanged(double leftTime_, double rightTime_);
  void channelsChanged(void);
  void onChunkUpdate(void);

  void musicKeyChanged(int key);
  void musicKeyTypeChanged(int type);
  void temperedTypeChanged(int type);

public slots:
  void setAmplitudeMode(int amplitudeMode);
  void setPitchContourMode(int pitchContourMode);

  inline void setMusicKey(int key);
  inline void setMusicKeyType(int type);
  void setTemperedType(int type);
  void setFreqA(double x);
  inline void setFreqA(int x);

  void pauseSound(void);
  bool openPlayRecord(SoundFile *sRec, SoundFile *sPlay);
  bool playSound(SoundFile *s);
  void updateViewLeftRightTimes(void);
  void updateActiveChunkTime(double t);
  void updateQuickRefSettings(void);
  
  void beginning(void);
  void rewind(void);
  bool play(void);
  void stop(void);
  void end(void);
  void fastforward(void);

  bool closeAllFiles(void);
  void saveActiveFile(void);
  void closeActiveFile(void);
  QString saveFileAsk(QString oldFilename);
  int saveFile(SoundFile *s, QString newFilename);
  int closeFile(SoundFile *s, int theSavingMode/*, bool ask=true*/);
  void resetActiveIntThreshold(int thresholdPercentage);

  void doChunkUpdate(void);

 private:
  QSettings *qsettings;
  int soundMode;
  AudioStream *audio_stream;
  bool need_update;

  /**
     highpass filter
   */
  std::vector<Filter*> filter_hp;

  /**
     lowpass filter
   */
  std::vector<Filter*> filter_lp;
  AudioThread audioThread;
  int running;

  std::vector<SoundFile*> soundFiles;
  std::vector<Channel*> channels;

  // To check if really usefull
  std::vector<QColor> lineColor;
  int nextColorIndex;
  // End to check

  View *view;

  /**
     Pointer to the active channel
   */
  Channel *activeChannel; 
  bool _doingHarmonicAnalysis;
  bool _doingFreqAnalysis;
  bool _doingEqualLoudness;
  bool _doingAutoNoiseFloor;
  int _doingActiveAnalysis;
  int _doingActiveFFT;
  int _doingActiveCepstrum;
  bool _doingDetailedPitch;
  int _fastUpdateSpeed;
  int _slowUpdateSpeed;
  bool _polish;
  bool _showMeanVarianceBars;
  int _savingMode;
  bool _vibratoSineStyle;
  int _musicKeyType;
  int _temperedType;
  bool _mouseWheelZooms;
  double _freqA;
  double _semitoneOffset;

  int _amplitudeMode;
  int _pitchContourMode;
  int _analysisType;
  double _dBFloor;
  double amp_thresholds[NUM_AMP_MODES][2];
  double amp_weights[NUM_AMP_MODES];

  QPixmap *_drawingBuffer;
  QColor _backgroundColor;
  QColor _shading1Color;
  QColor _shading2Color; 
  
  /**
     The lower bound of the start times of all channels
   */
  double _leftTime;

  /**
     The upper bound of the finish times of all channels 
   */
  double _rightTime;

  /**
     The highest possible note pitch allowed (lowest possible is 0)
   */
  double _topPitch;

};

extern GData *gdata;

#include "gdata.hpp"

#endif
