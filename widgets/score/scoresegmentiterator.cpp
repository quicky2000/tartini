/***************************************************************************
                          scorsegmentiterator.cpp  -  description
                             -------------------
    begin                : Mar 29 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include <QPixmap>
#include <QPainter>

#include "scorewidget.h"
#include "scoresegmentiterator.h"

//------------------------------------------------------------------------------
double ScoreSegmentIterator::widthX(void)
{
    return (m_right_time - m_left_time) * m_score_widget->m_scale_X;
}

//------------------------------------------------------------------------------
void ScoreSegmentIterator::reset(ScoreWidget * p_score_widget, Channel * p_channel)
{
    m_score_widget = p_score_widget;
    m_channel = p_channel;
    m_stave_height = m_score_widget->getStaveHeight();
    m_half_stave_height = m_score_widget->getStaveCenterY();
    m_num_rows = std::max((m_score_widget->height() - toInt(m_score_widget->m_boarder_Y * 2)) / m_stave_height, 1);
    m_total_row_time = (double)(m_score_widget->width() - m_score_widget->m_boarder_X * 2) / m_score_widget->m_scale_X;
    m_total_page_time = m_total_row_time * m_num_rows;
    if(m_channel)
    {
        m_cur_time = m_channel->timeAtChunk(m_channel->currentChunk());
        m_cur_page = (int)floor(m_cur_time / m_total_page_time);
        m_look_ahead_gap_time = m_score_widget->m_look_ahead_gap * m_total_page_time;
        m_look_ahead_time = m_cur_time + m_score_widget->m_look_ahead * m_total_page_time;
        m_look_behind_time = m_look_ahead_time - m_total_page_time;
        m_start_of_page_time = floor(m_cur_time / m_total_page_time) * m_total_page_time;
        m_cur_rel_page_time = m_cur_time - m_start_of_page_time;
        m_end_time = m_channel->totalTime();
        m_num_pages = (int)ceil(m_end_time / m_total_page_time);
        m_end_of_last_page_time = m_num_pages * m_total_page_time;
        m_look_ahead_time2 = (m_cur_page == m_num_pages - 1) ? std::min(m_end_time, m_look_ahead_time) : m_look_ahead_time;
        m_look_ahead_time2 = (m_cur_page == 0) ? m_end_of_last_page_time : m_look_ahead_time2;
        m_look_behind_time2 = std::min(m_look_behind_time, m_end_time - m_total_page_time);
        //FIXME: The ending page isn't drawn correctly
        m_look_behind_time2 = (m_cur_page == 0) ? m_look_behind_time + m_total_page_time : m_look_behind_time2;
        m_look_behind_time3 = std::min(m_look_behind_time, m_end_time - m_total_page_time);
        m_cur_row = (int)floor(m_cur_rel_page_time / m_total_row_time);
    }
    m_row_counter = 0;
    m_sub_row_counter = -1;
    m_left_time = m_right_time = m_left_X = 0.0;
    m_line_center_Y = 0;
    m_is_valid = false;
}

//------------------------------------------------------------------------------
bool ScoreSegmentIterator::next(void)
{
    if(m_channel)
    {
        while(m_row_counter < m_num_rows)
        {
            int l_j = m_row_counter;
            double l_start_of_row_time = m_start_of_page_time + l_j * m_total_row_time;
            double l_end_of_row_time = l_start_of_row_time + m_total_row_time;
            m_line_center_Y = toInt(m_score_widget->m_boarder_Y) + m_half_stave_height + m_stave_height * l_j;
            while(++m_sub_row_counter < 4)
            {
                switch(m_sub_row_counter)
                {
                    case 1:
                        if(l_start_of_row_time < m_look_behind_time3)
                        {
                            //draw any parts of the next page
                            m_left_time = l_start_of_row_time + m_total_page_time;
                            m_right_time = std::min(l_end_of_row_time, m_look_behind_time3) + m_total_page_time;
                            m_left_X = (double)m_score_widget->m_boarder_X;
                            return (m_is_valid = true);
                        }
                    break;
                    case 2:
                        if(l_end_of_row_time > m_look_behind_time3 + m_look_ahead_gap_time && l_start_of_row_time < m_look_ahead_time2)
                        {
                            //normal case
                            m_left_time = std::max(l_start_of_row_time, m_look_behind_time3 + m_look_ahead_gap_time);
                            m_right_time = std::min(l_start_of_row_time + m_total_row_time, m_look_ahead_time2);
                            m_left_X = (double)m_score_widget->m_boarder_X + (m_left_time - l_start_of_row_time) * m_score_widget->m_scale_X;
                            return (m_is_valid = true);
                        }
                        break;
                    case 3:
                        if(l_end_of_row_time - m_total_page_time > m_look_behind_time2 + m_look_ahead_gap_time)
                        {
                            m_left_time = std::max(l_start_of_row_time - m_total_page_time, m_look_behind_time2 + m_look_ahead_gap_time);
                            m_left_time = std::min(m_left_time, l_end_of_row_time - m_total_page_time);
                            m_right_time = l_end_of_row_time - m_total_page_time;
                            m_left_X = (double)m_score_widget->m_boarder_X + (m_left_time -(l_start_of_row_time - m_total_page_time)) * m_score_widget->m_scale_X;
                            return (m_is_valid = true);
                        }
                }
            }
            m_row_counter++;
            m_sub_row_counter = -1;
        }
    }
    else
    {
        while(m_row_counter < m_num_rows)
        {
            double l_start_of_row_time = m_start_of_page_time + m_row_counter * m_total_row_time;
            double l_end_of_row_time = l_start_of_row_time + m_total_row_time;
            m_line_center_Y = toInt(m_score_widget->m_boarder_Y) + m_half_stave_height + m_stave_height * m_row_counter;
            m_left_X = m_score_widget->m_boarder_X;
            m_left_time = l_start_of_row_time;
            m_right_time = l_end_of_row_time;
            m_row_counter++;
            return (m_is_valid = true);
        }
    }
    return (m_is_valid = false);
}

// EOF
