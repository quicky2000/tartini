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
SoundFileStream::SoundFileStream(void)
{
  _total_frames=-1;
  _pos=0;
}

//------------------------------------------------------------------------------
SoundFileStream::~SoundFileStream(void)
{
}

//------------------------------------------------------------------------------
int SoundFileStream::data_length(void)
{
  return totalFrames() * frame_size();
}

//------------------------------------------------------------------------------
int SoundFileStream::totalFrames(void)
{
  return _total_frames;
}

//------------------------------------------------------------------------------
int SoundFileStream::pos(void)
{
  return _pos;
}

//------------------------------------------------------------------------------
void SoundFileStream::setPos(int newPos)
{
  _pos = newPos;
}

//------------------------------------------------------------------------------
int SoundFileStream::open_read(const char * /*filename*/)
{
  return 0;
};

//------------------------------------------------------------------------------
long SoundFileStream::read_bytes(void * /*data*/, long /*length*/)
{
  return 0;
};

//------------------------------------------------------------------------------
long SoundFileStream::read_frames(void * /*data*/, long /*length*/)
{
  return 0;
};


//------------------------------------------------------------------------------
int SoundFileStream::open_write(const char * /*filename*/, int /*freq_*/, int /*channels_*/, int /*bits_*/)
{
  return 0;
};

//------------------------------------------------------------------------------
long SoundFileStream::write_bytes(void * /*data*/, long /*length*/)
{
  return 0;
};

//------------------------------------------------------------------------------
long SoundFileStream::write_frames(void * /*data*/, long /*length*/)
{
  return 0;
};


//------------------------------------------------------------------------------
void SoundFileStream::close(void)
{
};



//------------------------------------------------------------------------------
void SoundFileStream::jump_to_frame(int /*frame*/)
{
}


//------------------------------------------------------------------------------
void SoundFileStream::jump_back(int /*frames*/)
{
}


//------------------------------------------------------------------------------
void SoundFileStream::jump_forward(int /*frames*/)
{
}


//EOF
