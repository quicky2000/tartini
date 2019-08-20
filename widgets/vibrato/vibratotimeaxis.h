/***************************************************************************
                          vibratotimeaxis.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOTIMEAXIS_H
#define VIBRATOTIMEAXIS_H

#include "drawwidget.h"

class VibratoTimeAxis: public DrawWidget
{
  Q_OBJECT

  public:
    VibratoTimeAxis(QWidget * p_parent, int p_nls);
    virtual ~VibratoTimeAxis(void);

    QSize sizeHint(void) const;

    void paintEvent(QPaintEvent *);
    void resizeEvent( QResizeEvent *);

 private:
    //data goes here
    /**
     * The horizontal space in pixels a note label requires
     */
    int m_note_label_offset;

    int m_start_chunk_to_use;

    int m_current_chunk_to_use;

    int m_end_chunk_to_use;

    double m_note_length_to_use;

    int m_window_offset_to_use;

    int m_prev_current_chunk;

    int m_prev_window_offset;

    double m_zoom_factor_X;

 public slots:
    void doUpdate(void);

    void setZoomFactorX(double p_x);
};
#endif // VIBRATOTIMEAXIS_H
// EOF
