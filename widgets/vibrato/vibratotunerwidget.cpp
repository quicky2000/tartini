/***************************************************************************
                          vibratotunerwidget.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "vibratotunerwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "musicnotes.h"
#include <glu.h>
#include <gl.h>

//------------------------------------------------------------------------------
VibratoTunerWidget::VibratoTunerWidget(QWidget *parent):
  QGLWidget(parent)
{
  m_needle_value_to_draw = -999;
  m_prev_needle_value = -999;
  m_prev_close_pitch = 0;
  m_cur_pitch = 0.0;
  m_tuner_label_counter = 0;
  for (int l_index = 0; l_index < 100; l_index++)
    {
      m_tuner_labels[l_index].set(QString(8, ' '), 0.0f, 0.0f);
    }
  m_tuner_font = QFont();
  m_tuner_font.setPointSize(9);
}

//------------------------------------------------------------------------------
VibratoTunerWidget::~VibratoTunerWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(m_dial, 1);
  glDeleteLists(m_needle, 1);
}

//------------------------------------------------------------------------------
void VibratoTunerWidget::initializeGL()
{
  QColor l_background = g_data->backgroundColor();
  glClearColor( double(l_background.red()) / 256.0, double(l_background.green()) / 256.0, double(l_background.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  m_dial = glGenLists(1);
  m_needle = glGenLists(1);

}

//------------------------------------------------------------------------------
void VibratoTunerWidget::resizeGL(int p_width
                                 ,int p_height
                                 )
{
  glViewport(0, 0, (GLint)p_width, (GLint)p_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, p_width, 0, p_height);

  // Calculate the dial with the new width & height
  const float l_half_width = 0.5 * width();
  const float l_half_height = 0.5 * height();
  const float l_radius = 1.8 * MAX(l_half_height, l_half_width);
  const float l_theta = asin(float(width()) / (2 * l_radius));
  const float l_rho = 0.5 * PI - l_theta;
  const float l_center_X = l_half_width;
  const float l_center_Y = height() - l_radius;

  glNewList(m_dial, GL_COMPILE);

  // Calculate the coordinates of the points on the upper arc
  float * l_upper_arc_X;
  float * l_upper_arc_Y;
  l_upper_arc_X = new float[width()];
  l_upper_arc_Y = new float[width()];
  float l_upper_arc_theta = -l_theta;
  float l_upper_arc_theta_step = (2 * l_theta) / width();
  for (int x = 0; x < width(); x++)
    {
      l_upper_arc_X[x] = x;
      l_upper_arc_Y[x] = l_center_Y + (sin(0.5 * PI - l_upper_arc_theta) * l_radius);
      l_upper_arc_theta += l_upper_arc_theta_step;
    }

  // Calculate the white semicircle of the dial
  glBegin(GL_TRIANGLE_FAN);
  glColor3ub(255, 255, 255);
  glVertex2f(l_center_X, l_center_Y);
  for (int l_x = 0; l_x < width(); l_x++)
    {
      glVertex2f(l_upper_arc_X[l_x], l_upper_arc_Y[l_x]);
    }
  glEnd();

  // Calculate the black outline of the dial
  glBegin(GL_LINE_LOOP);
  glColor3ub(0, 0, 0);
  glVertex2f(l_center_X, l_center_Y);
  for (int l_x = 0; l_x < width(); l_x++)
    {
      glVertex2f(l_upper_arc_X[l_x], l_upper_arc_Y[l_x]);
    }
  glEnd();

  // Calculate the black arc inside the dial
  glBegin(GL_LINE_STRIP);
  glColor3ub(0, 0, 0);
  float l_lower_arc_theta = -l_theta;
  float l_lower_arc_theta_step = (2 * l_theta) / l_half_width;
  for (float l_x = 0.25 * width(); l_x < 0.75 * width(); l_x++)
    {
      glVertex2f(l_x, l_center_Y + (0.5 * sin(0.5 * PI - l_lower_arc_theta) * l_radius));
      l_lower_arc_theta += l_lower_arc_theta_step;
    }
  glEnd();

  // Calculate line markings & text labels
  float l_step = (2 * l_theta) / 12.0;
  m_tuner_label_counter = 0;
  char * l_tuner_label_lookup[11] =
    {
      (char*)"+50   ",
      (char*)"+40",
      (char*)"+30",
      (char*)"+20",
      (char*)"+10",
      (char*)"0",
      (char*)"-10",
      (char*)"-20",
      (char*)"-30",
      (char*)"-40",
      (char*)"   -50"
    };

  for (int l_j = 0; l_j < 11; l_j++)
    {
      float l_i = l_rho + l_step * (l_j + 1);
      float l_start_X = l_center_X + l_radius * cos(l_i);
      float l_start_Y = l_center_Y + l_radius * sin(l_i);
      if (width() < 350)
	{
	  // Small dial
	  if ((l_j == 0) || (l_j == 3) || (l_j == 5) || (l_j == 7) || (l_j == 10))
	    {
	      // Bigger marking + text label
	      glBegin(GL_LINES);
	      glColor3ub(0, 0, 0);
	      glVertex2f(l_start_X, l_start_Y);
	      glVertex2f(l_start_X + 0.05 * (l_center_X - l_start_X), l_start_Y + 0.05 * (l_center_Y - l_start_Y));
	      glEnd();

	      m_tuner_labels[m_tuner_label_counter++].set(l_tuner_label_lookup[l_j]
	                                              , l_start_X + 0.08 * (l_center_X - l_start_X)
	                                              , l_start_Y + 0.08 * (l_center_Y - l_start_Y)
                                                     );
	    }
	  else
	    {
	      // Smaller marking, no text label
	      glBegin(GL_LINES);
	      glColor3ub(0, 0, 0);
	      glVertex2f(l_start_X, l_start_Y);
	      glVertex2f(l_start_X + 0.03 * (l_center_X - l_start_X), l_start_Y + 0.03 * (l_center_Y - l_start_Y));
	      glEnd();
	    }
	}
      else
	{
	  // Big dial
	  // Bigger marking + text label
	  l_tuner_label_lookup[0] = (char*)"+50";
	  l_tuner_label_lookup[10] = (char*)"-50";

	  glBegin(GL_LINES);
	  glColor3ub(0, 0, 0);
	  glVertex2f(l_start_X, l_start_Y);
	  glVertex2f(l_start_X + 0.05 * (l_center_X - l_start_X), l_start_Y + 0.05 * (l_center_Y - l_start_Y));
	  glEnd();

	  m_tuner_labels[m_tuner_label_counter++].set(l_tuner_label_lookup[l_j]
	                                          , l_start_X + 0.08 * (l_center_X - l_start_X)
	                                          , l_start_Y + 0.08 * (l_center_Y - l_start_Y)
                                                 );
	}
    }

  // Calculate the position of the "Cents" label
  m_cents_label_X = l_center_X;
  m_cents_label_Y = l_center_Y + 0.7 * l_radius;

  glEndList();

  delete[] l_upper_arc_X;
  delete[] l_upper_arc_Y;

  // Do forced update on resize
  m_needle_value_to_draw = -999;
  m_prev_needle_value = -999;
  m_prev_close_pitch = -1;
  doUpdate(m_cur_pitch);
}

//------------------------------------------------------------------------------
void VibratoTunerWidget::paintGL()
{
  QColor l_background = g_data->backgroundColor();
  glClearColor( double(l_background.red()) / 256.0, double(l_background.green()) / 256.0, double(l_background.blue()) / 256.0, 0.0 );
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the dial
  glLineWidth(1.5);
  glCallList(m_dial);

  // Draw the labels
  QFontMetrics l_font_metric = QFontMetrics(m_tuner_font);

  glColor3ub(0,0,0);
  renderText(m_cents_label_X - (0.5 * l_font_metric.width("Cents")), m_cents_label_Y, 0, "Cents", m_tuner_font);
  for (int l_index = 0; l_index < m_tuner_label_counter; l_index++)
    {
      renderText(m_tuner_labels[l_index].get_x() - (0.5 * l_font_metric.width(m_tuner_labels[l_index].get_label())), m_tuner_labels[l_index].get_y() - 8, 0, m_tuner_labels[l_index].get_label(), m_tuner_font);
    }

  // Draw the needle
  glLineWidth(1.0);
  glCallList(m_needle);

}

//------------------------------------------------------------------------------
void VibratoTunerWidget::doUpdate(double p_pitch)
{
  m_cur_pitch = p_pitch;

  Channel * l_active_channel = g_data->getActiveChannel();

  float l_needle_value = 0;
  int l_close_pitch = 0;
  bool l_is_pitch_black_note = true;

  if (l_active_channel)
    {
      AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
      if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
	{
	}
    }
  
  l_close_pitch = toInt(p_pitch);
  l_is_pitch_black_note = isBlackNote(l_close_pitch);
  l_needle_value = 100 * (p_pitch - float(l_close_pitch));

  if ((fabs(m_prev_needle_value - l_needle_value) < 0.05) && (m_prev_close_pitch == l_close_pitch))
    {
      // Pitch hasn't changed (much), no update needed    
    }
  else
    {
      // Pitch has changed
      makeCurrent();

      glNewList(m_needle, GL_COMPILE);
      glEndList();

      if (l_close_pitch == 0)
	{
	  // No pitch, don't draw the needle this update
	  m_prev_needle_value = -999;
	  m_prev_close_pitch = 0;
	  m_needle_value_to_draw = -999;
	  resetLeds();
	  updateGL();
	}
      else
	{
	  // Pitch, draw the needle this update
	  int l_VTLED_letter_lookup[12] = { 2, 2, 3, 3, 4, 5, 5, 6, 6, 0, 0, 1 };
	  resetLeds();
	  emit(ledSet(l_VTLED_letter_lookup[noteValue(l_close_pitch)], true));
	  if(isBlackNote(l_close_pitch))
	    {
	      emit(ledSet(7, true));
	    }

	  m_prev_needle_value = l_needle_value;
	  m_prev_close_pitch = l_close_pitch;
	  m_prev_is_pitch_black_note = l_is_pitch_black_note;

	  m_needle_value_to_draw = l_needle_value;

	  // Calculate the needle, if m_needle_value_to_draw is set to sensible value
	  if((m_needle_value_to_draw > -60.0) && (m_needle_value_to_draw < 60.0))
	    {
	      const float l_half_width = 0.5 * width();
	      const float l_half_height = 0.5 * height();
	      const float l_radius = 1.8 * MAX(l_half_height, l_half_width);
	      const float l_theta = asin(float(width()) / (2 * l_radius));
	      const float l_rho = (PI * 0.5) - l_theta;
	      const float l_center_X = l_half_width;
	      const float l_center_Y = height() - l_radius;

	      float l_cent_angle = (2 * l_theta) / 120;
	      float l_note = l_rho + (fabs(m_needle_value_to_draw - 60) * l_cent_angle);
	      int l_half_knob_width = MAX(toInt(l_radius * 0.02), 1);

	      float l_note_X = l_center_X + l_radius * cos(l_note);
	      float l_note_Y = l_center_Y + l_radius * sin(l_note);
	      float l_knob_left_X = l_center_X - l_half_knob_width;
	      float l_knob_left_Y = l_center_Y;
	      float l_knob_right_X = l_center_X + l_half_knob_width;
	      float l_knob_right_Y = l_center_Y;

	      glNewList(m_needle, GL_COMPILE);

	      // Inside of the needle
	      glBegin(GL_TRIANGLES);
	      glColor3ub(255, 0, 0);
	      glVertex2f(l_note_X, l_note_Y);
	      glVertex2f(l_knob_left_X, l_knob_left_Y);
	      glVertex2f(l_knob_right_X, l_knob_right_Y);
	      glEnd();

	      // Outside of the needle
	      glBegin(GL_LINE_LOOP);
	      glColor3ub(127, 0, 0);
	      glVertex2f(l_note_X, l_note_Y);
	      glVertex2f(l_knob_left_X, l_knob_left_Y);
	      glVertex2f(l_knob_right_X, l_knob_right_Y);
	      glEnd();

	      glEndList();
	    }
	  updateGL();
	}
    }
}

//------------------------------------------------------------------------------
void VibratoTunerWidget::resetLeds()
{
  for (int l_index = 0; l_index < 8; l_index++)
    {
      emit(ledSet(l_index, false));
    }
}

//------------------------------------------------------------------------------
QSize VibratoTunerWidget::minimumSizeHint() const
{
  return QSize(100, 75);
}

//------------------------------------------------------------------------------
void
VibratoTunerWidget::tunerLabelStruct::set( const QString & p_label
                                         , float p_x
                                         , float p_y
                                         )
{
    m_label = p_label;
    m_x = p_x;
    m_y = p_y;
}

//------------------------------------------------------------------------------
const QString &
VibratoTunerWidget::tunerLabelStruct::get_label() const
{
    return m_label;
}

//------------------------------------------------------------------------------
float
VibratoTunerWidget::tunerLabelStruct::get_x() const
{
    return m_x;
}

//------------------------------------------------------------------------------
float
VibratoTunerWidget::tunerLabelStruct::get_y() const
{
    return m_y;
}

// EOF
