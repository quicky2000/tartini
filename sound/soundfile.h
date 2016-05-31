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
 public:
  SoundFile(void);
  ~SoundFile(void);

  /**
     free up all the memory of everything used
  */
  void uninit(void);
  void setFilename(const char *filename_);
  void setFilteredFilename(const char *filteredFilename_);
  QString getNextTempFilename(void) const;
  inline int numChannels(void) const;
  bool openRead(const char *filename_);
  bool openWrite(const char *filename_, int rate_, int channels_, int bits_, int windowSize_, int stepSize_);

  /**
     Preprocess the whole sound file,
     by looping through and processing every chunk in the file.
     A progress bar is displayed in the toolbar, because this can
     be time consuming.
  */
  void preProcess(void);
  void rec2play(void);
  void close(void);

  /**
     Lock the mutex's of all the channels
  */
  void lock(void);

  /**
     Unlock the mutex's of all the channels
  */
  void unlock(void);
  
  int readN(int n);

  /**
     Plays one chunk of sound to the associated stream
     @return true on success. false for end of file or error
  */
  bool playChunk(void);
  bool setupPlayChunk(void);
  void recordChunk(int n);
  void finishRecordChunk(int n);
  /**
     Plays one chunk of sound to the associated stream
     @return true on success. false for end of file or error
  */
  static bool playRecordChunk(SoundFile *playSoundFile, SoundFile *recSoundFile);
  void applyEqualLoudnessFilter(int n);

  /**
     Reads framesPerChunk frames from the SoundStream s
     into the end of the channel buffers. I.e losing framesPerChunk
     frames from the beginning of the channel buffers.
     If s is NULL (the defult) the the file stream is used
     @param s The SoundStream to read from or NULL (the default) the current file stream is used    
     @return The number of frames actually read ( <= framesPerChunk() )
  */
  int readChunk(int n);

  /**
     Process the chunks for all the channels
     Increment the chunkNum.
  */
  void processNewChunk(void);
  inline const double & startTime(void) const;
  inline void setStartTime(const double & t);
  inline int currentStreamChunk(void) const;
  inline int currentRawChunk(void) const;
  inline int currentChunk(void) const;
  inline void setCurrentChunk(int x);
  inline void incrementChunkNum(void);

  inline int offset(void) const;
  inline double timePerChunk(void) const;
  inline int chunkAtTime(const double & t) const;
  inline double chunkFractionAtTime(const double & t) const;
  inline double timeAtChunk(int chunk) const;
  inline double timeAtCurrentChunk(void) const;
  inline int chunkAtCurrentTime(void) const;

  /**
     shift all the channels data left by n frames
  */
  void shift_left(int n);
  void jumpToChunk(int chunk);
  inline void jumpToTime(const double & t);
  
  inline int rate(void) const;
  inline int bits(void) const;
  inline int framesPerChunk(void) const;
  inline void setFramesPerChunk(int stepSize);
  int bufferSize(void) const;
  int totalChunks(void) const;
  bool inFile(void) const; /**< Returns false if past end of file */

  inline bool saved(void) const;
  inline void setSaved(bool newState);
  inline bool equalLoudness(void) const;
  inline bool doingDetailedPitch(void) const;


  friend bool playRecordChunk(SoundFile *playSoundFile, SoundFile *recSoundFile, int n);

  inline bool isFirstTimeThrough(void) const;
  inline void setFirstTimeThrough(bool);
  inline const char * getFileName(void) const;

  inline void calculateAnalysisData(int chunk, Channel *ch);
  inline int getChannelIndex(const Channel &)const;
  inline Channel & getChannel(int p_index);

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

  /**
     Waits until there is n frames of data to read from s,
     Then puts the data in buffer.
     @param s The SoundStream to read from
     @param buffer The chunk buffer to put the data into. (s->channels of at least n length)
     This is padded with zeros (up to n) if there is less than n frames read.
     @param n The number of frames to read
     @return The number of frames read.
  */
  int blockingRead(SoundStream *s, float **buffer, int n); //low level

  /**
     Writes n frames of data to s from buffer.
     @param s The SoundStream to write to
     @param buffer The chunk buffer data to use. (s->channels of at least n length)
     @param n The number of frames to write
     @return The number of frames written.
  */
  int blockingWrite(SoundStream *s, float **buffer, int n); //low level

  /**
     Writes n frames of data to s and reads n frames of data from s
     If less than n frams are read, the remaining buffer is filled with zeros
     @param s The SoundStream to write to then read from
     @param writeBuffer The data that is to be written to the stream
     @param readBuffer The data that is read back from the stream
     @param n The amount of data in each of the buffers. Note: They must be the same size
     @param ch The number of channels
  */
  static int blockingWriteRead(SoundStream *s, float **writeBuffer, int writeCh, float **readBuffer, int readCh, int n);
  void toChannelBuffers(int n); //low level

  /**
     @param c Channel number
     @param n Number of frames to copy from tempWindowBuffer into channel
  */
  void toChannelBuffer(int c, int n);

 public:
  SoundFileStream *stream; /**< Pointer to the file's SoundFileStream */
  SoundFileStream *filteredStream; /**< Pointer to the file's filtered SoundFileStream */

 private:
  char *filename;
  char *filteredFilename;
  Array1d<Channel*> channels; /**< The actual sound data is stored seperately for each channel */
  MyTransforms myTransforms;
  bool firstTimeThrough;

};

#include "soundfile.hpp"

#endif // SOUND_FILE
//EOF

