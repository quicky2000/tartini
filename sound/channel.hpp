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
int Channel::size(void)
{
  return directInput.size();
}

//------------------------------------------------------------------------------
float & Channel::at(int pos)
{
 return directInput.at(pos);
}

//------------------------------------------------------------------------------
int Channel::rate(void)
{
 return parent->rate();
}

//------------------------------------------------------------------------------
int Channel::framesPerChunk(void)
{
  return parent->framesPerChunk();
}

//------------------------------------------------------------------------------
void Channel::setParent(SoundFile *parent_)
{
  parent = parent_;
}

//------------------------------------------------------------------------------
SoundFile* Channel::getParent(void)
{
  return parent;
}

//------------------------------------------------------------------------------
void Channel::setPitchMethod(int pitch_method)
{
  _pitch_method = pitch_method;
}

//------------------------------------------------------------------------------
int Channel::pitchMethod(void)
{
  return _pitch_method;
}

//------------------------------------------------------------------------------
bool Channel::isVisible(void)
{
  return visible;
}

//------------------------------------------------------------------------------
void Channel::setVisible(bool state)
{
  visible = state;
}

//------------------------------------------------------------------------------
double Channel::timePerChunk(void)
{
  return parent->timePerChunk();
}


//------------------------------------------------------------------------------
double Channel::startTime(void)
{
  return parent->startTime();
}


//------------------------------------------------------------------------------
void Channel::setStartTime(double newStartTime)
{
  parent->setStartTime(newStartTime);
}


//------------------------------------------------------------------------------
int Channel::totalChunks(void)
{
  return lookup.size();
}


//------------------------------------------------------------------------------
double Channel::finishTime(void)
{
  return startTime() + totalTime();
}


//------------------------------------------------------------------------------
double Channel::totalTime(void)
{
  return double(MAX(totalChunks()-1, 0)) * timePerChunk();
}


//------------------------------------------------------------------------------
void Channel::jumpToTime(double t)
{
  parent->jumpToTime(t);
}


//------------------------------------------------------------------------------
int Channel::chunkAtTime(double t)
{
  return parent->chunkAtTime(t);
}


//------------------------------------------------------------------------------
double Channel::chunkFractionAtTime(double t)
{
  return parent->chunkFractionAtTime(t);
}


//------------------------------------------------------------------------------
int Channel::chunkAtCurrentTime(void)
{
  return parent->chunkAtCurrentTime();
}


//------------------------------------------------------------------------------
int Channel::currentChunk(void)
{
  return parent->currentChunk();
} //this one should be use to retrieve current info


//------------------------------------------------------------------------------
double Channel::timeAtChunk(int chunk)
{
  return parent->timeAtChunk(chunk);
}




//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtChunk(int chunk)
{
  return (isValidChunk(chunk)) ? &lookup[chunk] : NULL;
}


//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtCurrentChunk(void)
{
  return dataAtChunk(currentChunk());
}


//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtTime(double t)
{
  return dataAtChunk(chunkAtTime(t));
}


//------------------------------------------------------------------------------
large_vector<AnalysisData>::iterator Channel::dataIteratorAtChunk(int chunk)
{
  return lookup.iterator_at(chunk);
}


//------------------------------------------------------------------------------
bool Channel::hasAnalysisData(void)
{
  return !lookup.empty();
}


//------------------------------------------------------------------------------
bool Channel::isValidChunk(int chunk)
{
  return (chunk >= 0 && chunk < totalChunks());
}


//------------------------------------------------------------------------------
bool Channel::isValidTime(double t)
{
  return isValidChunk(chunkAtTime(t));
}


//------------------------------------------------------------------------------
bool Channel::isValidCurrentTime(void)
{
  return isValidChunk(chunkAtCurrentTime());
}



//------------------------------------------------------------------------------
float Channel::threshold(void)
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
bool Channel::isNotePlaying(void)
{
  return noteIsPlaying;
}


//------------------------------------------------------------------------------
bool Channel::isVisibleChunk(int chunk_)
{
  return isVisibleChunk(dataAtChunk(chunk_));
}


//------------------------------------------------------------------------------
int Channel::getCurrentNoteIndex(void)
{
  return int(noteData.size())-1;
}

//------------------------------------------------------------------------------
bool Channel::firstTimeThrough(void)
{
  return parent->firstTimeThrough;
}

//------------------------------------------------------------------------------
bool Channel::doingDetailedPitch(void)
{
  return parent->doingDetailedPitch();
}

//------------------------------------------------------------------------------
int Channel::pronyDelay(void)
{
  return pronyWindowSize / 2;
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
