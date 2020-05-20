/***************************************************************************
                          scorewidget.cpp  -  description
                             -------------------
    begin                : Mar 26 2006
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
#include <QMouseEvent>
#include <QPaintEvent>
#include <QtGlobal>

#include "scorewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "qfontmetrics.h"
#include "qpen.h"
#include "scoresegmentiterator.h"
#include "music_note.h"

//------------------------------------------------------------------------------
ScoreWidget::ScoreWidget(QWidget *p_parent)
: DrawWidget(p_parent, "ScoreWidget")
{
    m_scale_Y = 10;
    m_scale_X = 40;
    m_boarder_X = 15;
    m_boarder_Y = 15;
    m_show_base_clef = true;
    m_use_flats = false;
    m_show_notes = true;
    m_show_opaque_notes = false;
    m_show_all_mode = false;
    m_font_height = 14;
    m_pitch_offset = 0; //-12; //display 1 octave down
    m_font = QFont(tr("AnyStyle"), m_font_height);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    m_font_width = QFontMetrics(m_font).horizontalAdvance("♯");
#else // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    m_font_width = QFontMetrics(m_font).width("♯");
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    //half a page look ahead
    m_look_ahead = 0.5;
    m_look_ahead_gap = 0.2;
    m_mouse_down = false;

    int l_counter = 0;
    for(int l_j = 0; l_j <= 128; l_j++)
    {
        m_flats_lookup[l_j] = l_counter;
        if(!isBlackNote(l_j))
        {
            l_counter++;
        }
        m_sharps_lookup[l_j] = l_counter;
    }

    //make relative to Middle C
    int l_sharps_middle_C = m_sharps_lookup[60];
    int l_flats_middle_C = m_flats_lookup[60];
    for(int l_j = 0; l_j <= 128; l_j++)
    {
        m_sharps_lookup[l_j] -= l_sharps_middle_C;
        m_flats_lookup[l_j] -= l_flats_middle_C;
    }
}

//------------------------------------------------------------------------------
ScoreWidget::~ScoreWidget()
{
}

//------------------------------------------------------------------------------
void ScoreWidget::drawNote( int p_x
                          , int p_y
                          , StemType p_step_type
                          , FillType p_fill_type
                          , int p_num_flicks
                          )
{
    double l_note_width = m_scale_Y;
    double l_note_height = m_scale_Y;

    //draw the left hand side of the note on the point
    if(GData::getUniqueInstance().polish())
    {
        get_painter().setRenderHint(QPainter::Antialiasing, true);
    }
    if(p_fill_type == FillType::FilledNote)
    {
        get_painter().setBrush(get_painter().pen().color());
    }
    else
    {
        get_painter().setBrush(Qt::NoBrush);
    }
    get_painter().drawEllipse(p_x, p_y - toInt(l_note_height / 2), toInt(l_note_width), toInt(l_note_height));

    if(GData::getUniqueInstance().polish())
    {
        get_painter().setRenderHint(QPainter::Antialiasing, false);
    }
    if(p_step_type == StemType::StemUp)
    {
        int l_stem_X = p_x + toInt(l_note_width);
        get_painter().drawLine(l_stem_X, p_y - toInt(3 * m_scale_Y), l_stem_X, p_y);
        for(int l_j = 0; l_j < p_num_flicks; l_j++)
        {
            get_painter().drawLine(l_stem_X, p_y - toInt((3.0 - 0.5 * l_j) * m_scale_Y), l_stem_X + toInt(l_note_width * 0.75), p_y - toInt((2.5 - 0.5 * l_j) * m_scale_Y));
        }
    }
    else if(p_step_type == StemType::StemDown)
    {
        int l_stem_X = p_x;
        get_painter().drawLine(l_stem_X, p_y + toInt(3 * m_scale_Y), l_stem_X, p_y);
        for(int l_j = 0; l_j < p_num_flicks; l_j++)
        {
            get_painter().drawLine(l_stem_X, p_y + toInt((3.0 - 0.5 * l_j ) * m_scale_Y), l_stem_X + toInt(l_note_width * 0.75), p_y + toInt((2.5 - 0.5 * l_j) * m_scale_Y));
        }
    }
}

//------------------------------------------------------------------------------
ScoreWidget::NoteType ScoreWidget::getNoteType(double p_note_length)
{
    double l_crotchet_length = 0.4;
    p_note_length /= l_crotchet_length;

    if(p_note_length < 0.25 - 0.1  )
    {
        return NoteType::DemiSemiQuaver;
    }
    else if(p_note_length < 0.5 - 0.125)
    {
        return NoteType::SemiQuaver;
    }
    else if(p_note_length < 1.0 - 0.25 )
    {
        return NoteType::Quaver;
    }
    else if(p_note_length < 2.0 - 0.5  )
    {
        return NoteType::Crotchet;
    }
    else if(p_note_length < 4.0 - 1.0  )
    {
        return NoteType::Minum;
    }
    else
    {
        return NoteType::SemiBreve;
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawNoteAtPitch( int p_x
                                 , int p_y
                                 , int p_pitch
                                 , double p_note_length
                                 , double p_volume
                                 )
{
    p_pitch = bound(p_pitch + m_pitch_offset, 0, 128);
    if(!m_show_all_mode)
    {
        if(p_note_length < 0.2)
        {
            //Don't show really short notes
            return;
        }

        if(p_pitch > 84)
        {
            //Don't show extreame notes
            return;
        }

        if(m_show_base_clef)
        {
            if(p_pitch < 36)
            {
                return;
            }
        }
        else
        {
            if(p_pitch < 57)
            {
                return;
            }
        }
    }

    double l_note_width = m_scale_Y;
    double l_note_height = m_scale_Y;
    int l_y_offset;
    int l_y_Steps;
    int l_accidental_offset_X = (m_show_notes) ? 2 : 0;
    int l_j;
    if(!m_show_notes)
    {
        get_painter().setPen(QPen(Qt::red, 1));
    }
    if(m_use_flats)
    {
        l_y_Steps = m_flats_lookup[p_pitch];
        l_y_offset = toInt(m_scale_Y * static_cast<double>(l_y_Steps) * 0.5);
        if(isBlackNote(p_pitch))
        {
            get_painter().drawText(p_x - l_accidental_offset_X - m_font_width, p_y - l_y_offset + m_font_height / 2, "♭");
        }
    }
    else
    {
        l_y_Steps = m_sharps_lookup[p_pitch];
        l_y_offset = toInt(m_scale_Y * static_cast<double>(l_y_Steps) * 0.5);
        if(isBlackNote(p_pitch))
        {
            get_painter().drawText(p_x - l_accidental_offset_X - m_font_width, p_y - l_y_offset + m_font_height / 2, "♯");
        }
    }

    //draw any ledger lines
    get_painter().setPen(QPen(Qt::black, 1));
    int l_leger_left_X = p_x - toInt(l_note_width / 2);
    int l_leger_right_X = p_x + toInt(l_note_width + l_note_width / 2);
    if(l_y_Steps == 0)
    {
        //Middle C ledger line
        get_painter().drawLine(l_leger_left_X, p_y, l_leger_right_X, p_y);
    }
    for(l_j = 6; l_j <= l_y_Steps / 2; l_j++)
    {
        //top ledger lines
        get_painter().drawLine(l_leger_left_X, p_y - toInt(m_scale_Y * l_j), l_leger_right_X, p_y - toInt(m_scale_Y * l_j));
    }
    for(l_j = (m_show_base_clef) ? -6 : 0; l_j >= l_y_Steps / 2; l_j--)
    {
        //bottom ledger lines
        get_painter().drawLine(l_leger_left_X, p_y - toInt(m_scale_Y * l_j), l_leger_right_X, p_y - toInt(m_scale_Y * l_j));
    }

    NoteType l_note_type = getNoteType(p_note_length);
    QColor l_note_color(Qt::black);
    if(!m_show_opaque_notes)
    {
        l_note_color.setAlphaF(p_volume);
    }
    get_painter().setPen(QPen(l_note_color, 2));
    if(m_show_notes)
    {
        StemType l_stem_type = StemType::StemNone;
        if(l_note_type <= NoteType::Minum)
        {
            l_stem_type = (l_y_Steps < 7) ? StemType::StemUp : StemType::StemDown;
        }
        int l_num_flicks = 0;
        if(l_note_type <= NoteType::Quaver)
        {
            l_num_flicks = 1 + (static_cast<int>(NoteType::Quaver) - static_cast<int>(l_note_type));
        }
        drawNote(p_x, p_y-l_y_offset, l_stem_type, (l_note_type<=NoteType::Crotchet) ? FillType::FilledNote : FillType::EmptyNote, l_num_flicks);
    }
    else
    {
      get_painter().drawRoundedRect(p_x, p_y - l_y_offset - toInt(l_note_height / 2), toInt(p_note_length * m_scale_X) - 1, toInt(l_note_height), 3, 3, Qt::RelativeSize);
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawStaveSegment( int p_left_X
                                  , int p_line_center_Y
                                  , int p_width_X
                                  )
{
    int l_j;
    get_painter().setPen(QPen(Qt::blue, 1));
    for(l_j = 1; l_j <= 5; l_j++)
    {
        get_painter().drawLine(p_left_X, p_line_center_Y - toInt(l_j * m_scale_Y), p_left_X + p_width_X, p_line_center_Y - toInt(l_j * m_scale_Y));
    }

    if(m_show_base_clef)
    {
        get_painter().drawLine(p_left_X, p_line_center_Y - toInt(5 * m_scale_Y), p_left_X, p_line_center_Y + toInt(5 * m_scale_Y));
        for(l_j = 1; l_j <= 5; l_j++)
        {
            get_painter().drawLine(p_left_X, p_line_center_Y + toInt(l_j * m_scale_Y), p_left_X + p_width_X, p_line_center_Y + toInt(l_j * m_scale_Y));
        }
    }
    else
    {
        get_painter().drawLine(p_left_X, p_line_center_Y - toInt(5 * m_scale_Y), p_left_X, p_line_center_Y - toInt(1 * m_scale_Y));
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::drawScoreSegment( Channel * p_channel
                                  , double p_left_X
                                  , int p_line_center_Y
                                  , double p_left_time
                                  , double p_right_time
                                  )
{
    myassert(p_channel);
    unsigned int l_j;

    drawStaveSegment(toInt(p_left_X), p_line_center_Y, toInt((p_right_time - p_left_time) * m_scale_X));

    if(p_channel)
    {
        get_painter().setPen(QPen(Qt::black, 2));
        ChannelLocker l_channel_locker(p_channel);
        //draw the notes
        for(l_j = 0; l_j < p_channel->get_note_data().size(); l_j++)
        {
            if(p_channel->isVisibleNote(l_j) && p_channel->isLabelNote(l_j))
            {
                double l_note_time = p_channel->timeAtChunk(p_channel->get_note_data()[l_j].startChunk());
                if(between(l_note_time, p_left_time, p_right_time))
                {
                    //FIXME: avgPitch is quite slow to calc
                    drawNoteAtPitch(toInt(p_left_X + (l_note_time - p_left_time) * m_scale_X)
                                   ,p_line_center_Y
                                   ,toInt(p_channel->get_note_data()[l_j].avgPitch())
                                   ,p_channel->get_note_data()[l_j].noteLength()
                                   ,p_channel->get_note_data()[l_j].volume()
                                   );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::paintEvent(QPaintEvent *)
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel * l_active_channel = GData::getUniqueInstance().getActiveChannel();

    beginDrawing();
    get_painter().setFont(m_font);
    get_painter().setBrush(Qt::black);

    ScoreSegmentIterator l_score_segment_iter(this, l_active_channel);
    if(l_active_channel)
    {
        std::string l_page_string{"Page "};
        l_page_string += std::to_string(l_score_segment_iter.curPage() + 1) + " of " + std::to_string(l_score_segment_iter.numPages());
        get_painter().drawText(m_font_width, height() - 4, QString::fromStdString(l_page_string));
        while(l_score_segment_iter.next())
        {
            drawScoreSegment(l_active_channel, l_score_segment_iter.leftX(), l_score_segment_iter.lineCenterY(), l_score_segment_iter.leftTime(), l_score_segment_iter.rightTime());
            if(l_score_segment_iter.contains(l_score_segment_iter.curTime()))
            {
                int l_pixel_X = toInt(l_score_segment_iter.leftX() + (l_score_segment_iter.curTime() - l_score_segment_iter.leftTime()) * m_scale_X);
                get_painter().setPen(QPen(Qt::black));
                get_painter().drawLine(l_pixel_X, l_score_segment_iter.staveTop(), l_pixel_X, l_score_segment_iter.staveBottom());
            }
        }
    }
    else
    {
        while(l_score_segment_iter.next())
        {
            drawStaveSegment(toInt(l_score_segment_iter.leftX()), l_score_segment_iter.lineCenterY(), toInt(l_score_segment_iter.widthX()));
        }
    }
    endDrawing();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: ScoreWidget::paintEvent()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
void ScoreWidget::mousePressEvent(QMouseEvent *p_event)
{
    m_mouse_down = true;
    Channel * l_active_channel = GData::getUniqueInstance().getActiveChannel();

    ScoreSegmentIterator l_score_segment_iterator(this, l_active_channel);
    if(l_active_channel)
    {
        while(l_score_segment_iterator.next())
        {
            if(between(p_event->x(), static_cast<int>(l_score_segment_iterator.leftX()), static_cast<int>(l_score_segment_iterator.leftX()+l_score_segment_iterator.widthX())) &&
	           between(p_event->y(), l_score_segment_iterator.staveTop(), l_score_segment_iterator.staveBottom()))
            {
                double l_t = l_score_segment_iterator.leftTime() + (static_cast<double>(p_event->x()) - l_score_segment_iterator.leftX()) / m_scale_X;

                //Find the last note played at time l_t
                int l_chunk = l_active_channel->chunkAtTime(l_t);
                AnalysisData * l_data = l_active_channel->dataAtChunk(l_chunk);
                while (l_data && l_data->getNoteIndex() < 0 && l_chunk > 0)
                {
                    l_data = l_active_channel->dataAtChunk(--l_chunk);
                }
                if(l_data && l_data->getNoteIndex() >= 0)
                {
                    int l_start_chunk = l_active_channel->get_note_data()[l_data->getNoteIndex()].startChunk();
                    GData::getUniqueInstance().updateActiveChunkTime(l_active_channel->timeAtChunk(l_start_chunk));
                    if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_STOP)
                    {
                        GData::getUniqueInstance().playSound(l_active_channel->getParent());
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void ScoreWidget::mouseMoveEvent(QMouseEvent *)
{
}

//------------------------------------------------------------------------------
void ScoreWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_mouse_down = false;
}
// EOF
