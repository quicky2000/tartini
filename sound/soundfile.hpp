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
int SoundFile::numChannels(void) const
{
  return channels.size();
}

//------------------------------------------------------------------------------
const double & SoundFile::startTime(void) const
{
  return _startTime;
}

//------------------------------------------------------------------------------
void SoundFile::setStartTime(const double & t)
{
  _startTime = t;
}

//------------------------------------------------------------------------------
int SoundFile::currentStreamChunk(void) const
{
  return (stream->pos() - offset()) / framesPerChunk();
}

//------------------------------------------------------------------------------
int SoundFile::currentRawChunk(void) const
{
  return _chunkNum;
}

//------------------------------------------------------------------------------
int SoundFile::currentChunk(void) const
{
  return _chunkNum;
}

//------------------------------------------------------------------------------
void SoundFile::setCurrentChunk(int x)
{
  _chunkNum = x;
}

//------------------------------------------------------------------------------
void SoundFile::incrementChunkNum(void)
{
 _chunkNum++;
}

//------------------------------------------------------------------------------
int SoundFile::offset(void) const
{
  return _offset;
}

//------------------------------------------------------------------------------
double SoundFile::timePerChunk(void) const
{
  return double(framesPerChunk()) / double(rate());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtTime(const double & t) const
{
  return toInt(chunkFractionAtTime(t));
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::chunkFractionAtTime(const double & t) const
{
  return t / timePerChunk();
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::timeAtChunk(int chunk) const
{
  return double(chunk) * timePerChunk();
}

//------------------------------------------------------------------------------
double SoundFile::timeAtCurrentChunk(void) const
{
  return timeAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtCurrentTime(void) const
{
  return chunkAtTime(gdata->getView().currentTime());
}

//------------------------------------------------------------------------------
void SoundFile::jumpToTime(const double & t)
{
  jumpToChunk(chunkAtTime(t));
}

//------------------------------------------------------------------------------
int SoundFile::rate(void) const
{
  myassert(stream != NULL);
  return stream->freq;
}

//------------------------------------------------------------------------------
int SoundFile::bits(void) const
{
  myassert(stream != NULL);
  return stream->bits;
}

//------------------------------------------------------------------------------
int SoundFile::framesPerChunk(void) const
{
  return _framesPerChunk;
}

//------------------------------------------------------------------------------
void SoundFile::setFramesPerChunk(int stepSize)
{
  _framesPerChunk = stepSize;
}

//------------------------------------------------------------------------------
bool SoundFile::saved(void) const
{
  return _saved;
}

//------------------------------------------------------------------------------
void SoundFile::setSaved(bool newState)
{
  _saved = newState;
}

//------------------------------------------------------------------------------
bool SoundFile::equalLoudness(void) const
{
  return myTransforms.equalLoudness;
}

//------------------------------------------------------------------------------
bool SoundFile::doingDetailedPitch(void) const
{
  return _doingDetailedPitch;
}

//------------------------------------------------------------------------------
bool SoundFile::isFirstTimeThrough(void) const
{
  return firstTimeThrough;
}

//------------------------------------------------------------------------------
const char * SoundFile::getFileName(void) const
{
  return filename;
}

//------------------------------------------------------------------------------
void SoundFile::setFirstTimeThrough(bool p_firstTimeThrough)
{
  firstTimeThrough = p_firstTimeThrough;
}

//------------------------------------------------------------------------------
void SoundFile::calculateAnalysisData(int chunk, Channel *ch)
{
  myTransforms.calculateAnalysisData(chunk,ch);
}

//------------------------------------------------------------------------------
int SoundFile::getChannelIndex(const Channel & p_channel)const
{
  for(int i = 0; i < channels.size(); ++i)
    {
      if(&p_channel == channels.at(i))
	{
	  return i;
	}
    }
  // If we're here, we didn't find the channel
  // This should never happen!
  myassert(false);
  return -1;
 }

//------------------------------------------------------------------------------
Channel & SoundFile::getChannel(int p_index)
{
  return *(channels.at(p_index));
}

//------------------------------------------------------------------------------
SoundFileStream & SoundFile::getStream(void)
{
  assert(stream);
  return *stream;
}

//EOF
