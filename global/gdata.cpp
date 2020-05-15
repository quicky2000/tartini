/***************************************************************************
                          g_data.cpp  -
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
#include <QFileDialog>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <unistd.h>
#endif

#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <stdio.h>

#include "gdata.h"
#include "wave_stream.h"
#ifdef USE_SOX
#include "sox_stream.h"
#else
#if USE_OGG
#include "ogg_stream.h"
#endif
#endif
#include "audio_stream.h"
#include "Filter.h"
#include "mystring.h"
#include "soundfile.h"
#include "channel.h"
#include "tartinisettingsdialog.h"
#include "savedialog.h"
#include "conversions.h"
#include "musicnotes.h"
#include "music_scale.h"
#include <sstream>
#include <iomanip>

#ifndef WINDOWS
//for multi-threaded profiling
struct itimerval g_profiler_value;
struct itimerval g_profiler_ovalue;
#endif

GData * GData::g_data = nullptr;

//Define the Phase function. This one is applicable to 
//accelerating sources since the phase goes as x^2.
//------------------------------------------------------------------------------
float phase_function(float p_x)
{
    float phase;
    //phase = p_x*p_x;
    phase = p_x;
    return(phase);
}

//------------------------------------------------------------------------------
GData::GData()
: m_settings(nullptr)
, m_play_or_record(false)
, m_sound_mode(SoundMode::SOUND_PLAY)
, m_audio_stream(nullptr)
, m_need_update(false)
, m_running(RunningMode::STREAM_STOP)
, m_next_color_index(0)
, m_view(nullptr)
, m_active_channel(nullptr)
, m_doing_harmonic_analysis(false)
, m_doing_freq_analysis(false)
, m_doing_equal_loudness(false)
, m_doing_auto_noise_floor(false)
, m_doing_active_analysis(false)
, m_doing_active_FFT(false)
, m_doing_active_cepstrum(false)
, m_fast_update_speed(0)
, m_slow_update_speed(0)
, m_polish(true)
, m_show_mean_variance_bars(false)
, m_saving_mode(SavingModes::ALWAYS_ASK)
, m_vibrato_sine_style(false)
, m_music_key(3) // C
, m_music_scale(MusicScale::ScaleType::Chromatic) //ALL_NOTES
, m_music_temperament(MusicTemperament::TemperamentType::Even) //EVEN_TEMPERED
, m_mouse_wheel_zooms(false)
, m_freq_A(440)
, m_semitone_offset(0.0)
, m_amplitude_mode(t_amplitude_modes::AMPLITUDE_RMS)
, m_pitch_contour_mode(0)
, m_analysis_type(AnalysisModes::MPM)
, m_dB_floor(-150.0)
, m_drawing_buffer(new QPixmap(1, 1))
, m_left_time(1.0) // Put a dummy value so that setLeftTime will be completely executed
, m_right_time(0.0) // Put a dummy value so that setRightTime will be completely executed
, m_top_pitch(128.0)
{
    setLeftTime(0.0);
    setRightTime(5.0);

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)][0]           = -85.0;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_RMS)][1]           = -0.0;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_MAX_INTENSITY)][0] = -30.0;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_MAX_INTENSITY)][1] = -20.0;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)][0]   =  0.40;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::AMPLITUDE_CORRELATION)][1]   =  1.00;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::FREQ_CHANGENESS)][0]         =  0.50;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::FREQ_CHANGENESS)][1]         =  0.02;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::DELTA_FREQ_CENTROID)][0]     =  0.00;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::DELTA_FREQ_CENTROID)][1]     =  0.10;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::NOTE_SCORE)][0]              =  0.03;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::NOTE_SCORE)][1]              =  0.20;

    m_amp_thresholds[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE)][0]       =  0.12;
    m_amp_thresholds[static_cast<int>(t_amplitude_modes::NOTE_CHANGE_SCORE)][1]       =  0.30;

    m_amp_weights[0] = 0.2;
    m_amp_weights[1] = 0.2;
    m_amp_weights[2] = 0.2;
    m_amp_weights[3] = 0.2;
    m_amp_weights[4] = 0.2;

    m_settings = new QSettings("cs.otago.ac.nz", TARTINI_NAME_STR);
    TartiniSettingsDialog::setUnknownsToDefault(*this);

    updateQuickRefSettings();

    m_line_color.push_back(Qt::darkRed);
    m_line_color.push_back(Qt::darkGreen);
    m_line_color.push_back(Qt::darkBlue);
    m_line_color.push_back(Qt::darkCyan);
    m_line_color.push_back(Qt::darkMagenta);
    m_line_color.push_back(Qt::darkYellow);
    m_line_color.push_back(Qt::darkGray);

    initMusicStuff();
}

//------------------------------------------------------------------------------
GData::~GData()
{
    delete m_view;
    m_view = nullptr;
    m_audio_thread.stopAndWait();

    m_settings->sync();

    //Note: The m_sound_files is responsible for cleaning up the data the channels point to
    m_channels.clear();
    for(unsigned int l_j = 0; l_j < m_sound_files.size(); l_j++)
    {
        delete m_sound_files[l_j];
    }
    m_sound_files.clear();
      
    std::vector<Filter *>::iterator l_filter_iter;
    for(l_filter_iter = m_filter_hp.begin(); l_filter_iter != m_filter_hp.end(); ++l_filter_iter)
    {
        delete (*l_filter_iter);
    }
    m_filter_hp.clear();
    for(l_filter_iter = m_filter_lp.begin(); l_filter_iter != m_filter_lp.end(); ++l_filter_iter)
    {
        delete (*l_filter_iter);
    }

    m_filter_lp.clear();

    delete m_settings;

    delete m_drawing_buffer;
}

//------------------------------------------------------------------------------
SoundFile * GData::getActiveSoundFile()
{
    return (m_active_channel) ? m_active_channel->getParent() : nullptr;
}

//------------------------------------------------------------------------------
void GData::pauseSound()
{
    if(m_running == RunningMode::STREAM_FORWARD)
    {
        m_running = RunningMode::STREAM_PAUSE;
    }
    else if(m_running == RunningMode::STREAM_PAUSE)
    {
        m_running = RunningMode::STREAM_FORWARD;
    }
}

/**
   Opens the soundcard for recording.
   @param p_sound_file_rec A new created blank SoundFile to recort to
   @param p_soundfile_play A SoundFile to play when recording or NULL for record only
   @return true if success
*/
bool GData::openPlayRecord( SoundFile * p_sound_file_rec
                          , SoundFile * p_soundfile_play
                          )
{
    SoundStream::t_open_mode l_open_mode;

    stop();

    set_play_or_record(true);

    int l_rate = p_sound_file_rec->rate();
    int l_num_channels = p_sound_file_rec->numChannels();
    int l_bits = p_sound_file_rec->bits();
    int l_buffer_size = p_sound_file_rec->framesPerChunk();

    if(p_soundfile_play)
    {
        m_sound_mode = SoundMode::SOUND_PLAY_REC;
        l_open_mode = SoundStream::t_open_mode::F_RDWR;
        p_soundfile_play->jumpToChunk(0);
        g_data->m_view->setCurrentTime(0);
    }
    else
    {
        m_sound_mode = SoundMode::SOUND_REC;
        l_open_mode = SoundStream::t_open_mode::F_READ;
    }

    //open the audio input
    m_audio_stream = new AudioStream;
    if(m_audio_stream->open(l_open_mode, l_rate, l_num_channels, l_bits, l_buffer_size))
    {
        fprintf(stderr, "Error initilizing sound\n");
        delete m_audio_stream;
        m_audio_stream = nullptr;
        return false;
    }
    else
    {
        if(m_audio_stream->is_write_mode())
        {
            m_audio_thread.start(p_soundfile_play, p_sound_file_rec);
        }
        else
        {
            m_audio_thread.start(nullptr, p_sound_file_rec);
        }
    }
    return true;
}

/**
 * Starts playing the SoundFile s
 * @return true if the sound started playing (or is already playing)
*/
bool GData::playSound(SoundFile * p_sound_file)
{
    if(p_sound_file == nullptr)
    {
        return false;
    }
    if(m_running == RunningMode::STREAM_PAUSE)
    {
        m_running = RunningMode::STREAM_FORWARD;
        return true;
    }
    if(m_running != RunningMode::STREAM_STOP)
    {
        return true;
    }

    stop();
    set_play_or_record(true);
    int l_chunk = p_sound_file->currentChunk();
    if(l_chunk < 0 || l_chunk + 1 >= p_sound_file->totalChunks())
    {
        p_sound_file->jumpToChunk(0); /*< If at the end of the file reset to the start */
    }

    m_sound_mode = SoundMode::SOUND_PLAY;

    if(!m_audio_stream)
    {
        m_audio_stream = new AudioStream;
        if(m_audio_stream->open(SoundStream::t_open_mode::F_WRITE, p_sound_file->rate(), p_sound_file->numChannels(), p_sound_file->bits(), p_sound_file->bufferSize() / 2))
        {
            fprintf(stderr, "Error initialising sound\n");
            delete m_audio_stream;
            m_audio_stream = nullptr;
            g_main_window->message("Error opening sound device. Another program might be using it", 2000);
        }
        m_audio_thread.start(p_sound_file, nullptr);
    }
    return true;
}

//------------------------------------------------------------------------------
void GData::updateViewLeftRightTimes()
{
    double l_left = 0.0; //in seconds
    double l_right = 0.0; //in seconds
    Channel * l_channel;
    for(unsigned int l_j = 0; l_j < m_channels.size(); l_j++)
    {
        l_channel = m_channels.at(l_j);
        if(l_channel->startTime() < l_left)
        {
            l_left = l_channel->startTime();
        }
        if(l_channel->finishTime() > l_right)
        {
            l_right = l_channel->finishTime();
        }
    }
    setLeftTime(l_left); //in seconds
    setRightTime(l_right); //in seconds
}

//------------------------------------------------------------------------------
void GData::setActiveChannel(Channel * p_to_active)
{
    m_active_channel = p_to_active;
    if(m_active_channel)
    {
        updateActiveChunkTime(m_view->currentTime());
    }
    emit activeChannelChanged(m_active_channel);
    emit activeIntThresholdChanged(getActiveIntThreshold());
    m_view->doUpdate();
}

//------------------------------------------------------------------------------
void GData::updateActiveChunkTime(double p_time)
{
    if((m_running != RunningMode::STREAM_STOP) && isSoundModeRecording())
    {
        return;
    }

    Channel * l_active_channel = getActiveChannel();
    p_time = bound(p_time, leftTime(), rightTime());
    if(l_active_channel)
    {
        l_active_channel->jumpToTime(p_time);
        if(g_data->doingActive())
        {
            l_active_channel->lock();
            l_active_channel->processChunk(l_active_channel->currentChunk());
            l_active_channel->unlock();
        }
    }
    m_view->setCurrentTime(p_time);
    doChunkUpdate();
}

//------------------------------------------------------------------------------
void GData::setLeftTime(double p_x)
{
    if(p_x != m_left_time)
    {
        m_left_time = p_x;
        emit timeRangeChanged(leftTime(), rightTime());
        emit leftTimeChanged(leftTime());
    }
}

//------------------------------------------------------------------------------
void GData::setRightTime(double p_x)
{
    if(p_x != m_right_time)
    {
        m_right_time = p_x;
        emit timeRangeChanged(leftTime(), rightTime());
        emit rightTimeChanged(rightTime());
    }
}

//------------------------------------------------------------------------------
void GData::setTopPitch(double p_y)
{
    if(p_y != m_top_pitch)
    {
        m_top_pitch = p_y;
    }
}

//------------------------------------------------------------------------------
void GData::beginning()
{
    updateActiveChunkTime(leftTime());
    m_view->doSlowUpdate();
}


//------------------------------------------------------------------------------
void GData::rewind()
{
    double l_speed = 16;
    double l_diff_time = m_view->zoomX() * l_speed;
    if(getActiveChannel())
    {
        l_diff_time = MAX(l_diff_time, getActiveChannel()->timePerChunk());
    }
    updateActiveChunkTime(m_view->currentTime() - l_diff_time);
    m_view->doSlowUpdate();
}

//------------------------------------------------------------------------------
bool GData::play()
{
    return playSound(getActiveSoundFile());
}

//------------------------------------------------------------------------------
void GData::stop()
{
    set_play_or_record(false);
    m_audio_thread.stopAndWait();
}

//------------------------------------------------------------------------------
void GData::fastforward()
{
    double l_speed = 16;
    double l_diff_time = m_view->zoomX() * l_speed;
    if(getActiveChannel())
    {
        l_diff_time = MAX(l_diff_time, getActiveChannel()->timePerChunk());
    }
    updateActiveChunkTime(m_view->currentTime() + l_diff_time);
    m_view->doSlowUpdate();
}
  
//------------------------------------------------------------------------------
void GData::end()
{
    updateActiveChunkTime(rightTime());
    m_view->doSlowUpdate();
}

//------------------------------------------------------------------------------
int GData::getAnalysisBufferSize(int p_rate)
{  
    int l_window_size = m_settings->value("Analysis/bufferSizeValue", 48).toInt();
    QString l_window_size_unit = m_settings->value("Analysis/bufferSizeUnit", "milli-seconds").toString();
    if(l_window_size_unit.toLower() == "milli-seconds")
    {
        //convert to samples
        l_window_size = int(double(l_window_size) * double(p_rate) / 1000.0);
    }
    if(m_settings->value("Analysis/bufferSizeRound", true).toBool())
    {
        l_window_size = toInt(nearestPowerOf2(l_window_size));
    }
    return l_window_size;
}

//------------------------------------------------------------------------------
int GData::getAnalysisStepSize(int p_rate)
{  
    int l_step_size = m_settings->value("Analysis/stepSizeValue", 24).toInt();
    QString l_step_size_unit = m_settings->value("Analysis/stepSizeUnit", "milli-seconds").toString();
    if(l_step_size_unit.toLower() == "milli-seconds")
    {
        //convert to samples
        l_step_size = int(double(l_step_size) * double(p_rate) / 1000.0);
    }
    if(m_settings->value("Analysis/stepSizeRound", true).toBool())
    {
        l_step_size = toInt(nearestPowerOf2(l_step_size));
    }
    return l_step_size;
}

//------------------------------------------------------------------------------
void GData::updateQuickRefSettings()
{
    m_background_color.setNamedColor(m_settings->value("Display/theBackgroundColor", "#BBCDE2").toString());
    m_shading_1_color.setNamedColor(m_settings->value("Display/shading1Color", "#BBCDEF").toString());
    m_shading_2_Color.setNamedColor(m_settings->value("Display/shading2Color", "#CBCDE2").toString());
    m_doing_harmonic_analysis = m_settings->value("Analysis/doingHarmonicAnalysis", true).toBool();
    m_doing_freq_analysis = m_settings->value("Analysis/doingFreqAnalysis", true).toBool();
    m_doing_equal_loudness = m_settings->value("Analysis/doingEqualLoudness", true).toBool();
    m_doing_auto_noise_floor = m_settings->value("Analysis/doingAutoNoiseFloor", true).toBool();
    m_doing_detailed_pitch = m_settings->value("Analysis/doingDetailedPitch", false).toBool();
    m_fast_update_speed = m_settings->value("Display/fastUpdateSpeed", 75).toInt();
    m_slow_update_speed = m_settings->value("Display/slowUpdateSpeed", 150).toInt();
    m_vibrato_sine_style = m_settings->value("Advanced/vibratoSineStyle", false).toBool();
    QString l_analysis_string = m_settings->value("Analysis/analysisType", "MPM").toString();
    if(l_analysis_string == QString("MPM"))
    {
        m_analysis_type = AnalysisModes::MPM;
    }
    else if(l_analysis_string == QString("AUTOCORRELATION"))
    {
        m_analysis_type = AnalysisModes::AUTOCORRELATION;
    }
    else
    {
        m_analysis_type = AnalysisModes::MPM_MODIFIED_CEPSTRUM;
    }
    m_show_mean_variance_bars = m_settings->value("Advanced/showMeanVarianceBars", false).toBool();
    QString l_string = m_settings->value("Advanced/savingMode", "Ask when closing unsaved files (normal)").toString();
    if(l_string == "Ask when closing unsaved files (normal)")
    {
        m_saving_mode = SavingModes::ALWAYS_ASK;
    }
    else if(l_string == "Don't ask when closing unsaved files (use with care)")
    {
        m_saving_mode = SavingModes::NEVER_SAVE;
    }
    else
    {
        m_saving_mode = SavingModes::ALWAYS_SAVE;
    }
    m_mouse_wheel_zooms = m_settings->value("Advanced/mouseWheelZooms", false).toBool();
    setFreqA(m_settings->value("View/freqA", 440).toInt());
}

//------------------------------------------------------------------------------
QString GData::getFilenameString()
{
    QString l_file_generating_string = m_settings->value("General/filenameGeneratingString", "Untitled").toString();
    QString l_filename;
    int l_file_generating_number = m_settings->value("General/fileGeneratingNumber", 1).toInt();
    int l_digits = m_settings->value("General/fileNumberOfDigits", 2).toInt();
    if(l_digits == 0)
    {
        l_filename = QString::fromStdString(l_file_generating_string.toStdString() + ".wav");
    }
    else
    {
        std::stringstream l_stream;
        l_stream << l_file_generating_string.toStdString() << std::setfill('0') << std::setw(l_digits) << l_file_generating_number;
        l_filename = QString::fromStdString(l_stream.str() + ".wav");
    }
    return l_filename;
}

//------------------------------------------------------------------------------
QColor GData::getNextColor()
{
    return m_line_color.at((m_next_color_index++) % m_line_color.size());
}

//------------------------------------------------------------------------------
void GData::addFileToList(SoundFile * p_sound_file)
{
    int l_channel_index;
    m_sound_files.push_back(p_sound_file);
    for(l_channel_index = 0; l_channel_index < p_sound_file->numChannels(); l_channel_index++)
    {
      m_channels.push_back(&(p_sound_file->getChannel(l_channel_index)));
    }
    emit channelsChanged();
}

//------------------------------------------------------------------------------
void GData::removeFileFromList(SoundFile * p_sound_file)
{
    int l_j;
    int l_cur_pos;
    int l_prev_pos;
    //remove all the channels in p_sound_file from the channels list
    for(l_j = 0; l_j < p_sound_file->numChannels(); l_j++)
    {
        Channel * l_channel = &(p_sound_file->getChannel(l_j));
        l_cur_pos = l_prev_pos = 0;
        for(std::vector<Channel *>::iterator l_channel_iter_1 = m_channels.begin(); l_channel_iter_1 != m_channels.end(); l_channel_iter_1++, l_cur_pos++)
        {
            if((*l_channel_iter_1) == l_channel)
            {
                if(l_channel == getActiveChannel())
                {
                    l_prev_pos = l_cur_pos;
                }
                l_channel_iter_1 = m_channels.erase(l_channel_iter_1) - 1;
            }
        }
        if(m_channels.empty())
        {
            setActiveChannel(nullptr);
        }
        else
        {
            setActiveChannel(m_channels.at(bound(l_prev_pos, 0, int(m_channels.size() - 1))));
        }
    }
    //remove the soundFile from the m_sound_files list
    for(std::vector<SoundFile *>::iterator l_sound_file_iter_2 = m_sound_files.begin(); l_sound_file_iter_2 != m_sound_files.end(); l_sound_file_iter_2++)
    {
        if((*l_sound_file_iter_2) == p_sound_file)
        {
            l_sound_file_iter_2 = m_sound_files.erase(l_sound_file_iter_2) - 1;
        }
    }
    emit channelsChanged();
}

//------------------------------------------------------------------------------
void GData::saveActiveFile()
{
    SoundFile * l_sound_file = getActiveSoundFile();
    if(!l_sound_file)
    {
        return;
    }
    if(l_sound_file->saved())
    {
        return;
    }
    if(m_audio_thread.playSoundFile() == l_sound_file || m_audio_thread.recSoundFile() == l_sound_file)
    {
        stop();
    }
    int l_val = saveFile(l_sound_file, saveFileAsk(QString::fromStdString(l_sound_file->getFileName())));
    if(l_val == 0)
    {
        //success
        emit channelsChanged();
    }
    else if(l_val == -1)
    {
        QMessageBox::warning(g_main_window, "Error", QString("Error saving file '") + QString::fromStdString(l_sound_file->getFileName()) + QString("'"), QMessageBox::Ok, Qt::NoButton);
    }
}

//------------------------------------------------------------------------------
QString GData::saveFileAsk(QString p_old_filename)
{
    QString l_new_filename = SaveDialog::getSaveWavFileName(g_main_window);
    if(l_new_filename.isNull())
    {
        return QString();
    }
    l_new_filename = QDir::toNativeSeparators(l_new_filename);
    if(l_new_filename != p_old_filename && QFile::exists(l_new_filename))
    {
        if(QMessageBox::warning(g_main_window, tr("Overwrite File?"),
                                QString("A file called '") + l_new_filename + QString("' already exists.\n Do you want to overwrite it?"),
			      tr("&Yes"), tr("&No"), QString(), 0, 1 )
		  )
        {
            return QString(); //user canceled overwrite
        }
    }
    return l_new_filename;
}

/**
  @return 0 if the file was saved, 1 if cancled, or -1 if an error occured
*/
int GData::saveFile( SoundFile * p_sound_file
                   , QString p_new_filename
                   )
{
    if(p_new_filename.isNull())
    {
        return 1;
    }
    QString l_old_filename = QString::fromStdString(p_sound_file->getFileName());
    l_old_filename = QDir::toNativeSeparators(l_old_filename);
    int l_pos = p_sound_file->getStream().pos();
    p_sound_file->getStream().close();
  
    int l_ret = (moveFile(l_old_filename.toStdString(), p_new_filename.toStdString())) ? 0 : -1;
    if(l_ret == 0)
    {
        p_sound_file->getStream().open_read(p_new_filename.toStdString());
        p_sound_file->getStream().jump_to_frame(l_pos);
        p_sound_file->setSaved(true);
        p_sound_file->setFilename(p_new_filename.toStdString());
    }
    else
    {
        p_sound_file->getStream().open_read(l_old_filename.toStdString());
        p_sound_file->getStream().jump_to_frame(l_pos);
    }
    return l_ret;
}

/**
  @return true if all the files were closed. false if cancelled
*/
bool GData::closeAllFiles()
{
    while(!m_sound_files.empty())
    {
        if(closeFile(m_sound_files.at(0), g_data->savingMode()) == 1)
        {
            return false; //cancelled
        }
    }
    return true;
}

//------------------------------------------------------------------------------
void GData::closeActiveFile()
{
    SoundFile * l_sound_file = getActiveSoundFile();
    if(!l_sound_file)
    {
        return;
    }
    closeFile(l_sound_file, g_data->savingMode());
    m_view->doUpdate();
    doChunkUpdate();
}

/**
  @param p_sound_file The sound file to be closed (This pointer will become invalid after returning)
  //@param ask If true (default), the user will be asked to save the file if it's been modified.
  @return 0 if the file was closed, 1 if canclled, -1 if error
*/
int GData::closeFile( SoundFile * p_sound_file
                    , t_saving_modes p_saving_mode
                    )
{
    if(p_sound_file == nullptr)
    {
        return -1;
    }
    QString l_new_filename;
    QString l_old_filename = QString::fromStdString(p_sound_file->getFileName());
    l_old_filename = QDir::toNativeSeparators(l_old_filename);

    if(g_data->m_audio_thread.playSoundFile() == p_sound_file || g_data->m_audio_thread.recSoundFile() == p_sound_file)
    {
        g_data->stop();
    }

    if(p_sound_file->saved())
    {
        //file is already saved
        removeFileFromList(p_sound_file);
        delete p_sound_file;
        return 0;
    }

    if(p_saving_mode == SavingModes::ALWAYS_ASK)
    {
        QString l_filename = QString(getFilenamePart(l_old_filename.toStdString()).c_str());
        int l_option = QMessageBox::question(nullptr
                                            , QString("Save changes to file '") + l_filename + "' ?"
                                            , QString("Save changes to the file '") + l_filename + QString("' ?\n")
                                            , "&Yes"
                                            , "&No"
                                            , "&Cancel"
                                            , 0
                                            , 2
                                            );

        switch(l_option)
        {
            case 0: //Yes
                l_new_filename = saveFileAsk(l_old_filename);
                if(l_new_filename.isNull())
                {
                    return 1;
                }
                removeFileFromList(p_sound_file);
                delete p_sound_file;
                break;
            case 1: //No
                removeFileFromList(p_sound_file);
                delete p_sound_file;
                break;
            default: //Cancelled
                return 1;
        }
    }
    else if(p_saving_mode == SavingModes::NEVER_SAVE)
    {
        removeFileFromList(p_sound_file);
        delete p_sound_file;
    }
    else if(p_saving_mode == SavingModes::ALWAYS_SAVE)
    {
        removeFileFromList(p_sound_file);
        delete p_sound_file;
    }
    return 0;
}

//------------------------------------------------------------------------------
void GData::clearFreqLookup()
{
    for(std::vector<Channel *>::iterator l_channel_iterator = m_channels.begin(); l_channel_iterator != m_channels.end(); l_channel_iterator++)
    {
        (*l_channel_iterator)->clearFreqLookup();
    }
}

//------------------------------------------------------------------------------
void GData::setAmplitudeMode(t_amplitude_modes p_amplitude_mode)
{
    if(p_amplitude_mode != m_amplitude_mode)
    {
        m_amplitude_mode = p_amplitude_mode;
        clearAmplitudeLookup();
    }
}

//------------------------------------------------------------------------------
void GData::setPitchContourMode(int p_pitch_contour_mode)
{
    if(p_pitch_contour_mode != m_pitch_contour_mode)
    {
        m_pitch_contour_mode = p_pitch_contour_mode;
        clearFreqLookup();
    }
}

//------------------------------------------------------------------------------
void GData::clearAmplitudeLookup()
{
    for(std::vector<Channel *>::iterator l_channel_iter = m_channels.begin(); l_channel_iter != m_channels.end(); l_channel_iter++)
    {
        (*l_channel_iter)->clearAmplitudeLookup();
    }
}
  
//------------------------------------------------------------------------------
int GData::getActiveIntThreshold()
{
    Channel * l_active_channel = getActiveChannel();
    if(l_active_channel)
    {
        return toInt(l_active_channel->threshold() * 100.0f);
    }
    else
    {
        return m_settings->value("Analysis/thresholdValue", 93).toInt();
    }
}

//------------------------------------------------------------------------------
void GData::resetActiveIntThreshold(int p_threshold_percentage)
{
    Channel * l_active_channel = getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->resetIntThreshold(p_threshold_percentage);
    }
}

//------------------------------------------------------------------------------
void GData::setAmpThreshold( t_amplitude_modes p_mode
                           , int p_index
                           , double p_value
                           )
{
    m_amp_thresholds[static_cast<int>(p_mode)][p_index] = p_value;
    clearFreqLookup();
    recalcScoreThresholds();
}

//------------------------------------------------------------------------------
double GData::ampThreshold( t_amplitude_modes p_mode
                          , int p_index
                          )
{
    return m_amp_thresholds[static_cast<int>(p_mode)][p_index];
}

//------------------------------------------------------------------------------
void GData::setAmpWeight( int p_mode
                        , double p_value
                        )
{
    m_amp_weights[p_mode] = p_value;
    clearFreqLookup();
    recalcScoreThresholds();
}

//------------------------------------------------------------------------------
double GData::ampWeight(int p_mode)
{
    return m_amp_weights[p_mode];
}

//------------------------------------------------------------------------------
void GData::recalcScoreThresholds()
{
    for(std::vector<Channel *>::iterator l_channel_iter = m_channels.begin(); l_channel_iter != m_channels.end(); l_channel_iter++)
    {
        (*l_channel_iter)->recalcScoreThresholds();
    }
}

//------------------------------------------------------------------------------
void GData::doChunkUpdate()
{
    if(!m_play_or_record)
    {
        emit onChunkUpdate();
    }
}

//------------------------------------------------------------------------------
void GData::doFastChunkUpdate()
{
    if(m_play_or_record)
    {
        emit onChunkUpdate();
    }
}

//------------------------------------------------------------------------------
void GData::setMusicTemperament(int p_music_temperament)
{
    setMusicTemperament(static_cast<MusicTemperament::TemperamentType>(p_music_temperament));
}

//------------------------------------------------------------------------------
void GData::setMusicTemperament(MusicTemperament::TemperamentType p_music_temperament)
{
    if(m_music_temperament != p_music_temperament)
    {
        // If the current key type is not compatible with the new tempered type, then set the key type to Chromatic.
        if(!MusicScale::getScale(m_music_scale).isCompatibleWithTemparament(p_music_temperament))
        {
            setMusicScale(MusicScale::ScaleType::Chromatic);
        }
        m_music_temperament = p_music_temperament;
        emit musicTemperamentChanged(static_cast<int>(p_music_temperament));
    }
}

//------------------------------------------------------------------------------
void GData::setFreqA(double p_x)
{
    m_freq_A = p_x;
    m_semitone_offset = freq2pitch(p_x) - freq2pitch(440.0);
    m_settings->setValue("View/freqA", p_x);
}

//EOF
