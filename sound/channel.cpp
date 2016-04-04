/***************************************************************************
                          channel.cpp  -  description
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

#include "channel.h"
//#include "gdata.h"
#include "soundfile.h"
//#include "fundamental.h"

#include <algorithm>
#include "myassert.h"
#include "mystring.h"

/*
This filter was created in Matlab using
[Bb,Ab]=butter(2,(150/(sampleRate/2)),'high');
num2str(Bb, '%1.52f\n')
num2str(Ab, '%1.52f\n')
*/
double highPassFilterCoeffB[6][3] = {
  { //96000 Hz
    0.9930820351754101604768720790161751210689544677734375,
    -1.9861640703508203209537441580323502421379089355468750,
    0.9930820351754101604768720790161751210689544677734375
  },  { //48000 Hz
    0.9862119246270822925382049106701742857694625854492188,
    -1.9724238492541645850764098213403485715389251708984375,
    0.9862119246270822925382049106701742857694625854492188
  },  { //44100 Hz
    0.9850017578724193922923291211191099137067794799804688,
    -1.9700035157448387845846582422382198274135589599609375,
    0.9850017578724193922923291211191099137067794799804688
  },  { //22050 Hz
    0.9702283766983297308428291216841898858547210693359375,
    -1.9404567533966594616856582433683797717094421386718750,
    0.9702283766983297308428291216841898858547210693359375
  },  { //11025 Hz
    0.9413417959923573441471944533986970782279968261718750,
    -1.8826835919847146882943889067973941564559936523437500,
    0.9413417959923573441471944533986970782279968261718750
  },  { //8000 Hz
    0.9200661584291680572533778104116208851337432861328125,
    -1.8401323168583361145067556208232417702674865722656250,
    0.9200661584291680572533778104116208851337432861328125
  }
};

double highPassFilterCoeffA[6][3] = {
  { //96000 Hz
    1.0,
    -1.9861162115408896866597387997899204492568969726562500,
    0.9862119291607510662700519787904340773820877075195312
  },  { //48000 Hz
    1.0,
    -1.9722337291952660720539824978914111852645874023437500,
    0.9726139693130629870765346822736319154500961303710938
  },  { //44100 Hz
    1.0,
    -1.9697785558261799998547303403029218316078186035156250,
    0.9702284756634975693145861441735178232192993164062500
  },  { //22050 Hz
    1.0,
    -1.9395702073516702945710221683839336037635803222656250,
    0.9413432994416484067556893933215178549289703369140625
  },  { //11025 Hz
    1.0,
    -1.8792398422342264652229459898080676794052124023437500,
    0.8861273417352029113658318237867206335067749023437500
  },  { //8000 Hz
    1.0,
    -1.8337326589246480956774121295893564820289611816406250,
    0.8465319747920239112914941870258189737796783447265625
  }
};

/*
Channel::Channel() : lookup(0, 512), filteredData(0, 8192)
{
  _pitch_method = 2;
  color = Qt::black;
  visible = true;
  //setThreshold(0.97f);
  setIntThreshold(gdata->settings.getInt("Analysis", "thresholdValue"));
  //estimate = 0.0;
  freq = 0.0;
  mutex = new QMutex(true);
  filterStateX1 = filterStateX2 = 0.0;
  filterStateY1 = filterStateY2 = 0.0;
}
*/

Channel::Channel(SoundFile *parent_, int size_, int k_) : lookup(0, 128), filteredData(0, 8192)
{
  setParent(parent_);
  if(k_ == 0) k_ = (size_ + 1) / 2;
  directInput.resize(size_, 0.0);
  filteredInput.resize(size_, 0.0);
  nsdfData.resize(k_, 0.0);
  fftData1.resize(size_/2, 0.0);
  fftData2.resize(size_/2, 0.0);
  cepstrumData.resize(size_/2, 0.0);
  _pitch_method = 2;
  color = Qt::black;
  coefficients_table.resize(size_*4);
  //freqLookup.resize(0);
  //noteLookup.resize(0);
  //errorLookup.resize(0);
  //lookup.resize(0);
  //lookup.clear();
  visible = true;
  //setThreshold(0.97f);
  setIntThreshold(gdata->settings.getInt("Analysis", "thresholdValue"));
  //estimate = 0.0;
  freq = 0.0;
  mutex = new QMutex(true);
  highPassFilter = new Filter();
  int sampleRate = parent->rate();
  int filterIndex = 2;
  //choose the closest filter index
  if(sampleRate > (48000 + 96000) / 2) filterIndex = 0; //96000 Hz
  else if(sampleRate > (44100 + 48000) / 2) filterIndex = 1; //48000 Hz
  else if(sampleRate > (22050 + 44100) / 2) filterIndex = 2; //44100 Hz
  else if(sampleRate > (11025 + 22050) / 2) filterIndex = 3; //22050 Hz
  else if(sampleRate > (8000 + 11025) / 2) filterIndex = 4; //11025 Hz
  else filterIndex = 5; //8000 Hz
 // printf("filterIndex = %d\n", filterIndex);
  highPassFilter->make_IIR(highPassFilterCoeffB[filterIndex], highPassFilterCoeffA[filterIndex], 3);
  //highPassFilter->print();
  filterStateX1 = filterStateX2 = 0.0;
  filterStateY1 = filterStateY2 = 0.0;
}

Channel::~Channel()
{
  delete mutex;
  delete highPassFilter;
}

void Channel::resetIntThreshold(int thresholdPercentage)
{
  _threshold = float(thresholdPercentage) / 100.0f;
  uint j;
  for(j=0; j<lookup.size(); j++) {
    parent->myTransforms.chooseCorrelationIndex(lookup.at(j), threshold());
  }
  clearFreqLookup();
}

void Channel::resize(int newSize, int k_)
{
  //Array1d<float>::resize(newSize);
  coefficients_table.resize(newSize*4);
  if(k_ == 0) k_ = (newSize + 1) / 2;
  directInput.resize(newSize, 0.0);
  filteredInput.resize(newSize, 0.0);
  nsdfData.resize(k_, 0.0);
  fftData1.resize(newSize/2, 0.0);
  fftData2.resize(newSize/2, 0.0);
  cepstrumData.resize(newSize/2, 0.0);
  //freqLookup.resize(0);
  //noteLookup.resize(0);
  //errorLookup.resize(0);
  //lookup.resize(0);
  lookup.clear();
}

void Channel::shift_left(int n)
{
  //Array1d<float>::shift_left(n);
  directInput.shift_left(n);
  filteredInput.shift_left(n);
  coefficients_table.shift_left(n*4);
  //frame_num += n;
}

void Channel::calc_last_n_coefficients(int n)
{
  //build a table of coefficients for calculating interpolation
  //gdata->coefficients_table.shift_left(n*4);
	int start_pos = MAX(size() - n, 3);
	float *buf_pos = begin() + start_pos;
	float *coeff_pos = coefficients_table.begin()+start_pos*4;
	//printf("toRead=%d, length()=%d, w=%d, start_pos=%d\n", toRead, gdata->mainBuffer->length(), gdata->coefficients_table.w(), start_pos);
	for(;buf_pos < end(); buf_pos++) {
    const float &xm1 = buf_pos[-3];
    const float &x0 = buf_pos[-2];
    const float &x1 = buf_pos[-1];
    const float &x2 = buf_pos[0];
    *coeff_pos++ = (3 * (x0-x1) - xm1 + x2) / 2;
    *coeff_pos++ = 2*x1 + xm1 - (5*x0 + x2) / 2;
    *coeff_pos++ = (x1 - xm1) / 2;
    *coeff_pos++ = x0;
  }
}

/** Analysis the current data and add it to the end of the lookup table
  Note: The Channel shoud be locked before calling this.
*/
void Channel::processNewChunk(FilterState *filterState)
{
/*
  int n = framesPerChunk();
  if(!coefficients_table.isEmpty()) {
    calc_last_n_coefficients(n);
    //std::cout << coefficients_table;
  }
*/
  
  //printf("%d, %d, %d, %d\n", currentChunk(), parent->currentRawChunk(), parent->currentStreamChunk(), lookup.size());
  //fflush(stdout);
  //lock();
  myassert(parent->currentRawChunk() == MAX(0, parent->currentStreamChunk()-1) ||
           parent->currentRawChunk() == MAX(0, parent->currentStreamChunk()));
  lookup.push_back(AnalysisData());
  lookup.back().filterState = *filterState;
  //printf("lookup=%d\n", lookup.capacity());
  //AnalysisData &analysisData = lookup.back();
  //parent->myTransforms.calculateAnalysisData(begin(), analysisData, nsdfData.begin(), threshold());
  //parent->myTransforms.calculateAnalysisData(begin(), lookup.back(), nsdfData.begin(), threshold());
  //parent->myTransforms.calculateAnalysisData(begin(), lookup.back(), this, threshold());
  parent->myTransforms.calculateAnalysisData(begin(), int(lookup.size())-1, this, threshold());
  //unlock();
}

/** Analysis the current data and put it at chunk position in the lookup table
  Note: The Channel shoud be locked before calling this.
  @param chunk The chunk number to store the data at
*/
void Channel::processChunk(int chunk)
{
  //myassert(chunk >= 0 && chunk < totalChunks());
  //lock();
  if(chunk >= 0 && chunk < totalChunks())
    //parent->myTransforms.calculateAnalysisData(begin(), *dataAtChunk(chunk), nsdfData.begin(), threshold());
    //parent->myTransforms.calculateAnalysisData(begin(), *dataAtChunk(chunk), this, threshold());
    parent->myTransforms.calculateAnalysisData(begin(), chunk, this, threshold());
  //unlock();
}

void Channel::reset()
{
  std::fill(begin(), end(), 0.0f);
  std::fill(filteredInput.begin(), filteredInput.end(), 0.0f);
  std::fill(coefficients_table.begin(), coefficients_table.end(), 0.0f);
  //estimate = 0.0;
}

/*
QColor Channel::getNextColour()
{
	return colours[nextColour++ % 12];
}
*/

QString Channel::getUniqueFilename()
{
	QString endingStar = (parent->saved()) ? QString("") : QString("*");

	if (getParent()->channels.size() == 1) {
		return QString(getFilenamePart(getParent()->filename)) + endingStar;
		//return QString(getParent()->filename) + endingStar;
	} else {
		for (int i = 0; i < getParent()->channels.size(); i++) {
			if ( getParent()->channels.at(i) == this ) {
				return QString(getFilenamePart(getParent()->filename)) + " (" + QString::number(i+1) + ")" + endingStar;
				//return QString(getParent()->filename) + " (" + QString::number(i+1) + ")" + endingStar;
			}
		}
	}

	// If we're here, we didn't find the channel in the parent's channels array.
	// This should never happen!
	myassert(false);
	return QString(getFilenamePart(getParent()->filename)) + endingStar;
}

float Channel::averageNote(Channel *ch, int begin, int end)
{
  ChannelLocker channelLocker(ch);

  //myassert(begin >= 0 && begin < int(ch->lookup.size()));
  //myassert(end >= 0 && end < int(ch->lookup.size()));
  if(ch->totalChunks() == 0) return -1;
  if(!ch->hasAnalysisData()) return -1;
  begin = bound(begin, 0, ch->totalChunks()-1);
  end = bound(end, 0, ch->totalChunks());
  //if (begin > end || begin >= ch->lookup.size() || end >= ch->lookup.size()) return -1;

  // Init the total to be the first item if certain enough or zero if not
  float goodCount = 0.0f;
  float total = 0.0f;
  /*
  if (ch->dataAtChunk(begin)->correlation >= 0.9) {
    total = ch->dataAtChunk(begin)->note;
    goodCount = 1;
  }*/
  

  for (int i = begin; i < end; i++) {
    if (ch->dataAtChunk(i)->correlation >= 0.8) {
    //if (ch->isVisibleNote(ch->dataAtChunk(i)->noteIndex)) {
      total += ch->dataAtChunk(i)->note * ch->dataAtChunk(i)->correlation;
      //printf("Note is %f\n", ch->lookup.at(i).note);
      goodCount += ch->dataAtChunk(i)->correlation;
    }
  }
  //printf("begin = %d, end = %d\n", begin, end);
  //printf("total = %f, goodCount = %f\n", total, goodCount);
  if (goodCount == 0.0f) return -1;
  return (total / goodCount);
}

float Channel::averageMaxCorrelation(Channel *ch, int begin, int end)
{
  ChannelLocker channelLocker(ch);
  //myassert(begin >= 0 && begin < int(ch->lookup.size()));
  //myassert(end >= 0 && end < int(ch->lookup.size()));
  if(!ch->hasAnalysisData()) return -1;
  begin = bound(begin, 0, ch->totalChunks()-1);
  end = bound(end, 0, ch->totalChunks()-1);
  //if (begin > end || begin >= ch->lookup.size() || end >= ch->lookup.size()) return -1;

  // Init the total to be the first item
  float totalCorrelation = ch->dataAtChunk(begin)->correlation;

  for (int i = begin + 1; i < end; i++) {
     //total += ch->dataAtChunk(i)->volumeValue;
     totalCorrelation += ch->dataAtChunk(i)->correlation;
  }
  //return (total / (end - begin + 1));
  return (totalCorrelation / (end - begin + 1));
}

AnalysisData *Channel::getActiveChannelCurrentChunkData()
{
  Channel *active = gdata->getActiveChannel();
  return (active) ? active->dataAtCurrentChunk() : NULL;
}

void Channel::clearFreqLookup()
{
  ChannelLocker channelLocker(this);
  summaryZoomLookup.clear();
  normalZoomLookup.clear();
}

void Channel::clearAmplitudeLookup()
{
  ChannelLocker channelLocker(this);
  amplitudeZoomLookup.clear();
}

/**
  @param noteIndex_ the index of the note to inquire about
  @return true if the loudest part of the note is above the noiseThreshold
*/
bool Channel::isVisibleNote(int noteIndex_)
{
  myassert(noteIndex_ < (int)noteData.size());
  if(!(noteIndex_ >= 0)) return false;
  if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
    if(noteData[noteIndex_].maxRms >= gdata->noiseThreshold()) return true;
  } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
    if(noteData[noteIndex_].maxIntensity >= gdata->noiseThreshold()) return true;
  } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
    if((noteData[noteIndex_].maxCorrelation-1.0f) * 160.0f >= gdata->noiseThresholdDB()) return true;
  } else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
    if(noteData[noteIndex_].maxPurity >= gdata->noiseThresholdDB()) return true;
  }
  return false;
}

bool Channel::isVisibleChunk(AnalysisData *data)
{
  myassert(data);
  if(gdata->pitchContourMode() == 0) {
    if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
      if(data->rms >= gdata->noiseThreshold()) return true;
    } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
      if(data->maxIntensity >= gdata->noiseThreshold()) return true;
    } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
      if((data->correlation-1.0f) * 160.0f >= gdata->noiseThresholdDB()) return true;
    } else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
      if(data->volumeValue >= gdata->noiseThresholdDB()) return true;
    }
  } else {
    return isVisibleNote(data->noteIndex);
  }
  return false;
}

/**
  @param noteIndex_ the index of the note to inquire about
  @return true if the note is long enough
*/
bool Channel::isLabelNote(int noteIndex_)
{
  myassert(noteIndex_ < (int)noteData.size());
  if(noteIndex_ >= 0 && noteData[noteIndex_].size() > 4) return true;
  else return false;
}

