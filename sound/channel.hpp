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
void Channel::lock()
{
    m_mutex->lock();
    m_is_locked = true;
}

//------------------------------------------------------------------------------
void Channel::unlock()
{
    m_is_locked = false;
    m_mutex->unlock();
}

//------------------------------------------------------------------------------
bool Channel::locked() const
{
    return m_is_locked; //For same thread testing of asserts only
}

//------------------------------------------------------------------------------
float * Channel::begin()
{
    return m_direct_input.begin();
}

//------------------------------------------------------------------------------
float * Channel::end()
{
    return m_direct_input.end();
}

//------------------------------------------------------------------------------
int Channel::size() const
{
    return m_direct_input.size();
}

//------------------------------------------------------------------------------
const float & Channel::at(int p_pos) const
{
    return m_direct_input.at(p_pos);
}

//------------------------------------------------------------------------------
int Channel::rate() const
{
    return m_parent->rate();
}

//------------------------------------------------------------------------------
int Channel::framesPerChunk() const
{
    return m_parent->framesPerChunk();
}

//------------------------------------------------------------------------------
void Channel::setParent(SoundFile * p_parent)
{
    m_parent = p_parent;
}

//------------------------------------------------------------------------------
SoundFile * Channel::getParent() const
{
    return m_parent;
}

//------------------------------------------------------------------------------
void Channel::setPitchMethod(int p_pitch_method)
{
    m_pitch_method = p_pitch_method;
}

//------------------------------------------------------------------------------
int Channel::pitchMethod() const
{
    return m_pitch_method;
}

//------------------------------------------------------------------------------
bool Channel::isVisible() const
{
    return m_visible;
}

//------------------------------------------------------------------------------
void Channel::setVisible(bool p_state)
{
    m_visible = p_state;
}

//------------------------------------------------------------------------------
double Channel::timePerChunk() const
{
    return m_parent->timePerChunk();
}


//------------------------------------------------------------------------------
double Channel::startTime() const
{
    return m_parent->startTime();
}


//------------------------------------------------------------------------------
void Channel::setStartTime(double p_new_start_time)
{
    m_parent->setStartTime(p_new_start_time);
}


//------------------------------------------------------------------------------
int Channel::totalChunks() const
{
    return m_lookup.size();
}


//------------------------------------------------------------------------------
double Channel::finishTime() const
{
    return startTime() + totalTime();
}


//------------------------------------------------------------------------------
double Channel::totalTime() const
{
    return double(MAX(totalChunks()-1, 0)) * timePerChunk();
}


//------------------------------------------------------------------------------
void Channel::jumpToTime(double p_time)
{
    m_parent->jumpToTime(p_time);
}


//------------------------------------------------------------------------------
int Channel::chunkAtTime(double p_time) const
{
    return m_parent->chunkAtTime(p_time);
}


//------------------------------------------------------------------------------
double Channel::chunkFractionAtTime(double p_time) const
{
    return m_parent->chunkFractionAtTime(p_time);
}


//------------------------------------------------------------------------------
int Channel::chunkAtCurrentTime() const
{
    return m_parent->chunkAtCurrentTime();
}


//------------------------------------------------------------------------------
int Channel::currentChunk() const
{
    return m_parent->currentChunk();
} //this one should be use to retrieve current info


//------------------------------------------------------------------------------
double Channel::timeAtChunk(int p_chunk) const
{
    return m_parent->timeAtChunk(p_chunk);
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtChunk(int p_chunk) const
{
    return (isValidChunk(p_chunk)) ? &m_lookup[p_chunk] : nullptr;
}

//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtChunk(int p_chunk)
{
    return (isValidChunk(p_chunk)) ? &m_lookup[p_chunk] : nullptr;
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtCurrentChunk() const
{
    return dataAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
AnalysisData * Channel::dataAtCurrentChunk()
{
    return dataAtChunk(currentChunk());
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::dataAtTime(double p_time) const
{
    return dataAtChunk(chunkAtTime(p_time));
}

//------------------------------------------------------------------------------
large_vector<AnalysisData>::iterator Channel::dataIteratorAtChunk(int p_chunk)
{
    return m_lookup.iterator_at(p_chunk);
}


//------------------------------------------------------------------------------
bool Channel::hasAnalysisData() const
{
    return !m_lookup.empty();
}


//------------------------------------------------------------------------------
bool Channel::isValidChunk(int p_chunk) const
{
    return (p_chunk >= 0 && p_chunk < totalChunks());
}


//------------------------------------------------------------------------------
bool Channel::isValidTime(double p_time) const
{
    return isValidChunk(chunkAtTime(p_time));
}


//------------------------------------------------------------------------------
bool Channel::isValidCurrentTime() const
{
    return isValidChunk(chunkAtCurrentTime());
}

//------------------------------------------------------------------------------
float Channel::threshold() const
{
    return m_threshold;
}

//------------------------------------------------------------------------------
void Channel::setIntThreshold(int p_threshold_percentage)
{
    m_threshold = float(p_threshold_percentage) / 100.0f;
}


//------------------------------------------------------------------------------
void Channel::setColor(QColor p_color)
{
    m_color = p_color;
}


//------------------------------------------------------------------------------
bool Channel::isNotePlaying() const
{
    return m_note_is_playing;
}


//------------------------------------------------------------------------------
bool Channel::isVisibleChunk(int p_chunk) const
{
    return isVisibleChunk(dataAtChunk(p_chunk));
}

//------------------------------------------------------------------------------
int Channel::getCurrentNoteIndex() const
{
    return int(m_note_data.size()) - 1;
}

//------------------------------------------------------------------------------
bool Channel::firstTimeThrough() const
{
    return m_parent->isFirstTimeThrough();
}

//------------------------------------------------------------------------------
bool Channel::doingDetailedPitch() const
{
    return m_parent->doingDetailedPitch();
}

//------------------------------------------------------------------------------
int Channel::pronyDelay() const
{
    return m_prony_window_size / 2;
}

//------------------------------------------------------------------------------
const large_vector<float> & Channel::get_pitch_lookup() const
{
    return m_pitch_lookup;
}

//------------------------------------------------------------------------------
large_vector<float> & Channel::get_pitch_lookup()
{
    return m_pitch_lookup;
}

//------------------------------------------------------------------------------
const large_vector<float> & Channel::get_pitch_lookup_smoothed() const
{
    return m_pitch_lookup_smoothed;
}

//------------------------------------------------------------------------------
large_vector<float> & Channel::get_pitch_lookup_smoothed()
{
    return m_pitch_lookup_smoothed;
}

//------------------------------------------------------------------------------
const QColor & Channel::get_color()const
{
    return m_color;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_direct_input() const
{
    return m_direct_input;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_filtered_input() const
{
    return m_filtered_input;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_nsdf_data() const
{
    return m_nsdf_data;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_fft_data1() const
{
    return m_fft_data1;
}

//------------------------------------------------------------------------------
Array1d<float> & Channel::get_fft_data1()
{
    return m_fft_data1;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_fft_data2() const
{
    return m_fft_data2;
}

//------------------------------------------------------------------------------
Array1d<float> & Channel::get_fft_data2()
{
    return m_fft_data2;
}

//------------------------------------------------------------------------------
Array1d<float> & Channel::get_cepstrum_data()
{
    return m_cepstrum_data;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_detailed_pitch_data() const
{
    return m_detailed_pitch_data;
}

//------------------------------------------------------------------------------
const Array1d<float> & Channel::get_detailed_pitch_data_smoothed() const
{
    return m_detailed_pitch_data_smoothed;
}

//------------------------------------------------------------------------------
const large_vector<NoteData> & Channel::get_note_data() const
{
    return m_note_data;
}

//------------------------------------------------------------------------------
void Channel::apply_highpass_filter( const float * p_input
                                   , float * p_output
                                   , int p_n
                                   )
{
    m_high_pass_filter->filter(p_input, p_output, p_n);
}

//------------------------------------------------------------------------------
const Filter & Channel::get_pitch_small_smoothing_filter()const
{
    return *m_pitch_small_smoothing_filter;
}

//------------------------------------------------------------------------------
const Filter & Channel::get_pitch_big_smoothing_filter()const
{
    return *m_pitch_big_smoothing_filter;
}

//------------------------------------------------------------------------------
const double & Channel::get_rms_floor()const
{
    return m_rms_floor;
}

//------------------------------------------------------------------------------
void Channel::set_rms_floor(const double & p_rms_floor)
{
    m_rms_floor = p_rms_floor;
}

//------------------------------------------------------------------------------
const double & Channel::get_rms_ceiling()const
{
    return m_rms_ceiling;
}

//------------------------------------------------------------------------------
void Channel::set_rms_ceiling(const double & p_rms_ceiling)
{
    m_rms_ceiling = p_rms_ceiling;
}

//------------------------------------------------------------------------------
ZoomLookup & Channel::get_summary_zoom_lookup()
{
    return m_summary_zoom_lookup;
}

//------------------------------------------------------------------------------
ZoomLookup & Channel::get_normal_zoom_lookup()
{
    return m_normal_zoom_lookup;
}

//------------------------------------------------------------------------------
ZoomLookup & Channel::get_amplitude_zoom_lookup()
{
    return m_amplitude_zoom_lookup;
}

//------------------------------------------------------------------------------
ChannelLocker::ChannelLocker(Channel * p_channel)
{
    myassert(p_channel);
    m_channel = p_channel;
    m_channel->lock();
}

//------------------------------------------------------------------------------
ChannelLocker::~ChannelLocker()
{
    m_channel->unlock();
}
//EOF
