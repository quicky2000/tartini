/***************************************************************************
                          soundfile.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@pmcleod.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef SOUND_FILE
#define SOUND_FILE

#include "array1d.h"
#include "sound_file_stream.h"
#include "gdata.h"
#include "mytransforms.h"
#include "mainwindow.h"

class Channel;

extern const double v8, v16, v32;

class SoundFile
{
protected:
  int _chunkNum;
  int _framesPerChunk; /**< The number of samples to move every chunk */
  float **tempWindowBuffer; //array is indexed from -16 !!
  double **tempWindowBufferDouble; //array is indexed from -16 !!
  float **tempWindowBufferFiltered; //array is indexed from -16 !!
  double **tempWindowBufferFilteredDouble; //array is indexed from -16 !!
  double _startTime;
  int _offset;
  bool _saved;
  QMutex *mutex;
  bool _doingDetailedPitch;

  int blockingRead(SoundStream *s, float **buffer, int n); //low level
  int blockingWrite(SoundStream *s, float **buffer, int n); //low level
  static int blockingWriteRead(SoundStream *s, float **writeBuffer, int writeCh, float **readBuffer, int readCh, int n);
  void toChannelBuffers(int n); //low level
  void toChannelBuffer(int c, int n);
public:
  char *filename;
  char *filteredFilename;
  SoundFileStream *stream; /**< Pointer to the file's SoundFileStream */
  SoundFileStream *filteredStream; /**< Pointer to the file's filtered SoundFileStream */
  Array1d<Channel*> channels; /**< The actual sound data is stored seperately for each channel */
  MyTransforms myTransforms;
  bool firstTimeThrough;
  
  SoundFile();
  ~SoundFile();
  void uninit();
  void setFilename(const char *filename_);
  void setFilteredFilename(const char *filteredFilename_);
  QString getNextTempFilename();
  inline int numChannels();
  bool openRead(const char *filename_);
  bool openWrite(const char *filename_, int rate_, int channels_, int bits_, int windowSize_, int stepSize_);
  void preProcess();
  void rec2play();
  void close();

  void lock();
  void unlock();
  
  int readN(int n);
  bool playChunk();
  bool setupPlayChunk();
  void recordChunk(int n);
  void finishRecordChunk(int n);
  static bool playRecordChunk(SoundFile *playSoundFile, SoundFile *recSoundFile);
  void applyEqualLoudnessFilter(int n);
  int readChunk(int n);
  void processNewChunk();
  inline double startTime();
  inline void setStartTime(double t);
  inline int currentStreamChunk();
  inline int currentRawChunk();
  inline int currentChunk();
  inline void setCurrentChunk(int x);
  inline void incrementChunkNum();

  inline int offset();
  inline double timePerChunk();
  inline int chunkAtTime(double t);
  inline double chunkFractionAtTime(double t);
  inline double timeAtChunk(int chunk);
  inline double timeAtCurrentChunk();
  inline int chunkAtCurrentTime();
  void shift_left(int n);
  void jumpToChunk(int chunk);
  inline void jumpToTime(double t);
  
  inline int rate();
  inline int bits() ;
  inline int framesPerChunk();
  inline void setFramesPerChunk(int stepSize);
  int bufferSize();
  int totalChunks();
  bool inFile(); /**< Returns false if past end of file */

  inline bool saved();
  inline void setSaved(bool newState);
  inline bool equalLoudness();
  inline bool doingDetailedPitch();

  friend bool playRecordChunk(SoundFile *playSoundFile, SoundFile *recSoundFile, int n);
};

#include "soundfile.hpp"

#endif
