/***************************************************************************
                          channel.cpp  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "channel.h"
#include "soundfile.h"

#include <algorithm>
#include "myassert.h"
#include "mystring.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "FastSmoothedAveragingFilter.h"
#include "FixedAveragingFilter.h"
#include "GrowingAveragingFilter.h"
#include "prony.h"
#include "musicnotes.h"
#include "bspline.h"

const double shortTime = 0.08;
const float shortBase = 0.1f;
const float shortStretch = 1.5;

const double longTime = 0.8;
const float longBase = 0.02f;
const float longStretch = 0.2f;


/**
  This filter was created in Matlab using
  [Bb,Ab]=butter(2,(150/(sampleRate/2)),'high');
  num2str(Bb, '%1.52f\n')
  num2str(Ab, '%1.52f\n')
*/

double highPassFilterCoeffB[6][3] =
  {
    { //96000 Hz
      0.9930820351754101604768720790161751210689544677734375,
      -1.9861640703508203209537441580323502421379089355468750,
      0.9930820351754101604768720790161751210689544677734375
    },
    { //48000 Hz
      0.9862119246270822925382049106701742857694625854492188,
      -1.9724238492541645850764098213403485715389251708984375,
      0.9862119246270822925382049106701742857694625854492188
    },
    { //44100 Hz
      0.9850017578724193922923291211191099137067794799804688,
      -1.9700035157448387845846582422382198274135589599609375,
      0.9850017578724193922923291211191099137067794799804688
    },
    { //22050 Hz
      0.9702283766983297308428291216841898858547210693359375,
      -1.9404567533966594616856582433683797717094421386718750,
      0.9702283766983297308428291216841898858547210693359375
    },
    { //11025 Hz
      0.9413417959923573441471944533986970782279968261718750,
      -1.8826835919847146882943889067973941564559936523437500,
      0.9413417959923573441471944533986970782279968261718750
    },
    { //8000 Hz
      0.9200661584291680572533778104116208851337432861328125,
      -1.8401323168583361145067556208232417702674865722656250,
      0.9200661584291680572533778104116208851337432861328125
    }
  };

double highPassFilterCoeffA[6][3] =
  {
    { //96000 Hz
      1.0,
      -1.9861162115408896866597387997899204492568969726562500,
      0.9862119291607510662700519787904340773820877075195312
    },
    { //48000 Hz
      1.0,
      -1.9722337291952660720539824978914111852645874023437500,
      0.9726139693130629870765346822736319154500961303710938
    },
    { //44100 Hz
      1.0,
      -1.9697785558261799998547303403029218316078186035156250,
      0.9702284756634975693145861441735178232192993164062500
    },
    { //22050 Hz
      1.0,
      -1.9395702073516702945710221683839336037635803222656250,
      0.9413432994416484067556893933215178549289703369140625
    },
    { //11025 Hz
      1.0,
      -1.8792398422342264652229459898080676794052124023437500,
      0.8861273417352029113658318237867206335067749023437500
    },
    { //8000 Hz
      1.0,
      -1.8337326589246480956774121295893564820289611816406250,
      0.8465319747920239112914941870258189737796783447265625
    }
  };

/**
  This filter was created in matlab using
  0  [Bb,Ab]=butter(2,1000/(44100/2),'low');
  num2str(Bb, '%1.52f\n')
  num2str(Ab, '%1.52f\n')
  1  [Bb,Ab]=butter(2,0.25,'low');
  2  [Bb,Ab]=butter(2,50/(44100/2),'low');
*/
double lowPassFilterCoeffB[3][3] =
  {
    { //44100 Hz
      0.0046039984750224638432314350211527198553085327148438,
      0.0092079969500449276864628700423054397106170654296875,
      0.0046039984750224638432314350211527198553085327148438
    },
    {
      0.0976310729378175312653809214680222794413566589355469,
      0.1952621458756350625307618429360445588827133178710938,
      0.0976310729378175312653809214680222794413566589355469
    },
    {
      0.0000126234651238732453748525585979223251342773437500,
      0.0000252469302477464907497051171958446502685546875000,
      0.0000126234651238732453748525585979223251342773437500
    }
  };

double lowPassFilterCoeffA[3][3] =
  {
    { //44100 Hz
      1.0,
      -1.7990964094846682019834815946524031460285186767578125,
      0.8175124033847580573564073347370140254497528076171875
    },
    {
      1.0,
      -0.9428090415820633563015462641487829387187957763671875,
      0.3333333333333333703407674875052180141210556030273438
    },
    {
      1.0,
      -1.9899255200849252922523646702757105231285095214843750,
      0.9899760139454206742115616179944481700658798217773438
    }
  };

//------------------------------------------------------------------------------
Channel::Channel(SoundFile *p_parent, int p_size, int p_k) :
  parent(p_parent),
  freq(0.0),
  _pitch_method(2),
  visible(true),
  noteIsPlaying(false),
  lookup(0, 128),
  mutex(new QMutex(true)),
  isLocked(false),
  pronyWindowSize(int(ceil(0.4 / timePerChunk()))),
  fastSmooth(new fast_smooth(p_size / 8)),
  color(Qt::black),
  nsdfAggregateRoof(0.0),
  highPassFilter(NULL),
  pitchSmallSmoothingFilter(new GrowingAverageFilter(parent->rate() / 64)),
  pitchBigSmoothingFilter(new FastSmoothedAveragingFilter(parent->rate() / 16)),
  rmsFloor(gdata->dBFloor()),
  rmsCeiling(gdata->dBFloor())
{
  if(p_k == 0)
    {
      p_k = (p_size + 1) / 2;
    }
  directInput.resize(p_size, 0.0);
  filteredInput.resize(p_size, 0.0);
  nsdfData.resize(p_k, 0.0);
  nsdfAggregateData.resize(p_k, 0.0);
  nsdfAggregateDataScaled.resize(p_k, 0.0);
  fftData1.resize(p_size / 2, 0.0);
  fftData2.resize(p_size / 2, 0.0);
  cepstrumData.resize(p_size / 2, 0.0);
  detailedPitchData.resize(p_size / 2, 0.0);
  detailedPitchDataSmoothed.resize(p_size / 2, 0.0);
  coefficients_table.resize(p_size * 4);
  pronyData.resize(pronyWindowSize);

  setIntThreshold(gdata->getSettingsValue("Analysis/thresholdValue", 93));
  int filterIndex = 2;
  int sampleRate = parent->rate();
  if(sampleRate > (48000 + 96000) / 2)
    {
      filterIndex = 0; //96000 Hz
    }
  else if(sampleRate > (44100 + 48000) / 2)
    {
      filterIndex = 1; //48000 Hz
    }
  else if(sampleRate > (22050 + 44100) / 2)
    {
      filterIndex = 2; //44100 Hz
    }
  else if(sampleRate > (11025 + 22050) / 2)
    {
      filterIndex = 3; //22050 Hz
    }
  else if(sampleRate > (8000 + 11025) / 2)
    {
      filterIndex = 4; //11025 Hz
    }
  else
    {
      filterIndex = 5; //8000 Hz
    }
  highPassFilter = new IIR_Filter(highPassFilterCoeffB[filterIndex], highPassFilterCoeffA[filterIndex], 3);

}

//------------------------------------------------------------------------------
Channel::~Channel(void)
{
  delete fastSmooth;
  delete mutex;
  delete highPassFilter;
  delete pitchSmallSmoothingFilter;
  delete pitchBigSmoothingFilter;
}

//------------------------------------------------------------------------------
void Channel::resetIntThreshold(int thresholdPercentage)
{
  _threshold = float(thresholdPercentage) / 100.0f;
  uint j;
  for(j = 0; j < lookup.size(); j++)
    {
      chooseCorrelationIndex(j, periodOctaveEstimate(j));
      calcDeviation(j);
    }
  clearFreqLookup();
}

//------------------------------------------------------------------------------
void Channel::resize(int newSize, int k_)
{
  coefficients_table.resize(newSize*4);
  if(k_ == 0)
    {
      k_ = (newSize + 1) / 2;
    }
  directInput.resize(newSize, 0.0);
  filteredInput.resize(newSize, 0.0);
  nsdfData.resize(k_, 0.0);
  nsdfAggregateData.resize(k_, 0.0);
  nsdfAggregateDataScaled.resize(k_, 0.0);
  fftData1.resize(newSize / 2, 0.0);
  fftData2.resize(newSize / 2, 0.0);
  cepstrumData.resize(newSize / 2, 0.0);
  detailedPitchData.resize(newSize / 2, 0.0);
  detailedPitchDataSmoothed.resize(newSize / 2, 0.0);
  lookup.clear();
}

//------------------------------------------------------------------------------
void Channel::shift_left(int n)
{
  directInput.shift_left(n);
  filteredInput.shift_left(n);
  coefficients_table.shift_left(n * 4);
}

//------------------------------------------------------------------------------
void Channel::calc_last_n_coefficients(int n)
{
  //build a table of coefficients for calculating interpolation
  int start_pos = MAX(size() - n, 3);
  float *buf_pos = begin() + start_pos;
  float *coeff_pos = coefficients_table.begin() + start_pos * 4;
#ifdef DEBUG_PRINTF
  printf("toRead=%d, length()=%d, w=%d, start_pos=%d\n", toRead, gdata->mainBuffer->length(), gdata->coefficients_table.w(), start_pos);
#endif // DEBUG_PRINTF

  for(;buf_pos < end(); buf_pos++)
    {
      const float &xm1 = buf_pos[-3];
      const float &x0 = buf_pos[-2];
      const float &x1 = buf_pos[-1];
      const float &x2 = buf_pos[0];
      *coeff_pos++ = (3 * (x0 - x1) - xm1 + x2) / 2;
      *coeff_pos++ = 2 * x1 + xm1 - (5 * x0 + x2) / 2;
      *coeff_pos++ = (x1 - xm1) / 2;
      *coeff_pos++ = x0;
    }
}

//------------------------------------------------------------------------------
void Channel::processNewChunk(FilterState *filterState)
{
  myassert(locked());
#ifdef DEBUG_PRINTF
  printf("%d, %d, %d, %d\n", currentChunk(), parent->currentRawChunk(), parent->currentStreamChunk(), lookup.size());
#endif // DEBUG_PRINTF
  myassert(parent->currentRawChunk() == MAX(0, parent->currentStreamChunk() - 1) ||
           parent->currentRawChunk() == MAX(0, parent->currentStreamChunk()));
  lookup.push_back(AnalysisData());
  lookup.back().setFilterState(*filterState);
#ifdef DEBUG_PRINTF
  printf("lookup=%d\n", lookup.capacity());
#endif // DEBUG_PRINTF
  parent->calculateAnalysisData(/*begin(), */int(lookup.size()) - 1, this/*, threshold()*/);
}

//------------------------------------------------------------------------------
void Channel::processChunk(int chunk)
{
  myassert(locked());
  if(chunk >= 0 && chunk < totalChunks())
    {
      parent->calculateAnalysisData(/*begin(), */chunk, this/*, threshold()*/);
    }
}

//------------------------------------------------------------------------------
void Channel::reset(void)
{
  std::fill(begin(), end(), 0.0f);
  std::fill(filteredInput.begin(), filteredInput.end(), 0.0f);
  std::fill(coefficients_table.begin(), coefficients_table.end(), 0.0f);
}

//------------------------------------------------------------------------------
QString Channel::getUniqueFilename(void) const
{
  QString endingStar = (parent->saved()) ? QString("") : QString("*");

  if (getParent()->numChannels() == 1)
    {
      return QString(getFilenamePart(getParent()->getFileName())) + endingStar;
    }
  else
    {
      int l_index = getParent()->getChannelIndex(*this);
      return QString(getFilenamePart(getParent()->getFileName())) + " (" + QString::number(l_index + 1) + ")" + endingStar;
    }

  // If we're here, we didn't find the channel in the parent's channels array.
  // This should never happen!
  myassert(false);
  return QString(getFilenamePart(getParent()->getFileName())) + endingStar;
}

//------------------------------------------------------------------------------
float Channel::averagePitch(int begin, int end) const
{
  if(begin < 0)
    {
      begin = 0;
    }
  if (begin >= end)
    {
      return -1;
    }
  myassert(locked());
  myassert(isValidChunk(begin) && isValidChunk(end-1));

  // Init the total to be the first item if certain enough or zero if not

  //do a weighted sum (using cosine window smoothing) to find the average note
  double goodCount = 0.0;
  double total = 0.0;
  double size = double(end - begin);
  double window_pos, window_weight, weight;
  const AnalysisData *data;
  for (int i = begin; i < end; i++)
    {
      window_pos = double(i - begin) / size;
      window_weight = 0.5 - 0.5 * cos(window_pos * (2 * PI));
      data = dataAtChunk(i);
      weight = window_weight * data->getCorrelation() * dB2Linear(data->getLogRms());
      total += data->getPitch() * weight;
      goodCount += weight;
    }
  return float(total / goodCount);
}

//------------------------------------------------------------------------------
float Channel::averageMaxCorrelation(int begin, int end) const
{
  myassert(locked());
  if(!hasAnalysisData())
    {
      return -1;
    }
  begin = bound(begin, 0, totalChunks() - 1);
  end = bound(end, 0, totalChunks() - 1);

  // Init the total to be the first item
  float totalCorrelation = dataAtChunk(begin)->getCorrelation();

  for (int i = begin + 1; i < end; i++)
    {
      totalCorrelation += dataAtChunk(i)->getCorrelation();
    }
  return (totalCorrelation / (end - begin + 1));
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::getActiveChannelCurrentChunkData(void)
{
  Channel *active = gdata->getActiveChannel();
  return (active) ? active->dataAtCurrentChunk() : NULL;
}

//------------------------------------------------------------------------------
void Channel::clearFreqLookup(void)
{
  ChannelLocker channelLocker(this);
  summaryZoomLookup.clear();
  normalZoomLookup.clear();
}

//------------------------------------------------------------------------------
void Channel::clearAmplitudeLookup(void)
{
  ChannelLocker channelLocker(this);
  amplitudeZoomLookup.clear();
}

//------------------------------------------------------------------------------
void Channel::recalcScoreThresholds(void)
{
  AnalysisData *d;
  ChannelLocker channelLocker(this);
  for(int j = 0; j < totalChunks(); j++)
    {
      if((d = dataAtChunk(j)) != NULL)
	{
	  d->calcScores();
	}
    }
}

//------------------------------------------------------------------------------
bool Channel::isVisibleNote(int noteIndex_) const
{
  myassert(noteIndex_ < (int)noteData.size());
  if(noteIndex_ == NO_NOTE)
    {
      return false;
    }
  return true;
}

//------------------------------------------------------------------------------
bool Channel::isVisibleChunk(const AnalysisData *data) const
{
  myassert(data);
  if(data->getNoteScore() >= gdata->ampThreshold(NOTE_SCORE,0))
    {
      return true;
    }
  return false;
}

//------------------------------------------------------------------------------
bool Channel::isChangingChunk(AnalysisData *data) const
{
  myassert(data);
  if(data->getNoteChangeScore() >= gdata->ampThreshold(NOTE_CHANGE_SCORE,0))
    {
      return true;
    }
  else
    {
      return false;
    }
}

//------------------------------------------------------------------------------
void Channel::backTrackNoteChange(int chunk)
{
  int first = MAX(chunk - (int)ceil(longTime / timePerChunk()), getLastNote()->startChunk());
#ifdef DEBUG_PRINTF
  printf("ceil = %d, %d\n", (int)ceil(longTime / timePerChunk()), chunk-first);
#endif // DEBUG_PRINTF
  int last = chunk; //currentNote->endChunk();
  if(first >= last)
    {
      return;
    }
  float largestWeightedDiff = 0.0f;
  int largestDiffChunk = first;
  for(int curChunk = first + 1, j = 1; curChunk <= last; curChunk++, j++)
    {
      float weightedDiff = fabs(dataAtChunk(curChunk)->getPitch() - dataAtChunk(curChunk)->getShortTermMean());
      if(weightedDiff > largestWeightedDiff)
	{
	  largestWeightedDiff = weightedDiff;
	  largestDiffChunk = curChunk;
	}
    }
  getLastNote()->setEndChunk(largestDiffChunk);
  getLastNote()->recalcAvgPitch();
  dataAtChunk(largestDiffChunk)->setReason(5);

  //start on next note
  for(int curChunk = largestDiffChunk; curChunk <= last; curChunk++)
    {
      dataAtChunk(curChunk)->setNoteIndex(NO_NOTE);
      dataAtChunk(curChunk)->setNotePlaying(false);
      dataAtChunk(curChunk)->setShortTermMean(dataAtChunk(curChunk)->getPitch());
      dataAtChunk(curChunk)->setLongTermMean(dataAtChunk(curChunk)->getPitch());
      dataAtChunk(curChunk)->setShortTermDeviation(0.2f);
      dataAtChunk(curChunk)->setLongTermDeviation(0.05f);
      dataAtChunk(curChunk)->setPeriodRatio(1.0f);
    }

  int curChunk = largestDiffChunk;
  if(curChunk < last)
    {
      curChunk++;
    }
  while((curChunk < last) && !isVisibleChunk(dataAtChunk(curChunk)))
    {
      curChunk++;
    }
  if((curChunk < last) && isVisibleChunk(dataAtChunk(curChunk)))
    {
      noteIsPlaying = true;
      noteBeginning(curChunk);
      NoteData * currentNote = getLastNote();
      myassert(currentNote);
      dataAtChunk(curChunk)->setNoteIndex(getCurrentNoteIndex());
      dataAtChunk(curChunk)->setNotePlaying(true);
      curChunk++;
      while((curChunk < last) && isVisibleChunk(dataAtChunk(curChunk)))
	{
	  dataAtChunk(curChunk)->setNoteIndex(getCurrentNoteIndex());
	  dataAtChunk(curChunk)->setNotePlaying(true);
	  currentNote->addData(*dataAtChunk(curChunk), float(framesPerChunk()) / float(dataAtChunk(curChunk)->getPeriod()));
	  curChunk++;
	}
      //just start the NSDF Aggregate from where we are now
      resetNSDFAggregate(dataAtChunk(last - 1)->getPeriod());
      //just start with the octave estimate from the last note
      currentNote->setPeriodOctaveEstimate(getNote(getCurrentNoteIndex() - 1)->periodOctaveEstimate());
    }
}

//------------------------------------------------------------------------------
bool Channel::isNoteChanging(int chunk)
{
  AnalysisData *prevData = dataAtChunk(chunk-1);
  if(!prevData)
    {
      return false;
    }
  myassert(dataAtChunk(chunk));
  myassert(noteData.size() > 0);
  AnalysisData *analysisData = dataAtChunk(chunk);
  //Note: analysisData.noteIndex is still undefined here
  int numChunks = getLastNote()->numChunks();

  float diff = fabs(analysisData->getPitch() - analysisData->getShortTermMean());
  double spread = fabs(analysisData->getShortTermMean() - analysisData->getLongTermMean()) -
    (analysisData->getShortTermDeviation() + analysisData->getLongTermDeviation());
  if(numChunks >= 5 && spread > 0.0)
    {
      analysisData->setReason(1);
      return true;
    }

  int firstShortChunk = MAX(chunk - (int)ceil(shortTime/timePerChunk()), getLastNote()->startChunk());
  AnalysisData *firstShortData = dataAtChunk(firstShortChunk);
  double spread2 = fabs(analysisData->getShortTermMean() - firstShortData->getLongTermMean()) -
    (analysisData->getShortTermDeviation() + firstShortData->getLongTermDeviation());
  analysisData->setSpread(spread);
  analysisData->setSpread2(spread2);

  if(numChunks >= (int)(ceil(longTime/timePerChunk()) / 2.0) && spread2 > 0.0)
    {
      analysisData->setReason(4);
      return true;
    }
  if(numChunks > 1 && diff > 2)
    { //if jumping to fast anywhere then note is changing
#ifdef DEBUG_PRINTF
      printf("numChunks=%d\n", getCurrentNote()->numChunks());
      printf("analysisData->pitch=%f, ", analysisData->pitch);
      printf("prevData->shortTermMean=%f\n", prevData->shortTermMean);
#endif // DEBUG_PRINTF
      analysisData->setReason(2);
      return true;
    }
  return false;
}

//------------------------------------------------------------------------------
bool Channel::isLabelNote(int noteIndex_) const
{
  myassert(noteIndex_ < (int)noteData.size());
  if(noteIndex_ >= 0 && noteData[noteIndex_].isValid())
    {
      return true;
    }
  else
    {
      return false;
    }
}

//------------------------------------------------------------------------------
void Channel::processNoteDecisions(int chunk, float periodDiff)
{
  myassert(locked());
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);

  analysisData.setReason(0);
  //look for note transitions
  if(noteIsPlaying)
    {
      if(isVisibleChunk(&analysisData) && !isNoteChanging(chunk))
	{
	}
      else
	{
#ifdef DEBUG_PRINTF
	  printf("ending chunk %d, ", chunk);
	  printf("%s, %s\n", isVisibleChunk(&analysisData)?"T":"F", isNoteChanging(chunk)?"T":"F");
#endif // DEBUG_PRINTF
	  noteIsPlaying = false;
	  noteEnding(chunk);
	}
    }
  else
    {
      if(isVisibleChunk(&analysisData))
	{
	  noteBeginning(chunk);
	  periodDiff = 0.0f;
	  noteIsPlaying = true;
	}
    }

  analysisData.setNotePlaying(noteIsPlaying);

  if(noteIsPlaying)
    {
      addToNSDFAggregate(dB2Linear(analysisData.getLogRms()), periodDiff);
      NoteData *currentNote = getLastNote();
      myassert(currentNote);

      analysisData.setNoteIndex(getCurrentNoteIndex());
      currentNote->setEndChunk(chunk + 1);

      currentNote->addData(analysisData, float(framesPerChunk()) / float(analysisData.getPeriod()));
      currentNote->setPeriodOctaveEstimate(calcOctaveEstimate());
      if(gdata->analysisType() != MPM_MODIFIED_CEPSTRUM)
	{
	  recalcNotePitches(chunk);
	}
    }
  else
    {
    }
}

//------------------------------------------------------------------------------
void Channel::noteBeginning(int chunk)
{
  AnalysisData *analysisData = dataAtChunk(chunk);
  noteData.push_back(NoteData(*this, chunk, *analysisData));
  //initalise the aggregate NSDF data with the current NSDF data
  resetNSDFAggregate(analysisData->getPeriod());
#ifdef DEBUG_PRINTF
  printf("Note Beginning: period = %f\n", analysisData->period);
#endif // DEBUG_PRINTF

  pitchSmallSmoothingFilter->reset();
  pitchBigSmoothingFilter->reset();
}

//------------------------------------------------------------------------------
void Channel::noteEnding(int chunk)
{
  AnalysisData &analysisData = *dataAtChunk(chunk);
  //recalcNotePitches(chunk);
  if(analysisData.getReason() > 0)
    {
      backTrackNoteChange(chunk);
    }
}

//------------------------------------------------------------------------------
float Channel::calcOctaveEstimate(void)
{
  Array1d<float> agData = nsdfAggregateDataScaled;
  std::vector<int> nsdfAggregateMaxPositions;
  MyTransforms::findNSDFMaxima(agData.begin(), agData.size(), nsdfAggregateMaxPositions);
  if(nsdfAggregateMaxPositions.empty())
    {
      return -1.0;
    }
  
  //get the highest nsdfAggregateMaxPosition
  uint j;
  int nsdfAggregateMaxIndex = 0;
  for(j = 1; j < nsdfAggregateMaxPositions.size(); j++)
    {
      if(agData[nsdfAggregateMaxPositions[j]] > agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]])
	{
	  nsdfAggregateMaxIndex = j;
	}
    }
  //get the choosen nsdfAggregateMaxPosition
  double highest = agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]];
  double nsdfAggregateCutoff = highest * threshold();
  // Allow for overide by the threshold value
  int nsdfAggregateChoosenIndex = 0;
  for(j = 0; j < nsdfAggregateMaxPositions.size(); j++)
    {
      if(agData[nsdfAggregateMaxPositions[j]] >= nsdfAggregateCutoff)
	{
	  nsdfAggregateChoosenIndex = j;
	  break;
	}
    }
 //add 1 for index offset, ie position 0 = 1 period
  float periodEstimate = float(nsdfAggregateMaxPositions[nsdfAggregateChoosenIndex] + 1);
  return periodEstimate;
}

//------------------------------------------------------------------------------
void Channel::recalcNotePitches(int chunk)
{
  if(!isValidChunk(chunk))
    {
      return;
    }

  //recalculate the values for the note using the overall periodOctaveEstimate
  NoteData *currentNote = getLastNote();
  if(currentNote == NULL)
    {
      return;
    }
  myassert(currentNote);
  int first = currentNote->startChunk();
  int last = chunk;
  currentNote->resetData();
  int numNotesChangedIndex = 0;
  for(int curChunk=first; curChunk <= last; curChunk++)
    {
      if(chooseCorrelationIndex(curChunk, periodOctaveEstimate(curChunk)))
	{
	  numNotesChangedIndex++;
	}
      calcDeviation(curChunk);
      currentNote->addData(*dataAtChunk(curChunk), float(framesPerChunk()) / float(dataAtChunk(curChunk)->getPeriod()));
    }
#ifdef DEBUG_PRINTF
  printf("numNotesChangedIndex=%d/%d\n", numNotesChangedIndex, last - first + 1);
#endif // DEBUG_PRINTF
}

//------------------------------------------------------------------------------
const NoteData * Channel::getLastNote(void) const
{
  return (noteData.empty()) ? NULL : &noteData.back();
}

//------------------------------------------------------------------------------
NoteData * Channel::getLastNote(void)
{
  return (noteData.empty()) ? NULL : &noteData.back();
}

//------------------------------------------------------------------------------
const NoteData * Channel::getCurrentNote(void) const
{
  const AnalysisData *analysisData = dataAtCurrentChunk();
  if(analysisData)
    {
      int noteIndex = analysisData->getNoteIndex();
      if(noteIndex >= 0 && noteIndex < (int)noteData.size())
	{
	  return &noteData[noteIndex];
	}
    }
  return NULL;
}

//------------------------------------------------------------------------------
const NoteData * Channel::getNote(int noteIndex) const
{
  if(noteIndex >= 0 && noteIndex < (int)noteData.size())
    {
      return &noteData[noteIndex];
    }
  else
    {
      return NULL;
    }
}

//------------------------------------------------------------------------------
void Channel::chooseCorrelationIndex1(int chunk)
{
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  if(analysisData.isPeriodEstimatesEmpty())
    { //no period found
      return;
    }
  //choose a cutoff value based on the highest value and a relative threshold
  float highest = analysisData.getPeriodEstimatesAmpAt(analysisData.getHighestCorrelationIndex());
  float cutoff = highest * threshold();
  //find the first of the maxPositions which is above the cutoff
  for(iterPos = 0; iterPos < analysisData.getPeriodEstimatesAmpSize(); iterPos++)
    {
      if(analysisData.getPeriodEstimatesAmpAt(iterPos) >= cutoff)
	{
	  choosenMaxIndex = iterPos;
	  break;
	}
    }
  analysisData.setChosenCorrelationIndex(choosenMaxIndex);
  analysisData.setCorrelation(analysisData.getPeriodEstimatesAmpAt(choosenMaxIndex));
  
  analysisData.setPeriod(analysisData.getPeriodEstimatesAt(choosenMaxIndex));
  double freq = rate() / analysisData.getPeriod();
  analysisData.setFundamentalFreq(float(freq));
  analysisData.setPitch(bound(freq2pitch(freq), 0.0, gdata->topPitch()));
  analysisData.setPitchSum((double)analysisData.getPitch());
  analysisData.setPitch2Sum(sq((double)analysisData.getPitch()));
}

//------------------------------------------------------------------------------
bool Channel::chooseCorrelationIndex(int chunk, float periodOctaveEstimate)
{
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  bool isDifferentIndex = false;
  if(analysisData.isPeriodEstimatesEmpty())
    {
      //no period found
      return false;
    }
  if(gdata->analysisType() == MPM || gdata->analysisType() == MPM_MODIFIED_CEPSTRUM)
    {
      //choose the periodEstimate which is closest to the periodOctaveEstimate
      float minDist = fabs(analysisData.getPeriodEstimatesAt(0) - periodOctaveEstimate);
      float dist;
      for(iterPos = 1; iterPos < analysisData.getPeriodEstimatesSize(); iterPos++)
	{
	  dist = fabs(analysisData.getPeriodEstimatesAt(iterPos) - periodOctaveEstimate);
	  if(dist < minDist)
	    {
	      minDist = dist;
	      choosenMaxIndex = iterPos;
	    }
	}
  }
  else
    {
      choosenMaxIndex = analysisData.getHighestCorrelationIndex();
    }
  if(choosenMaxIndex != analysisData.getChosenCorrelationIndex())
    {
      isDifferentIndex = true;
    }
  analysisData.setChosenCorrelationIndex(choosenMaxIndex);
  analysisData.setCorrelation(analysisData.getPeriodEstimatesAmpAt(choosenMaxIndex));
  
  analysisData.setPeriod(analysisData.getPeriodEstimatesAt(choosenMaxIndex));
  double freq = rate() / analysisData.getPeriod();
  analysisData.setFundamentalFreq(float(freq));
  analysisData.setPitch(bound(freq2pitch(freq), 0.0, gdata->topPitch()));
  if(chunk > 0 && !isFirstChunkInNote(chunk))
    {
      analysisData.setPitchSum(dataAtChunk(chunk - 1)->getPitchSum() + (double)analysisData.getPitch());
      analysisData.setPitch2Sum(dataAtChunk(chunk - 1)->getPitch2Sum() + sq((double)analysisData.getPitch()));
    }
  else
    {
      analysisData.setPitchSum((double)analysisData.getPitch());
      analysisData.setPitch2Sum(sq((double)analysisData.getPitch()));
    }
  return isDifferentIndex;
}

//------------------------------------------------------------------------------
void Channel::calcDeviation(int chunk)
{
  int lastChunk = chunk;
  int currentNoteIndex = getCurrentNoteIndex();
  myassert(dataAtChunk(chunk));
  AnalysisData &lastChunkData = *dataAtChunk(lastChunk);
  if(currentNoteIndex < 0)
    {
      return;
    }

  //Do long term calculation
  int firstChunk = MAX(lastChunk - (int)ceil(longTime/timePerChunk()), noteData[currentNoteIndex].startChunk());
  AnalysisData *firstChunkData = dataAtChunk(firstChunk);
  int numChunks = (lastChunk - firstChunk);
  double mean_sum, mean, sumX2, variance, standard_deviation;
  if(numChunks > 0)
    {
      mean_sum = (lastChunkData.getPitchSum() - firstChunkData->getPitchSum());
      mean = mean_sum / double(numChunks);
      lastChunkData.setLongTermMean(mean);
      sumX2 = (lastChunkData.getPitch2Sum() - firstChunkData->getPitch2Sum());
      variance = sumX2 / double(numChunks) - sq(mean);
      standard_deviation = sqrt(fabs(variance));
      lastChunkData.setLongTermDeviation(longBase + sqrt(standard_deviation)*longStretch);
    }
  else
    {
      lastChunkData.setLongTermMean(firstChunkData->getPitch());
      lastChunkData.setLongTermDeviation(longBase);
    }

  //Do short term calculation
  firstChunk = MAX(lastChunk - (int)ceil(shortTime/timePerChunk()), noteData[currentNoteIndex].startChunk());
  firstChunkData = dataAtChunk(firstChunk);
  numChunks = (lastChunk - firstChunk);
  if(numChunks > 0)
    {
      mean_sum = (lastChunkData.getPitchSum() - firstChunkData->getPitchSum());
      mean = mean_sum / double(numChunks);
      lastChunkData.setShortTermMean(mean);
      sumX2 = (lastChunkData.getPitch2Sum() - firstChunkData->getPitch2Sum());
      variance = sumX2 / double(numChunks) - sq(mean);
      standard_deviation = sqrt(fabs(variance));
      lastChunkData.setShortTermDeviation(shortBase + sqrt(standard_deviation) * shortStretch);
    }
  else
    {
      lastChunkData.setShortTermMean(firstChunkData->getPitch());
      lastChunkData.setShortTermDeviation(shortBase);
    }
}

//------------------------------------------------------------------------------
bool Channel::isFirstChunkInNote(int chunk) const
{
  const AnalysisData *analysisData = dataAtChunk(chunk);
  if(analysisData && analysisData->getNoteIndex() >= 0 && noteData[analysisData->getNoteIndex()].startChunk() == chunk)
    {
      return true;
    }
  else
    {
      return false;
    }
}

//------------------------------------------------------------------------------
void Channel::resetNSDFAggregate(float period)
{
  nsdfAggregateRoof = 0.0;
  std::fill(nsdfAggregateData.begin(), nsdfAggregateData.end(), 0.0f);
  std::fill(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), 0.0f);

  NoteData *currentNote = getLastNote();
  myassert(currentNote);
  currentNote->set_nsdf_aggregate_roof(0.0);
  currentNote->set_current_nsdf_period(period);
  currentNote->set_first_nsdf_period(period);
}

//------------------------------------------------------------------------------
void Channel::addToNSDFAggregate(const float scaler, float periodDiff)
{
  AnalysisData * analysisData = dataAtCurrentChunk();

  nsdfAggregateRoof += scaler;
  addElements(nsdfAggregateData.begin(), nsdfAggregateData.end(), nsdfData.begin(), scaler);

  NoteData *currentNote = getLastNote();
  myassert(currentNote);
  currentNote->add_nsdf_aggregate_roof(scaler);
  currentNote->add_current_nsdf_period(periodDiff);
  float periodRatio = currentNote->get_current_nsdf_period() / currentNote->get_first_nsdf_period();
  analysisData->setPeriodRatio(periodRatio);
#ifdef DEBUG_PRINTF
  printf("%f, periodDiff = %f\n", currentNote->currentNsdfPeriod, periodDiff);
  printf("currentNsdfPeriod = %f\n", currentNote->currentNsdfPeriod);
  printf("ratio=%f\n", periodRatio);
#endif // DEBUG_PRINTF
  int len = nsdfData.size();
  float stretch_len = float(len) * periodRatio;
  Array1d<float> stretch_data(len);

  //the scaled version
  stretch_array(len, nsdfData.begin(), len, stretch_data.begin(), 0.0f, stretch_len, LINEAR);
  addElements(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), stretch_data.begin(), scaler);
  copyElementsDivide(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), currentNote->get_nsdf_aggregate_data_scaled().begin(), currentNote->get_nsdf_aggregate_roof());

  //the unscaled version
  copyElementsDivide(nsdfAggregateData.begin(), nsdfAggregateData.end(), currentNote->get_nsdf_aggregate_data().begin(), currentNote->get_nsdf_aggregate_roof());

#ifdef DEBUG_PRINTF
  printf("chunk = %d, ", currentChunk());
  printf("scaler = %f, ", scaler);
  printf("periodDiff = %f, ", periodDiff);
  printf("currentNote->currentNsdfPeriod = %f, ", currentNote->currentNsdfPeriod);
  printf("currentNote->firstNsdfPeriod = %f, ", currentNote->firstNsdfPeriod);
  printf("periodRatio = %f\n", periodRatio);
#endif // DEBUG_PRINTF

}

//------------------------------------------------------------------------------
float Channel::calcDetailedPitch(float *input, double period, int /*chunk*/)
{
#ifdef DEBUG_PRINTF
  printf("begin calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
  const int pitchSamplesRange = 4; //look 4 samples either side of the period. Total of 9 different subwindows.

  int n = size();
  int i, j, j2;
  int subwindow_size;
  if(period < pitchSamplesRange || period > double(n) * (3.0 / 8.0) - pitchSamplesRange)
    {
      std::fill(detailedPitchData.begin(), detailedPitchData.end(), 0.0f); //invalid period
      std::fill(detailedPitchDataSmoothed.begin(), detailedPitchDataSmoothed.end(), 0.0f); //invalid period
#ifdef DEBUG_PRINTF
      printf("end calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
      return 0.0f;
    }
  int iPeriod = int(floor(period));
  subwindow_size = n / 4;
  int num = n / 2;
  if(iPeriod > subwindow_size)
    {
      subwindow_size = n / 4 - (iPeriod - n / 4);
    }

#ifdef DEBUG_PRINTF
  printf("n=%d, subwindow_size=%d, iPeriod=%d\n", n, subwindow_size, iPeriod);
#endif // DEBUG_PRINTF

  std::vector<int> periods;
  for(j = -pitchSamplesRange; j <= pitchSamplesRange; j++)
    {
      periods.push_back(iPeriod + j);
    }
  int ln = periods.size();

  std::vector<float> squareTable(n);
  for(j = 0; j < n; j++)
    {
      squareTable[j] = sq(input[j]);
    }

  std::vector<float> left(ln);
  std::vector<float> right(ln);
  std::vector<float> left_pow(ln);
  std::vector<float> right_pow(ln);
  std::vector<float> pow(ln);
  std::vector<float> err(ln);
  std::vector<float> result(ln);
  Array1d<float> unsmoothed(num);

  //calc the values of pow and err for the first in each row.
  for(i = 0; i < ln; i++)
    {
      left_pow[i] = right_pow[i] = pow[i] = err[i] = 0;
      int offset = periods[i] - iPeriod;
      for(j = 0, j2 = periods[i]; j < subwindow_size - offset; j++, j2++)
	{
	  left_pow[i] += squareTable[j]; //sq(input[j]);
	  right_pow[i] += squareTable[j2]; //sq(input[j2]);
	  err[i] += sq(input[j] - input[j2]);
	}
    }
#ifdef DEBUG_PRINTF
  printf("subwindow_size=%d, num=%d, period=%lf\n", subwindow_size, num, period);
#endif // DEBUG_PRINTF
  //TODO: speed up this for loop
  int left1 = 0;
  int left2;
  int right1;
  int right2 = left1 + subwindow_size + iPeriod;

  for(; left1 < num; left1++, right2++)
    {
      for(i = 0; i < ln; i++)
	{
	  right1 = left1 + periods[i];
	  left2 = right2 - periods[i];

	  pow[i] = left_pow[i] + right_pow[i];
	  result[i] = 1.0 - (err[i] / pow[i]);

	  err[i] += sq(input[left2] - input[right2]) - sq(input[left1] - input[right1]);
	  left_pow[i]  += squareTable[left2]  - squareTable[left1];
	  right_pow[i] += squareTable[right2] - squareTable[right1];
	}

      int pos = int(std::max_element(result.begin(), result.begin()+ln) - result.begin());
      myassert(pos >= 0 && pos < ln);
      if(pos > 0 && pos < ln - 1)
	{
	  unsmoothed[left1] = double(periods[pos]) + parabolaTurningPoint(result[pos - 1], result[pos], result[pos + 1]);
	}
      else
	{
	  unsmoothed[left1] = double(periods[pos]);
	}
    }

  float periodDiff = unsmoothed.back() - unsmoothed.front();

  for(j = 0; j < num; j++)
    {
      unsmoothed[j] = freq2pitch(rate() / unsmoothed[j]);
    }

  pitchBigSmoothingFilter->filter(unsmoothed.begin(), detailedPitchDataSmoothed.begin(), num);
#ifdef DEBUG_PRINTF
  printf("big filter size = %d\n", pitchBigSmoothingFilter->delay());
#endif // DEBUG_PRINTF
  for(j = 0; j < num; j++)
    {
      detailedPitchDataSmoothed[j] = bound(detailedPitchDataSmoothed[j], 0.0f, (float)gdata->topPitch());
    }

  pitchSmallSmoothingFilter->filter(unsmoothed.begin(), detailedPitchData.begin(), num);
#ifdef DEBUG_PRINTF
  printf("small filter size = %d\n", pitchSmallSmoothingFilter->delay());
#endif // DEBUG_PRINTF
  for(j = 0; j < num; j++)
    {
      detailedPitchData[j] = bound(detailedPitchData[j], 0.0f, (float)gdata->topPitch());
    }
#ifdef DEBUG_PRINTF
  printf("end calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
  return periodDiff;
}

//------------------------------------------------------------------------------
void Channel::calcVibratoData(int chunk)
{
  NoteData *currentNote = getLastNote();
  if (currentNote && (dataAtChunk(chunk)->getNoteIndex() >= 0))
    {
      currentNote->addVibratoData(chunk);
    }
}

//------------------------------------------------------------------------------
float Channel::periodOctaveEstimate(int chunk) const
{
  const AnalysisData *analysisData = dataAtChunk(chunk);
  if(analysisData && analysisData->getNoteIndex() >= 0)
    {
      return noteData[analysisData->getNoteIndex()].periodOctaveEstimate() * analysisData->getPeriodRatio();
    }
  else
    {
      return -1.0f;
    }
}

//------------------------------------------------------------------------------
void Channel::exportChannel(int type, QString typeString) const
{
  QString s = QFileDialog::getSaveFileName(mainWindow, "Choose a filename to save under", ".", typeString);
#ifdef DEBUG_PRINTF
  printf("file = %s\n", s.latin1());
#endif // DEBUG_PRINTF
  if(s.isEmpty())
    {
      return;
    }

  QFile f(s);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  if(type == 0)
    { //plain text
      out << "        Time(secs) Pitch(semi-tones)       Volume(rms)" << endl;
      out << qSetFieldWidth(18);
      for(int j = 0; j < totalChunks(); j++)
	{
	  out << timeAtChunk(j) <<  dataAtChunk(j)->getPitch() << dataAtChunk(j)->getLogRms() << endl;
	}
    }
  else if(type == 1)
    {
      //matlab file
      out << "t = [";
      for(int j = 0; j < totalChunks(); j++)
	{
	  if(j > 0)
	    {
	      out << ", ";
	    }
	  out << timeAtChunk(j);
	}
      out << "];" << endl;

      out << "pitch = [";
      for(int j = 0; j < totalChunks(); j++)
	{
	  if(j > 0)
	    {
	      out << ", ";
	    }
	  out << dataAtChunk(j)->getPitch();
	}
      out << "];" << endl;

      out << "volume = [";
      for(int j = 0; j < totalChunks(); j++)
	{
	  if(j > 0)
	    {
	      out << ", ";
	    }
	  out << dataAtChunk(j)->getLogRms();
	}
      out << "];" << endl;
    }
  f.close();
}

//------------------------------------------------------------------------------
void Channel::doPronyFit(int chunk)
{
  if(chunk < pronyWindowSize)
    {
      return;
    }

  int start = chunk - pronyWindowSize;
  int center = chunk - pronyDelay();
  AnalysisData *data = dataAtChunk(center);
  for(int j = 0; j < pronyWindowSize; j++)
    {
      pronyData[j] = dataAtChunk(start + j)->getPitch();
    }
  PronyData p;
  if(pronyFit(&p, pronyData.begin(), pronyWindowSize, 2, true))
    {
      data->setVibratoError(p.error);
      data->setVibratoPitch(p.yOffset);
      if(p.error < 1.0)
	{
	  data->setVibratoSpeed(p.freqHz(timePerChunk()));
	  if(p.omega * pronyWindowSize < TwoPi)
	    {
	      data->setVibratoPitch(data->getPitch());
	    }
	  else
	    {
	      data->setVibratoWidth(p.amp);
	      data->setVibratoPhase(p.phase);
	      data->setVibratoWidthAdjust(0.0f);
	    }
	}
      else
	{
	  data->setVibratoPitch(data->getPitch());
	}
    }
  else
    {
      data->setVibratoPitch(data->getPitch());
    }
}

//EOF
