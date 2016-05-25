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
double freq2pitch(double freq)
{
#ifdef log2
  //From log rules  log(x/y) = log(x) - log(y)
  //return 69 + 12*(log2(freq) - log2(440));
  return -36.3763165622959152488 + 12.0*log2(freq);
#else
  //From log rules  log_b(x) = log_a(x) / log_a(b)
  //return 69 + 39.8631371386483481 * log10(freq / 440);
  return -36.3763165622959152488 + 39.8631371386483481 * log10(freq);
#endif
}

//------------------------------------------------------------------------------
double pitch2freq(double note)
{
  double result = pow10((note + 36.3763165622959152488) / 39.8631371386483481);
  return result;
}

//------------------------------------------------------------------------------
const char* noteName(double pitch)
{
  return noteName(toInt(pitch));
}

//------------------------------------------------------------------------------
int noteOctave(double pitch)
{
  return noteOctave(toInt(pitch));
}

//------------------------------------------------------------------------------
int noteValue(double pitch)
{
  return noteValue(toInt(pitch));
}

//------------------------------------------------------------------------------
bool isBlackNote(double pitch)
{
  return isBlackNote(toInt(pitch));
}

//------------------------------------------------------------------------------
MusicScale::MusicScale(void):
  pName(NULL),
  _semitoneOffset(0)
{
}

//------------------------------------------------------------------------------
int MusicScale::size(void)
{
  return pNotes.size();
}

//------------------------------------------------------------------------------
int MusicScale::note(int j)
{
  return pNotes[j];
}

//------------------------------------------------------------------------------
bool MusicScale::hasSemitone(int j)
{
  return pSemitoneLookup[j];
}

//------------------------------------------------------------------------------
const char * MusicScale::name(void)
{
  return pName;
}

//------------------------------------------------------------------------------
int MusicScale::semitoneOffset(void)
{
  return _semitoneOffset;
}

//------------------------------------------------------------------------------
MusicKey::MusicKey(void):
  pName(NULL)
{
}

//------------------------------------------------------------------------------
const char * MusicKey::name(void)
{
  return pName;
}

//------------------------------------------------------------------------------
int MusicKey::size(void) const
{
  return noteOffsets.size();
}

//------------------------------------------------------------------------------
double MusicKey::noteOffset(int j) const
{
  return noteOffsets[j];
}

//------------------------------------------------------------------------------
int MusicKey::noteType(int j) const
{
  return noteTypes[j];
}
//EOF
