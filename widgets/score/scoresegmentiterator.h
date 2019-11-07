/***************************************************************************
                          scoresegmentiterator.h  -  description
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
#ifndef SCORESEGMENTITERATOR_H
#define SCORESEGMENTITERATOR_H

#include "channel.h"

class ScoreWidget;

class ScoreSegmentIterator
{

  public:

    inline ScoreSegmentIterator( ScoreWidget *p_score_widget
                               , Channel *p_channel
                               );
    /**
     Initalise the iterator to just before the first segment
     @param p_score_widget The ScoreWidget to iterate through it's segments
    */
    void reset( ScoreWidget * p_score_widget
              , Channel * p_channel
              );

    /**
     move to the next score segment
     @return true, if their was another score segment
    */
    bool next();
    inline bool isValid();

    inline double leftTime();
    inline double rightTime();
    inline double leftX();
    inline int lineCenterY();
    inline int staveTop();
    inline int staveBottom();
    double widthX();
    inline double curTime();
    inline int curPage();
    inline int numPages();
    inline bool contains(double p_t);

  private:

    ScoreWidget * m_score_widget;

    Channel * m_channel;
    int m_stave_height;
    int m_half_stave_height;
    int m_num_rows;
    double m_total_row_time;
    double m_total_page_time;
    double m_cur_time;

    int m_cur_page;
    double m_look_ahead_gap_time;
    double m_look_ahead_time;
    double m_look_behind_time;
    double m_start_of_page_time;
    double m_cur_rel_page_time;
    double m_end_time;
    int m_num_pages;
    double m_end_of_last_page_time;
    double m_look_ahead_time2;
    double m_look_behind_time2;
    double m_look_behind_time3;
    int m_cur_row;
    int m_row_counter;
    int m_sub_row_counter;

    double m_left_time;
    double m_right_time;
    double m_left_X;
    int m_line_center_Y;
    bool m_is_valid;

};

#include "scoresegmentiterator.hpp"

#endif // SCORESEGMENTITERATOR_H
// EOF
