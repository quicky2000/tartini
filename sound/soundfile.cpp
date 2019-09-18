/***************************************************************************
                           soundfile.cpp  -  description
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

#include "myassert.h"
#include "soundfile.h"
#include "mystring.h"
#include "array1d.h"
#include "useful.h"
#include <algorithm>
#include "channel.h"

#include <q3progressbar.h>
#include "mainwindow.h"
#include <qstatusbar.h>
#include <qlabel.h>
#include <qdir.h>

#include "audio_stream.h"
#include "wave_stream.h"
#ifdef USE_SOX
#include "sox_stream.h"
#else
#if USE_OGG
#include "ogg_stream.h"
#endif
#endif

#include "QMessageBox"

typedef unsigned char byte;

//------------------------------------------------------------------------------
SoundFile::SoundFile(void)
{
  filename = NULL;
  filteredFilename = NULL;
  stream = NULL;
  filteredStream = NULL;
  _framesPerChunk = 0;
  tempWindowBuffer = NULL;
  tempWindowBufferDouble = NULL;
  tempWindowBufferFiltered = NULL;
  tempWindowBufferFilteredDouble = NULL;
  _startTime = 0.0;
  _chunkNum = 0;
  _offset = 0; //Number of frame to read into file to get to time 0 (half buffer size).
  _saved = true;
  mutex = new QMutex(true);
  firstTimeThrough = true;
  _doingDetailedPitch = false;
}

//------------------------------------------------------------------------------
SoundFile::~SoundFile(void)
{
  uninit();
  delete mutex;
}

//------------------------------------------------------------------------------
void SoundFile::uninit(void)
{
  if(gdata->getAudioThread().playSoundFile() == this || gdata->getAudioThread().recSoundFile() == this)
    {
      gdata->stop();
    }

  if(stream)
    {
      delete stream; stream = NULL;
    }
  if(filteredStream)
    {
      delete filteredStream; filteredStream = NULL;
      //Delete the temporary filtered file from disk!
      if(::remove(filteredFilename) == -1)
	{
	  fprintf(stderr, "Error removing file %s\n", filteredFilename);
	}
    }
  setFilename(NULL);
  setFilteredFilename(NULL);
  for(int j=0; j<numChannels(); j++)
    {
      delete channels(j);
      delete[] (tempWindowBuffer[j]-16);
      delete[] (tempWindowBufferDouble[j]-16);
      delete[] (tempWindowBufferFiltered[j]-16);
      delete[] (tempWindowBufferFilteredDouble[j]-16);
    }
  channels.resize(0);
  delete[] tempWindowBuffer; tempWindowBuffer = NULL;
  delete[] tempWindowBufferDouble; tempWindowBufferDouble = NULL;
  delete[] tempWindowBufferFiltered; tempWindowBufferFiltered = NULL;
  delete[] tempWindowBufferFilteredDouble; tempWindowBufferFilteredDouble = NULL;

  setFramesPerChunk(0);
  _startTime = 0.0;
  _chunkNum = 0;
  _offset = 0;
}

//------------------------------------------------------------------------------
void SoundFile::setFilename(const char *filename_)
{
  if(filename)
    {
      free(filename);
      filename = NULL;
    }
  if(filename_)
    {
      filename = copy_string(filename_);
    }
}

//------------------------------------------------------------------------------
void SoundFile::setFilteredFilename(const char *filteredFilename_)
{
  if(filteredFilename)
    {
      free(filteredFilename);
      filteredFilename = NULL;
    }
  if(filteredFilename_)
    {
      filteredFilename = copy_string(filteredFilename_);
    }
}

//------------------------------------------------------------------------------
QString SoundFile::getNextTempFilename(void) const
{
  QString tempFileFolder = gdata->getSettingsValue("General/tempFilesFolder", QDir::convertSeparators(QDir::currentDirPath()));
  QDir dir = QDir(tempFileFolder);
  QFileInfo fileInfo;
  QString fileName;
  bool fileExists;
  int index = 1;
  do
    {
      fileExists = false;
      fileName.sprintf("temp%03d.wav", index);
#ifdef PRINTF_DEBUG
      printf("trying %s\n", fileName.latin1());
#endif // PRINTF_DEBUG
      fileInfo.setFile(dir, fileName);
      if(fileInfo.exists())
	{
	  fileExists = true;
	  index++;
	}
    }
  while(fileExists);
  return fileInfo.absFilePath();
}

//------------------------------------------------------------------------------
bool SoundFile::openRead(const char *filename_)
{
  uninit();
  setSaved(true);

  setFilename(filename_);
  setFilteredFilename(getNextTempFilename());
  fprintf(stderr, "Opening file: %s\n(FilteredFilename: %s)\n", filename, filteredFilename);
  if(str_case_cmp(getFileExtension(filename), "wav") == 0)
    {
      stream = new WaveStream;
      filteredStream = new WaveStream;
    }
  else
    {
      fprintf(stderr, "Cannot open file of this type. %s\n", filename);
      return false;
    }

  if(stream->open_read(filename))
    {
      fprintf(stderr, "Error opening %s\n", filename);
      return false;
    }
  if(filteredStream->open_write(filteredFilename, stream->freq, stream->channels, stream->bits))
    {
      fprintf(stderr, "Error opening %s\n", filteredFilename);
      delete stream; stream = NULL;
      QString s = QString("Error opening ") + QString(filteredFilename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
      QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
      g_main_window->menuPreferences();
      return false;
    }

  channels.resize(stream->channels);
  int windowSize_ = gdata->getAnalysisBufferSize(stream->freq);
  fprintf(stderr, "channels = %d\n", numChannels());

  int stepSize_ = gdata->getAnalysisStepSize(stream->freq);
  _offset = windowSize_ / 2;
  setFramesPerChunk(stepSize_); // The user needs to be able to set this

  for(int j=0; j<numChannels(); j++)
    {
      channels(j) = new Channel(this, windowSize_);
      fprintf(stderr, "channel size = %d\n", channels(j)->size());
      channels(j)->setColor(gdata->getNextColor());
    }
  myTransforms.init(windowSize_, 0, stream->freq, gdata->doingEqualLoudness());

  fprintf(stderr, "----------Opening------------\n");
  fprintf(stderr, "filename = %s\n", filename_);
  fprintf(stderr, "rate = %d\n", rate());
  fprintf(stderr, "channels = %d\n", numChannels());
  fprintf(stderr, "bits = %d\n", bits());
  fprintf(stderr, "windowSize = %d\n", windowSize_);
  fprintf(stderr, "stepSize = %d\n", stepSize_);
  fprintf(stderr, "-----------------------------\n");

  //setup the tempChunkBuffers
  tempWindowBuffer = new float *[numChannels()];
  tempWindowBufferDouble = new double *[numChannels()];
  tempWindowBufferFiltered = new float *[numChannels()];
  tempWindowBufferFilteredDouble = new double *[numChannels()];
  for(int c=0; c<numChannels(); c++)
    {
      tempWindowBuffer[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferDouble[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferFiltered[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferFilteredDouble[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
    }

  _doingDetailedPitch = gdata->doingDetailedPitch();

  return true;
}

//------------------------------------------------------------------------------
bool SoundFile::openWrite(const char *filename_, int rate_, int channels_, int bits_, int windowSize_, int stepSize_)
{
  uninit();
  setSaved(false);

  _offset = windowSize_ / 2;
  fprintf(stderr, "--------Recording------------\n");
  fprintf(stderr, "filename = %s\n", filename_);
  fprintf(stderr, "rate = %d\n", rate_);
  fprintf(stderr, "channels = %d\n", channels_);
  fprintf(stderr, "bits = %d\n", bits_);
  fprintf(stderr, "windowSize = %d\n", windowSize_);
  fprintf(stderr, "stepSize = %d\n", stepSize_);
  fprintf(stderr, "-----------------------------\n");

  setFramesPerChunk(stepSize_);

  setFilename(filename_);
  setFilteredFilename(getNextTempFilename());
  
  stream = new WaveStream;
  filteredStream = new WaveStream;
  if(stream->open_write(filename, rate_, channels_, bits_))
    {
      fprintf(stderr, "Error opening %s for writting\n", filename);
      delete stream; stream = NULL;
      delete filteredStream; filteredStream = NULL;
      QString s = QString("Error opening ") + QString(filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
      QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
      g_main_window->menuPreferences();
      return false;
    }
  if(filteredStream->open_write(filteredFilename, rate_, channels_, bits_))
    {
      fprintf(stderr, "Error opening %s for writting\n", filteredFilename);
      delete stream; stream = NULL;
      delete filteredStream; filteredStream = NULL;
      QString s = QString("Error opening ") + QString(filteredFilename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
      QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
      g_main_window->menuPreferences();
      return false;
    }
#ifdef PRINTF_DEBUG
  printf("in_channels = %d\n", gdata->in_channels);
  printf("stream->channels=%d\n", stream->channels);
#endif // PRINTF_DEBUG

  channels.resize(stream->channels);
  fprintf(stderr, "channels = %d\n", numChannels());
  for(int j=0; j<numChannels(); j++)
    {
      channels(j) = new Channel(this, windowSize_);
      fprintf(stderr, "channel size = %d\n", channels(j)->size());
      channels(j)->setColor(gdata->getNextColor());
    }
  myTransforms.init(windowSize_, 0, stream->freq, gdata->doingEqualLoudness());

  //setup the tempChunkBuffers
  tempWindowBuffer = new float*[numChannels()];
  tempWindowBufferDouble = new double*[numChannels()];
  tempWindowBufferFiltered = new float*[numChannels()];
  tempWindowBufferFilteredDouble = new double*[numChannels()];
  for(int c=0; c<numChannels(); c++)
    {
      tempWindowBuffer[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferDouble[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferFiltered[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
      tempWindowBufferFilteredDouble[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
    }

  _doingDetailedPitch = gdata->doingDetailedPitch();

  return true;
}

//------------------------------------------------------------------------------
void SoundFile::rec2play(void)
{
  stream->close();
  stream->open_read(filename);
  filteredStream->close();
  filteredStream->open_read(filteredFilename);
  jumpToChunk(totalChunks());
  fprintf(stderr, "filteredFilename = %s\n", filteredFilename);
  fprintf(stderr, "totalChunks = %d\n", totalChunks());
}

//------------------------------------------------------------------------------
void SoundFile::lock(void)
{
  mutex->lock();
  for(int j=0; j<numChannels(); j++)
    {
      channels(j)->lock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::unlock(void)
{
  for(int j=0; j<numChannels(); j++)
    {
      channels(j)->unlock();
    }
  mutex->unlock();
}

//------------------------------------------------------------------------------
bool SoundFile::playChunk(void)
{
  if(!setupPlayChunk())
    {
      return false;
    }
  int ret = blockingWrite(gdata->getAudioStream(), tempWindowBuffer, framesPerChunk());
  if(ret < framesPerChunk())
    {
      fprintf(stderr, "Error writing to audio device\n");
    }
  return true;
}

//------------------------------------------------------------------------------
void SoundFile::applyEqualLoudnessFilter(int n)
{
  int c, j;
  for(c=0; c<numChannels(); c++)
    {
#ifdef PRINTF_DEBUG
      printf("before: %f == %f\n", channels(c)->filterStateX1, channels(c)->highPassFilter->_x[0]);
      printf("before: %f == %f\n", channels(c)->filterStateX2, channels(c)->highPassFilter->_x[1]);
      printf("before: %f == %f\n", channels(c)->filterStateY1, channels(c)->highPassFilter->_y[0]);
      printf("before: %f == %f\n", channels(c)->filterStateY2, channels(c)->highPassFilter->_y[1]);
#endif // PRINTF_DEBUG

      channels(c)->apply_highpass_filter(tempWindowBuffer[c], tempWindowBufferFiltered[c], n);
      for(j=0; j<n; j++)
	{
	  tempWindowBufferFiltered[c][j] = bound(tempWindowBufferFiltered[c][j], -1.0f, 1.0f);
	}
    }
}

//------------------------------------------------------------------------------
void SoundFile::recordChunk(int n)
{
  int ret = blockingRead(gdata->getAudioStream(), tempWindowBuffer, n);
  if(ret < n)
    {
      fprintf(stderr, "Data lost in reading from audio device\n");
    }
  finishRecordChunk(n);
}

//------------------------------------------------------------------------------
void SoundFile::finishRecordChunk(int n)
{
  if(equalLoudness())
    {
      applyEqualLoudnessFilter(n);
    }

  FilterState filterState;

  lock(); 
  for(int c=0; c<numChannels(); c++)
    {
      channels(c)->shift_left(n);
      toChannelBuffer(c, n);
      channels(c)->processNewChunk(&filterState);
    }
  unlock();

  int ret = blockingWrite(stream, tempWindowBuffer, n);
  if(ret < n)
    {
      fprintf(stderr, "Error writing to disk\n");
    }
  if(equalLoudness())
    {
      ret = blockingWrite(filteredStream, tempWindowBufferFiltered, n);
    }
  if(ret < n)
    {
      fprintf(stderr, "Error writing to disk\n");
    }
  setCurrentChunk(currentStreamChunk());
}

//------------------------------------------------------------------------------
bool SoundFile::setupPlayChunk(void)
{
  int c;
  int n = framesPerChunk();
  int ret = blockingRead(stream, tempWindowBuffer, n);
  if(equalLoudness())
    {
      ret = blockingRead(filteredStream, tempWindowBufferFiltered, n);
    }
  if(ret < n)
    {
      return false;
    }

  lock();
  for(c=0; c<numChannels(); c++)
    {
      channels(c)->shift_left(n);
      toChannelBuffer(c, n);
      if(gdata->doingActive() && channels(c) == gdata->getActiveChannel())
	{
	  channels(c)->processChunk(currentChunk()+1);
	}
    }
  setCurrentChunk(currentStreamChunk());
  unlock();
  return true;
}

//Note: static
//------------------------------------------------------------------------------
bool SoundFile::playRecordChunk(SoundFile *play, SoundFile *rec)
{
  int n = rec->framesPerChunk();
  myassert(n == play->framesPerChunk());
  myassert(rec->numChannels() == play->numChannels());

  play->setupPlayChunk();

  int ret = blockingWriteRead(gdata->getAudioStream(), play->tempWindowBuffer, play->numChannels(), rec->tempWindowBuffer, rec->numChannels(), n);
  if(ret < n)
    {
      fprintf(stderr, "Error writing/reading to audio device\n");
    }

  rec->finishRecordChunk(n);
  return true;
}

//------------------------------------------------------------------------------
int SoundFile::readChunk(int n)
{
  int c;
  
  int ret = blockingRead(stream, tempWindowBuffer, n);
  if(equalLoudness())
    {
      applyEqualLoudnessFilter(n);
      ret = blockingWrite(filteredStream, tempWindowBufferFiltered, ret);
    }
  if(ret < n)
    {
      return ret;
    }

  FilterState filterState;
  
  lock();

  for(c=0; c<numChannels(); c++)
    {
      channels(c)->shift_left(n);
      toChannelBuffer(c, n);
      channels(c)->processNewChunk(&filterState);
    }
  setCurrentChunk(currentStreamChunk());
  unlock();
  return ret;
}

//------------------------------------------------------------------------------
int SoundFile::blockingRead(SoundStream *s, float **buffer, int n)
{ 
  int c;
  myassert(s);
  myassert(n >= 0 && n <= bufferSize());
  int ret = s->readFloats(buffer, n, numChannels());
  if(ret < n)
    { //if not all bytes are read
      for(c=0; c<numChannels(); c++)
	{ //set remaining bytes to zeros
	  std::fill(buffer[c]+ret, buffer[c]+n, 0.0f);
	}
    }
  return ret;
}

//------------------------------------------------------------------------------
int SoundFile::blockingWrite(SoundStream *s, float **buffer, int n)
{
  if(s == NULL) return n;
  return s->writeFloats(buffer, n, numChannels());
}

//Note: static
//------------------------------------------------------------------------------
int SoundFile::blockingWriteRead(SoundStream *s, float **writeBuffer, int writeCh, float **readBuffer, int readCh, int n)
{
  int c;
  myassert(s);
  int ret = s->writeReadFloats(writeBuffer, writeCh, readBuffer, readCh, n);
  if(ret < n)
    { //if not all bytes are read
      for(c=0; c<readCh; c++)
	{ //set remaining bytes to zeros
	  std::fill(readBuffer[c]+ret, readBuffer[c]+n, 0.0f);
	}
    }
  return ret;
}

//------------------------------------------------------------------------------
void SoundFile::toChannelBuffer(int c, int n)
{
  std::copy(tempWindowBuffer[c], tempWindowBuffer[c]+n, channels(c)->end() - n);
  if(equalLoudness())
    {
      std::copy(tempWindowBufferFiltered[c], tempWindowBufferFiltered[c] + n, channels(c)->get_filtered_input().end() - n);
    }
}

//------------------------------------------------------------------------------
void SoundFile::toChannelBuffers(int n)
{
  int c;
  lock();
  myassert(n >= 0 && n <= bufferSize());
  for(c=0; c<numChannels(); c++)
    {
      channels(c)->shift_left(n);
      toChannelBuffer(c, n);
    }
  unlock();
}

//------------------------------------------------------------------------------
int SoundFile::readN(int n)
{
  myassert(n >= 0 && n <= bufferSize());
  int ret = blockingRead(stream, tempWindowBuffer, n);
  if(equalLoudness())
    {
      ret = blockingRead(filteredStream, tempWindowBufferFiltered, n);
    }
  toChannelBuffers(n);
  return ret;
}

//------------------------------------------------------------------------------
void SoundFile::preProcess(void)
{
  gdata->setDoingActiveAnalysis(true);
  myassert(firstTimeThrough == true);
  readChunk(bufferSize() - offset());

  // Create a progress bar in the status bar to tell the user we're preprocessing
  MainWindow *theMainWindow = (MainWindow*) qApp->mainWidget();
  QStatusBar *theStatusBar = theMainWindow->statusBar();
  QLabel *message = new QLabel("Preprocessing data:", theStatusBar, "message");

  Q3ProgressBar *progress = new Q3ProgressBar(stream->totalFrames() / framesPerChunk(), theStatusBar, "progress bar");
  progress->setProgress(0);
  progress->setMaximumHeight(16);


  theStatusBar->addWidget(message);
  theStatusBar->addWidget(progress);

  message->show();
  progress->show();

  int frameCount = 1;
  int updateInterval = MAX(1, progress->totalSteps() / 50); // We'll update 50 times only

  while(readChunk(framesPerChunk()) == framesPerChunk())
    { // put data in channels
#ifdef PRINTF_DEBUG
      printf("pos = %d\n", stream->pos);
#endif // PRINTF_DEBUG
      frameCount++;

      if (frameCount % updateInterval == 0)
	{
	  progress->setProgress(progress->progress() + updateInterval);
	  qApp->processEvents();
	  frameCount = 1;
	}
    }
#ifdef PRINTF_DEBUG
  printf("totalChunks=%d\n", totalChunks());
  printf("currentChunks=%d\n", currentChunk());
#endif // PRINTF_DEBUG
  filteredStream->close();
  filteredStream->open_read(filteredFilename);
  jumpToChunk(0);


  progress->setProgress(progress->totalSteps());
  theStatusBar->removeWidget(progress);
  theStatusBar->removeWidget(message);
  delete progress;
  delete message;

  gdata->setDoingActiveAnalysis(false);
  firstTimeThrough = false;
#ifdef PRINTF_DEBUG
  printf("freqLookup.size()=%d\n", channels(0)->freqLookup.size());
#endif // PRINTF_DEBUG
}

//------------------------------------------------------------------------------
void SoundFile::jumpToChunk(int chunk)
{
  int c;
  lock();
  int pos = chunk * framesPerChunk() - offset();
  if(pos < 0)
    {
      stream->jump_to_frame(0);
      if(equalLoudness())
	{
	  filteredStream->jump_to_frame(0);
	}
      for(c=0; c<numChannels(); c++)
	{
	  channels(c)->reset();
	}
      readN(bufferSize() + pos);
    }
  else
    {
      stream->jump_to_frame(pos);
      if(equalLoudness())
	{
	  filteredStream->jump_to_frame(pos);
	}
      readN(bufferSize());
    }
  setCurrentChunk(chunk);
  unlock();
}

//------------------------------------------------------------------------------
int SoundFile::bufferSize(void) const
{
  myassert(!channels.isEmpty());
  return channels(0)->size();
}

//------------------------------------------------------------------------------
int SoundFile::totalChunks(void) const
{
  myassert(!channels.isEmpty());
  return channels(0)->totalChunks();
}

//------------------------------------------------------------------------------
bool SoundFile::inFile(void) const
{
  int c = currentChunk();
  return (c >= 0 && c < totalChunks());
}

//------------------------------------------------------------------------------
void SoundFile::close(void)
{
  uninit();
}

//------------------------------------------------------------------------------
void SoundFile::processNewChunk(void)
{
  FilterState filterState;
  for(int j=0; j<numChannels(); j++)
    {
      channels(j)->lock();
      channels(j)->processNewChunk(&filterState);
      channels(j)->unlock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::shift_left(int n)
{
  for(int j=0; j<numChannels(); j++)
    {
      channels(j)->shift_left(n);
    }
}
//EOF
