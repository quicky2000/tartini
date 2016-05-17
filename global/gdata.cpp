/***************************************************************************
                          gdata.cpp  -  
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
#include <q3filedialog.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <unistd.h>
#endif

#include <qpixmap.h>
#include <QMessageBox>
#include <qfile.h>
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
#include "q3listview.h"
#include "soundfile.h"
#include "channel.h"
#include "tartinisettingsdialog.h"
#include "savedialog.h"
#include "conversions.h"
#include "musicnotes.h"

#ifndef WINDOWS
//for multi-threaded profiling
struct itimerval profiler_value;
struct itimerval profiler_ovalue;
#endif

int frame_window_sizes[NUM_WIN_SIZES] =
  {
    512,
    1024,
    2048,
    4096,
    8192
  };

const char *frame_window_strings[NUM_WIN_SIZES] =
  {
    "512",
    "1024",
    "2048",
    "4096",
    "8192"
  };

float step_sizes[NUM_STEP_SIZES] =
  {
    1.0f,
    0.5f,
    0.25f,
    0.2f,
    0.1f,
    0.05f
  };

const char *step_size_strings[NUM_STEP_SIZES] =
  { "100%",
    "50%",
    "25%",
    "20%",
    "10%",
    "5%"
  };

const char *pitch_method_strings[NUM_PITCH_METHODS] =
  {
    "FFT interpolation",
    "Fast-correlation",
    "Correlation (squared error) 1",
    "Correlation (squared error) 2",
    "Correlation (abs error) 1",
    "Correlation (abs error) 2",
    "Correlation (multiplied) 1",
    "Correlation (multiplied) 2"
  };

GData *gdata = NULL;

//Define the Phase function. This one is applicable to 
//accelerating sources since the phase goes as x^2.
//------------------------------------------------------------------------------
float phase_function(float x)
{
  float phase;
  
  //phase = x*x;
  phase = x;
  return(phase);
}

//------------------------------------------------------------------------------
GData::GData(/*int buffer_size_, int winfunc_, float step_size_*/)
{
  _polish = true;
  _drawingBuffer = new QPixmap(1, 1);
  setDBFloor(-150.0);
  setTopPitch(128.0);
  setLeftTime(0.0);
  setRightTime(5.0);

  amp_thresholds[AMPLITUDE_RMS][0]           = -85.0; amp_thresholds[AMPLITUDE_RMS][1]           = -0.0;
  amp_thresholds[AMPLITUDE_MAX_INTENSITY][0] = -30.0; amp_thresholds[AMPLITUDE_MAX_INTENSITY][1] = -20.0;
  amp_thresholds[AMPLITUDE_CORRELATION][0]   =  0.40; amp_thresholds[AMPLITUDE_CORRELATION][1]   =  1.00;
  amp_thresholds[FREQ_CHANGENESS][0]         =  0.50; amp_thresholds[FREQ_CHANGENESS][1]         =  0.02;
  amp_thresholds[DELTA_FREQ_CENTROID][0]     =  0.00; amp_thresholds[DELTA_FREQ_CENTROID][1]     =  0.10;
  amp_thresholds[NOTE_SCORE][0]              =  0.03; amp_thresholds[NOTE_SCORE][1]              =  0.20;
  amp_thresholds[NOTE_CHANGE_SCORE][0]       =  0.12; amp_thresholds[NOTE_CHANGE_SCORE][1]       =  0.30;

  amp_weights[0] = 0.2;
  amp_weights[1] = 0.2;
  amp_weights[2] = 0.2;
  amp_weights[3] = 0.2;
  amp_weights[4] = 0.2;

  qsettings = new QSettings("cs.otago.ac.nz", TARTINI_NAME_STR);
  TartiniSettingsDialog::setUnknownsToDefault(*this);
  
  activeChannel = NULL;
  _doingActiveAnalysis = 0;
  _doingActiveFFT = 0;
  _amplitudeMode = 0;
  _pitchContourMode = 0;
  _analysisType = 0;
  _doingActiveCepstrum = 0;

  updateQuickRefSettings();
  
  running = STREAM_STOP;
    
  audio_stream = NULL;
  soundMode = SOUND_PLAY;

  need_update = false;

  lineColor.push_back(Qt::darkRed);
  lineColor.push_back(Qt::darkGreen);
  lineColor.push_back(Qt::darkBlue);
  lineColor.push_back(Qt::darkCyan);
  lineColor.push_back(Qt::darkMagenta);
  lineColor.push_back(Qt::darkYellow);
  lineColor.push_back(Qt::darkGray);

  nextColorIndex = 0;

  _musicKeyType = 0; //ALL_NOTES
  _temperedType = 0; //EVEN_TEMPERED
  initMusicStuff();
}

//------------------------------------------------------------------------------
GData::~GData(void)
{
  audioThread.stopAndWait();

  qsettings->sync();

  //Note: The soundFiles is responsible for cleaning up the data the channels point to
  channels.clear();
  for(uint j=0; j<soundFiles.size(); j++)
    delete soundFiles[j];
  soundFiles.clear();
      
  std::vector<Filter*>::iterator fi;
  for(fi=filter_hp.begin(); fi!=filter_hp.end(); ++fi)
    delete (*fi);
  filter_hp.clear();
  for(fi=filter_lp.begin(); fi!=filter_lp.end(); ++fi)
    delete (*fi);

  filter_lp.clear();

  delete qsettings;

  delete _drawingBuffer;
}

//------------------------------------------------------------------------------
SoundFile* GData::getActiveSoundFile(void)
{
  return (activeChannel) ? activeChannel->getParent() : NULL;
}

//------------------------------------------------------------------------------
void GData::pauseSound(void)
{
    if(running == STREAM_FORWARD) running = STREAM_PAUSE;
    else if(running == STREAM_PAUSE) running = STREAM_FORWARD;
}

/**
   Opens the soundcard for recording.
   @param sRec A new created blank SoundFile to recort to
   @param sPlay A SoundFile to play when recording or NULL for record only
   @return true if success
*/
bool GData::openPlayRecord(SoundFile *sRec, SoundFile *sPlay)
{
  int theOpenMode;

  stop();

  int rate = sRec->rate();
  int numChannels = sRec->numChannels();
  int bits = sRec->bits();
  //int theBufferSize = sRec->bufferSize();
  int theBufferSize = sRec->framesPerChunk();

  if(sPlay) {
    soundMode = SOUND_PLAY_REC;
    theOpenMode = F_RDWR;
    sPlay->jumpToChunk(0);
    gdata->view->setCurrentTime(0);

  } else {
    soundMode = SOUND_REC;
    theOpenMode = F_READ;
  }

  //open the audio input
  audio_stream = new AudioStream;
  if(audio_stream->open(theOpenMode, rate, numChannels, bits, theBufferSize)) {

    fprintf(stderr, "Error initilizing sound\n");
    delete audio_stream;
    audio_stream = NULL;
    return false;
  } else {
    if((theOpenMode & F_WRITE)) {
      audioThread.start(sPlay, sRec);
    } else {
      audioThread.start(NULL, sRec);
    }
  }
  return true;
}

/** Starts playing the SoundFile s
@return true if the sound started playing (or is already playing)
*/
bool GData::playSound(SoundFile *s)
{
  if(s == NULL) return false;
  if(running == STREAM_PAUSE) { running = STREAM_FORWARD; return true; }
  if(running != STREAM_STOP) return true;

  stop();
  
  int chunk = s->currentChunk();
  if(chunk < 0 || chunk+1 >= s->totalChunks()) s->jumpToChunk(0); /*< If at the end of the file reset to the start */
  
  soundMode = SOUND_PLAY;

  if(!audio_stream) {
    audio_stream = new AudioStream;
    if(audio_stream->open(F_WRITE, s->rate(), s->numChannels(), s->bits(), s->bufferSize()/2)) {
      fprintf(stderr, "Error initialising sound\n");
      delete audio_stream;
      audio_stream = NULL;
      mainWindow->message("Error opening sound device. Another program might be using it", 2000);
    }
    audioThread.start(s, NULL);
  }
  return true;
}

//------------------------------------------------------------------------------
void GData::updateViewLeftRightTimes(void)
{
  double left = 0.0; //in seconds
  double right = 0.0; //in seconds
  Channel *ch;
  for(uint j = 0; j < channels.size(); j++) {
    ch = channels.at(j);
    if(ch->startTime() < left) left = ch->startTime();
    if(ch->finishTime() > right) right = ch->finishTime();
  }
  setLeftTime(left); //in seconds
  setRightTime(right); //in seconds
}

//------------------------------------------------------------------------------
void GData::setActiveChannel(Channel *toActive)
{
  activeChannel = toActive;
  if(activeChannel) updateActiveChunkTime(view->currentTime());
  emit activeChannelChanged(activeChannel);
  emit activeIntThresholdChanged(getActiveIntThreshold());
  view->doUpdate();
}

//------------------------------------------------------------------------------
void GData::updateActiveChunkTime(double t)
{
  if((running != STREAM_STOP) && (soundMode & SOUND_REC)) return;

  Channel *active = getActiveChannel();
  t = bound(t, leftTime(), rightTime());
  if(active) {
    active->jumpToTime(t);
    if(gdata->doingActive()) {
	    active->lock();
      active->processChunk(active->currentChunk());
	    active->unlock();
    }
  }
  view->setCurrentTime(t);
  doChunkUpdate();
}

//------------------------------------------------------------------------------
void GData::setLeftTime(double x)
{
  if(x != _leftTime) {
    _leftTime = x;
    emit timeRangeChanged(leftTime(), rightTime());
    emit leftTimeChanged(leftTime());
  }
}

//------------------------------------------------------------------------------
void GData::setRightTime(double x)
{
  if(x != _rightTime) {
    _rightTime = x;
    emit timeRangeChanged(leftTime(), rightTime());
    emit rightTimeChanged(rightTime());
  }
}

//------------------------------------------------------------------------------
void GData::setTopPitch(double y)
{
  if(y != _topPitch) {
    _topPitch = y;
  }
}

//------------------------------------------------------------------------------
void GData::beginning(void)
{
  updateActiveChunkTime(leftTime());
  view->doSlowUpdate();
}


//------------------------------------------------------------------------------
void GData::rewind(void)
{
  double speed = 16;
  double diffTime = view->zoomX() * speed;
  if(getActiveChannel())
    diffTime = MAX(diffTime, getActiveChannel()->timePerChunk());
  updateActiveChunkTime(view->currentTime() - diffTime);
  view->doSlowUpdate();
}

//------------------------------------------------------------------------------
bool GData::play(void)
{
  return playSound(getActiveSoundFile());
}

//------------------------------------------------------------------------------
void GData::stop(void)
{
  audioThread.stopAndWait();
}

//------------------------------------------------------------------------------
void GData::fastforward(void)
{
  double speed = 16;
  double diffTime = view->zoomX() * speed;
  if(getActiveChannel())
    diffTime = MAX(diffTime, getActiveChannel()->timePerChunk());
  updateActiveChunkTime(view->currentTime() + diffTime);
  view->doSlowUpdate();
}
  
//------------------------------------------------------------------------------
void GData::end(void)
{
  updateActiveChunkTime(rightTime());
  view->doSlowUpdate();
}

//------------------------------------------------------------------------------
int GData::getAnalysisBufferSize(int rate)
{  
  int windowSize = qsettings->value("Analysis/bufferSizeValue", 48).toInt();
  QString windowSizeUnit = qsettings->value("Analysis/bufferSizeUnit", "milli-seconds").toString();
  if(windowSizeUnit.lower() == "milli-seconds") { //convert to samples
    windowSize = int(double(windowSize) * double(rate) / 1000.0);
  }
  if(qsettings->value("Analysis/bufferSizeRound", true).toBool()) {
    windowSize = toInt(nearestPowerOf2(windowSize));
  }
  return windowSize;
}

//------------------------------------------------------------------------------
int GData::getAnalysisStepSize(int rate)
{  
  int stepSize = qsettings->value("Analysis/stepSizeValue", 24).toInt();
  QString stepSizeUnit = qsettings->value("Analysis/stepSizeUnit", "milli-seconds").toString();
  if(stepSizeUnit.lower() == "milli-seconds") { //convert to samples
    stepSize = int(double(stepSize) * double(rate) / 1000.0);
  }
  if(qsettings->value("Analysis/stepSizeRound", true).toBool()) {
    stepSize = toInt(nearestPowerOf2(stepSize));
  }
  return stepSize;
}

//------------------------------------------------------------------------------
void GData::updateQuickRefSettings(void)
{
  _backgroundColor.setNamedColor(qsettings->value("Display/theBackgroundColor", "#BBCDE2").toString());
  _shading1Color.setNamedColor(qsettings->value("Display/shading1Color", "#BBCDEF").toString());
  _shading2Color.setNamedColor(qsettings->value("Display/shading2Color", "#CBCDE2").toString());
  _doingHarmonicAnalysis = qsettings->value("Analysis/doingHarmonicAnalysis", true).toBool();
  _doingFreqAnalysis = qsettings->value("Analysis/doingFreqAnalysis", true).toBool();
  _doingEqualLoudness = qsettings->value("Analysis/doingEqualLoudness", true).toBool();
  _doingAutoNoiseFloor = qsettings->value("Analysis/doingAutoNoiseFloor", true).toBool();
  _doingDetailedPitch = qsettings->value("Analysis/doingDetailedPitch", false).toBool();
  _fastUpdateSpeed = qsettings->value("Display/fastUpdateSpeed", 75).toInt();
  _slowUpdateSpeed = qsettings->value("Display/slowUpdateSpeed", 150).toInt();
  _vibratoSineStyle = qsettings->value("Advanced/vibratoSineStyle", false).toBool();
  QString analysisString = qsettings->value("Analysis/analysisType", "MPM").toString();
  if(analysisString == QString("MPM")) _analysisType = MPM;
  else if(analysisString == QString("AUTOCORRELATION")) _analysisType = AUTOCORRELATION;
  else _analysisType = MPM_MODIFIED_CEPSTRUM;
  _showMeanVarianceBars = qsettings->value("Advanced/showMeanVarianceBars", false).toBool();
  QString s = qsettings->value("Advanced/savingMode", "Ask when closing unsaved files (normal)").toString();
  if(s == "Ask when closing unsaved files (normal)") _savingMode = 0;
  else if(s == "Don't ask when closing unsaved files (use with care)") _savingMode = 1;
  else _savingMode = 2;
  _mouseWheelZooms = qsettings->value("Advanced/mouseWheelZooms", false).toBool();
  setFreqA(qsettings->value("View/freqA", 440).toInt());
}

//------------------------------------------------------------------------------
QString GData::getFilenameString(void)
{
  QString fileGeneratingString = qsettings->value("General/filenameGeneratingString", "Untitled").toString();
  QString filename;
  int fileGeneratingNumber = qsettings->value("General/fileGeneratingNumber", 1).toInt();
  int digits = qsettings->value("General/fileNumberOfDigits", 2).toInt();
  if(digits == 0) {
    filename.sprintf("%s.wav", fileGeneratingString.latin1());
  } else {
    filename.sprintf("%s%0*d.wav", fileGeneratingString.latin1(), digits, fileGeneratingNumber);
  }
  return filename;
}

//------------------------------------------------------------------------------
QColor GData::getNextColor(void)
{
  return lineColor.at((nextColorIndex++) % lineColor.size());
}

//------------------------------------------------------------------------------
void GData::addFileToList(SoundFile *s)
{
  int c;
  soundFiles.push_back(s);
  for(c=0; c<s->numChannels(); c++) {
    channels.push_back(s->channels(c));
  }
  emit channelsChanged();
}

//------------------------------------------------------------------------------
void GData::removeFileFromList(SoundFile *s)
{
  int j;
  int curPos;
  int prevPos;  
  //remove all the channels in s from the channels list
  for(j=0; j<s->numChannels(); j++) {
    Channel *c = s->channels(j);
    curPos = prevPos = 0;
    for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++, curPos++) {
      if((*it1) == c) {
        if(c == getActiveChannel()) prevPos = curPos;
        it1 = channels.erase(it1) - 1;
      }
    }
    if(channels.empty()) setActiveChannel(NULL);
    else setActiveChannel(channels.at(bound(prevPos, 0, int(channels.size()-1))));
  }
  //remove the soundFile from the soundFiles list
  for(std::vector<SoundFile*>::iterator it2=soundFiles.begin(); it2 != soundFiles.end(); it2++) {
    if((*it2) == s) {
      it2 = soundFiles.erase(it2) - 1;
    }
  }
  emit channelsChanged();
}

//------------------------------------------------------------------------------
void GData::saveActiveFile(void) {
  SoundFile *s = getActiveSoundFile();
  if(!s) return;
  if(s->saved()) return;
  if(audioThread.playSoundFile() == s || audioThread.recSoundFile() == s) {
    stop();
  }
  int val = saveFile(s, saveFileAsk(s->filename));
  if(val == 0) { //success
    emit channelsChanged();
  } else if(val == -1) {
    QMessageBox::warning(mainWindow, "Error", QString("Error saving file '") + QString(s->filename) + QString("'"), QMessageBox::Ok, Qt::NoButton);
  }
}

//------------------------------------------------------------------------------
QString GData::saveFileAsk(QString oldFilename)
{
  QString newFilename = SaveDialog::getSaveWavFileName(mainWindow);
  if(newFilename.isNull()) return QString();
  newFilename = QDir::convertSeparators(newFilename);
  if(newFilename != oldFilename && QFile::exists(newFilename)) {
    if(QMessageBox::warning(mainWindow, tr("Overwrite File?"),
      QString("A file called '") + newFilename + QString("' already exists.\n Do you want to overwrite it?"),
      tr("&Yes"), tr("&No"), QString::null, 0, 1 )) {
        return QString(); //user canceled overwrite
	  }
  }
  return newFilename;
}

/**
  @return 0 if the file was saved, 1 if cancled, or -1 if an error occured
*/
int GData::saveFile(SoundFile *s, QString newFilename)
{
  if(newFilename.isNull()) { /*printf("cancled\n");*/ return 1; }
  QString oldFilename(s->filename);
  oldFilename = QDir::convertSeparators(oldFilename);
  int pos = s->stream->pos();
  s->stream->close();
  
  int ret = (moveFile(oldFilename.latin1(), newFilename.latin1())) ? 0 : -1;
  if(ret == 0) {
	s->stream->open_read(newFilename.latin1());
    s->stream->jump_to_frame(pos);
	s->setSaved(true);
    s->setFilename(newFilename.latin1());
  } else {
    s->stream->open_read(oldFilename.latin1());
	s->stream->jump_to_frame(pos);
  }
  return ret;
}

/**
  @return true if all the files were closed. false if cancelled
*/
bool GData::closeAllFiles(void) {
  while(!soundFiles.empty()) {
    if(closeFile(soundFiles.at(0), gdata->savingMode()) == 1) return false; //cancelled
  }
  return true;
}

//------------------------------------------------------------------------------
void GData::closeActiveFile(void) {
  SoundFile *s = getActiveSoundFile();
  if(!s) return;
  closeFile(s, gdata->savingMode());
  view->doUpdate();
  doChunkUpdate();
}

/**
  @param s The sound file to be closed (This pointer will become invalid after returning)
  //@param ask If true (default), the user will be asked to save the file if it's been modified.
  @return 0 if the file was closed, 1 if canclled, -1 if error
*/
int GData::closeFile(SoundFile *s, int theSavingMode/*, bool ask*/)
{
  if(s == NULL) return -1;
  QString newFilename;
  QString oldFilename(s->filename);
  oldFilename = QDir::convertSeparators(oldFilename);

  if(gdata->audioThread.playSoundFile() == s || gdata->audioThread.recSoundFile() == s) {
    gdata->stop();
  }

  if(s->saved()) { //file is already saved
    removeFileFromList(s);
    delete s;
    return 0;
  }

  if(theSavingMode == ALWAYS_ASK) {
    QString filename = QString(getFilenamePart(oldFilename.latin1()));
    int option = QMessageBox::question(NULL, QString("Save changes to file '") + filename + "' ?",
      QString("Save changes to the file '") + filename + QString("' ?\n"),
      "&Yes", "&No", "&Cancel", 0, 2);

    switch(option) {
    case 0: //Yes
      newFilename = saveFileAsk(oldFilename);
      if(newFilename.isNull()) return 1;
      removeFileFromList(s);
      delete s;
      break;
    case 1: //No
      removeFileFromList(s);
      delete s;
      break;
    default: //Cancelled
      return 1;
    }
  } else if(theSavingMode == NEVER_SAVE) {
    removeFileFromList(s);
    delete s;
  } else if(theSavingMode == ALWAYS_SAVE) {
    removeFileFromList(s);
    delete s;
  }
  return 0;
}

//------------------------------------------------------------------------------
void GData::clearFreqLookup(void)
{
  for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++) {
    (*it1)->clearFreqLookup();
  }
}

//------------------------------------------------------------------------------
void GData::setAmplitudeMode(int amplitudeMode)
{
  if(amplitudeMode != _amplitudeMode) {
    _amplitudeMode = amplitudeMode;
    clearAmplitudeLookup();
  }
}

//------------------------------------------------------------------------------
void GData::setPitchContourMode(int pitchContourMode)
{
  if(pitchContourMode != _pitchContourMode) {
    _pitchContourMode = pitchContourMode;
    clearFreqLookup();
  }
}

//------------------------------------------------------------------------------
void GData::clearAmplitudeLookup(void)
{
  for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++) {
    (*it1)->clearAmplitudeLookup();
  }
}
  
//------------------------------------------------------------------------------
int GData::getActiveIntThreshold(void)
{
  Channel* active = getActiveChannel();
  if(active) return toInt(active->threshold() * 100.0f);
  else return qsettings->value("Analysis/thresholdValue", 93).toInt();
}

//------------------------------------------------------------------------------
void GData::resetActiveIntThreshold(int thresholdPercentage)
{
  Channel* active = getActiveChannel();
  if(active) active->resetIntThreshold(thresholdPercentage);
}

//------------------------------------------------------------------------------
void GData::setAmpThreshold(int mode, int index, double value)
{
  amp_thresholds[mode][index] = value;
  clearFreqLookup();
  recalcScoreThresholds();
}

//------------------------------------------------------------------------------
double GData::ampThreshold(int mode, int index)
{
  return amp_thresholds[mode][index];
}

//------------------------------------------------------------------------------
void GData::setAmpWeight(int mode, double value)
{
  amp_weights[mode] = value;
  clearFreqLookup();
  recalcScoreThresholds();
}

//------------------------------------------------------------------------------
double GData::ampWeight(int mode)
{
  return amp_weights[mode];
}

//------------------------------------------------------------------------------
void GData::recalcScoreThresholds(void)
{
  for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++) {
    (*it1)->recalcScoreThresholds();
  }
}

//------------------------------------------------------------------------------
void GData::doChunkUpdate()
{
  emit onChunkUpdate();
}

//------------------------------------------------------------------------------
void GData::setTemperedType(int type)
{
  if(_temperedType != type) {
    if(_temperedType == 0 && type > 0) { //remove out the minors
      if(_musicKeyType >= 2) setMusicKeyType(0);
      for(int j=gMusicScales.size()-1; j>=2; j--) {
        mainWindow->keyTypeComboBox->removeItem(j);
      }
    } else if(_temperedType > 0 && type == 0) {
      QStringList s;
      for(unsigned int j=2; j<gMusicScales.size(); j++) s << gMusicScales[j].name();
      mainWindow->keyTypeComboBox->addItems(s);
    }
    _temperedType = type; emit temperedTypeChanged(type);
  }
}

//------------------------------------------------------------------------------
void GData::setFreqA(double x)
{
	_freqA = x;
	_semitoneOffset = freq2pitch(x) - freq2pitch(440.0);
	qsettings->setValue("View/freqA", x);
}

//EOF
