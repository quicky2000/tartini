/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
void GData::set_play_or_record(bool p_value)
{
    m_play_or_record = p_value & !getSettingsBoolValue("Display/updateForEachChunk");
}

//------------------------------------------------------------------------------
void GData::do_nothing()const
{
}

//------------------------------------------------------------------------------
bool GData::settingsContains(const std::string & p_key)const
{
    return m_settings->contains(QString::fromStdString(p_key));
}

//------------------------------------------------------------------------------
int GData::getSettingsValue( const std::string & p_key
                           , const int & p_default_value
                           )const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key, p_default_value).toInt();
}

//------------------------------------------------------------------------------
bool GData::getSettingsValue( const std::string & p_key
                            , const bool & p_default_value
                            )const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key, p_default_value).toBool();
}

//------------------------------------------------------------------------------
std::string GData::getSettingsValue( const std::string & p_key
                                   , const std::string & p_default_value
                                   )const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    std::string l_result = m_settings->value(l_key, QString::fromStdString(p_default_value)).toString().toStdString();
    return l_result;
}

//------------------------------------------------------------------------------
QPoint GData::getSettingsValue( const std::string & p_key
                              , const QPoint & p_default_value
                              )const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key, p_default_value).toPoint();
}

//------------------------------------------------------------------------------
QSize GData::getSettingsValue( const std::string & p_key
                             , const QSize & p_default_value
                             )const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key, p_default_value).toSize();
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const std::string & p_key
                            , const std::string & p_value
                            )
{
    m_settings->setValue(QString::fromStdString(p_key), QString::fromStdString(p_value));
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const std::string & p_key
                            , const int & p_value
                            )
{
    m_settings->setValue(QString::fromStdString(p_key), p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const std::string & p_key
                            , const QPoint & p_value
                            )
{
    m_settings->setValue(QString::fromStdString(p_key), p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const std::string & p_key
                            , const QSize & p_value
                            )
{
    m_settings->setValue(QString::fromStdString(p_key), p_value);
}

//------------------------------------------------------------------------------
void GData::clearSettings()
{
    m_settings->clear();
}

//------------------------------------------------------------------------------
void GData::syncSettings()
{
    m_settings->sync();
}

//------------------------------------------------------------------------------
int GData::getSettingsIntValue(const std::string & p_key)const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key).toInt();
}

//------------------------------------------------------------------------------
bool GData::getSettingsBoolValue(const std::string & p_key)const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    return m_settings->value(l_key).toBool();
}

//------------------------------------------------------------------------------
std::string GData::getSettingsStringValue(const std::string & p_key)const
{
    QString l_key = QString::fromStdString(p_key);
    assert(m_settings->contains(l_key));
    std::string l_result = m_settings->value(l_key).toString().toStdString();
    return l_result;
}

//------------------------------------------------------------------------------
int GData::getSoundMode()const
{
    return m_sound_mode;
}

//------------------------------------------------------------------------------
void GData::setSoundMode(const int & p_mode)
{
    m_sound_mode = p_mode;
}

//------------------------------------------------------------------------------
AudioStream * GData::getAudioStream()const
{
    return m_audio_stream;
}

//------------------------------------------------------------------------------
void GData::setAudioStream(AudioStream * p_audio_stream)
{
    m_audio_stream = p_audio_stream;
}

//------------------------------------------------------------------------------
bool GData::needUpdate()const
{
    return m_need_update;
}

//------------------------------------------------------------------------------
void GData::setNeedUpdate(bool p_need_update)
{
    m_need_update = p_need_update;
}

//------------------------------------------------------------------------------
const AudioThread & GData::getAudioThread()const
{
    return m_audio_thread;
}

//------------------------------------------------------------------------------
void GData::stopAndWaitAudioThread()
{
    m_audio_thread.stopAndWait();
}

//------------------------------------------------------------------------------
void GData::setRunning(int p_running)
{
    m_running = p_running;
}

//------------------------------------------------------------------------------
int GData::getRunning()const
{
    return m_running;
}

//------------------------------------------------------------------------------
size_t GData::getChannelsSize()const
{
    return m_channels.size();
}

//------------------------------------------------------------------------------
Channel * GData::getChannelAt(size_t p_index)const
{
    return m_channels.at(p_index);
}

//------------------------------------------------------------------------------
const View & GData::getView()const
{
    assert(m_view);
    return *m_view;
}

//------------------------------------------------------------------------------
View & GData::getView()
{
    assert(m_view);
    return *m_view;
}

//------------------------------------------------------------------------------
void GData::setView(View & p_view)
{
    m_view = &p_view;
    connect(m_view, SIGNAL(onFastUpdate(double)), this, SLOT(doFastChunkUpdate()));
}

//------------------------------------------------------------------------------
Channel * GData::getActiveChannel()
{
    return m_active_channel;
}

//------------------------------------------------------------------------------
QPixmap * GData::drawingBuffer()
{
    return m_drawing_buffer;
}


//------------------------------------------------------------------------------
const double & GData::leftTime()const
{
    return m_left_time;
}

//------------------------------------------------------------------------------
const double & GData::rightTime()const
{
    return m_right_time;
}

//------------------------------------------------------------------------------
double GData::totalTime()const
{
    return m_right_time - m_left_time;
}

//------------------------------------------------------------------------------
const double & GData::topPitch()const
{
    return m_top_pitch;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveAnalysis(bool p_x)
{
    m_doing_active_analysis += (p_x) ? 1 : -1;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveFFT(bool p_x)
{
    m_doing_active_FFT += (p_x) ? 1 : -1;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveCepstrum(bool p_x)
{
    m_doing_active_cepstrum += (p_x) ? 1 : -1; setDoingActiveFFT(p_x);
}

//------------------------------------------------------------------------------
void GData::setDoingDetailedPitch(bool p_x)
{
    m_doing_detailed_pitch = p_x;
}

//------------------------------------------------------------------------------
bool GData::doingHarmonicAnalysis()const
{
    return m_doing_harmonic_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingAutoNoiseFloor()const
{
    return m_doing_auto_noise_floor;
}

//------------------------------------------------------------------------------
bool GData::doingEqualLoudness()const
{
    return m_doing_equal_loudness;
}

//------------------------------------------------------------------------------
bool GData::doingFreqAnalysis()const
{
    return m_doing_freq_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveAnalysis()const
{
    return m_doing_active_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveFFT()const
{
    return m_doing_active_FFT;
}

//------------------------------------------------------------------------------
bool GData::doingActiveCepstrum()const
{
    return m_doing_active_cepstrum;
}

//------------------------------------------------------------------------------
bool GData::doingDetailedPitch()const
{
    return m_doing_detailed_pitch;
}

//------------------------------------------------------------------------------
bool GData::doingActive()const
{
    return (doingActiveAnalysis() || doingActiveFFT() || doingActiveCepstrum());
}

//------------------------------------------------------------------------------
bool GData::vibratoSineStyle()const
{
    return m_vibrato_sine_style;
}

//------------------------------------------------------------------------------
int GData::amplitudeMode()const
{
    return m_amplitude_mode;
}

//------------------------------------------------------------------------------
int GData::pitchContourMode()const
{
    return m_pitch_contour_mode;
}

//------------------------------------------------------------------------------
int GData::fastUpdateSpeed()const
{
    return m_fast_update_speed;
}

//------------------------------------------------------------------------------
int GData::slowUpdateSpeed()const
{
    return m_slow_update_speed;
}

//------------------------------------------------------------------------------
bool GData::mouseWheelZooms()const
{
    return m_mouse_wheel_zooms;
}

//------------------------------------------------------------------------------
int GData::analysisType()const
{
    return m_analysis_type;
}

//------------------------------------------------------------------------------
bool GData::polish()const
{
  return m_polish;
}

//------------------------------------------------------------------------------
bool GData::showMeanVarianceBars()const
{
    return m_show_mean_variance_bars;
}

//------------------------------------------------------------------------------
int GData::savingMode()const
{
    return m_saving_mode;
}

//------------------------------------------------------------------------------
const QColor & GData::backgroundColor()const
{
    return m_background_color;
}

//------------------------------------------------------------------------------
const QColor & GData::shading1Color()const
{
    return m_shading_1_color;
}

//------------------------------------------------------------------------------
const QColor & GData::shading2Color()const
{
    return m_shading_2_Color;
}

//------------------------------------------------------------------------------
const double & GData::dBFloor()const
{
    return m_dB_floor;
}

//------------------------------------------------------------------------------
void GData::setDBFloor(double p_dB_floor)
{
    m_dB_floor = p_dB_floor;
}

//------------------------------------------------------------------------------
const double & GData::rmsFloor()const
{
    return m_amp_thresholds[AMPLITUDE_RMS][0];
}

//------------------------------------------------------------------------------
const double & GData::rmsCeiling()const
{
    return m_amp_thresholds[AMPLITUDE_RMS][1];
}

//------------------------------------------------------------------------------
void GData::set_rms_floor(const double & p_rms_floor)
{
    m_amp_thresholds[AMPLITUDE_RMS][0] = p_rms_floor;
}

//------------------------------------------------------------------------------
void GData::set_rms_ceiling(const double & p_rms_ceiling)
{
    m_amp_thresholds[AMPLITUDE_RMS][1] = p_rms_ceiling;
}

//------------------------------------------------------------------------------
int GData::musicKey()const
{
    return g_music_key;
}

//------------------------------------------------------------------------------
int GData::musicKeyType()const
{
    return m_music_key_type;
}

//------------------------------------------------------------------------------
int GData::temperedType()const
{
    return m_tempered_type;
}

//------------------------------------------------------------------------------
const double & GData::freqA()const
{
    return m_freq_A;
}

//------------------------------------------------------------------------------
const double & GData::semitoneOffset()const
{
    return m_semitone_offset;
}

//------------------------------------------------------------------------------
void GData::setMusicKey(int p_key)
{
    if(g_music_key != p_key)
    {
        g_music_key = p_key;
        emit musicKeyChanged(p_key);
    }
}

//------------------------------------------------------------------------------
void GData::setMusicKeyType(int p_type)
{
    if(m_music_key_type != p_type)
    {
        m_music_key_type = p_type;
        emit musicKeyTypeChanged(p_type);
    }
}

//------------------------------------------------------------------------------
void GData::setFreqA(int p_x)
{
    setFreqA(double(p_x));
}
//EOF
