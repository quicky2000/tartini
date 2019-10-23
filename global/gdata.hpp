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
void GData::do_nothing(void)const
{
}

//------------------------------------------------------------------------------
bool GData::settingsContains(const QString & p_key)const
{
    return m_settings->contains(p_key);
}

//------------------------------------------------------------------------------
int GData::getSettingsValue( const QString & p_key
                           , const int & p_default_value
                           )const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key, p_default_value).toInt();
}

//------------------------------------------------------------------------------
bool GData::getSettingsValue( const QString & p_key
                            , const bool & p_default_value
                            )const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key, p_default_value).toBool();
}

//------------------------------------------------------------------------------
QString GData::getSettingsValue( const QString & p_key
                               , const QString & p_default_value
                               )const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key, p_default_value).toString();
}

//------------------------------------------------------------------------------
QPoint GData::getSettingsValue( const QString & p_key
                              , const QPoint & p_default_value
                              )const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key, p_default_value).toPoint();
}

//------------------------------------------------------------------------------
QSize GData::getSettingsValue( const QString & p_key
                             , const QSize & p_default_value
                             )const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key, p_default_value).toSize();
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const QString & p_key
                            , const QString & p_value
                            )
{
    m_settings->setValue(p_key, p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const QString & p_key
                            , const int & p_value
                            )
{
    m_settings->setValue(p_key, p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const QString & p_key
                            , const QPoint & p_value
                            )
{
    m_settings->setValue(p_key, p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue( const QString & p_key
                            , const QSize & p_value
                            )
{
    m_settings->setValue(p_key, p_value);
}

//------------------------------------------------------------------------------
void GData::clearSettings(void)
{
    m_settings->clear();
}

//------------------------------------------------------------------------------
void GData::syncSettings(void)
{
    m_settings->sync();
}

//------------------------------------------------------------------------------
int GData::getSettingsIntValue(const QString & p_key)const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key).toInt();
}

//------------------------------------------------------------------------------
bool GData::getSettingsBoolValue(const QString & p_key)const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key).toBool();
}

//------------------------------------------------------------------------------
QString GData::getSettingsStringValue(const QString & p_key)const
{
    assert(m_settings->contains(p_key));
    return m_settings->value(p_key).toString();
}

//------------------------------------------------------------------------------
int GData::getSoundMode(void)const
{
    return m_sound_mode;
}

//------------------------------------------------------------------------------
void GData::setSoundMode(const int & p_mode)
{
    m_sound_mode = p_mode;
}

//------------------------------------------------------------------------------
AudioStream * GData::getAudioStream(void)const
{
    return m_audio_stream;
}

//------------------------------------------------------------------------------
void GData::setAudioStream(AudioStream * p_audio_stream)
{
    m_audio_stream = p_audio_stream;
}

//------------------------------------------------------------------------------
bool GData::needUpdate(void)const
{
    return m_need_update;
}

//------------------------------------------------------------------------------
void GData::setNeedUpdate(bool p_need_update)
{
    m_need_update = p_need_update;
}

//------------------------------------------------------------------------------
const AudioThread & GData::getAudioThread(void)const
{
    return m_audio_thread;
}

//------------------------------------------------------------------------------
void GData::stopAndWaitAudioThread(void)
{
    m_audio_thread.stopAndWait();
}

//------------------------------------------------------------------------------
void GData::setRunning(int p_running)
{
    m_running = p_running;
}

//------------------------------------------------------------------------------
int GData::getRunning(void)const
{
    return m_running;
}

//------------------------------------------------------------------------------
size_t GData::getChannelsSize(void)const
{
    return m_channels.size();
}

//------------------------------------------------------------------------------
Channel * GData::getChannelAt(size_t p_index)const
{
    return m_channels.at(p_index);
}

//------------------------------------------------------------------------------
const View & GData::getView(void)const
{
    assert(m_view);
    return *m_view;
}

//------------------------------------------------------------------------------
View & GData::getView(void)
{
    assert(m_view);
    return *m_view;
}

//------------------------------------------------------------------------------
void GData::setView(View & p_view)
{
    m_view = &p_view;
}

//------------------------------------------------------------------------------
Channel * GData::getActiveChannel(void)
{
    return m_active_channel;
}

//------------------------------------------------------------------------------
QPixmap * GData::drawingBuffer(void)
{
    return m_drawing_buffer;
}


//------------------------------------------------------------------------------
const double & GData::leftTime(void)const
{
    return m_left_time;
}

//------------------------------------------------------------------------------
const double & GData::rightTime(void)const
{
    return m_right_time;
}

//------------------------------------------------------------------------------
double GData::totalTime(void)const
{
    return m_right_time - m_left_time;
}

//------------------------------------------------------------------------------
const double & GData::topPitch(void)const
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
bool GData::doingHarmonicAnalysis(void)const
{
    return m_doing_harmonic_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingAutoNoiseFloor(void)const
{
    return m_doing_auto_noise_floor;
}

//------------------------------------------------------------------------------
bool GData::doingEqualLoudness(void)const
{
    return m_doing_equal_loudness;
}

//------------------------------------------------------------------------------
bool GData::doingFreqAnalysis(void)const
{
    return m_doing_freq_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveAnalysis(void)const
{
    return m_doing_active_analysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveFFT(void)const
{
    return m_doing_active_FFT;
}

//------------------------------------------------------------------------------
bool GData::doingActiveCepstrum(void)const
{
    return m_doing_active_cepstrum;
}

//------------------------------------------------------------------------------
bool GData::doingDetailedPitch(void)const
{
    return m_doing_detailed_pitch;
}

//------------------------------------------------------------------------------
bool GData::doingActive(void)const
{
    return (doingActiveAnalysis() || doingActiveFFT() || doingActiveCepstrum());
}

//------------------------------------------------------------------------------
bool GData::vibratoSineStyle(void)const
{
    return m_vibrato_sine_style;
}

//------------------------------------------------------------------------------
int GData::amplitudeMode(void)const
{
    return m_amplitude_mode;
}

//------------------------------------------------------------------------------
int GData::pitchContourMode(void)const
{
    return m_pitch_contour_mode;
}

//------------------------------------------------------------------------------
int GData::fastUpdateSpeed(void)const
{
    return m_fast_update_speed;
}

//------------------------------------------------------------------------------
int GData::slowUpdateSpeed(void)const
{
    return m_slow_update_speed;
}

//------------------------------------------------------------------------------
bool GData::mouseWheelZooms(void)const
{
    return m_mouse_wheel_zooms;
}

//------------------------------------------------------------------------------
int GData::analysisType(void)const
{
    return m_analysis_type;
}

//------------------------------------------------------------------------------
bool GData::polish(void)const
{
  return m_polish;
}

//------------------------------------------------------------------------------
bool GData::showMeanVarianceBars(void)const
{
    return m_show_mean_variance_bars;
}

//------------------------------------------------------------------------------
int GData::savingMode(void)const
{
    return m_saving_mode;
}

//------------------------------------------------------------------------------
const QColor & GData::backgroundColor(void)const
{
    return m_background_color;
}

//------------------------------------------------------------------------------
const QColor & GData::shading1Color(void)const
{
    return m_shading_1_color;
}

//------------------------------------------------------------------------------
const QColor & GData::shading2Color(void)const
{
    return m_shading_2_Color;
}

//------------------------------------------------------------------------------
const double & GData::dBFloor(void)const
{
    return m_dB_floor;
}

//------------------------------------------------------------------------------
void GData::setDBFloor(double p_dB_floor)
{
    m_dB_floor = p_dB_floor;
}

//------------------------------------------------------------------------------
const double & GData::rmsFloor(void)const
{
    return m_amp_thresholds[AMPLITUDE_RMS][0];
}

//------------------------------------------------------------------------------
const double & GData::rmsCeiling(void)const
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
int GData::musicKey(void)const
{
    return g_music_key;
}

//------------------------------------------------------------------------------
int GData::musicKeyType(void)const
{
    return m_music_key_type;
}

//------------------------------------------------------------------------------
int GData::temperedType(void)const
{
    return m_tempered_type;
}

//------------------------------------------------------------------------------
const double & GData::freqA(void)const
{
    return m_freq_A;
}

//------------------------------------------------------------------------------
const double & GData::semitoneOffset(void)const
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
