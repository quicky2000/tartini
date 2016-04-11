/***************************************************************************
                          scorewidget.h  -  description
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
#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include "drawwidget.h"
#include <QFont>
#include <QMouseEvent>
#include <QPaintEvent>


class ScoreWidget : public DrawWidget
{
  Q_OBJECT

    enum StemType
            { StemNone
            , StemUp
            , StemDown
            };

    enum FillType
            { FilledNote
            , EmptyNote
            };

    enum NoteType
            { DemiSemiQuaver
            , SemiQuaver
            , Quaver
            , Crotchet
            , Minum
            , SemiBreve
            };

  public:
    ScoreWidget(QWidget * p_parent);
    virtual ~ScoreWidget(void);

    /**
     * Draw a crotchet at x, y
     * @param p_x The lefthand-side x pixel
     * @param p_y The center y pixel
     * @param stemUp true if the stem is to be drawn upwards, else its drawn downwards
     */
    void drawNote( int p_x
                 , int p_y
                 , StemType p_step_type
                 , FillType p_fill_type
                 , int p_num_flicks
                 );

    /**
     * Draw a single note
     * @param p_x The x pos in pixels
     * @param p_y The y pos of the center of the scale (Middle C)
     * @param p_pitch The midi number of the note (60 == Middle C)
     * @param p_note_length in seconds
     */
    void drawNoteAtPitch( int p_x
                        , int p_y
                        , int p_pitch
                        , double p_note_length
                        , double p_volume = 1.0
                        );

    /** Draws a segment of the musical score
     * @param p_channel The channel to get the notes from.
     * @param p_left_X In pixels (even though a doule)
     * @param p_line_center_Y In pixels
     * @param p_left_time The time is seconds at which is at the lefthand side of the scoreSegment to draw
     */
    void drawScoreSegment( Channel * p_channel
                         , double p_left_X
                         , int p_line_center_Y
                         , double p_left_time
                         , double p_right_time
                         );

    /** Draw a segment of stave lines
     * @param p_left_X The pixel at the left of the segment
     * @param p_line_center_Y The y pixel to put middle C on
     * @param p_width_X The width of the stave segment
     */
    void drawStaveSegment( int p_left_X
                         , int p_line_center_Y
                         , int p_width_X
                         );

    void paintEvent(QPaintEvent * );
    inline int getStaveHeight(void);
    inline int getStaveCenterY(void);
    inline QSize sizeHint(void) const;
    NoteType getNoteType(double p_note_length);

    inline double scaleX(void);
    inline double scaleY(void);

  public slots:
    inline void setScaleX(double p_x);
    inline void setScaleY(double p_y);
    inline void setSharpsMode(int p_mode);
    inline void setNotesMode(int p_mode);
    inline void setClefMode(int p_mode);
    inline void setOpaqueMode(int p_mode);
    inline void setTransposeLevel(int p_index);
    inline void setShowAllMode(int p_mode);

  private:
    void mousePressEvent(QMouseEvent * p_event);
    void mouseMoveEvent(QMouseEvent * p_event);
    void mouseReleaseEvent(QMouseEvent * p_event);


    //data goes here
    //num. pixels between lines
    double m_scale_Y;
    //num. pixels per second
    double m_scale_X;
    double m_boarder_X;
    double m_boarder_Y;
    bool m_show_base_clef;
    bool m_show_notes;
    bool m_show_opaque_notes;
    bool m_use_flats;
    bool m_show_all_mode;
    int m_pitch_offset;
    int m_sharps_lookup[129];
    int m_flats_lookup[129];
    int m_font_height;
    int m_font_width;
    QFont m_font;
    //in fractions of a page
    double m_look_ahead;
    //in fractions of a page
    double m_look_ahead_gap;
    bool m_mouse_down;

    friend class ScoreSegmentIterator;
};

#include "scorewidget.hpp"

#endif // SCOREWIDGET_H
// EOF
