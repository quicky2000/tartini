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
void Channel::lock(void)
{
  mutex->lock();
  isLocked = true;
}

//------------------------------------------------------------------------------
void Channel::unlock(void)
{
  isLocked = false;
  mutex->unlock();
}

//------------------------------------------------------------------------------
bool Channel::locked(void) const
{
  return isLocked; //For same thread testing of asserts only
}

//------------------------------------------------------------------------------
float * Channel::begin(void)
{
  return directInput.begin();
}

//------------------------------------------------------------------------------
float * Channel::end(void)
{
 return directInput.end();
}

//------------------------------------------------------------------------------
int Channel::size(void) const
{
  return directInput.size();
}

//------------------------------------------------------------------------------
const float & Channel::at(int pos) const
{
 return directInput.at(pos);
}

//------------------------------------------------------------------------------
int Channel::rate(void) const
{
 return parent->rate();
}

//------------------------------------------------------------------------------
int Channel::framesPerChunk(void) const
{
  return parent->framesPerChunk();
}

//------------------------------------------------------------------------------
void Channel::setParent(SoundFile *parent_)
{
  parent = parent_;
}

//------------------------------------------------------------------------------
SoundFile* Channel::getParent(void) const
{
  return parent;
}

//------------------------------------------------------------------------------
void Channel::setPitchMethod(int pitch_method)
{
  _pitch_method = pitch_method;
}

//------------------------------------------------------------------------------
int Channel::pitchMethod(void) const
{
  return _pitch_method;
}

//------------------------------------------------------------------------------
bool Channel::isVisible(void) const
{
  return visible;
}

//------------------------------------------------------------------------------
void Channel::setVisible(bool state)
{
  visible = state;
}

//------------------------------------------------------------------------------
double Channel::timePerChunk(void) const
{
  return parent->timePerChunk();
}


//------------------------------------------------------------------------------
double Channel::startTime(void) const
{
  return parent->startTime();
}


//------------------------------------------------------------------------------
void Channel::setStartTime(double newStartTime)
{
  parent->setStartTime(newStartTime);
}


//------------------------------------------------------------------------------
int Channel::totalChunks(void) const
{
  return lookup.size();
}


//------------------------------------------------------------------------------
double Channel::finishTime(void) const
{
  return startTime() + totalTime();
}


//------------------------------------------------------------------------------
double Channel::totalTime(void) const
{
  return double(MAX(totalChunks()-1, 0)) * timePerChunk();
}


//------------------------------------------------------------------------------
void Channel::jumpToTime(double t)
{
  parent->jumpToTime(t);
}


//------------------------------------------------------------------------------
int Channel::chunkAtTime(double t) const
{
  return parent->chunkAtTime(t);
}


//------------------------------------------------------------------------------
double Channel::chunkFractionAtTime(double t) const
{
  return parent->chunkFractionAtTime(t);
}


//------------------------------------------------------------------------------
int Channel::chunkAtCurrentTime(void) const
{
  return parent->chunkAtCurrentTime();
}


//------------------------------------------------------------------------------
int Channel::currentChunk(void) const
{
  return parent->currentChunk();
} //this one should be use to retrieve current info


//------------------------------------------------------------------------------
double Channel::timeAtChunk(int chunk) const
{
  return parent->timeAtChunk(chunk);
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtChunk(int chunk) const
{
  return (isValidChunk(chunk)) ? &lookup[chunk] : NULL;
}

//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtChunk(int chunk)
{
  return (isValidChunk(chunk)) ? &lookup[chunk] : NULL;
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtCurrentChunk(void) const
{
  return dataAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtCurrentChunk(void)
{
  return dataAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtTime(double t) const
{
  return dataAtChunk(chunkAtTime(t));
}


//------------------------------------------------------------------------------
large_vector<AnalysisData>::iterator Channel::dataIteratorAtChunk(int chunk)
{
  return lookup.iterator_at(chunk);
}


//------------------------------------------------------------------------------
bool Channel::hasAnalysisData(void) const
{
  return !lookup.empty();
}


//------------------------------------------------------------------------------
bool Channel::isValidChunk(int chunk) const
{
  return (chunk >= 0 && chunk < totalChunks());
}


//------------------------------------------------------------------------------
bool Channel::isValidTime(double t) const
{
  return isValidChunk(chunkAtTime(t));
}


//------------------------------------------------------------------------------
bool Channel::isValidCurrentTime(void) const
{
  return isValidChunk(chunkAtCurrentTime());
}



//------------------------------------------------------------------------------
float Channel::threshold(void) const
{
  return _threshold;
}


//------------------------------------------------------------------------------
void Channel::setIntThreshold(int thresholdPercentage)
{
  _threshold = float(thresholdPercentage) / 100.0f;
}


//------------------------------------------------------------------------------
void Channel::setColor(QColor c)
{
  color = c;
}


//------------------------------------------------------------------------------
bool Channel::isNotePlaying(void) const
{
  return noteIsPlaying;
}


//------------------------------------------------------------------------------
bool Channel::isVisibleChunk(int chunk_) const
{
  return isVisibleChunk(dataAtChunk(chunk_));
}


//------------------------------------------------------------------------------
int Channel::getCurrentNoteIndex(void) const
{
  return int(noteData.size()) - 1;
}

//------------------------------------------------------------------------------
bool Channel::firstTimeThrough(void) const
{
  return parent->isFirstTimeThrough();
}

//------------------------------------------------------------------------------
bool Channel::doingDetailedPitch(void) const
{
  return parent->doingDetailedPitch();
}

//------------------------------------------------------------------------------
int Channel::pronyDelay(void) const
{
  return pronyWindowSize / 2;
}

//------------------------------------------------------------------------------
const large_vector<float> & Channel::get_pitch_lookup(void) const
{
  return pitchLookup;
}

//------------------------------------------------------------------------------
large_vector<float> & Channel::get_pitch_lookup(void)
{
  return pitchLookup;
}

//------------------------------------------------------------------------------
const large_vector<float> & Channel::get_pitch_lookup_smoothed(void) const
{
  return pitchLookupSmoothed;
}

//------------------------------------------------------------------------------
large_vector<float> & Channel::get_pitch_lookup_smoothed(void)
{
  return pitchLookupSmoothed;
}

//------------------------------------------------------------------------------
const QColor & Channel::get_color(void)const
{
  return color;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_direct_input(void) const
{
  return directInput;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_filtered_input(void) const
{
  return filteredInput;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_nsdf_data(void) const
{
  return nsdfData;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_fft_data1(void) const
{
  return fftData1;
}

//------------------------------------------------------------------------------
Array1d<float> & Channel::get_fft_data1(void)
{
  return fftData1;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_fft_data2(void) const
{
  return fftData2;
}

//------------------------------------------------------------------------------
Array1d<float> & Channel::get_fft_data2(void)
{
  return fftData2;
}

//------------------------------------------------------------------------------
ChannelLocker::ChannelLocker(Channel *channel_)
{
  myassert(channel_);
  channel = channel_;
  channel->lock();
}

//------------------------------------------------------------------------------
ChannelLocker::~ChannelLocker()
{
  channel->unlock();
}
//EOF
