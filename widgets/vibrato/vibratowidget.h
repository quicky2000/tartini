/***************************************************************************
                          vibratowidget.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOWIDGET_H
#define VIBRATOWIDGET_H

#include <QGLWidget>

class VibratoWidget :public QGLWidget
{
  Q_OBJECT

 public:
  VibratoWidget(QWidget * parent, int nls);
  virtual ~VibratoWidget(void);

  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL(void);

  QSize sizeHint(void) const;

 public slots:
  void doUpdate(void);
  void setZoomFactorX(double x);
  void setZoomFactorY(double y);
  void setOffsetY(int value);

 private:
  static void compose_note_label(QString & p_note_label, const int & p_note);

  /**
     The horizontal space in pixels a note label requires
  */
  int m_note_label_offset;
  double m_zoom_factor_X;
  double m_zoom_factor_Y;
  int m_offset_Y;

  QFont m_vibrato_font;

  GLuint m_vertical_period_bars;
  GLuint m_vertical_separator_lines;
  GLuint m_reference_lines;
  GLuint m_prony_width_band;
  GLuint m_prony_average_pitch;
  GLuint m_vibrato_polyline;
  GLuint m_current_window_band;
  GLuint m_current_time_line;
  GLuint m_maxima_minima_points;

  int m_note_label_counter;
  class noteLabelStruct
  {
    public:
      void set( const QString & p_label
              , float p_y
              );
      float get_y() const;
      const QString & get_label() const;

    private:

      QString m_label;
      float m_y;
  };
  noteLabelStruct m_note_labels[100];

};

#endif // VIBRATOWIDGET_H
// EOF
