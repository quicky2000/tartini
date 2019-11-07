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

const double g_short_time = 0.08;
const float g_short_base = 0.1f;
const float g_short_stretch = 1.5;

const double g_long_time = 0.8;
const float g_long_base = 0.02f;
const float g_long_stretch = 0.2f;


/**
  This filter was created in Matlab using
  [Bb,Ab]=butter(2,(150/(sampleRate/2)),'high');
  num2str(Bb, '%1.52f\n')
  num2str(Ab, '%1.52f\n')
*/

double g_high_pass_filter_coeff_B[6][3] =
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

double g_high_pass_filter_coeff_A[6][3] =
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
double g_low_pass_filter_coeff_B[3][3] =
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

double g_low_pass_filter_coeff_A[3][3] =
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
Channel::Channel( SoundFile * p_parent
                , int p_size
                , int p_k
                )
: m_parent(p_parent)
, m_freq(0.0)
, m_pitch_method(2)
, m_visible(true)
, m_note_is_playing(false)
, m_lookup(0, 128)
, m_mutex(new QMutex(QMutex::Recursive))
, m_is_locked(false)
, m_prony_window_size(int(ceil(0.4 / timePerChunk())))
, m_fast_smooth(new fast_smooth(p_size / 8))
, m_color(Qt::black)
, m_nsdf_aggregate_roof(0.0)
, m_high_pass_filter(NULL)
, m_pitch_small_smoothing_filter(new GrowingAverageFilter(m_parent->rate() / 64))
, m_pitch_big_smoothing_filter(new FastSmoothedAveragingFilter(m_parent->rate() / 16))
, m_rms_floor(g_data->dBFloor())
, m_rms_ceiling(g_data->dBFloor())
{
    if(p_k == 0)
    {
        p_k = (p_size + 1) / 2;
    }
    m_direct_input.resize(p_size, 0.0);
    m_filtered_input.resize(p_size, 0.0);
    m_nsdf_data.resize(p_k, 0.0);
    m_nsdf_aggregate_data.resize(p_k, 0.0);
    m_nsdf_aggregate_data_scaled.resize(p_k, 0.0);
    m_fft_data1.resize(p_size / 2, 0.0);
    m_fft_data2.resize(p_size / 2, 0.0);
    m_cepstrum_data.resize(p_size / 2, 0.0);
    m_detailed_pitch_data.resize(p_size / 2, 0.0);
    m_detailed_pitch_data_smoothed.resize(p_size / 2, 0.0);
    m_coefficients_table.resize(p_size * 4);
    m_prony_data.resize(m_prony_window_size);

    setIntThreshold(g_data->getSettingsValue("Analysis/thresholdValue", 93));
    int l_filter_index = 2;
    int l_sample_rate = m_parent->rate();
    if(l_sample_rate > (48000 + 96000) / 2)
    {
        l_filter_index = 0; //96000 Hz
    }
    else if(l_sample_rate > (44100 + 48000) / 2)
    {
        l_filter_index = 1; //48000 Hz
    }
    else if(l_sample_rate > (22050 + 44100) / 2)
    {
        l_filter_index = 2; //44100 Hz
    }
    else if(l_sample_rate > (11025 + 22050) / 2)
    {
        l_filter_index = 3; //22050 Hz
    }
    else if(l_sample_rate > (8000 + 11025) / 2)
    {
        l_filter_index = 4; //11025 Hz
    }
    else
    {
        l_filter_index = 5; //8000 Hz
    }
    m_high_pass_filter = new IIR_Filter(g_high_pass_filter_coeff_B[l_filter_index], g_high_pass_filter_coeff_A[l_filter_index], 3);

}

//------------------------------------------------------------------------------
Channel::~Channel()
{
    delete m_fast_smooth;
    delete m_mutex;
    delete m_high_pass_filter;
    delete m_pitch_small_smoothing_filter;
    delete m_pitch_big_smoothing_filter;
}

//------------------------------------------------------------------------------
void Channel::resetIntThreshold(int p_threshold_percentage)
{
    m_threshold = float(p_threshold_percentage) / 100.0f;
    uint l_j;
    for(l_j = 0; l_j < m_lookup.size(); l_j++)
    {
        chooseCorrelationIndex(l_j, periodOctaveEstimate(l_j));
        calcDeviation(l_j);
    }
    clearFreqLookup();
}

//------------------------------------------------------------------------------
void Channel::resize( int p_new_size
                    , int p_k
                    )
{
    m_coefficients_table.resize(p_new_size * 4);
    if(p_k == 0)
    {
        p_k = (p_new_size + 1) / 2;
    }
    m_direct_input.resize(p_new_size, 0.0);
    m_filtered_input.resize(p_new_size, 0.0);
    m_nsdf_data.resize(p_k, 0.0);
    m_nsdf_aggregate_data.resize(p_k, 0.0);
    m_nsdf_aggregate_data_scaled.resize(p_k, 0.0);
    m_fft_data1.resize(p_new_size / 2, 0.0);
    m_fft_data2.resize(p_new_size / 2, 0.0);
    m_cepstrum_data.resize(p_new_size / 2, 0.0);
    m_detailed_pitch_data.resize(p_new_size / 2, 0.0);
    m_detailed_pitch_data_smoothed.resize(p_new_size / 2, 0.0);
    m_lookup.clear();
}

//------------------------------------------------------------------------------
void Channel::shift_left(int p_n)
{
    m_direct_input.shift_left(p_n);
    m_filtered_input.shift_left(p_n);
    m_coefficients_table.shift_left(p_n * 4);
}

//------------------------------------------------------------------------------
void Channel::calc_last_n_coefficients(int p_n)
{
    //build a table of coefficients for calculating interpolation
    int l_start_pos = MAX(size() - p_n, 3);
    float * l_buf_pos = begin() + l_start_pos;
    float * l_coeff_pos = m_coefficients_table.begin() + l_start_pos * 4;

    for(; l_buf_pos < end(); l_buf_pos++)
    {
        const float &l_xm1 = l_buf_pos[-3];
        const float &l_x0 = l_buf_pos[-2];
        const float &l_x1 = l_buf_pos[-1];
        const float &l_x2 = l_buf_pos[0];
        *l_coeff_pos++ = (3 * (l_x0 - l_x1) - l_xm1 + l_x2) / 2;
        *l_coeff_pos++ = 2 * l_x1 + l_xm1 - (5 * l_x0 + l_x2) / 2;
        *l_coeff_pos++ = (l_x1 - l_xm1) / 2;
        *l_coeff_pos++ = l_x0;
    }
}

//------------------------------------------------------------------------------
void Channel::processNewChunk(FilterState * p_filter_state)
{
    myassert(locked());
#ifdef DEBUG_PRINTF
    printf("%d, %d, %d, %d\n", currentChunk(), m_parent->currentRawChunk(), m_parent->currentStreamChunk(), m_lookup.size());
#endif // DEBUG_PRINTF
    myassert(m_parent->currentRawChunk() == MAX(0, m_parent->currentStreamChunk() - 1) ||
             m_parent->currentRawChunk() == MAX(0, m_parent->currentStreamChunk()));
    m_lookup.push_back(AnalysisData());
    m_lookup.back().setFilterState(*p_filter_state);
#ifdef DEBUG_PRINTF
    printf("lookup=%d\n", m_lookup.size());
#endif // DEBUG_PRINTF
    m_parent->calculateAnalysisData(/*begin(), */int(m_lookup.size()) - 1, this/*, threshold()*/);
}

//------------------------------------------------------------------------------
void Channel::processChunk(int p_chunk)
{
    myassert(locked());
    if(p_chunk >= 0 && p_chunk < totalChunks())
    {
        m_parent->calculateAnalysisData(/*begin(), */p_chunk, this/*, threshold()*/);
    }
}

//------------------------------------------------------------------------------
void Channel::reset()
{
    std::fill(begin(), end(), 0.0f);
    std::fill(m_filtered_input.begin(), m_filtered_input.end(), 0.0f);
    std::fill(m_coefficients_table.begin(), m_coefficients_table.end(), 0.0f);
}

//------------------------------------------------------------------------------
QString Channel::getUniqueFilename() const
{
    QString l_ending_star = (m_parent->saved()) ? QString("") : QString("*");

    if(getParent()->numChannels() == 1)
    {
        return QString(getFilenamePart(getParent()->getFileName())) + l_ending_star;
    }
    else
    {
        int l_index = getParent()->getChannelIndex(*this);
        return QString(getFilenamePart(getParent()->getFileName())) + " (" + QString::number(l_index + 1) + ")" + l_ending_star;
    }

    // If we're here, we didn't find the channel in the parent's channels array.
    // This should never happen!
    myassert(false);
    return QString(getFilenamePart(getParent()->getFileName())) + l_ending_star;
}

//------------------------------------------------------------------------------
float Channel::averagePitch( int p_begin
                           , int p_end
                           ) const
{
    if(p_begin < 0)
    {
        p_begin = 0;
    }
    if (p_begin >= p_end)
    {
        return -1;
    }
    myassert(locked());
    myassert(isValidChunk(p_begin) && isValidChunk(p_end-1));

    // Init the total to be the first item if certain enough or zero if not

    //do a weighted sum (using cosine window smoothing) to find the average note
    double l_good_count = 0.0;
    double l_total = 0.0;
    double l_size = double(p_end - p_begin);
    double l_window_pos;
    double l_window_weight;
    double l_weight;
    const AnalysisData * l_data;
    for(int l_i = p_begin; l_i < p_end; l_i++)
    {
        l_window_pos = double(l_i - p_begin) / l_size;
        l_window_weight = 0.5 - 0.5 * cos(l_window_pos * (2 * PI));
        l_data = dataAtChunk(l_i);
        l_weight = l_window_weight * l_data->getCorrelation() * dB2Linear(l_data->getLogRms());
        l_total += l_data->getPitch() * l_weight;
        l_good_count += l_weight;
    }
    return float(l_total / l_good_count);
}

//------------------------------------------------------------------------------
float Channel::averageMaxCorrelation( int p_begin
                                    , int p_end
                                    ) const
{
    myassert(locked());
    if(!hasAnalysisData())
    {
        return -1;
    }
    p_begin = bound(p_begin, 0, totalChunks() - 1);
    p_end = bound(p_end, 0, totalChunks() - 1);

    // Init the total to be the first item
    float l_total_correlation = dataAtChunk(p_begin)->getCorrelation();

    for(int l_i = p_begin + 1; l_i < p_end; l_i++)
    {
        l_total_correlation += dataAtChunk(l_i)->getCorrelation();
    }
    return (l_total_correlation / (p_end - p_begin + 1));
}

//------------------------------------------------------------------------------
const AnalysisData * Channel::getActiveChannelCurrentChunkData()
{
    Channel * l_active_channel = g_data->getActiveChannel();
    return (l_active_channel) ? l_active_channel->dataAtCurrentChunk() : NULL;
}

//------------------------------------------------------------------------------
void Channel::clearFreqLookup()
{
    ChannelLocker l_channel_locker(this);
    m_summary_zoom_lookup.clear();
    m_normal_zoom_lookup.clear();
}

//------------------------------------------------------------------------------
void Channel::clearAmplitudeLookup()
{
    ChannelLocker l_channel_locker(this);
    m_amplitude_zoom_lookup.clear();
}

//------------------------------------------------------------------------------
void Channel::recalcScoreThresholds()
{
    AnalysisData * l_analysis_data;
    ChannelLocker l_channel_locker(this);
    for(int l_j = 0; l_j < totalChunks(); l_j++)
    {
        if((l_analysis_data = dataAtChunk(l_j)) != NULL)
        {
            l_analysis_data->calcScores();
        }
    }
}

//------------------------------------------------------------------------------
bool Channel::isVisibleNote(int p_note_index) const
{
    myassert(p_note_index < (int)m_note_data.size());
    if(p_note_index == NO_NOTE)
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool Channel::isVisibleChunk(const AnalysisData * p_data) const
{
    myassert(p_data);
    if(p_data->getNoteScore() >= g_data->ampThreshold(NOTE_SCORE, 0))
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool Channel::isChangingChunk(AnalysisData * p_data) const
{
    myassert(p_data);
    if(p_data->getNoteChangeScore() >= g_data->ampThreshold(NOTE_CHANGE_SCORE, 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
void Channel::backTrackNoteChange(int p_chunk)
{
    int l_first = MAX(p_chunk - (int)ceil(g_long_time / timePerChunk()), getLastNote()->startChunk());
#ifdef DEBUG_PRINTF
    printf("ceil = %d, %d\n", (int)ceil(g_long_time / timePerChunk()), p_chunk-l_first);
#endif // DEBUG_PRINTF
    int l_last = p_chunk; //currentNote->endChunk();
    if(l_first >= l_last)
    {
        return;
    }
    float l_largest_weighted_diff = 0.0f;
    int l_largest_diff_chunk = l_first;
    for(int l_cur_chunk = l_first + 1, j = 1; l_cur_chunk <= l_last; l_cur_chunk++, j++)
    {
        float l_weighted_diff = fabs(dataAtChunk(l_cur_chunk)->getPitch() - dataAtChunk(l_cur_chunk)->getShortTermMean());
        if(l_weighted_diff > l_largest_weighted_diff)
        {
            l_largest_weighted_diff = l_weighted_diff;
            l_largest_diff_chunk = l_cur_chunk;
        }
    }
    getLastNote()->setEndChunk(l_largest_diff_chunk);
    getLastNote()->recalcAvgPitch();
    dataAtChunk(l_largest_diff_chunk)->setReason(5);

    //start on next note
    for(int l_cur_chunk = l_largest_diff_chunk; l_cur_chunk <= l_last; l_cur_chunk++)
    {
        dataAtChunk(l_cur_chunk)->setNoteIndex(NO_NOTE);
        dataAtChunk(l_cur_chunk)->setNotePlaying(false);
        dataAtChunk(l_cur_chunk)->setShortTermMean(dataAtChunk(l_cur_chunk)->getPitch());
        dataAtChunk(l_cur_chunk)->setLongTermMean(dataAtChunk(l_cur_chunk)->getPitch());
        dataAtChunk(l_cur_chunk)->setShortTermDeviation(0.2f);
        dataAtChunk(l_cur_chunk)->setLongTermDeviation(0.05f);
        dataAtChunk(l_cur_chunk)->setPeriodRatio(1.0f);
    }

    int l_cur_chunk = l_largest_diff_chunk;
    if(l_cur_chunk < l_last)
    {
        l_cur_chunk++;
    }
    while((l_cur_chunk < l_last) && !isVisibleChunk(dataAtChunk(l_cur_chunk)))
    {
        l_cur_chunk++;
    }
    if((l_cur_chunk < l_last) && isVisibleChunk(dataAtChunk(l_cur_chunk)))
    {
        m_note_is_playing = true;
        noteBeginning(l_cur_chunk);
        NoteData * l_current_note = getLastNote();
        myassert(l_current_note);
        dataAtChunk(l_cur_chunk)->setNoteIndex(getCurrentNoteIndex());
        dataAtChunk(l_cur_chunk)->setNotePlaying(true);
        l_cur_chunk++;
        while((l_cur_chunk < l_last) && isVisibleChunk(dataAtChunk(l_cur_chunk)))
        {
            dataAtChunk(l_cur_chunk)->setNoteIndex(getCurrentNoteIndex());
            dataAtChunk(l_cur_chunk)->setNotePlaying(true);
            l_current_note->addData(*dataAtChunk(l_cur_chunk), float(framesPerChunk()) / float(dataAtChunk(l_cur_chunk)->getPeriod()));
            l_cur_chunk++;
        }
        //just start the NSDF Aggregate from where we are now
        resetNSDFAggregate(dataAtChunk(l_last - 1)->getPeriod());
        //just start with the octave estimate from the last note
        l_current_note->setPeriodOctaveEstimate(getNote(getCurrentNoteIndex() - 1)->periodOctaveEstimate());
    }
}

//------------------------------------------------------------------------------
bool Channel::isNoteChanging(int p_chunk)
{
    AnalysisData * l_previous_data = dataAtChunk(p_chunk-1);
    if(!l_previous_data)
    {
        return false;
    }
    myassert(dataAtChunk(p_chunk));
    myassert(m_note_data.size() > 0);
    AnalysisData * l_analysis_data = dataAtChunk(p_chunk);
    //Note: analysisData.m_note_index is still undefined here
    int l_num_chunks = getLastNote()->numChunks();

    float l_diff = fabs(l_analysis_data->getPitch() - l_analysis_data->getShortTermMean());
    double l_spread = fabs(l_analysis_data->getShortTermMean() - l_analysis_data->getLongTermMean()) -
                           (l_analysis_data->getShortTermDeviation() + l_analysis_data->getLongTermDeviation()
                          );
    if(l_num_chunks >= 5 && l_spread > 0.0)
    {
        l_analysis_data->setReason(1);
        return true;
    }

    int l_first_short_chunk = MAX(p_chunk - (int)ceil(g_short_time / timePerChunk()), getLastNote()->startChunk());
    AnalysisData * l_first_short_data = dataAtChunk(l_first_short_chunk);
    double l_spread_2 = fabs(l_analysis_data->getShortTermMean() - l_first_short_data->getLongTermMean()) -
                             (l_analysis_data->getShortTermDeviation() + l_first_short_data->getLongTermDeviation()
                            );
    l_analysis_data->setSpread(l_spread);
    l_analysis_data->setSpread2(l_spread_2);

    if(l_num_chunks >= (int)(ceil(g_long_time / timePerChunk()) / 2.0) && l_spread_2 > 0.0)
    {
        l_analysis_data->setReason(4);
        return true;
    }
    if(l_num_chunks > 1 && l_diff > 2)
    { //if jumping to fast anywhere then note is changing
#ifdef DEBUG_PRINTF
        printf("numChunks=%d\n", getCurrentNote()->numChunks());
        printf("analysisData->pitch=%f, ", l_analysis_data->getPitch());
        printf("prevData->shortTermMean=%f\n", l_previous_data->getShortTermMean());
#endif // DEBUG_PRINTF
        l_analysis_data->setReason(2);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool Channel::isLabelNote(int p_note_index) const
{
    myassert(p_note_index < (int)m_note_data.size());
    if(p_note_index >= 0 && m_note_data[p_note_index].isValid())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
void Channel::processNoteDecisions( int p_chunk
                                  , float p_period_diff
                                  )
{
    myassert(locked());
    myassert(dataAtChunk(p_chunk));
    AnalysisData & l_analysis_data = *dataAtChunk(p_chunk);

    l_analysis_data.setReason(0);
    //look for note transitions
    if(m_note_is_playing)
    {
        if(isVisibleChunk(&l_analysis_data) && !isNoteChanging(p_chunk))
        {
        }
        else
        {
#ifdef DEBUG_PRINTF
            printf("ending chunk %d, ", p_chunk);
            printf("%s, %s\n", isVisibleChunk(&l_analysis_data)?"T":"F", isNoteChanging(p_chunk)?"T":"F");
#endif // DEBUG_PRINTF
            m_note_is_playing = false;
            noteEnding(p_chunk);
        }
    }
    else
    {
        if(isVisibleChunk(&l_analysis_data))
        {
            noteBeginning(p_chunk);
            p_period_diff = 0.0f;
            m_note_is_playing = true;
        }
    }

    l_analysis_data.setNotePlaying(m_note_is_playing);

    if(m_note_is_playing)
    {
        addToNSDFAggregate(dB2Linear(l_analysis_data.getLogRms()), p_period_diff);
        NoteData * l_current_note = getLastNote();
        myassert(l_current_note);

        l_analysis_data.setNoteIndex(getCurrentNoteIndex());
        l_current_note->setEndChunk(p_chunk + 1);

        l_current_note->addData(l_analysis_data, float(framesPerChunk()) / float(l_analysis_data.getPeriod()));
        l_current_note->setPeriodOctaveEstimate(calcOctaveEstimate());
        if(g_data->analysisType() != MPM_MODIFIED_CEPSTRUM)
        {
            recalcNotePitches(p_chunk);
        }
    }
    else
    {
    }
}

//------------------------------------------------------------------------------
void Channel::noteBeginning(int p_chunk)
{
    AnalysisData * l_analysis_data = dataAtChunk(p_chunk);
    m_note_data.push_back(NoteData(*this, p_chunk, *l_analysis_data));
    //initalise the aggregate NSDF data with the current NSDF data
    resetNSDFAggregate(l_analysis_data->getPeriod());
#ifdef DEBUG_PRINTF
    printf("Note Beginning: period = %f\n", l_analysis_data->getPeriod());
#endif // DEBUG_PRINTF

    m_pitch_small_smoothing_filter->reset();
    m_pitch_big_smoothing_filter->reset();
}

//------------------------------------------------------------------------------
void Channel::noteEnding(int p_chunk)
{
    AnalysisData & l_analysis_data = *dataAtChunk(p_chunk);
    //recalcNotePitches(chunk);
    if(l_analysis_data.getReason() > 0)
    {
        backTrackNoteChange(p_chunk);
    }
}

//------------------------------------------------------------------------------
float Channel::calcOctaveEstimate()
{
    Array1d<float> l_ag_data = m_nsdf_aggregate_data_scaled;
    std::vector<int> l_nsdf_aggregate_max_positions;
    MyTransforms::findNSDFMaxima(l_ag_data.begin(), l_ag_data.size(), l_nsdf_aggregate_max_positions);
    if(l_nsdf_aggregate_max_positions.empty())
    {
        return -1.0;
    }
  
    //get the highest nsdfAggregateMaxPosition
    uint l_j;
    int l_nsdf_aggregate_max_index = 0;
    for(l_j = 1; l_j < l_nsdf_aggregate_max_positions.size(); l_j++)
    {
        if(l_ag_data[l_nsdf_aggregate_max_positions[l_j]] > l_ag_data[l_nsdf_aggregate_max_positions[l_nsdf_aggregate_max_index]])
        {
            l_nsdf_aggregate_max_index = l_j;
        }
    }
    //get the choosen nsdfAggregateMaxPosition
    double l_highest = l_ag_data[l_nsdf_aggregate_max_positions[l_nsdf_aggregate_max_index]];
    double l_nsdf_aggregate_cutoff = l_highest * threshold();
    // Allow for overide by the threshold value
    int l_nsdf_aggregate_choosen_index = 0;
    for(l_j = 0; l_j < l_nsdf_aggregate_max_positions.size(); l_j++)
    {
        if(l_ag_data[l_nsdf_aggregate_max_positions[l_j]] >= l_nsdf_aggregate_cutoff)
        {
            l_nsdf_aggregate_choosen_index = l_j;
            break;
        }
    }
    //add 1 for index offset, ie position 0 = 1 period
    float l_period_estimate = float(l_nsdf_aggregate_max_positions[l_nsdf_aggregate_choosen_index] + 1);
    return l_period_estimate;
}

//------------------------------------------------------------------------------
void Channel::recalcNotePitches(int p_chunk)
{
    if(!isValidChunk(p_chunk))
    {
        return;
    }

    //recalculate the values for the note using the overall periodOctaveEstimate
    NoteData * l_current_note = getLastNote();
    if(l_current_note == NULL)
    {
        return;
    }
    myassert(l_current_note);
    int l_first = l_current_note->startChunk();
    int l_last = p_chunk;
    l_current_note->resetData();
    int l_num_notes_changed_index = 0;
    for(int l_cur_chunk=l_first; l_cur_chunk <= l_last; l_cur_chunk++)
    {
        if(chooseCorrelationIndex(l_cur_chunk, periodOctaveEstimate(l_cur_chunk)))
        {
            l_num_notes_changed_index++;
        }
        calcDeviation(l_cur_chunk);
        l_current_note->addData(*dataAtChunk(l_cur_chunk), float(framesPerChunk()) / float(dataAtChunk(l_cur_chunk)->getPeriod()));
    }
#ifdef DEBUG_PRINTF
    printf("numNotesChangedIndex=%d/%d\n", l_num_notes_changed_index, l_last - l_first + 1);
#endif // DEBUG_PRINTF
}

//------------------------------------------------------------------------------
const NoteData * Channel::getLastNote() const
{
    return (m_note_data.empty()) ? NULL : &m_note_data.back();
}

//------------------------------------------------------------------------------
NoteData * Channel::getLastNote()
{
    return (m_note_data.empty()) ? NULL : &m_note_data.back();
}

//------------------------------------------------------------------------------
const NoteData * Channel::getCurrentNote() const
{
    const AnalysisData *l_analysis_data = dataAtCurrentChunk();
    if(l_analysis_data)
    {
        int l_note_index = l_analysis_data->getNoteIndex();
        if(l_note_index >= 0 && l_note_index < (int)m_note_data.size())
        {
            return &m_note_data[l_note_index];
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
const NoteData * Channel::getNote(int p_note_index) const
{
    if(p_note_index >= 0 && p_note_index < (int)m_note_data.size())
    {
        return &m_note_data[p_note_index];
    }
    else
    {
        return NULL;
    }
}

//------------------------------------------------------------------------------
void Channel::chooseCorrelationIndex1(int p_chunk)
{
    myassert(dataAtChunk(p_chunk));
    AnalysisData & l_analysis_data = *dataAtChunk(p_chunk);
    uint l_iter_pos;
    int l_choosen_max_index = 0;
    if(l_analysis_data.isPeriodEstimatesEmpty())
    { //no period found
        return;
    }
    //choose a cutoff value based on the highest value and a relative threshold
    float l_highest = l_analysis_data.getPeriodEstimatesAmpAt(l_analysis_data.getHighestCorrelationIndex());
    float l_cutoff = l_highest * threshold();
    //find the first of the maxPositions which is above the cutoff
    for(l_iter_pos = 0; l_iter_pos < l_analysis_data.getPeriodEstimatesAmpSize(); l_iter_pos++)
    {
        if(l_analysis_data.getPeriodEstimatesAmpAt(l_iter_pos) >= l_cutoff)
        {
            l_choosen_max_index = l_iter_pos;
            break;
        }
    }
    l_analysis_data.setChosenCorrelationIndex(l_choosen_max_index);
    l_analysis_data.setCorrelation(l_analysis_data.getPeriodEstimatesAmpAt(l_choosen_max_index));
  
    l_analysis_data.setPeriod(l_analysis_data.getPeriodEstimatesAt(l_choosen_max_index));
    double l_freq = rate() / l_analysis_data.getPeriod();
    l_analysis_data.setFundamentalFreq(float(l_freq));
    l_analysis_data.setPitch(bound(freq2pitch(l_freq), 0.0, g_data->topPitch()));
    l_analysis_data.setPitchSum((double)l_analysis_data.getPitch());
    l_analysis_data.setPitch2Sum(sq((double)l_analysis_data.getPitch()));
}

//------------------------------------------------------------------------------
bool Channel::chooseCorrelationIndex( int p_chunk
                                    , float p_period_octave_estimate
                                    )
{
    myassert(dataAtChunk(p_chunk));
    AnalysisData & l_analysis_data = *dataAtChunk(p_chunk);
    uint l_iter_pos;
    int l_choosen_max_index = 0;
    bool l_is_different_index = false;
    if(l_analysis_data.isPeriodEstimatesEmpty())
    {
        //no period found
        return false;
    }
    if(g_data->analysisType() == MPM || g_data->analysisType() == MPM_MODIFIED_CEPSTRUM)
    {
        //choose the periodEstimate which is closest to the periodOctaveEstimate
        float l_min_dist = fabs(l_analysis_data.getPeriodEstimatesAt(0) - p_period_octave_estimate);
        float l_dist;
        for(l_iter_pos = 1; l_iter_pos < l_analysis_data.getPeriodEstimatesSize(); l_iter_pos++)
        {
            l_dist = fabs(l_analysis_data.getPeriodEstimatesAt(l_iter_pos) - p_period_octave_estimate);
            if(l_dist < l_min_dist)
            {
                l_min_dist = l_dist;
                l_choosen_max_index = l_iter_pos;
            }
        }
    }
    else
    {
        l_choosen_max_index = l_analysis_data.getHighestCorrelationIndex();
    }
    if(l_choosen_max_index != l_analysis_data.getChosenCorrelationIndex())
    {
        l_is_different_index = true;
    }
    l_analysis_data.setChosenCorrelationIndex(l_choosen_max_index);
    l_analysis_data.setCorrelation(l_analysis_data.getPeriodEstimatesAmpAt(l_choosen_max_index));
  
    l_analysis_data.setPeriod(l_analysis_data.getPeriodEstimatesAt(l_choosen_max_index));
    double l_freq = rate() / l_analysis_data.getPeriod();
    l_analysis_data.setFundamentalFreq(float(l_freq));
    l_analysis_data.setPitch(bound(freq2pitch(l_freq), 0.0, g_data->topPitch()));
    if(p_chunk > 0 && !isFirstChunkInNote(p_chunk))
    {
        l_analysis_data.setPitchSum(dataAtChunk(p_chunk - 1)->getPitchSum() + (double)l_analysis_data.getPitch());
        l_analysis_data.setPitch2Sum(dataAtChunk(p_chunk - 1)->getPitch2Sum() + sq((double)l_analysis_data.getPitch()));
    }
    else
    {
        l_analysis_data.setPitchSum((double)l_analysis_data.getPitch());
        l_analysis_data.setPitch2Sum(sq((double)l_analysis_data.getPitch()));
    }
    return l_is_different_index;
}

//------------------------------------------------------------------------------
void Channel::calcDeviation(int p_chunk)
{
    int l_last_chunk = p_chunk;
    int l_current_note_index = getCurrentNoteIndex();
    myassert(dataAtChunk(p_chunk));
    AnalysisData & l_last_chunk_data = *dataAtChunk(l_last_chunk);
    if(l_current_note_index < 0)
    {
        return;
    }

    //Do long term calculation
    int l_first_chunk = MAX(l_last_chunk - (int)ceil(g_long_time / timePerChunk()), m_note_data[l_current_note_index].startChunk());
    AnalysisData * l_first_chunk_data = dataAtChunk(l_first_chunk);
    int l_num_chunks = (l_last_chunk - l_first_chunk);
    double l_mean_sum;
    double l_mean;
    double l_sum_X2;
    double l_variance;
    double l_standard_deviation;
    if(l_num_chunks > 0)
    {
        l_mean_sum = (l_last_chunk_data.getPitchSum() - l_first_chunk_data->getPitchSum());
        l_mean = l_mean_sum / double(l_num_chunks);
        l_last_chunk_data.setLongTermMean(l_mean);
        l_sum_X2 = (l_last_chunk_data.getPitch2Sum() - l_first_chunk_data->getPitch2Sum());
        l_variance = l_sum_X2 / double(l_num_chunks) - sq(l_mean);
        l_standard_deviation = sqrt(fabs(l_variance));
        l_last_chunk_data.setLongTermDeviation(g_long_base + sqrt(l_standard_deviation) * g_long_stretch);
    }
    else
    {
        l_last_chunk_data.setLongTermMean(l_first_chunk_data->getPitch());
        l_last_chunk_data.setLongTermDeviation(g_long_base);
    }

    //Do short term calculation
    l_first_chunk = MAX(l_last_chunk - (int)ceil(g_short_time / timePerChunk()), m_note_data[l_current_note_index].startChunk());
    l_first_chunk_data = dataAtChunk(l_first_chunk);
    l_num_chunks = (l_last_chunk - l_first_chunk);
    if(l_num_chunks > 0)
    {
        l_mean_sum = (l_last_chunk_data.getPitchSum() - l_first_chunk_data->getPitchSum());
        l_mean = l_mean_sum / double(l_num_chunks);
        l_last_chunk_data.setShortTermMean(l_mean);
        l_sum_X2 = (l_last_chunk_data.getPitch2Sum() - l_first_chunk_data->getPitch2Sum());
        l_variance = l_sum_X2 / double(l_num_chunks) - sq(l_mean);
        l_standard_deviation = sqrt(fabs(l_variance));
        l_last_chunk_data.setShortTermDeviation(g_short_base + sqrt(l_standard_deviation) * g_short_stretch);
    }
    else
    {
        l_last_chunk_data.setShortTermMean(l_first_chunk_data->getPitch());
        l_last_chunk_data.setShortTermDeviation(g_short_base);
    }
}

//------------------------------------------------------------------------------
bool Channel::isFirstChunkInNote(int p_chunk) const
{
    const AnalysisData * l_analysis_data = dataAtChunk(p_chunk);
    if(l_analysis_data && l_analysis_data->getNoteIndex() >= 0 && m_note_data[l_analysis_data->getNoteIndex()].startChunk() == p_chunk)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
void Channel::resetNSDFAggregate(float p_period)
{
    m_nsdf_aggregate_roof = 0.0;
    std::fill(m_nsdf_aggregate_data.begin(), m_nsdf_aggregate_data.end(), 0.0f);
    std::fill(m_nsdf_aggregate_data_scaled.begin(), m_nsdf_aggregate_data_scaled.end(), 0.0f);

    NoteData * l_current_note = getLastNote();
    myassert(l_current_note);
    l_current_note->set_nsdf_aggregate_roof(0.0);
    l_current_note->set_current_nsdf_period(p_period);
    l_current_note->set_first_nsdf_period(p_period);
}

//------------------------------------------------------------------------------
void Channel::addToNSDFAggregate( const float p_scaler
                                , float p_period_diff
                                )
{
    AnalysisData * l_analysis_data = dataAtCurrentChunk();

    m_nsdf_aggregate_roof += p_scaler;
    addElements(m_nsdf_aggregate_data.begin(), m_nsdf_aggregate_data.end(), m_nsdf_data.begin(), p_scaler);

    NoteData *l_current_note = getLastNote();
    myassert(l_current_note);
    l_current_note->add_nsdf_aggregate_roof(p_scaler);
    l_current_note->add_current_nsdf_period(p_period_diff);
    float l_period_ratio = l_current_note->get_current_nsdf_period() / l_current_note->get_first_nsdf_period();
    l_analysis_data->setPeriodRatio(l_period_ratio);
#ifdef DEBUG_PRINTF
    printf("%f, periodDiff = %f\n", l_current_note->get_current_nsdf_period(), p_period_diff);
    printf("currentNsdfPeriod = %f\n", l_current_note->get_current_nsdf_period());
    printf("ratio=%f\n", l_period_ratio);
#endif // DEBUG_PRINTF
    int l_len = m_nsdf_data.size();
    float l_stretch_len = float(l_len) * l_period_ratio;
    Array1d<float> l_stretch_data(l_len);

    //the scaled version
    stretch_array(l_len, m_nsdf_data.begin(), l_len, l_stretch_data.begin(), 0.0f, l_stretch_len, LINEAR);
    addElements(m_nsdf_aggregate_data_scaled.begin(), m_nsdf_aggregate_data_scaled.end(), l_stretch_data.begin(), p_scaler);
    copyElementsDivide(m_nsdf_aggregate_data_scaled.begin(), m_nsdf_aggregate_data_scaled.end(), l_current_note->get_nsdf_aggregate_data_scaled().begin(), l_current_note->get_nsdf_aggregate_roof());

    //the unscaled version
    copyElementsDivide(m_nsdf_aggregate_data.begin(), m_nsdf_aggregate_data.end(), l_current_note->get_nsdf_aggregate_data().begin(), l_current_note->get_nsdf_aggregate_roof());

#ifdef DEBUG_PRINTF
    printf("chunk = %d, ", currentChunk());
    printf("scaler = %f, ", p_scaler);
    printf("periodDiff = %f, ", p_period_diff);
    printf("l_current_note->currentNsdfPeriod = %f, ", l_current_note->get_current_nsdf_period());
    printf("l_current_note->firstNsdfPeriod = %f, ", l_current_note->get_first_nsdf_period());
    printf("l_period_ratio = %f\n", l_period_ratio);
#endif // DEBUG_PRINTF

}

//------------------------------------------------------------------------------
float Channel::calcDetailedPitch( float * p_input
                                , double p_period
                                , int /*chunk*/
                                )
{
#ifdef DEBUG_PRINTF
    printf("begin calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
    const int l_pitch_samples_range = 4; //look 4 samples either side of the period. Total of 9 different subwindows.

    int l_n = size();
    int l_i;
    int l_j;
    int l_j2;
    int l_subwindow_size;
    if(p_period < l_pitch_samples_range || p_period > double(l_n) * (3.0 / 8.0) - l_pitch_samples_range)
    {
        std::fill(m_detailed_pitch_data.begin(), m_detailed_pitch_data.end(), 0.0f); //invalid period
        std::fill(m_detailed_pitch_data_smoothed.begin(), m_detailed_pitch_data_smoothed.end(), 0.0f); //invalid period
#ifdef DEBUG_PRINTF
        printf("end calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
        return 0.0f;
    }
    int l_i_period = int(floor(p_period));
    l_subwindow_size = l_n / 4;
    int l_num = l_n / 2;
    if(l_i_period > l_subwindow_size)
    {
        l_subwindow_size = l_n / 4 - (l_i_period - l_n / 4);
    }

#ifdef DEBUG_PRINTF
    printf("n=%d, subwindow_size=%d, iPeriod=%d\n", l_n, l_subwindow_size, l_i_period);
#endif // DEBUG_PRINTF

    std::vector<int> l_periods;
    for(l_j = -l_pitch_samples_range; l_j <= l_pitch_samples_range; l_j++)
    {
        l_periods.push_back(l_i_period + l_j);
    }
    int l_ln = l_periods.size();

    std::vector<float> l_square_table(l_n);
    for(l_j = 0; l_j < l_n; l_j++)
    {
        l_square_table[l_j] = sq(p_input[l_j]);
    }

    std::vector<float> l_left(l_ln);
    std::vector<float> l_right(l_ln);
    std::vector<float> l_left_pow(l_ln);
    std::vector<float> l_right_pow(l_ln);
    std::vector<float> l_pow(l_ln);
    std::vector<float> l_err(l_ln);
    std::vector<float> l_result(l_ln);
    Array1d<float> l_unsmoothed(l_num);

    //calc the values of pow and err for the first in each row.
    for(l_i = 0; l_i < l_ln; l_i++)
    {
        l_left_pow[l_i] = l_right_pow[l_i] = l_pow[l_i] = l_err[l_i] = 0;
        int l_offset = l_periods[l_i] - l_i_period;
        for(l_j = 0, l_j2 = l_periods[l_i]; l_j < l_subwindow_size - l_offset; l_j++, l_j2++)
        {
            l_left_pow[l_i] += l_square_table[l_j]; //sq(input[l_j]);
            l_right_pow[l_i] += l_square_table[l_j2]; //sq(input[l_j2]);
            l_err[l_i] += sq(p_input[l_j] - p_input[l_j2]);
        }
    }
#ifdef DEBUG_PRINTF
    printf("subwindow_size=%d, num=%d, period=%lf\n", l_subwindow_size, l_num, p_period);
#endif // DEBUG_PRINTF
    //TODO: speed up this for loop
    int l_left1 = 0;
    int l_left2;
    int l_right1;
    int l_right2 = l_left1 + l_subwindow_size + l_i_period;

    for(; l_left1 < l_num; l_left1++, l_right2++)
    {
        for(l_i = 0; l_i < l_ln; l_i++)
        {
            l_right1 = l_left1 + l_periods[l_i];
            l_left2 = l_right2 - l_periods[l_i];

            l_pow[l_i] = l_left_pow[l_i] + l_right_pow[l_i];
            l_result[l_i] = 1.0 - (l_err[l_i] / l_pow[l_i]);

            l_err[l_i] += sq(p_input[l_left2] - p_input[l_right2]) - sq(p_input[l_left1] - p_input[l_right1]);
            l_left_pow[l_i]  += l_square_table[l_left2] - l_square_table[l_left1];
            l_right_pow[l_i] += l_square_table[l_right2] - l_square_table[l_right1];
        }

        int l_pos = int(std::max_element(l_result.begin(), l_result.begin() + l_ln) - l_result.begin());
        myassert(l_pos >= 0 && l_pos < l_ln);
        if(l_pos > 0 && l_pos < l_ln - 1)
        {
            l_unsmoothed[l_left1] = double(l_periods[l_pos]) + parabolaTurningPoint(l_result[l_pos - 1], l_result[l_pos], l_result[l_pos + 1]);
        }
        else
        {
            l_unsmoothed[l_left1] = double(l_periods[l_pos]);
        }
    }

    float l_period_diff = l_unsmoothed.back() - l_unsmoothed.front();

    for(l_j = 0; l_j < l_num; l_j++)
    {
        l_unsmoothed[l_j] = freq2pitch(rate() / l_unsmoothed[l_j]);
    }

    m_pitch_big_smoothing_filter->filter(l_unsmoothed.begin(), m_detailed_pitch_data_smoothed.begin(), l_num);
#ifdef DEBUG_PRINTF
    printf("big filter size = %d\n", m_pitch_big_smoothing_filter->delay());
#endif // DEBUG_PRINTF
    for(l_j = 0; l_j < l_num; l_j++)
    {
        m_detailed_pitch_data_smoothed[l_j] = bound(m_detailed_pitch_data_smoothed[l_j], 0.0f, (float)g_data->topPitch());
    }

    m_pitch_small_smoothing_filter->filter(l_unsmoothed.begin(), m_detailed_pitch_data.begin(), l_num);
#ifdef DEBUG_PRINTF
    printf("small filter size = %d\n", m_pitch_small_smoothing_filter->delay());
#endif // DEBUG_PRINTF
    for(l_j = 0; l_j < l_num; l_j++)
    {
        m_detailed_pitch_data[l_j] = bound(m_detailed_pitch_data[l_j], 0.0f, (float)g_data->topPitch());
    }
#ifdef DEBUG_PRINTF
    printf("end calcDetailedPitch\n"); fflush(stdout);
#endif // DEBUG_PRINTF
    return l_period_diff;
}

//------------------------------------------------------------------------------
void Channel::calcVibratoData(int p_chunk)
{
    NoteData * l_current_note = getLastNote();
    if (l_current_note && (dataAtChunk(p_chunk)->getNoteIndex() >= 0))
    {
        l_current_note->addVibratoData(p_chunk);
    }
}

//------------------------------------------------------------------------------
float Channel::periodOctaveEstimate(int p_chunk) const
{
    const AnalysisData * l_analysis_data = dataAtChunk(p_chunk);
    if(l_analysis_data && l_analysis_data->getNoteIndex() >= 0)
    {
        return m_note_data[l_analysis_data->getNoteIndex()].periodOctaveEstimate() * l_analysis_data->getPeriodRatio();
    }
    else
    {
        return -1.0f;
    }
}

//------------------------------------------------------------------------------
void Channel::exportChannel( int p_type
                           , QString p_type_string
                           ) const
{
    QString l_string = QFileDialog::getSaveFileName(g_main_window, "Choose a filename to save under", ".", p_type_string);
#ifdef DEBUG_PRINTF
    printf("file = %s\n", l_string.toStdString().c_str());
#endif // DEBUG_PRINTF
    if(l_string.isEmpty())
    {
        return;
    }

    QFile l_file(l_string);
    l_file.open(QIODevice::WriteOnly);
    QTextStream l_out(&l_file);
    if(p_type == 0)
    { //plain text
        l_out << "        Time(secs) Pitch(semi-tones)       Volume(rms)" << endl;
        l_out << qSetFieldWidth(18);
        for(int l_j = 0; l_j < totalChunks(); l_j++)
        {
            l_out << timeAtChunk(l_j) << dataAtChunk(l_j)->getPitch() << dataAtChunk(l_j)->getLogRms() << endl;
        }
    }
    else if(p_type == 1)
    {
        //matlab file
        l_out << "t = [";
        for(int l_j = 0; l_j < totalChunks(); l_j++)
        {
            if(l_j > 0)
            {
                l_out << ", ";
            }
            l_out << timeAtChunk(l_j);
        }
        l_out << "];" << endl;

        l_out << "pitch = [";
        for(int l_j = 0; l_j < totalChunks(); l_j++)
        {
            if(l_j > 0)
            {
                l_out << ", ";
            }
            l_out << dataAtChunk(l_j)->getPitch();
        }
        l_out << "];" << endl;

        l_out << "volume = [";
        for(int l_j = 0; l_j < totalChunks(); l_j++)
        {
            if(l_j > 0)
            {
                l_out << ", ";
            }
            l_out << dataAtChunk(l_j)->getLogRms();
        }
        l_out << "];" << endl;
    }
    l_file.close();
}

//------------------------------------------------------------------------------
void Channel::doPronyFit(int p_chunk)
{
    if(p_chunk < m_prony_window_size)
    {
        return;
    }

    int l_start = p_chunk - m_prony_window_size;
    int l_center = p_chunk - pronyDelay();
    AnalysisData * l_data = dataAtChunk(l_center);
    for(int l_j = 0; l_j < m_prony_window_size; l_j++)
    {
        m_prony_data[l_j] = dataAtChunk(l_start + l_j)->getPitch();
    }
    PronyData l_prony_data;
    if(pronyFit(&l_prony_data, m_prony_data.begin(), m_prony_window_size, 2, true))
    {
        l_data->setVibratoError(l_prony_data.get_error());
        l_data->setVibratoPitch(l_prony_data.get_y_offset());
        if(l_prony_data.get_error() < 1.0)
        {
            l_data->setVibratoSpeed(l_prony_data.freqHz(timePerChunk()));
            if(l_prony_data.get_omega() * m_prony_window_size < g_two_pi)
            {
                l_data->setVibratoPitch(l_data->getPitch());
            }
            else
            {
                l_data->setVibratoWidth(l_prony_data.get_amplitude());
                l_data->setVibratoPhase(l_prony_data.get_phase());
                l_data->setVibratoWidthAdjust(0.0f);
            }
        }
        else
        {
            l_data->setVibratoPitch(l_data->getPitch());
        }
    }
    else
    {
        l_data->setVibratoPitch(l_data->getPitch());
    }
}

//EOF
