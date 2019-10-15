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
    m_filename = NULL;
    m_filtered_filename = NULL;
    m_stream = NULL;
    m_filtered_stream = NULL;
    m_frames_per_chunk = 0;
    m_temp_window_buffer = NULL;
    m_temp_window_buffer_double = NULL;
    m_temp_window_buffer_filtered = NULL;
    m_temp_window_buffer_filtered_double = NULL;
    m_start_time = 0.0;
    m_chunk_num = 0;
    m_offset = 0; //Number of frame to read into file to get to time 0 (half buffer size).
    m_saved = true;
    m_mutex = new QMutex(true);
    m_first_time_through = true;
    m_doing_detailed_pitch = false;
}

//------------------------------------------------------------------------------
SoundFile::~SoundFile(void)
{
    uninit();
    delete m_mutex;
}

//------------------------------------------------------------------------------
void SoundFile::uninit(void)
{
    if(gdata->getAudioThread().playSoundFile() == this || gdata->getAudioThread().recSoundFile() == this)
    {
        gdata->stop();
    }

    if(m_stream)
    {
        delete m_stream; m_stream = NULL;
    }
    if(m_filtered_stream)
    {
        delete m_filtered_stream; m_filtered_stream = NULL;
        //Delete the temporary filtered file from disk!
        if(::remove(m_filtered_filename) == -1)
        {
            fprintf(stderr, "Error removing file %s\n", m_filtered_filename);
        }
    }
    setFilename(NULL);
    setFilteredFilename(NULL);
    for(int j = 0; j < numChannels(); j++)
    {
        delete m_channels(j);
        delete[] (m_temp_window_buffer[j] - 16);
        delete[] (m_temp_window_buffer_double[j] - 16);
        delete[] (m_temp_window_buffer_filtered[j] - 16);
        delete[] (m_temp_window_buffer_filtered_double[j] - 16);
    }
    m_channels.resize(0);
    delete[] m_temp_window_buffer; m_temp_window_buffer = NULL;
    delete[] m_temp_window_buffer_double; m_temp_window_buffer_double = NULL;
    delete[] m_temp_window_buffer_filtered; m_temp_window_buffer_filtered = NULL;
    delete[] m_temp_window_buffer_filtered_double; m_temp_window_buffer_filtered_double = NULL;

    setFramesPerChunk(0);
    m_start_time = 0.0;
    m_chunk_num = 0;
    m_offset = 0;
}

//------------------------------------------------------------------------------
void SoundFile::setFilename(const char * p_filename)
{
    if(m_filename)
    {
        free(m_filename);
        m_filename = NULL;
    }
    if(p_filename)
    {
        m_filename = copy_string(p_filename);
    }
}

//------------------------------------------------------------------------------
void SoundFile::setFilteredFilename(const char * p_filtered_filename)
{
    if(m_filtered_filename)
    {
        free(m_filtered_filename);
        m_filtered_filename = NULL;
    }
    if(p_filtered_filename)
    {
        m_filtered_filename = copy_string(p_filtered_filename);
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
bool SoundFile::openRead(const char * p_filename)
{
    uninit();
    setSaved(true);

    setFilename(p_filename);
    setFilteredFilename(getNextTempFilename());
    fprintf(stderr, "Opening file: %s\n(FilteredFilename: %s)\n", m_filename, m_filtered_filename);
    if(str_case_cmp(getFileExtension(m_filename), "wav") == 0)
    {
        m_stream = new WaveStream;
        m_filtered_stream = new WaveStream;
    }
    else
    {
        fprintf(stderr, "Cannot open file of this type. %s\n", m_filename);
        return false;
    }

    if(m_stream->open_read(m_filename))
    {
        fprintf(stderr, "Error opening %s\n", m_filename);
        return false;
    }
    if(m_filtered_stream->open_write(m_filtered_filename, m_stream->freq, m_stream->channels, m_stream->bits))
    {
        fprintf(stderr, "Error opening %s\n", m_filtered_filename);
        delete m_stream; m_stream = NULL;
        QString s = QString("Error opening ") + QString(m_filtered_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }

    m_channels.resize(m_stream->channels);
    int windowSize_ = gdata->getAnalysisBufferSize(m_stream->freq);
    fprintf(stderr, "channels = %d\n", numChannels());

    int stepSize_ = gdata->getAnalysisStepSize(m_stream->freq);
    m_offset = windowSize_ / 2;
    setFramesPerChunk(stepSize_); // The user needs to be able to set this

    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j) = new Channel(this, windowSize_);
        fprintf(stderr, "channel size = %d\n", m_channels(j)->size());
        m_channels(j)->setColor(gdata->getNextColor());
    }
    m_my_transforms.init(windowSize_, 0, m_stream->freq, gdata->doingEqualLoudness());

    fprintf(stderr, "----------Opening------------\n");
    fprintf(stderr, "filename = %s\n", p_filename);
    fprintf(stderr, "rate = %d\n", rate());
    fprintf(stderr, "channels = %d\n", numChannels());
    fprintf(stderr, "bits = %d\n", bits());
    fprintf(stderr, "windowSize = %d\n", windowSize_);
    fprintf(stderr, "stepSize = %d\n", stepSize_);
    fprintf(stderr, "-----------------------------\n");

    //setup the tempChunkBuffers
    m_temp_window_buffer = new float *[numChannels()];
    m_temp_window_buffer_double = new double *[numChannels()];
    m_temp_window_buffer_filtered = new float *[numChannels()];
    m_temp_window_buffer_filtered_double = new double *[numChannels()];
    for(int c = 0; c < numChannels(); c++)
    {
        m_temp_window_buffer[c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_double[c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered[c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered_double[c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
    }

    m_doing_detailed_pitch = gdata->doingDetailedPitch();

    return true;
}

//------------------------------------------------------------------------------
bool SoundFile::openWrite( const char * p_filename
                         , int p_rate
                         , int p_channels
                         , int p_bits
                         , int p_window_size
                         , int p_step_size
                         )
{
    uninit();
    setSaved(false);

    m_offset = p_window_size / 2;
    fprintf(stderr, "--------Recording------------\n");
    fprintf(stderr, "filename = %s\n", p_filename);
    fprintf(stderr, "rate = %d\n", p_rate);
    fprintf(stderr, "channels = %d\n", p_channels);
    fprintf(stderr, "bits = %d\n", p_bits);
    fprintf(stderr, "windowSize = %d\n", p_window_size);
    fprintf(stderr, "stepSize = %d\n", p_step_size);
    fprintf(stderr, "-----------------------------\n");

    setFramesPerChunk(p_step_size);

    setFilename(p_filename);
    setFilteredFilename(getNextTempFilename());
  
    m_stream = new WaveStream;
    m_filtered_stream = new WaveStream;
    if(m_stream->open_write(m_filename, p_rate, p_channels, p_bits))
    {
        fprintf(stderr, "Error opening %s for writting\n", m_filename);
        delete m_stream; m_stream = NULL;
        delete m_filtered_stream; m_filtered_stream = NULL;
        QString s = QString("Error opening ") + QString(m_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }
    if(m_filtered_stream->open_write(m_filtered_filename, p_rate, p_channels, p_bits))
    {
        fprintf(stderr, "Error opening %s for writting\n", m_filtered_filename);
        delete m_stream; m_stream = NULL;
        delete m_filtered_stream;
        m_filtered_stream = NULL;
        QString s = QString("Error opening ") + QString(m_filtered_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", s, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }
#ifdef PRINTF_DEBUG
    printf("in_channels = %d\n", gdata->in_channels);
    printf("stream->channels=%d\n", stream->channels);
#endif // PRINTF_DEBUG

    m_channels.resize(m_stream->channels);
    fprintf(stderr, "channels = %d\n", numChannels());
    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j) = new Channel(this, p_window_size);
        fprintf(stderr, "channel size = %d\n", m_channels(j)->size());
        m_channels(j)->setColor(gdata->getNextColor());
    }
    m_my_transforms.init(p_window_size, 0, m_stream->freq, gdata->doingEqualLoudness());

    //setup the tempChunkBuffers
    m_temp_window_buffer = new float*[numChannels()];
    m_temp_window_buffer_double = new double*[numChannels()];
    m_temp_window_buffer_filtered = new float*[numChannels()];
    m_temp_window_buffer_filtered_double = new double*[numChannels()];
    for(int c = 0; c < numChannels(); c++)
    {
        m_temp_window_buffer[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_double[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered[c] = (new float[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered_double[c] = (new double[bufferSize()+16]) + 16; //array ranges from -16 to bufferSize()
    }

    m_doing_detailed_pitch = gdata->doingDetailedPitch();

    return true;
}

//------------------------------------------------------------------------------
void SoundFile::rec2play(void)
{
    m_stream->close();
    m_stream->open_read(m_filename);
    m_filtered_stream->close();
    m_filtered_stream->open_read(m_filtered_filename);
    jumpToChunk(totalChunks());
    fprintf(stderr, "filteredFilename = %s\n", m_filtered_filename);
    fprintf(stderr, "totalChunks = %d\n", totalChunks());
}

//------------------------------------------------------------------------------
void SoundFile::lock(void)
{
    m_mutex->lock();
    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j)->lock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::unlock(void)
{
    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j)->unlock();
    }
    m_mutex->unlock();
}

//------------------------------------------------------------------------------
bool SoundFile::playChunk(void)
{
    if(!setupPlayChunk())
    {
        return false;
    }
    int ret = blockingWrite(gdata->getAudioStream(), m_temp_window_buffer, framesPerChunk());
    if(ret < framesPerChunk())
    {
        fprintf(stderr, "Error writing to audio device\n");
    }
    return true;
}

//------------------------------------------------------------------------------
void SoundFile::applyEqualLoudnessFilter(int p_n)
{
    int c;
    int j;
    for(c = 0; c < numChannels(); c++)
    {
#ifdef PRINTF_DEBUG
        printf("before: %f == %f\n", channels(c)->filterStateX1, channels(c)->highPassFilter->_x[0]);
        printf("before: %f == %f\n", channels(c)->filterStateX2, channels(c)->highPassFilter->_x[1]);
        printf("before: %f == %f\n", channels(c)->filterStateY1, channels(c)->highPassFilter->_y[0]);
        printf("before: %f == %f\n", channels(c)->filterStateY2, channels(c)->highPassFilter->_y[1]);
#endif // PRINTF_DEBUG

        m_channels(c)->apply_highpass_filter(m_temp_window_buffer[c], m_temp_window_buffer_filtered[c], p_n);
        for(j = 0; j < p_n; j++)
        {
            m_temp_window_buffer_filtered[c][j] = bound(m_temp_window_buffer_filtered[c][j], -1.0f, 1.0f);
        }
    }
}

//------------------------------------------------------------------------------
void SoundFile::recordChunk(int p_n)
{
    int ret = blockingRead(gdata->getAudioStream(), m_temp_window_buffer, p_n);
    if(ret < p_n)
    {
        fprintf(stderr, "Data lost in reading from audio device\n");
    }
    finishRecordChunk(p_n);
}

//------------------------------------------------------------------------------
void SoundFile::finishRecordChunk(int p_n)
{
    if(equalLoudness())
    {
        applyEqualLoudnessFilter(p_n);
    }

    FilterState filterState;

    lock();
    for(int c = 0; c < numChannels(); c++)
    {
        m_channels(c)->shift_left(p_n);
        toChannelBuffer(c, p_n);
        m_channels(c)->processNewChunk(&filterState);
    }
    unlock();

    int ret = blockingWrite(m_stream, m_temp_window_buffer, p_n);
    if(ret < p_n)
    {
        fprintf(stderr, "Error writing to disk\n");
    }
    if(equalLoudness())
    {
        ret = blockingWrite(m_filtered_stream, m_temp_window_buffer_filtered, p_n);
    }
    if(ret < p_n)
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
    int ret = blockingRead(m_stream, m_temp_window_buffer, n);
    if(equalLoudness())
    {
        ret = blockingRead(m_filtered_stream, m_temp_window_buffer_filtered, n);
    }
    if(ret < n)
    {
        return false;
    }

    lock();
    for(c = 0; c < numChannels(); c++)
    {
        m_channels(c)->shift_left(n);
        toChannelBuffer(c, n);
        if(gdata->doingActive() && m_channels(c) == gdata->getActiveChannel())
        {
            m_channels(c)->processChunk(currentChunk() + 1);
        }
    }
    setCurrentChunk(currentStreamChunk());
    unlock();
    return true;
}

//Note: static
//------------------------------------------------------------------------------
bool SoundFile::playRecordChunk( SoundFile * p_play_sound_file
                               , SoundFile * p_rec
                               )
{
    int n = p_rec->framesPerChunk();
    myassert(n == p_play_sound_file->framesPerChunk());
    myassert(p_rec->numChannels() == p_play_sound_file->numChannels());

    p_play_sound_file->setupPlayChunk();

    int ret = blockingWriteRead(gdata->getAudioStream(), p_play_sound_file->m_temp_window_buffer, p_play_sound_file->numChannels(), p_rec->m_temp_window_buffer, p_rec->numChannels(), n);
    if(ret < n)
    {
        fprintf(stderr, "Error writing/reading to audio device\n");
    }

    p_rec->finishRecordChunk(n);
    return true;
}

//------------------------------------------------------------------------------
int SoundFile::readChunk(int p_n)
{
    int c;
  
    int ret = blockingRead(m_stream, m_temp_window_buffer, p_n);
    if(equalLoudness())
    {
        applyEqualLoudnessFilter(p_n);
        ret = blockingWrite(m_filtered_stream, m_temp_window_buffer_filtered, ret);
    }
    if(ret < p_n)
    {
        return ret;
    }

    FilterState filterState;
  
    lock();

    for(c = 0; c < numChannels(); c++)
    {
        m_channels(c)->shift_left(p_n);
        toChannelBuffer(c, p_n);
        m_channels(c)->processNewChunk(&filterState);
    }
    setCurrentChunk(currentStreamChunk());
    unlock();
    return ret;
}

//------------------------------------------------------------------------------
int SoundFile::blockingRead( SoundStream * s
                           , float **p_buffer
                           , int n
                           )
{
    int c;
    myassert(s);
    myassert(n >= 0 && n <= bufferSize());
    int ret = s->readFloats(p_buffer, n, numChannels());
    if(ret < n)
    {
        //if not all bytes are read
        for(c = 0; c < numChannels(); c++)
        {
            //set remaining bytes to zeros
            std::fill(p_buffer[c] + ret, p_buffer[c] + n, 0.0f);
        }
    }
    return ret;
}

//------------------------------------------------------------------------------
int SoundFile::blockingWrite( SoundStream * p_stream
                            , float ** p_buffer
                            , int p_n
                            )
{
    if(p_stream == NULL)
    {

        return p_n;
    }
    return p_stream->writeFloats(p_buffer, p_n, numChannels());
}

//Note: static
//------------------------------------------------------------------------------
int SoundFile::blockingWriteRead( SoundStream * p_stream
                                , float ** p_write_buffer
                                , int p_write_channel
                                , float ** p_read_buffer
                                , int p_read_channel
                                , int p_n
                                )
{
    int c;
    myassert(p_stream);
    int ret = p_stream->writeReadFloats(p_write_buffer, p_write_channel, p_read_buffer, p_read_channel, p_n);
    if(ret < p_n)
    {
        //if not all bytes are read
        for(c = 0; c < p_read_channel; c++)
        {
            //set remaining bytes to zeros
            std::fill(p_read_buffer[c]+ret, p_read_buffer[c]+p_n, 0.0f);
        }
    }
    return ret;
}

//------------------------------------------------------------------------------
void SoundFile::toChannelBuffer( int p_c
                               , int p_n
                               )
{
    std::copy(m_temp_window_buffer[p_c], m_temp_window_buffer[p_c]+p_n, m_channels(p_c)->end() - p_n);
    if(equalLoudness())
    {
        std::copy(m_temp_window_buffer_filtered[p_c], m_temp_window_buffer_filtered[p_c] + p_n, m_channels(p_c)->get_filtered_input().end() - p_n);
    }
}

//------------------------------------------------------------------------------
void SoundFile::toChannelBuffers(int p_n)
{
    int c;
    lock();
    myassert(p_n >= 0 && p_n <= bufferSize());
    for(c = 0; c < numChannels(); c++)
    {
        m_channels(c)->shift_left(p_n);
        toChannelBuffer(c, p_n);
    }
    unlock();
}

//------------------------------------------------------------------------------
int SoundFile::readN(int n)
{
    myassert(n >= 0 && n <= bufferSize());
    int ret = blockingRead(m_stream, m_temp_window_buffer, n);
    if(equalLoudness())
    {
        ret = blockingRead(m_filtered_stream, m_temp_window_buffer_filtered, n);
    }
    toChannelBuffers(n);
    return ret;
}

//------------------------------------------------------------------------------
void SoundFile::preProcess(void)
{
    gdata->setDoingActiveAnalysis(true);
    myassert(m_first_time_through == true);
    readChunk(bufferSize() - offset());

    // Create a progress bar in the status bar to tell the user we're preprocessing
    MainWindow * theMainWindow = (MainWindow*) qApp->mainWidget();
    QStatusBar * theStatusBar = theMainWindow->statusBar();
    QLabel * message = new QLabel("Preprocessing data:", theStatusBar, "message");

    Q3ProgressBar * progress = new Q3ProgressBar(m_stream->totalFrames() / framesPerChunk(), theStatusBar, "progress bar");
    progress->setProgress(0);
    progress->setMaximumHeight(16);

    theStatusBar->addWidget(message);
    theStatusBar->addWidget(progress);

    message->show();
    progress->show();

    int frameCount = 1;
    int updateInterval = MAX(1, progress->totalSteps() / 50); // We'll update 50 times only

    while(readChunk(framesPerChunk()) == framesPerChunk())
    {
        // put data in channels
#ifdef PRINTF_DEBUG
        printf("pos = %d\n", stream->pos);
#endif // PRINTF_DEBUG
        frameCount++;

        if(frameCount % updateInterval == 0)
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
    m_filtered_stream->close();
    m_filtered_stream->open_read(m_filtered_filename);
    jumpToChunk(0);

    progress->setProgress(progress->totalSteps());
    theStatusBar->removeWidget(progress);
    theStatusBar->removeWidget(message);
    delete progress;
    delete message;

    gdata->setDoingActiveAnalysis(false);
    m_first_time_through = false;
#ifdef PRINTF_DEBUG
    printf("freqLookup.size()=%d\n", channels(0)->freqLookup.size());
#endif // PRINTF_DEBUG
}

//------------------------------------------------------------------------------
void SoundFile::jumpToChunk(int p_chunk)
{
    int c;
    lock();
    int pos = p_chunk * framesPerChunk() - offset();
    if(pos < 0)
    {
        m_stream->jump_to_frame(0);
        if(equalLoudness())
        {
            m_filtered_stream->jump_to_frame(0);
        }
      for(c = 0; c < numChannels(); c++)
      {
          m_channels(c)->reset();
      }
      readN(bufferSize() + pos);
    }
    else
    {
        m_stream->jump_to_frame(pos);
        if(equalLoudness())
        {
            m_filtered_stream->jump_to_frame(pos);
        }
        readN(bufferSize());
    }
    setCurrentChunk(p_chunk);
    unlock();
}

//------------------------------------------------------------------------------
int SoundFile::bufferSize(void) const
{
    myassert(!m_channels.isEmpty());
    return m_channels(0)->size();
}

//------------------------------------------------------------------------------
int SoundFile::totalChunks(void) const
{
    myassert(!m_channels.isEmpty());
    return m_channels(0)->totalChunks();
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
    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j)->lock();
        m_channels(j)->processNewChunk(&filterState);
        m_channels(j)->unlock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::shift_left(int p_n)
{
    for(int j = 0; j < numChannels(); j++)
    {
        m_channels(j)->shift_left(p_n);
    }
}
//EOF
