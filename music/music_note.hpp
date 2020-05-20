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
double MusicNote::freq2pitch(const double & p_freq)
{
#ifdef log2
    //From log rules  log(x/y) = log(x) - log(y)
    //return 69 + 12*(log2(p_freq) - log2(440));
    return -36.3763165622959152488 + 12.0*log2(p_freq);
#else
    //From log rules  log_b(x) = log_a(x) / log_a(b)
    //return 69 + 39.8631371386483481 * log10(p_freq / 440);
    return -36.3763165622959152488 + 39.8631371386483481 * log10(p_freq);
#endif
}

//------------------------------------------------------------------------------
double MusicNote::pitch2freq(const double & p_pitch)
{
    double l_result = pow10((p_pitch + 36.3763165622959152488) / 39.8631371386483481);
    return l_result;
}

//------------------------------------------------------------------------------
const std::string & MusicNote::noteName(const double & p_pitch)
{
    // This method is deprecated, but don't remove it.
    // Otherwise, a call to `noteName(double)` would use implicit conversions to call `noteName(int)`, which would truncate the pitch value.
    return noteName(closestNote(p_pitch));
}

//------------------------------------------------------------------------------
int MusicNote::noteOctave(const double & p_pitch)
{
    // This function is deprecated, but don't remove it.
    // Otherwise, a call to `noteOctave(double)` would use implicit conversions to call `noteOctave(int)`, which would truncate the pitch value.
    return noteOctave(closestNote(p_pitch));
}

//------------------------------------------------------------------------------
int MusicNote::noteValue(const double & p_pitch)
{
    // This function is deprecated, but don't remove it.
    // Otherwise, a call to `noteValue(double)` would use implicit conversions to call `noteValue(int)`, which would truncate the pitch value.
    return noteValue(closestNote(p_pitch));
}

//------------------------------------------------------------------------------
bool MusicNote::isBlackNote(const double & p_pitch)
{
    // This function is deprecated, but don't remove it.
    // Otherwise, a call to `isBlackNote(double)` would use implicit conversions to call `isBlackNote(int)`, which would truncate the pitch value.
    return isBlackNote(closestNote(p_pitch));
}

//EOF
