/***************************************************************************
                           soundfile.cpp  -  description
                              -------------------
     begin                : Sat Jul 10 2004
     copyright            : (C) 2004-2005 by Philip McLeod
     email                : pmcleod@cs.otago.ac.nz
     copyright            : (C) 2019 by Julien Thevenon
     email                : julien_thevenon at yahoo.fr

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

#include <QProgressBar>
#include "mainwindow.h"
#include <QStatusBar>
#include <QLabel>
#include <QDir>

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
    m_mutex = new QMutex(QMutex::Recursive);
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
    if(g_data->getAudioThread().playSoundFile() == this || g_data->getAudioThread().recSoundFile() == this)
    {
        g_data->stop();
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
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        delete m_channels(l_j);
        delete[] (m_temp_window_buffer[l_j] - 16);
        delete[] (m_temp_window_buffer_double[l_j] - 16);
        delete[] (m_temp_window_buffer_filtered[l_j] - 16);
        delete[] (m_temp_window_buffer_filtered_double[l_j] - 16);
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
    QString l_temp_file_folder = g_data->getSettingsValue("General/tempFilesFolder", QDir::convertSeparators(QDir::currentPath()));
    QDir l_dir = QDir(l_temp_file_folder);
    QFileInfo l_file_info;
    QString l_file_name;
    bool l_file_exists;
    int index = 1;
    do
    {
        l_file_exists = false;
        l_file_name.sprintf("temp%03d.wav", index);
#ifdef PRINTF_DEBUG
        printf("trying %s\n", fileName.latin1());
#endif // PRINTF_DEBUG
        l_file_info.setFile(l_dir, l_file_name);
        if(l_file_info.exists())
        {
            l_file_exists = true;
            index++;
        }
    }
    while(l_file_exists);
    return l_file_info.absoluteFilePath();
}

//------------------------------------------------------------------------------
bool SoundFile::openRead(const char * p_filename)
{
    uninit();
    setSaved(true);

    setFilename(p_filename);
    QByteArray l_array = getNextTempFilename().toLocal8Bit();
    setFilteredFilename(l_array.data());
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
    if(m_filtered_stream->open_write(m_filtered_filename, m_stream->get_frequency(), m_stream->get_channels(), m_stream->get_bits()))
    {
        fprintf(stderr, "Error opening %s\n", m_filtered_filename);
        delete m_stream; m_stream = NULL;
        QString l_string = QString("Error opening ") + QString(m_filtered_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", l_string, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }

    m_channels.resize(m_stream->get_channels());
    int l_window_size_ = g_data->getAnalysisBufferSize(m_stream->get_frequency());
    fprintf(stderr, "channels = %d\n", numChannels());

    int l_step_size = g_data->getAnalysisStepSize(m_stream->get_frequency());
    m_offset = l_window_size_ / 2;
    setFramesPerChunk(l_step_size); // The user needs to be able to set this

    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j) = new Channel(this, l_window_size_);
        fprintf(stderr, "channel size = %d\n", m_channels(l_j)->size());
        m_channels(l_j)->setColor(g_data->getNextColor());
    }
    m_my_transforms.init(l_window_size_, 0, m_stream->get_frequency(), g_data->doingEqualLoudness());

    fprintf(stderr, "----------Opening------------\n");
    fprintf(stderr, "filename = %s\n", p_filename);
    fprintf(stderr, "rate = %d\n", rate());
    fprintf(stderr, "channels = %d\n", numChannels());
    fprintf(stderr, "bits = %d\n", bits());
    fprintf(stderr, "windowSize = %d\n", l_window_size_);
    fprintf(stderr, "stepSize = %d\n", l_step_size);
    fprintf(stderr, "-----------------------------\n");

    //setup the tempChunkBuffers
    m_temp_window_buffer = new float *[numChannels()];
    m_temp_window_buffer_double = new double *[numChannels()];
    m_temp_window_buffer_filtered = new float *[numChannels()];
    m_temp_window_buffer_filtered_double = new double *[numChannels()];
    for(int l_c = 0; l_c < numChannels(); l_c++)
    {
        m_temp_window_buffer[l_c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_double[l_c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered[l_c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered_double[l_c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
    }

    m_doing_detailed_pitch = g_data->doingDetailedPitch();

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
    QByteArray l_array = getNextTempFilename().toLocal8Bit();
    setFilteredFilename(l_array.data());
  
    m_stream = new WaveStream;
    m_filtered_stream = new WaveStream;
    if(m_stream->open_write(m_filename, p_rate, p_channels, p_bits))
    {
        fprintf(stderr, "Error opening %s for writting\n", m_filename);
        delete m_stream;
        m_stream = NULL;
        delete m_filtered_stream;
        m_filtered_stream = NULL;
        QString l_string = QString("Error opening ") + QString(m_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", l_string, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }
    if(m_filtered_stream->open_write(m_filtered_filename, p_rate, p_channels, p_bits))
    {
        fprintf(stderr, "Error opening %s for writting\n", m_filtered_filename);
        delete m_stream;
        m_stream = NULL;
        delete m_filtered_stream;
        m_filtered_stream = NULL;
        QString l_string = QString("Error opening ") + QString(m_filtered_filename) + QString(" for writing.\nPossible cause: temp folder is read-only or disk is out of space.\nPlease select a writable Temp Folder");
        QMessageBox::warning(g_main_window, "Error", l_string, QMessageBox::Ok, QMessageBox::NoButton);
        g_main_window->menuPreferences();
        return false;
    }
#ifdef PRINTF_DEBUG
    printf("in_channels = %d\n", g_data->in_channels);
    printf("stream->channels=%d\n", stream->channels);
#endif // PRINTF_DEBUG

    m_channels.resize(m_stream->get_channels());
    fprintf(stderr, "channels = %d\n", numChannels());
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j) = new Channel(this, p_window_size);
        fprintf(stderr, "channel size = %d\n", m_channels(l_j)->size());
        m_channels(l_j)->setColor(g_data->getNextColor());
    }
    m_my_transforms.init(p_window_size, 0, m_stream->get_frequency(), g_data->doingEqualLoudness());

    //setup the tempChunkBuffers
    m_temp_window_buffer = new float*[numChannels()];
    m_temp_window_buffer_double = new double*[numChannels()];
    m_temp_window_buffer_filtered = new float*[numChannels()];
    m_temp_window_buffer_filtered_double = new double*[numChannels()];
    for(int l_c = 0; l_c < numChannels(); l_c++)
    {
        m_temp_window_buffer[l_c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_double[l_c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered[l_c] = (new float[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
        m_temp_window_buffer_filtered_double[l_c] = (new double[bufferSize() + 16]) + 16; //array ranges from -16 to bufferSize()
    }

    m_doing_detailed_pitch = g_data->doingDetailedPitch();

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
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j)->lock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::unlock(void)
{
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j)->unlock();
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
    int l_ret = blockingWrite(g_data->getAudioStream(), m_temp_window_buffer, framesPerChunk());
    if(l_ret < framesPerChunk())
    {
        fprintf(stderr, "Error writing to audio device\n");
    }
    return true;
}

//------------------------------------------------------------------------------
void SoundFile::applyEqualLoudnessFilter(int p_n)
{
    int l_c;
    int l_j;
    for(l_c = 0; l_c < numChannels(); l_c++)
    {
#ifdef PRINTF_DEBUG
        printf("before: %f == %f\n", channels(l_c)->filterStateX1, channels(c)->m_high_pass_filter->m_x[0]);
        printf("before: %f == %f\n", channels(l_c)->filterStateX2, channels(c)->m_high_pass_filter->m_x[1]);
        printf("before: %f == %f\n", channels(l_c)->filterStateY1, channels(c)->m_high_pass_filter->m_y[0]);
        printf("before: %f == %f\n", channels(l_c)->filterStateY2, channels(c)->m_high_pass_filter->m_y[1]);
#endif // PRINTF_DEBUG

        m_channels(l_c)->apply_highpass_filter(m_temp_window_buffer[l_c], m_temp_window_buffer_filtered[l_c], p_n);
        for(l_j = 0; l_j < p_n; l_j++)
        {
            m_temp_window_buffer_filtered[l_c][l_j] = bound(m_temp_window_buffer_filtered[l_c][l_j], -1.0f, 1.0f);
        }
    }
}

//------------------------------------------------------------------------------
void SoundFile::recordChunk(int p_n)
{
    int l_ret = blockingRead(g_data->getAudioStream(), m_temp_window_buffer, p_n);
    if(l_ret < p_n)
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
    for(int l_c = 0; l_c < numChannels(); l_c++)
    {
        m_channels(l_c)->shift_left(p_n);
        toChannelBuffer(l_c, p_n);
        m_channels(l_c)->processNewChunk(&filterState);
    }
    unlock();

    int l_ret = blockingWrite(m_stream, m_temp_window_buffer, p_n);
    if(l_ret < p_n)
    {
        fprintf(stderr, "Error writing to disk\n");
    }
    if(equalLoudness())
    {
        l_ret = blockingWrite(m_filtered_stream, m_temp_window_buffer_filtered, p_n);
    }
    if(l_ret < p_n)
    {
        fprintf(stderr, "Error writing to disk\n");
    }
    setCurrentChunk(currentStreamChunk());
}

//------------------------------------------------------------------------------
bool SoundFile::setupPlayChunk(void)
{
    int l_c;
    int l_n = framesPerChunk();
    int ret = blockingRead(m_stream, m_temp_window_buffer, l_n);
    if(equalLoudness())
    {
        ret = blockingRead(m_filtered_stream, m_temp_window_buffer_filtered, l_n);
    }
    if(ret < l_n)
    {
        return false;
    }

    lock();
    for(l_c = 0; l_c < numChannels(); l_c++)
    {
        m_channels(l_c)->shift_left(l_n);
        toChannelBuffer(l_c, l_n);
        if(g_data->doingActive() && m_channels(l_c) == g_data->getActiveChannel())
        {
            m_channels(l_c)->processChunk(currentChunk() + 1);
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
    int l_n = p_rec->framesPerChunk();
    myassert(l_n == p_play_sound_file->framesPerChunk());
    myassert(p_rec->numChannels() == p_play_sound_file->numChannels());

    p_play_sound_file->setupPlayChunk();

    int l_ret = blockingWriteRead(g_data->getAudioStream(), p_play_sound_file->m_temp_window_buffer, p_play_sound_file->numChannels(), p_rec->m_temp_window_buffer, p_rec->numChannels(), l_n);
    if(l_ret < l_n)
    {
        fprintf(stderr, "Error writing/reading to audio device\n");
    }

    p_rec->finishRecordChunk(l_n);
    return true;
}

//------------------------------------------------------------------------------
int SoundFile::readChunk(int p_n)
{
    int l_c;
  
    int l_ret = blockingRead(m_stream, m_temp_window_buffer, p_n);
    if(equalLoudness())
    {
        applyEqualLoudnessFilter(p_n);
        l_ret = blockingWrite(m_filtered_stream, m_temp_window_buffer_filtered, l_ret);
    }
    if(l_ret < p_n)
    {
        return l_ret;
    }

    FilterState l_filter_state;
  
    lock();

    for(l_c = 0; l_c < numChannels(); l_c++)
    {
        m_channels(l_c)->shift_left(p_n);
        toChannelBuffer(l_c, p_n);
        m_channels(l_c)->processNewChunk(&l_filter_state);
    }
    setCurrentChunk(currentStreamChunk());
    unlock();
    return l_ret;
}

//------------------------------------------------------------------------------
int SoundFile::blockingRead( SoundStream * p_stream
                           , float ** p_buffer
                           , int p_n
                           )
{
    int l_c;
    myassert(p_stream);
    myassert(p_n >= 0 && p_n <= bufferSize());
    int l_ret = p_stream->readFloats(p_buffer, p_n, numChannels());
    if(l_ret < p_n)
    {
        //if not all bytes are read
        for(l_c = 0; l_c < numChannels(); l_c++)
        {
            //set remaining bytes to zeros
            std::fill(p_buffer[l_c] + l_ret, p_buffer[l_c] + p_n, 0.0f);
        }
    }
    return l_ret;
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
    int l_c;
    myassert(p_stream);
    int l_ret = p_stream->writeReadFloats(p_write_buffer, p_write_channel, p_read_buffer, p_read_channel, p_n);
    if(l_ret < p_n)
    {
        //if not all bytes are read
        for(l_c = 0; l_c < p_read_channel; l_c++)
        {
            //set remaining bytes to zeros
            std::fill(p_read_buffer[l_c] + l_ret, p_read_buffer[l_c] + p_n, 0.0f);
        }
    }
    return l_ret;
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
    int l_c;
    lock();
    myassert(p_n >= 0 && p_n <= bufferSize());
    for(l_c = 0; l_c < numChannels(); l_c++)
    {
        m_channels(l_c)->shift_left(p_n);
        toChannelBuffer(l_c, p_n);
    }
    unlock();
}

//------------------------------------------------------------------------------
int SoundFile::readN(int p_n)
{
    myassert(p_n >= 0 && p_n <= bufferSize());
    int l_ret = blockingRead(m_stream, m_temp_window_buffer, p_n);
    if(equalLoudness())
    {
        l_ret = blockingRead(m_filtered_stream, m_temp_window_buffer_filtered, p_n);
    }
    toChannelBuffers(p_n);
    return l_ret;
}

//------------------------------------------------------------------------------
void SoundFile::preProcess(void)
{
    g_data->setDoingActiveAnalysis(true);
    myassert(m_first_time_through == true);
    readChunk(bufferSize() - offset());

    // Create a progress bar in the status bar to tell the user we're preprocessing
    MainWindow * l_the_main_window = (MainWindow*) qApp->mainWidget();
    QStatusBar * l_the_status_bar = l_the_main_window->statusBar();
    QLabel * l_message = new QLabel("Preprocessing data:", l_the_status_bar, "message");

    QProgressBar * l_progress = new QProgressBar(l_the_status_bar);
    l_progress->setRange(0,m_stream->totalFrames() / framesPerChunk() - 1);
    l_progress->setValue(0);
    l_progress->setMaximumHeight(16);

    l_the_status_bar->addWidget(l_message);
    l_the_status_bar->addWidget(l_progress);

    l_message->show();
    l_progress->show();

    int l_frame_count = 1;
    int l_update_interval = MAX(1, (l_progress->maximum() - l_progress->minimum() +1) / 50); // We'll update 50 times only

    while(readChunk(framesPerChunk()) == framesPerChunk())
    {
        // put data in channels
#ifdef PRINTF_DEBUG
        printf("pos = %d\n", stream->pos);
#endif // PRINTF_DEBUG
        l_frame_count++;

        if(l_frame_count % l_update_interval == 0)
        {
            l_progress->setValue(l_progress->value() + l_update_interval);
            qApp->processEvents();
            l_frame_count = 1;
        }
    }
#ifdef PRINTF_DEBUG
    printf("totalChunks=%d\n", totalChunks());
    printf("currentChunks=%d\n", currentChunk());
#endif // PRINTF_DEBUG
    m_filtered_stream->close();
    m_filtered_stream->open_read(m_filtered_filename);
    jumpToChunk(0);

    l_progress->setValue(l_progress->maximum());
    l_the_status_bar->removeWidget(l_progress);
    l_the_status_bar->removeWidget(l_message);
    delete l_progress;
    delete l_message;

    g_data->setDoingActiveAnalysis(false);
    m_first_time_through = false;
#ifdef PRINTF_DEBUG
    printf("freqLookup.size()=%d\n", channels(0)->freqLookup.size());
#endif // PRINTF_DEBUG
}

//------------------------------------------------------------------------------
void SoundFile::jumpToChunk(int p_chunk)
{
    int l_c;
    lock();
    int l_pos = p_chunk * framesPerChunk() - offset();
    if(l_pos < 0)
    {
        m_stream->jump_to_frame(0);
        if(equalLoudness())
        {
            m_filtered_stream->jump_to_frame(0);
        }
      for(l_c = 0; l_c < numChannels(); l_c++)
      {
          m_channels(l_c)->reset();
      }
      readN(bufferSize() + l_pos);
    }
    else
    {
        m_stream->jump_to_frame(l_pos);
        if(equalLoudness())
        {
            m_filtered_stream->jump_to_frame(l_pos);
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
    int l_c = currentChunk();
    return (l_c >= 0 && l_c < totalChunks());
}

//------------------------------------------------------------------------------
void SoundFile::close(void)
{
    uninit();
}

//------------------------------------------------------------------------------
void SoundFile::processNewChunk(void)
{
    FilterState l_filter_state;
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j)->lock();
        m_channels(l_j)->processNewChunk(&l_filter_state);
        m_channels(l_j)->unlock();
    }
}

//------------------------------------------------------------------------------
void SoundFile::shift_left(int p_n)
{
    for(int l_j = 0; l_j < numChannels(); l_j++)
    {
        m_channels(l_j)->shift_left(p_n);
    }
}
//EOF
