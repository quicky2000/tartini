/***************************************************************************
                          gdata.cpp  -  
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <qfiledialog.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <unistd.h>
#endif

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
//#include "mycolor.h"
#include "filter.h"
//#include "phils_templates.h"
#include "mystring.h"
#include "qlistview.h"
#include "soundfile.h"
#include "channel.h"
//#include "fwinfunc.h"
#include "settingsdialog.h"
#include "savedialog.h"

#include <qpixmap.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qfile.h>

#include <stdio.h>

#ifndef WINDOWS
//for multi-threaded profiling
struct itimerval profiler_value;
struct itimerval profiler_ovalue;
#endif

/*
//QColor myBackgroundColor(196, 196, 196);
QColor myBackgroundColor(128, 128, 128);
QColor myLineColor1(32, 32, 32);
QColor myLineColor2(0, 0, 255);
QColor myRefLineColor(0, 128, 0);
*/

int frame_window_sizes[NUM_WIN_SIZES] = { 512, 1024, 2048, 4096, 8192 };
const char *frame_window_strings[NUM_WIN_SIZES] = { "512", "1024", "2048", "4096", "8192" };

float step_sizes[NUM_STEP_SIZES] = { 1.0f, 0.5f, 0.25f, 0.2f, 0.1f, 0.05f };
const char *step_size_strings[NUM_STEP_SIZES] = { "100%", "50%", "25%", "20%", "10%", "5%" };

const char *pitch_method_strings[NUM_PITCH_METHODS] = { "FFT interpolation", "Fast-correlation",  "Correlation (squared error) 1", "Correlation (squared error) 2", "Correlation (abs error) 1", "Correlation (abs error) 2", "Correlation (multiplied) 1", "Correlation (multiplied) 2" };

const char *interpolating_type_strings[NUM_INTERPOLATING_TYPES] = { "Linear", "Cubic B-Spline", "Hermite Cubic" };
GData *gdata = NULL;

//Define the Phase function. This one is applicable to 
//accelerating sources since the phase goes as x^2.
float phase_function(float x)
{
  float phase;
  
  //phase = x*x;
  phase = x;
  return(phase);
}

GData::GData(/*int buffer_size_, int winfunc_, float step_size_*/)
{
  _drawingBuffer = new QPixmap(1, 1);
  setTopNote(128.0);
  setLeftTime(0.0);
  setRightTime(5.0);
  setNoiseThresholdDB(-100.0);

  settings.init("cs.otago.ac.nz", "Tartini");
  SettingsDialog::setDefaults(&settings);
  settings.load();
  
  activeChannel = NULL;
  _doingActiveAnalysis = 0;
  _doingActiveFFT = 0;
  _amplitudeMode = 0;
  _pitchContourMode = 0;
  _analysisType = 0;
  
  updateQuickRefSettings();
  
    //char *filename = getenv("PITCH_INI");
    //if(filename == NULL) {
	//filename = "pitch.ini";
    //}
    //settings.load(filename);

    //buffer_size = buffer_size_;
    //winfunc = winfunc_;
    //step_size = step_size_;
    
    peakThreshold = -60.0; //in dB
    correlationThreshold = 0.00001f; //0.5 in the other scale (log);
    frameCounter = 0;
    //equalLoudness = false; //true;
    //useMasking = false; //true;
    //harmonicNum = 1;

    //useRidgeFile = false; /**< Store harmonic info to a ridge file */
    
    doingStuff = false; /**< Active/inactive */
    running = STREAM_STOP;
    
    //in_channels = 2;
    //process_channels = 1;
    //out_channels = 2;
    interpolating_type = 2; //HERMITE_CUBIC;
    //bisection_steps = settings.get_int("Correlation", "bisectionSteps", 8); //8;
    //fast_correlation_repeats = settings.get_int("Correlation", "fastRepeats", 512);
    using_coefficients_table = true;

    audio_stream = NULL;
    soundMode = SOUND_PLAY;

#if 0 //dont use chirp stuff
    //setup fct
    fct.set_N_dimensions(2);
    fct.set_N_data(buffer_size);
    fct.set_phase_function(1,64,&phase_function);
    fct.set_N_tau0(buffer_size);
    fct.initialize();
    //Allocate fct memory
    fct_in_data  = (float *)calloc(2*buffer_size, sizeof(float));
    fct_out_data = (float *)calloc(2*fct.get_output_data_length(), sizeof(float));
    fct_draw_data = (FrameRGB *)calloc(buffer_size/2*64, sizeof(FrameRGB));
#endif

    cur_note = 0.0;

    sync_flag = 0;
    need_update = false;

    //view = new View();
    
    //lineColor.push_back(Qt::white);
    //lineColor.push_back(Qt::black);
    //lineColor.push_back(Qt::red);
    lineColor.push_back(Qt::darkRed);
    //lineColor.push_back(Qt::green);
    lineColor.push_back(Qt::darkGreen);
    //lineColor.push_back(Qt::blue);
    lineColor.push_back(Qt::darkBlue);
    //lineColor.push_back(Qt::cyan);
    lineColor.push_back(Qt::darkCyan);
    //lineColor.push_back(Qt::magenta);
    lineColor.push_back(Qt::darkMagenta);
    //lineColor.push_back(Qt::yellow);
    lineColor.push_back(Qt::darkYellow);
    //lineColor.push_back(Qt::gray);
    lineColor.push_back(Qt::darkGray);
    lineColor.push_back(Qt::lightGray);

    nextColorIndex = 0;

}

GData::~GData()
{
    audioThread.stopAndWait();
    
    settings.save();

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

    //if(fwinfunc) delete fwinfunc;
    //if(loudnessFunc) delete loudnessFunc;
    //if(sound_file_stream) delete sound_file_stream;
    //if(audio_stream) delete audio_stream;

#if 0
    free(fct_in_data);
    free(fct_out_data);
    free(fct_draw_data);
#endif


  delete _drawingBuffer;
}

/*
void GData::setBuffers(int freq, int channels)
{
    //bufferMutex.lock();

    //coefficients_table.resize(buffer_size*4*channels);
    coefficients_table.resize(buffer_size*4, channels);
    fwinfunc->create(winfunc, buffer_size);
    loudnessFunc->create_loudness(buffer_size/2, freq);
    freqHistory.resize(channels);

    std::vector<Filter*>::iterator fi;
    for(fi=filter_hp.begin(); fi!=filter_hp.end(); ++fi)
	delete (*fi);
    filter_hp.clear();
    for(fi=filter_lp.begin(); fi!=filter_lp.end(); ++fi)
	delete (*fi);
    filter_lp.clear();

    const double R = 0.94; //0.94 to 0.99
    double fhp_a[3] = { 1.0, -2.0, 1.0 };
    double fhp_b[2] = { -2.0*R, R*R };
    double flp_a[2] = { 0.109, 0.109 };
    double flp_b[8] = { -2.5359, 3.9295, -4.7532, 4.7251, -3.5548, 2.1396, -0.9879, 0.2836 };

    for(int j=0; j<channels; j++) {
	Filter *hp = new Filter();
	hp->make_IIR(fhp_a, 3, fhp_b, 2);
	Filter *lp = new Filter();
	lp->make_IIR(flp_a, 2, flp_b, 8);
	filter_hp.push_back(hp);
	filter_lp.push_back(lp);
    }

    //bufferMutex.unlock();
}
*/

/*
void GData::setFrameWindowSize(int index)
{
    buffer_size = frame_window_sizes[index];
    if(input_stream) setBuffers(input_stream->freq, input_stream->channels);
    need_update = false; //wait for buffers to get more data before the next update
}

void GData::setWinFunc(int index)
{
    winfunc = index;
    fwinfunc->create(winfunc, buffer_size);
}

void GData::setStepSize(int index)
{
    //printf("gdata waiting for lock\n"); fflush(stdout);
    //bufferMutex.lock();
    //printf("gdata got lock\n"); fflush(stdout);
    step_size = step_sizes[index];
    //printf("gdata lost lock\n"); fflush(stdout);
    //bufferMutex.unlock();
}
*/

SoundFile* GData::getActiveSoundFile()
{
  return (activeChannel) ? activeChannel->getParent() : NULL;
}

void GData::pauseSound()
{
    if(running == STREAM_FORWARD) running = STREAM_PAUSE;
    else if(running == STREAM_PAUSE) running = STREAM_FORWARD;
    //printf("paused called. Value is now %d\n", running);
}

/*
void GData::openMicrophone()
{
    stopAllSound();

    //if(!audio_stream) {
    audio_stream = new AudioStream;
    if(audio_stream->open(F_READ, 44100, in_channels, 16)) {
	fprintf(stderr, "Error initilizing sound\n");
	delete audio_stream;
	audio_stream = NULL;
	//output_stream = NULL;
    } else {
	//input_stream = audio_stream;
	//setBuffers(input_stream->freq, input_stream->channels);
	running = STREAM_FORWARD;
  soundMode = SOUND_REC;
#if (QT_VERSION >= 0x030200)
	//audioThread.start(QThread::HighPriority); //TimeCriticalPriority);
	audioThread.start();
#else
	audioThread.start();
#endif
    }
    //}
}
*/

/** Opens the soundcard for recording.
  @return true if success
*/
bool GData::openRecord(SoundFile *s)
{
  stop();

  soundMode = SOUND_REC;

  //open the audio input
  audio_stream = new AudioStream;
#ifdef LINUX  //fixme: Buffer sizes need to be tested
  if(audio_stream->open(F_READ, s->rate(), s->numChannels(), s->bits(), s->bufferSize()/2)) {
#else
  if(audio_stream->open(F_READ, s->rate(), s->numChannels(), s->bits(), s->framesPerChunk())) {
#endif
    fprintf(stderr, "Error initilizing sound\n");
    delete audio_stream;
    audio_stream = NULL;
    return false;
  } else {
//#ifdef WINDOWS
//    Sleep(100);
//#endif
    //running = STREAM_FORWARD;
    audioThread.start(s);
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

/*#ifdef USE_SOX
    sound_file_stream = new SoxStream;
#else
    if(str_case_cmp(getFileExtension(inputFile), "wav") == 0) {
	sound_file_stream = new WaveStream;
    }
#ifdef USE_OGG
    else if(str_case_cmp(getFileExtension(inputFile), "ogg") == 0) {
	sound_file_stream = new OggStream;
    }
#endif
  else {
      printf("Cannot open file of this type. %s\n", inputFile);
      return;
  }
#endif
  printf("Opening sound file for reading\n");
  int temp = sound_file_stream->open_read(inputFile);
  if(temp) {
      fprintf(stderr, "Error %d opening %s\n", temp, inputFile);
      delete sound_file_stream;
      sound_file_stream = NULL;
      running = STREAM_STOP;
      return;
  } else {
      input_stream = sound_file_stream;
      setBuffers(input_stream->freq, input_stream->channels);
      running = STREAM_FORWARD;
  }
 */
  if(!audio_stream) {
  //printf("Creating a new AudioStream\n");
    audio_stream = new AudioStream;
    //if(audio_stream->open(F_WRITE, input_stream->freq, input_stream->channels)) {
    //printf("The BufferSize!!! = %d\n", s->bufferSize());
#ifdef LINUX  //fixme: Buffer sizes need to be tested
    if(audio_stream->open(F_WRITE, s->rate(), s->numChannels(), s->bits(), s->bufferSize()/2)) {
#else
    if(audio_stream->open(F_WRITE, s->rate(), s->numChannels(), s->bits(), s->bufferSize())) {
#endif
      fprintf(stderr, "Error initialising sound\n");
      delete audio_stream;
      audio_stream = NULL;
      mainWindow->message("Error opening sound device. Another program might be using it", 2000);
    }
    audioThread.start(s);
  }
  return true;
}

/*
void GData::jump_forward(int frames)
{
    if(running == STREAM_PAUSE) {
	if(sound_file_stream) {
	    if(frames != 0) {
		//bufferMutex.lock();
		sound_file_stream->jump_forward(frames);
		//bufferMutex.unlock();
	    }
	    running = STREAM_UPDATE;
	}
    }
}
*/

void GData::updateViewLeftRightTimes()
{
  double left = 0.0; //in seconds
  double right = 0.0; //in seconds
  Channel *ch;
  for(uint j = 0; j < channels.size(); j++) {
    ch = channels.at(j);
    //if(ch->isVisible()) {
      if(ch->startTime() < left) left = ch->startTime();
      if(ch->finishTime() > right) right = ch->finishTime();
    //}
  }
  setLeftTime(left); //in seconds
  setRightTime(right); //in seconds
}

void GData::setActiveChannel(Channel *toActive)
{
  activeChannel = toActive;
  if(activeChannel) updateActiveChunkTime(view->currentTime());
  emit activeChannelChanged(activeChannel);
  emit activeIntThresholdChanged(getActiveIntThreshold());
  view->doUpdate();
}

void GData::updateActiveChunkTime(double t)
{
  if((running != STREAM_STOP) && (soundMode & SOUND_REC)) return;

  Channel *active = getActiveChannel();
  t = bound(t, leftTime(), rightTime());
  if(active) {
    //t = active->timeAtChunk(active->chunkAtTime(t)); //align time to an integer sample step
    active->jumpToTime(t);
    if(gdata->doingActive()) {
	    active->lock();
      active->processChunk(active->currentChunk());
	    active->unlock();
    }
  }
  view->setCurrentTime(t);
}

void GData::setLeftTime(double x)
{
  if(x != _leftTime) {
    _leftTime = x;
    emit timeRangeChanged(leftTime(), rightTime());
    emit leftTimeChanged(leftTime());
    //emit viewChanged();
  }
}

void GData::setRightTime(double x)
{
  if(x != _rightTime) {
    _rightTime = x;
    emit timeRangeChanged(leftTime(), rightTime());
    emit rightTimeChanged(rightTime());
    //emit viewChanged();
  }
}

void GData::setTopNote(double y)
{
  if(y != _topNote) {
    _topNote = y;
    //emit scrollableYChanged(topNote()-viewHeight());
  }
}



void GData::beginning()
{
  updateActiveChunkTime(leftTime());
  view->doSlowUpdate();
}


void GData::rewind()
{
  double speed = 16;
  double diffTime = view->zoomX() * speed;
  if(getActiveChannel())
    diffTime = MAX(diffTime, getActiveChannel()->timePerChunk());
  updateActiveChunkTime(view->currentTime() - diffTime);
  view->doSlowUpdate();
}

bool GData::play()
{
  return playSound(getActiveSoundFile());
}

void GData::stop()
{
  audioThread.stopAndWait();
}

void GData::fastforward()
{
  double speed = 16;
  double diffTime = view->zoomX() * speed;
  if(getActiveChannel())
    diffTime = MAX(diffTime, getActiveChannel()->timePerChunk());
  updateActiveChunkTime(view->currentTime() + diffTime);
  view->doSlowUpdate();
}
  
void GData::end()
{
  updateActiveChunkTime(rightTime());
  view->doSlowUpdate();
}

int GData::getAnalysisBufferSize(int rate)
{  
  int windowSize = settings.getInt("Analysis", "bufferSizeValue");
  QString windowSizeUnit = settings.getString("Analysis", "bufferSizeUnit");
  if(windowSizeUnit.lower() == "milli-seconds") { //convert to samples
    windowSize = int(double(windowSize) * double(rate) / 1000.0);
  }
  if(settings.getBool("Analysis", "bufferSizeRound")) {
    windowSize = toInt(nearestPowerOf2(windowSize));
  }
  return windowSize;
}

int GData::getAnalysisStepSize(int rate)
{  
  int stepSize = settings.getInt("Analysis", "stepSizeValue");
  QString stepSizeUnit = settings.getString("Analysis", "stepSizeUnit");
  if(stepSizeUnit.lower() == "milli-seconds") { //convert to samples
    stepSize = int(double(stepSize) * double(rate) / 1000.0);
  }
  if(settings.getBool("Analysis", "stepSizeRound")) {
    stepSize = toInt(nearestPowerOf2(stepSize));
  }
  return stepSize;
}

void GData::updateQuickRefSettings()
{
  _backgroundColor.setNamedColor(settings.getString("Colors", "backgroundColor"));
  _shading1Color.setNamedColor(settings.getString("Colors", "shading1Color"));
  _shading2Color.setNamedColor(settings.getString("Colors", "shading2Color"));
  _doingHarmonicAnalysis = settings.getBool("Analysis", "doingHarmonicAnalysis");
  _doingFreqAnalysis = settings.getBool("Analysis", "doingFreqAnalysis");
  _doingEqualLoudness = settings.getBool("Analysis", "doingEqualLoudness");
  _fastUpdateSpeed = settings.getInt("General", "fastUpdateSpeed");
  _slowUpdateSpeed = settings.getInt("General", "slowUpdateSpeed");
  if(settings.getString("Analysis", "analysisType") == QString("MPM")) _analysisType = 0;
  else _analysisType = 1;
}

QString GData::getFilenameString()
{
  QString fileGeneratingString = settings.getString("General", "filenameGeneratingString");
  QString filename;
  int fileGeneratingNumber = settings.getInt("General", "fileGeneratingNumber");
  int digits = settings.getInt("General", "fileNumberOfDigits");
  if(digits == 0) {
    filename.sprintf("%s.wav", fileGeneratingString.latin1());
  } else {
    filename.sprintf("%s%0*d.wav", fileGeneratingString.latin1(), digits, fileGeneratingNumber);
  }
  return filename;
}

QColor GData::getNextColor()
{
  return lineColor.at((nextColorIndex++) % lineColor.size());
}

void GData::addFileToList(SoundFile *s)
{
  int c;
  soundFiles.push_back(s);
  for(c=0; c<s->numChannels(); c++) {
    channels.push_back(s->channels(c));
  }
  emit channelsChanged();
}

void GData::removeFileFromList(SoundFile *s)
{
  int j;
  int curPos;
  int prevPos;  
  //remove all the channels in s from the channels list
  for(j=0; j<s->numChannels(); j++) {
    Channel *c = s->channels(j);
    //if(c == getActiveChannel()) { setActiveChannel(NULL); }
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
  //view->doSlowUpdate();
}

void GData::saveActiveFile() {
  SoundFile *s = getActiveSoundFile();
  if(!s) return;
  if(s->saved()) return;
  if(audioThread.soundFile() == s) {
    stop();
  }
  int val = saveFile(s, saveFileAsk(s->filename));
  if(val == 0) { //success
    emit channelsChanged();
  } else if(val == -1) {
    QMessageBox::warning(mainWindow, "Error", QString("Error saving file '") + QString(s->filename) + QString("'"), QMessageBox::Ok, QMessageBox::NoButton);
  }
}

QString GData::saveFileAsk(QString oldFilename)
{
/*
  RecordDialog d(mainWindow);
  if(d.exec() != QDialog::Accepted) return QString();
  QString newFilename = d.selectedFile();
  //QString newFilename = QFileDialog::getSaveFileName(oldFilename, "Wave files (*.wav)", mainWindow, "Save file dialog", "Choose a filename to save under", 0, false);
*/
  QString newFilename = SaveDialog::getSaveWavFileName(mainWindow);
  if(newFilename.isNull()) return QString();
  //if(!newFilename.endsWith(".wav")) { newFilename += ".wav"; }
//#ifdef WINDOWS
//  newFilename.replace(QChar('/'), QChar('\\'));
//#endif
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
//#ifdef WINDOWS
//  oldFilename.replace(QChar('/'), QChar('\\'));
//#endif
  oldFilename = QDir::convertSeparators(oldFilename);
  int pos = s->stream->pos();
  s->stream->close();
  
  //printf("moveFile(%s, %s);\n", oldFilename.latin1(), newFilename.latin1());
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
bool GData::closeAllFiles() {
  while(!soundFiles.empty()) {
    if(closeFile(soundFiles.at(0)) == 1) return false; //cancelled
  }
  return true;
}

void GData::closeActiveFile() {
  SoundFile *s = getActiveSoundFile();
  if(!s) return;
  closeFile(s);
  view->doSlowUpdate();
  view->doFastUpdate();
}

/**
  @param s The sound file to be closed (This pointer will become invalid after returning)
  @param ask If true (default), the user will be asked to save the file if it's been modified.
  @return 0 if the file was closed, 1 if canclled, -1 if error
*/
int GData::closeFile(SoundFile *s, bool ask)
{
  int ret = 0;
  if(s == NULL) return -1;
  QString newFilename;
  QString oldFilename(s->filename);
//#ifdef WINDOWS
//  oldFilename.replace(QChar('/'), QChar('\\'));
//#endif
  oldFilename = QDir::convertSeparators(oldFilename);

  if(gdata->audioThread.soundFile() == s) {
    gdata->stop();
  }
  if(!s->saved() && ask) {
    QString filename = QString(getFilenamePart(oldFilename.latin1()));
    int option = QMessageBox::information(mainWindow, QString("Save changes to file '") + filename + "' ?",
      QString("Save changes to the file '") + filename + QString("' ?\n"),
      "&Yes", "&No", "&Cancel", 0, 2);
    switch(option) {
    case 0:
      newFilename = saveFileAsk(oldFilename);
      if(newFilename.isNull()) return 1;
      removeFileFromList(s);
      delete s;
      //printf("move file %s to %s\n", oldFilename.latin1(), newFilename.latin1()); fflush(stdout);
      ret = (::moveFile(oldFilename.latin1(), newFilename.latin1())) ? 0 : -1;
      break;
    case 1:
      removeFileFromList(s);
      delete s;
      //printf("remove file %s\n", oldFilename.latin1()); fflush(stdout);
      ret = ::remove(oldFilename.latin1());
      break;
    default:
      return 1; //Cancelled
    }
  } else {
    removeFileFromList(s);
    delete s;
  }
  if(ret == -1) {
    QMessageBox::warning(mainWindow, "Error", QString("Error removing file '") + QString(oldFilename) + QString("'"), QMessageBox::Ok, QMessageBox::NoButton);
  }
  return ret;
}
/*
void GData::setNoiseThreshold(double noiseThreshold_)
{
  setNoiseThresholdDB(log10(noiseThreshold_) * 20.0);
}
*/
void GData::setNoiseThresholdDB(double noiseThresholdDB_)
{
  _noiseThresholdDB = bound(noiseThresholdDB_, -150.0, 0.0);
  _noiseThreshold = pow10(_noiseThresholdDB / 20.0);
  clearFreqLookup();
}

void GData::clearFreqLookup()
{
  for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++) {
    (*it1)->clearFreqLookup();
  }
}

void GData::setAmplitudeMode(int amplitudeMode)
{
  if(amplitudeMode != _amplitudeMode) {
    _amplitudeMode = amplitudeMode;
    clearAmplitudeLookup();
  }
}

void GData::setPitchContourMode(int pitchContourMode)
{
  if(pitchContourMode != _pitchContourMode) {
    _pitchContourMode = pitchContourMode;
    clearFreqLookup();
  }
}

void GData::clearAmplitudeLookup()
{
  for(std::vector<Channel*>::iterator it1=channels.begin(); it1 != channels.end(); it1++) {
    (*it1)->clearAmplitudeLookup();
  }
}
  
int GData::getActiveIntThreshold()
{
  Channel* active = getActiveChannel();
  if(active) return toInt(active->threshold() * 100.0f);
  else return settings.getInt("Analysis", "thresholdValue");
}

void GData::resetActiveIntThreshold(int thresholdPercentage)
{
  Channel* active = getActiveChannel();
  if(active) active->resetIntThreshold(thresholdPercentage);
}
