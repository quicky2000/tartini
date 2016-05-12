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
bool GData::settingsContains(const QString & p_key)const
{
  return qsettings->contains(p_key);

}

//------------------------------------------------------------------------------
int GData::getSettingsValue(const QString & p_key,const int & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toInt();

}

//------------------------------------------------------------------------------
bool GData::getSettingsValue(const QString & p_key,const bool & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toBool();

}

//------------------------------------------------------------------------------
QString GData::getSettingsValue(const QString & p_key,const QString & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toString();

}

//------------------------------------------------------------------------------
QPoint GData::getSettingsValue(const QString & p_key,const QPoint & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toPoint();

}

//------------------------------------------------------------------------------
QSize GData::getSettingsValue(const QString & p_key,const QSize & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toSize();

}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QString & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const int & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QPoint & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QSize & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::clearSettings(void)
{
  qsettings->clear();
}

//------------------------------------------------------------------------------
void GData::syncSettings(void)
{
  qsettings->sync();
}

//------------------------------------------------------------------------------
int GData::getSettingsIntValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toInt();

}

//------------------------------------------------------------------------------
bool GData::getSettingsBoolValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toBool();

}

//------------------------------------------------------------------------------
QString GData::getSettingsStringValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toString();

}

//------------------------------------------------------------------------------
int GData::getSoundMode(void)const
{
  return soundMode;
}

//------------------------------------------------------------------------------
void GData::setSoundMode(const int & p_mode)
{
  soundMode = p_mode;
}

//------------------------------------------------------------------------------
AudioStream * GData::getAudioStream(void)const
{
  return audio_stream;
}

//------------------------------------------------------------------------------
void GData::setAudioStream(AudioStream * p_audio_stream)
{
  audio_stream = p_audio_stream;
}

//------------------------------------------------------------------------------
bool GData::needUpdate(void)const
{
  return need_update;
}

//------------------------------------------------------------------------------
void GData::setNeedUpdate(bool p_need_update)
{
  need_update = p_need_update;
}

//------------------------------------------------------------------------------
const AudioThread & GData::getAudioThread(void)const
{
  return audioThread;
}

//------------------------------------------------------------------------------
void GData::stopAndWaitAudioThread(void)
{
  audioThread.stopAndWait();
}

//------------------------------------------------------------------------------
void GData::setRunning(int p_running)
{
  running = p_running;
}

//------------------------------------------------------------------------------
int GData::getRunning(void)const
{
  return running;
}

//------------------------------------------------------------------------------
size_t GData::getChannelsSize(void)const
{
  return channels.size();
}

//------------------------------------------------------------------------------
Channel* GData::getChannelAt(size_t p_index)const
{
  return channels.at(p_index);
}

//------------------------------------------------------------------------------
const View & GData::getView(void)const
{
  assert(view);
  return *view;
}

//------------------------------------------------------------------------------
View & GData::getView(void)
{
  assert(view);
  return *view;
}

//------------------------------------------------------------------------------
void GData::setView(View & p_view)
{
  view = & p_view;
}

//------------------------------------------------------------------------------
Channel* GData::getActiveChannel(void)
{
  return activeChannel;
}

//------------------------------------------------------------------------------
QPixmap* GData::drawingBuffer(void)
{
  return _drawingBuffer;
}


//------------------------------------------------------------------------------
const double & GData::leftTime(void)const
{
  return _leftTime;
}

//------------------------------------------------------------------------------
const double & GData::rightTime(void)const
{
  return _rightTime;
}

//------------------------------------------------------------------------------
double GData::totalTime(void)const
{
  return _rightTime - _leftTime;
}

//------------------------------------------------------------------------------
const double & GData::topPitch(void)const
{
  return _topPitch;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveAnalysis(bool x)
{
  _doingActiveAnalysis += (x) ? 1 : -1;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveFFT(bool x)
{
  _doingActiveFFT += (x) ? 1 : -1;
}

//------------------------------------------------------------------------------
void GData::setDoingActiveCepstrum(bool x)
{
  _doingActiveCepstrum += (x) ? 1 : -1; setDoingActiveFFT(x);
}

//------------------------------------------------------------------------------
void GData::setDoingDetailedPitch(bool x)
{
  _doingDetailedPitch = x;
}

//------------------------------------------------------------------------------
bool GData::doingHarmonicAnalysis(void)const
{
  return _doingHarmonicAnalysis;
}

//------------------------------------------------------------------------------
bool GData::doingAutoNoiseFloor(void)const
{
  return _doingAutoNoiseFloor;
}

//------------------------------------------------------------------------------
bool GData::doingEqualLoudness(void)const
{
  return _doingEqualLoudness;
}

//------------------------------------------------------------------------------
bool GData::doingFreqAnalysis(void)const
{
  return _doingFreqAnalysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveAnalysis(void)const
{
  return _doingActiveAnalysis;
}

//------------------------------------------------------------------------------
bool GData::doingActiveFFT(void)const
{
  return _doingActiveFFT;
}

//------------------------------------------------------------------------------
bool GData::doingActiveCepstrum(void)const
{
  return _doingActiveCepstrum;
}

//------------------------------------------------------------------------------
bool GData::doingDetailedPitch(void)const
{
  return _doingDetailedPitch;
}

//------------------------------------------------------------------------------
bool GData::doingActive(void)const
{
  return (doingActiveAnalysis() || doingActiveFFT() || doingActiveCepstrum());
}

//------------------------------------------------------------------------------
bool GData::vibratoSineStyle(void)const
{
  return _vibratoSineStyle;
}

//------------------------------------------------------------------------------
int GData::amplitudeMode(void)const
{
  return _amplitudeMode;
}

//------------------------------------------------------------------------------
int GData::pitchContourMode(void)const
{
  return _pitchContourMode;
}

//------------------------------------------------------------------------------
int GData::fastUpdateSpeed(void)const
{
  return _fastUpdateSpeed;
}

//------------------------------------------------------------------------------
int GData::slowUpdateSpeed(void)const
{
  return _slowUpdateSpeed;
}

//------------------------------------------------------------------------------
bool GData::mouseWheelZooms(void)const
{
  return _mouseWheelZooms;
}

//------------------------------------------------------------------------------
int GData::analysisType(void)const
{
  return _analysisType;
}

//------------------------------------------------------------------------------
bool GData::polish(void)const
{
  return _polish;
}

//------------------------------------------------------------------------------
bool GData::showMeanVarianceBars(void)const
{
  return _showMeanVarianceBars;
}

//------------------------------------------------------------------------------
int GData::savingMode(void)const
{
  return _savingMode;
}

//------------------------------------------------------------------------------
const QColor & GData::backgroundColor(void)const
{
  return _backgroundColor;
}

//------------------------------------------------------------------------------
const QColor & GData::shading1Color(void)const
{
  return _shading1Color;
}

//------------------------------------------------------------------------------
const QColor & GData::shading2Color(void)const
{
  return _shading2Color;
}

//------------------------------------------------------------------------------
const double & GData::dBFloor(void)const
{
  return _dBFloor;
}

//------------------------------------------------------------------------------
void GData::setDBFloor(double dBFloor_)
{
  _dBFloor = dBFloor_;
}

//------------------------------------------------------------------------------
double & GData::rmsFloor(void)
{
  return amp_thresholds[AMPLITUDE_RMS][0];
}

//------------------------------------------------------------------------------
double & GData::rmsCeiling(void)
{
  return amp_thresholds[AMPLITUDE_RMS][1];
}

//------------------------------------------------------------------------------
int GData::musicKey(void)const
{
  return gMusicKey;
}

//------------------------------------------------------------------------------
int GData::musicKeyType(void)const
{
  return _musicKeyType;
}

//------------------------------------------------------------------------------
int GData::temperedType(void)const
{
  return _temperedType;
}

//------------------------------------------------------------------------------
const double & GData::freqA(void)const
{
  return _freqA;
}

//------------------------------------------------------------------------------
const double & GData::semitoneOffset(void)const
{
  return _semitoneOffset;
}

//------------------------------------------------------------------------------
void GData::setMusicKey(int key)
{
  if(gMusicKey != key)
    {
      gMusicKey = key;
      emit musicKeyChanged(key);
    }
}

//------------------------------------------------------------------------------
void GData::setMusicKeyType(int type)
{
  if(_musicKeyType != type)
    {
      _musicKeyType = type;
      emit musicKeyTypeChanged(type);
    }
}

//------------------------------------------------------------------------------
void GData::setFreqA(int x)
{
  setFreqA(double(x));
}
//EOF
