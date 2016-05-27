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
int SoundFile::numChannels(void)
{
  return channels.size();
}

//------------------------------------------------------------------------------
double SoundFile::startTime(void)
{
  return _startTime;
}

//------------------------------------------------------------------------------
void SoundFile::setStartTime(double t)
{
  _startTime = t;
}

//------------------------------------------------------------------------------
int SoundFile::currentStreamChunk(void)
{
  return (stream->pos() - offset()) / framesPerChunk();
}

//------------------------------------------------------------------------------
int SoundFile::currentRawChunk(void)
{
  return _chunkNum;
}

//------------------------------------------------------------------------------
int SoundFile::currentChunk(void)
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
int SoundFile::offset(void)
{
  return _offset;
}

//------------------------------------------------------------------------------
double SoundFile::timePerChunk(void)
{
  return double(framesPerChunk()) / double(rate());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtTime(double t)
{
  return toInt(chunkFractionAtTime(t));
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::chunkFractionAtTime(double t)
{
  return t / timePerChunk();
} //this is not bounded

//------------------------------------------------------------------------------
double SoundFile::timeAtChunk(int chunk)
{
  return double(chunk) * timePerChunk();
}

//------------------------------------------------------------------------------
double SoundFile::timeAtCurrentChunk(void)
{
  return timeAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
int SoundFile::chunkAtCurrentTime(void)
{
  return chunkAtTime(gdata->getView().currentTime());
}

//------------------------------------------------------------------------------
void SoundFile::jumpToTime(double t)
{
  jumpToChunk(chunkAtTime(t));
}

//------------------------------------------------------------------------------
int SoundFile::rate(void)
{
  myassert(stream != NULL);
  return stream->freq;
}

//------------------------------------------------------------------------------
int SoundFile::bits(void)
{
  myassert(stream != NULL);
  return stream->bits;
}

//------------------------------------------------------------------------------
int SoundFile::framesPerChunk(void)
{
  return _framesPerChunk;
}

//------------------------------------------------------------------------------
void SoundFile::setFramesPerChunk(int stepSize)
{
  _framesPerChunk = stepSize;
}

//------------------------------------------------------------------------------
bool SoundFile::saved(void)
{
  return _saved;
}

//------------------------------------------------------------------------------
void SoundFile::setSaved(bool newState)
{
  _saved = newState;
}

//------------------------------------------------------------------------------
bool SoundFile::equalLoudness(void)
{
  return myTransforms.equalLoudness;
}

//------------------------------------------------------------------------------
bool SoundFile::doingDetailedPitch(void)
{
  return _doingDetailedPitch;
}
//EOF
