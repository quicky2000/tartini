/***************************************************************************
                          vibratowidget.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/
#include <QPainter>

#include "vibratowidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "myglfonts.h"
#include "musicnotes.h"
#include <glu.h>
#include <gl.h>
#include <sstream>

//------------------------------------------------------------------------------
VibratoWidget::VibratoWidget(QWidget * p_parent, int p_nls):
  QGLWidget(p_parent)
{
  // The horizontal space in pixels a note label requires
  m_note_label_offset = p_nls;
  m_zoom_factor_X = 2.0;
  m_zoom_factor_Y = 1.0;
  m_offset_Y = 0;
  m_note_label_counter = 0;
  for(int l_index = 0; l_index < 100; l_index++)
    {
      m_note_labels[l_index].set(QString(8,' '), 0.0f);
    }
  m_vibrato_font = QFont();
  m_vibrato_font.setPointSize(9);
}

//------------------------------------------------------------------------------
VibratoWidget::~VibratoWidget()
{
  // Remove display lists
  makeCurrent();

  glDeleteLists(m_vertical_period_bars, 1);
  glDeleteLists(m_vertical_separator_lines, 1);
  glDeleteLists(m_reference_lines, 1);
  glDeleteLists(m_prony_width_band, 1);
  glDeleteLists(m_prony_average_pitch, 1);
  glDeleteLists(m_vibrato_polyline, 1);
  glDeleteLists(m_current_window_band, 1);
  glDeleteLists(m_current_time_line, 1);
  glDeleteLists(m_maxima_minima_points, 1);
}

//------------------------------------------------------------------------------
void VibratoWidget::initializeGL()
{
  qglClearColor(GData::getUniqueInstance().backgroundColor());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_vertical_period_bars = glGenLists(1);
  m_vertical_separator_lines = glGenLists(1);
  m_reference_lines = glGenLists(1);
  m_prony_width_band = glGenLists(1);
  m_prony_average_pitch = glGenLists(1);
  m_vibrato_polyline = glGenLists(1);
  m_current_window_band = glGenLists(1);
  m_current_time_line = glGenLists(1);
  m_maxima_minima_points = glGenLists(1);
}

//------------------------------------------------------------------------------
void VibratoWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, static_cast<GLint>(w), static_cast<GLint>(h));

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);

  update();
}

//------------------------------------------------------------------------------
void VibratoWidget::paintGL()
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

  doUpdate();

  // Clear background
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertical bars that indicate the vibrato periods
  glCallList(m_vertical_period_bars);

  // Draw the vertical separator lines through the extrema
  glDisable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(m_vertical_separator_lines);

  // Draw the horizontal reference lines
  glDisable(GL_LINE_SMOOTH);
  glEnable(GL_LINE_STIPPLE);
  glLineWidth(1.0);
  glLineStipple(1, 64716);  // bitpattern 64716 = 1111110011001100
  glCallList(m_reference_lines);
  glDisable(GL_LINE_STIPPLE);

  // Draw the light grey band indicating the vibratowidth according to the Prony-algorithm
  glCallList(m_prony_width_band);

  // Draw the average pitch according to the Prony-algorithm
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(m_prony_average_pitch);

  // Draw the vibrato-polyline
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(2.0);
  glCallList(m_vibrato_polyline);

  // Draw the light grey band indicating which time is being used in the current window
  glCallList(m_current_window_band);

  // Draw the current timeline
  glDisable(GL_LINE_SMOOTH);
  glLineWidth(1.0);
  glCallList(m_current_time_line);

  // Draw the maxima & minima
  glEnable(GL_POINT_SMOOTH);
  glPointSize(3.0);
  glCallList(m_maxima_minima_points);

  // Draw the note labels
  g_mygl_font->beginGLtext(width(), height());
  glColor3ub(0,0,0);
  for(int l_index = 0; l_index < m_note_label_counter; l_index++)
    {
      g_mygl_font->drawGLtextRaw(3, m_note_labels[l_index].get_y() - 4, m_note_labels[l_index].get_label());
      g_mygl_font->drawGLtextRaw(width() - m_note_label_offset + 3, m_note_labels[l_index].get_y() - 4, m_note_labels[l_index].get_label());
    }
  g_mygl_font->endGLtext();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: VibratoWidget::paintGL()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
void VibratoWidget::doUpdate()
{
  m_note_label_counter = 0;

  makeCurrent();

  glNewList(m_vertical_period_bars, GL_COMPILE);
  glEndList();

  glNewList(m_vertical_separator_lines, GL_COMPILE);
  glEndList();

  glNewList(m_reference_lines, GL_COMPILE);
  glEndList();

  glNewList(m_prony_width_band, GL_COMPILE);
  glEndList();

  glNewList(m_prony_average_pitch, GL_COMPILE);
  glEndList();

  glNewList(m_vibrato_polyline, GL_COMPILE);
  glEndList();

  glNewList(m_current_window_band, GL_COMPILE);
  glEndList();

  glNewList(m_current_time_line, GL_COMPILE);
  glEndList();

  glNewList(m_maxima_minima_points, GL_COMPILE);
  glEndList();

  Channel * l_active = GData::getUniqueInstance().getActiveChannel();

  if (l_active)
    {
      ChannelLocker l_channelLocker(l_active);
      AnalysisData * l_data = l_active->dataAtCurrentChunk();
      if(l_data && l_active->isVisibleNote(l_data->getNoteIndex()) && l_active->isLabelNote(l_data->getNoteIndex()))
	{
	  const NoteData * l_note = &(l_active->get_note_data()[l_data->getNoteIndex()]);

	  const int l_my_start_chunk = l_note->startChunk();
	  const int l_my_end_chunk = l_note->endChunk();
	  const int l_my_current_chunk = l_active->chunkAtCurrentTime();
	  const float l_half_height = 0.5 * height();
	  const int l_maxima_size = l_note->get_maxima()->size();
	  const int l_minima_size = l_note->get_minima()->size();
	  const float l_avg_pitch = l_note->avgPitch();
	  const int l_frames_per_chunk = l_active->framesPerChunk();
	  const float l_zoom_factor_Y_x_100 = m_zoom_factor_Y * 100;

	  float l_window_offset;
	  large_vector<float> l_pitch_lookup_used = l_active->get_pitch_lookup_smoothed();
	  int l_smooth_delay = l_active->get_pitch_big_smoothing_filter().delay();

	  if ((l_my_end_chunk - l_my_start_chunk) * m_zoom_factor_X > width() - 2 * m_note_label_offset)
	    {
	      // The vibrato-polyline doesn't fit in the window
	      if ((l_my_current_chunk - l_my_start_chunk) * m_zoom_factor_X < (width() - 2 * m_note_label_offset) / 2)
		{
		  // We're at the left side of the vibrato-polyline
		  l_window_offset = 0 - m_note_label_offset;
		}
	      else if ((l_my_end_chunk - l_my_current_chunk) * m_zoom_factor_X < (width() - 2 * m_note_label_offset) / 2)
		{
		  // We're at the right side of the vibrato-polyline
		  l_window_offset = (l_my_end_chunk - l_my_start_chunk) * m_zoom_factor_X - width() + m_note_label_offset + 1;
		}
	      else
		{
		  // We're somewhere in the middle of the vibrato-polyline
		  l_window_offset = (l_my_current_chunk - l_my_start_chunk) * m_zoom_factor_X - width() / 2;
		}
	    }
	  else
	    {
	      // The vibrato-polyline does fit in the window
	      l_window_offset = 0 - m_note_label_offset;
	    }

	  GLfloat * l_vertices;
	  GLubyte * l_colors;
	  unsigned int l_vertices_counter = 0;
	  unsigned int l_colors_counter = 0;

	  glEnableClientState(GL_VERTEX_ARRAY);
	  glEnableClientState(GL_COLOR_ARRAY);

	  // Calculate the alternating vertical bars that indicate the vibrato periods
	  if ((l_active->doingDetailedPitch()) && (l_active->get_pitch_lookup_smoothed().size() > 0))
	    {
	      // No. of bars with the left side at a maximum
	      int l_color1_bars = 0;
	      // No. of bars with the left side at a minimum
	      int l_color2_bars = 0;
	      // The first extremum is a maximum?
	      bool l_maximum_first = true;

	      if (l_maxima_size + l_minima_size >= 2)
		{
		  // There is at least one bar to calculate
		  if (l_maxima_size == l_minima_size)
		    {
		      if (l_note->get_maxima()->at(0) < l_note->get_minima()->at(0))
			{
			  l_color1_bars = l_maxima_size;
			  l_color2_bars = l_minima_size - 1;
			  l_maximum_first = true;
			}
		      else
			{
			  l_color1_bars = l_maxima_size - 1;
			  l_color2_bars = l_minima_size;
			  l_maximum_first = false;
			}
		    }
		  else
		    {
		      l_color1_bars = l_maxima_size > l_minima_size ? l_minima_size : l_maxima_size;
		      l_color2_bars = l_maxima_size > l_minima_size ? l_minima_size : l_maxima_size;
		      l_maximum_first = l_maxima_size > l_minima_size;
		    }
		}

	      float l_x1, l_x2;

	      // Calculate the bars with the left side at a maximum
	      l_vertices = new GLfloat[(l_color1_bars + l_color2_bars) * 8];
	      l_colors = new GLubyte[(l_color1_bars + l_color2_bars) * 12];

	      const char l_color1_red = GData::getUniqueInstance().shading1Color().red();
	      const char l_color1_green = GData::getUniqueInstance().shading1Color().green();
	      const char l_color1_blue = GData::getUniqueInstance().shading1Color().blue();

	      l_vertices_counter = 0;
	      l_colors_counter = 0;

	      for(int l_index = 0; l_index < l_color1_bars; l_index++)
		{
		  l_x1 = (((static_cast<float>(l_note->get_maxima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x1 < m_note_label_offset)
		    {
		      l_x1 = m_note_label_offset;
		    }
		  if (l_x1 > width() - m_note_label_offset)
		    {
		      break;
		    }
		  if (l_maximum_first)
		    {
		      l_x2 = (((static_cast<float>(l_note->get_minima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		    }
		  else
		    {
		      l_x2 = (((static_cast<float>(l_note->get_minima()->at(l_index + 1)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		    }
		  if (l_x2 < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x2 > width() - m_note_label_offset)
		    {
		      l_x2 = width() - m_note_label_offset;
		    }

		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x2;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x2;
		  l_vertices[l_vertices_counter++] = height();
		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = height();

		  for(int l_j = 1; l_j <= 4; l_j++)
		    {
		      l_colors[l_colors_counter++] = l_color1_red;
		      l_colors[l_colors_counter++] = l_color1_green;
		      l_colors[l_colors_counter++] = l_color1_blue;
		    }
		}

	      // Calculate the bars with the left side at a minimum
	      const char l_color2_red = GData::getUniqueInstance().shading2Color().red();
	      const char l_color2_green = GData::getUniqueInstance().shading2Color().green();
	      const char l_color2_blue = GData::getUniqueInstance().shading2Color().blue();

	      for(int l_index = 0; l_index < l_color2_bars; l_index++)
		{
		  l_x1 = (((static_cast<float>(l_note->get_minima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x1 < m_note_label_offset)
		    {
		      l_x1 = m_note_label_offset;
		    }
		  if (l_x1 > width() - m_note_label_offset)
		    {
		      break;
		    }
		  if (l_maximum_first)
		    {
		      l_x2 = (((static_cast<float>(l_note->get_maxima()->at(l_index + 1)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		    }
		  else
		    {
		      l_x2 = (((static_cast<float>(l_note->get_maxima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		    }
		  if (l_x2 < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x2 > width() - m_note_label_offset)
		    {
		      l_x2 = width() - m_note_label_offset;
		    }

		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x2;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x2;
		  l_vertices[l_vertices_counter++] = height();
		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = height();

		  for(int l_j = 1; l_j <= 4; l_j++)
		    {
		      l_colors[l_colors_counter++] = l_color2_red;
		      l_colors[l_colors_counter++] = l_color2_green;
		      l_colors[l_colors_counter++] = l_color2_blue;
		    }
		}

	      glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	      glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	      glNewList(m_vertical_period_bars, GL_COMPILE);
	      glDrawArrays(GL_QUADS, 0, l_vertices_counter / 2);
	      glEndList();

	      delete[] l_vertices;
	      delete[] l_colors;


	      // Calculate the vertical separator lines through the maxima
	      l_vertices = new GLfloat[(l_maxima_size + l_minima_size) * 4];
	      l_colors = new GLubyte[(l_maxima_size + l_minima_size) * 6];

	      l_vertices_counter = 0;
	      l_colors_counter = 0;

	      for(int l_index = 0; l_index < l_maxima_size; l_index++)
		{
		  l_x1 = (((static_cast<float>(l_note->get_maxima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x1 < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x1 > width() - m_note_label_offset)
		    {
		      break;
		    }

		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = height();

		  l_colors[l_colors_counter++] = 131;
		  l_colors[l_colors_counter++] = 144;
		  l_colors[l_colors_counter++] = 159;
		  l_colors[l_colors_counter++] = 131;
		  l_colors[l_colors_counter++] = 144;
		  l_colors[l_colors_counter++] = 159;
		}

	      // Calculate the vertical separator lines through the minima
	      for(int l_index = 0; l_index < l_minima_size; l_index++)
		{
		  l_x1 = (((static_cast<float>(l_note->get_minima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x1 < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x1 > width() - m_note_label_offset)
		    {
		      break;
		    }

		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = 0;
		  l_vertices[l_vertices_counter++] = l_x1;
		  l_vertices[l_vertices_counter++] = height();

		  l_colors[l_colors_counter++] = 131;
		  l_colors[l_colors_counter++] = 144;
		  l_colors[l_colors_counter++] = 159;
		  l_colors[l_colors_counter++] = 131;
		  l_colors[l_colors_counter++] = 144;
		  l_colors[l_colors_counter++] = 159;
		}

	      glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	      glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	      glNewList(m_vertical_separator_lines, GL_COMPILE);
	      glDrawArrays(GL_LINES, 0, l_vertices_counter / 2);
	      glEndList();

	      delete[] l_vertices;
	      delete[] l_colors;
	    }


	  // Calculate the horizontal reference lines + note labels
	  l_vertices = new GLfloat[100 * 4];
	  l_colors = new GLubyte[100 * 6];

	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  const int l_nearest_note = toInt(l_avg_pitch);
	  QString l_note_label;
	  const float l_reference_line_X1 = m_note_label_offset;
	  const float l_reference_line_X2 = width() - m_note_label_offset;
	  float l_reference_line_Y;

	  // Calculate the nearest reference line + note label
	  l_reference_line_Y = l_half_height + ((l_nearest_note - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

	  l_vertices[l_vertices_counter++] = l_reference_line_X1;
	  l_vertices[l_vertices_counter++] = l_reference_line_Y;
	  l_vertices[l_vertices_counter++] = l_reference_line_X2;
	  l_vertices[l_vertices_counter++] = l_reference_line_Y;

	  l_colors[l_colors_counter++] = 144;
	  l_colors[l_colors_counter++] = 156;
	  l_colors[l_colors_counter++] = 170;
	  l_colors[l_colors_counter++] = 144;
	  l_colors[l_colors_counter++] = 156;
	  l_colors[l_colors_counter++] = 170;

	  compose_note_label(l_note_label, l_nearest_note);
	  m_note_labels[m_note_label_counter].set( l_note_label, l_reference_line_Y);
	  m_note_label_counter++;

	  // Calculate as many reference lines + note labels above the note as can be seen
	  for(int l_index = 1; ; l_index++)
	    {
	      l_reference_line_Y = l_half_height + ((l_nearest_note + l_index - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;
	      if (l_reference_line_Y > height())
		{
		  break;
		}

	      l_vertices[l_vertices_counter++] = l_reference_line_X1;
	      l_vertices[l_vertices_counter++] = l_reference_line_Y;
	      l_vertices[l_vertices_counter++] = l_reference_line_X2;
	      l_vertices[l_vertices_counter++] = l_reference_line_Y;

	      l_colors[l_colors_counter++] = 144;
	      l_colors[l_colors_counter++] = 156;
	      l_colors[l_colors_counter++] = 170;
	      l_colors[l_colors_counter++] = 144;
	      l_colors[l_colors_counter++] = 156;
	      l_colors[l_colors_counter++] = 170;
	
	      compose_note_label(l_note_label, l_nearest_note + l_index);
	      m_note_labels[m_note_label_counter].set( l_note_label, l_reference_line_Y);
	      m_note_label_counter++;
	    }

	  // Calculate as many reference lines + note labels below the note as can be seen
	  for(int l_index = -1; ; l_index--)
	    {
	      l_reference_line_Y = l_half_height + ((l_nearest_note + l_index - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;
	      if (l_reference_line_Y < 0)
		{
		  break;
		}

	      l_vertices[l_vertices_counter++] = l_reference_line_X1;
	      l_vertices[l_vertices_counter++] = l_reference_line_Y;
	      l_vertices[l_vertices_counter++] = l_reference_line_X2;
	      l_vertices[l_vertices_counter++] = l_reference_line_Y;

	      l_colors[l_colors_counter++] = 144;
	      l_colors[l_colors_counter++] = 156;
	      l_colors[l_colors_counter++] = 170;
	      l_colors[l_colors_counter++] = 144;
	      l_colors[l_colors_counter++] = 156;
	      l_colors[l_colors_counter++] = 170;

	      compose_note_label(l_note_label, l_nearest_note + l_index);
	      m_note_labels[m_note_label_counter].set( l_note_label, l_reference_line_Y);
	      m_note_label_counter++;
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_reference_lines, GL_COMPILE);
	  glDrawArrays(GL_LINES, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;

	  // Calculate the light grey band indicating the vibratowidth according to the Prony-algorithm
	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  l_vertices = new GLfloat[(l_my_end_chunk - l_my_start_chunk) * 8];
	  l_colors = new GLubyte[(l_my_end_chunk - l_my_start_chunk) * 16];

	  for(int l_chunk = l_my_start_chunk; l_chunk < l_my_end_chunk - 1; l_chunk++)
	    {
	      float l_x1 = (l_chunk - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
	      if (l_x1 < m_note_label_offset)
		{
		  l_x1 = m_note_label_offset;
		}
	      if (l_x1 > width() - m_note_label_offset)
		{
		  break;
		}
	      float l_x2 = (l_chunk + 1 - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
	      if (l_x2 < m_note_label_offset)
		{
		  continue;
		}
	      if (l_x2 > width() - m_note_label_offset)
		{
		  l_x2 = width() - m_note_label_offset;
		}

	      float l_vibrato_pitch1 = l_active->dataAtChunk(l_chunk)->getVibratoPitch();
	      float l_vibrato_width1 = l_active->dataAtChunk(l_chunk)->getVibratoWidth();
	      float l_vibrato_pitch2 = l_active->dataAtChunk(l_chunk + 1)->getVibratoPitch();
	      float l_vibrato_width2 = l_active->dataAtChunk(l_chunk + 1)->getVibratoWidth();
	      float l_y1 = l_half_height + ((l_vibrato_pitch1 + l_vibrato_width1 - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;
	      float l_y2 = l_half_height + ((l_vibrato_pitch1 - l_vibrato_width1 - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;
	      float l_y3 = l_half_height + ((l_vibrato_pitch2 - l_vibrato_width2 - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;
	      float l_y4 = l_half_height + ((l_vibrato_pitch2 + l_vibrato_width2 - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

	      l_vertices[l_vertices_counter++] = l_x1;
	      l_vertices[l_vertices_counter++] = l_y1;
	      l_vertices[l_vertices_counter++] = l_x1;
	      l_vertices[l_vertices_counter++] = l_y2;
	      l_vertices[l_vertices_counter++] = l_x2;
	      l_vertices[l_vertices_counter++] = l_y3;
	      l_vertices[l_vertices_counter++] = l_x2;
	      l_vertices[l_vertices_counter++] = l_y4;

	      for(int l_j = 1; l_j <= 4; l_j++)
		{
		  l_colors[l_colors_counter++] = 0;
		  l_colors[l_colors_counter++] = 0;
		  l_colors[l_colors_counter++] = 0;
		  l_colors[l_colors_counter++] = 64;
		}
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_prony_width_band, GL_COMPILE);
	  glDrawArrays(GL_QUADS, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;

	  // Calculate the average pitch according to the Prony-algorithm
	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  l_vertices = new GLfloat[(l_my_end_chunk - l_my_start_chunk) * 2];
	  l_colors = new GLubyte[(l_my_end_chunk - l_my_start_chunk) * 4];

	  for(int l_chunk = l_my_start_chunk; l_chunk < l_my_end_chunk; l_chunk++)
	    {
	      float l_x = (l_chunk - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
	      if (l_x < m_note_label_offset)
		{
		  continue;
		}
	      if (l_x > width() - m_note_label_offset)
		{
		  break;
		}
	      float l_y = l_half_height + ((l_active->dataAtChunk(l_chunk)->getVibratoPitch() - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

	      l_vertices[l_vertices_counter++] = l_x;
	      l_vertices[l_vertices_counter++] = l_y;

	      l_colors[l_colors_counter++] = 0;
	      l_colors[l_colors_counter++] = 0;
	      l_colors[l_colors_counter++] = 0;
	      l_colors[l_colors_counter++] = 127;
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_prony_average_pitch, GL_COMPILE);
	  glDrawArrays(GL_LINE_STRIP, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;

	  // Calculate the vibrato-polyline
	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  if ((l_active->doingDetailedPitch()) && (l_pitch_lookup_used.size() > 0))
	    {
	      // Detailed pitch information available, calculate polyline using this info
	      l_vertices = new GLfloat[(width() + 1) * 2];
	      l_colors = new GLubyte[(width() + 1) * 3];

	      const int l_pitch_lookup_used_size_limit = l_pitch_lookup_used.size() - 1;
	      const int l_beginning_of_note = l_my_start_chunk * l_frames_per_chunk;
	      const int l_end_of_note = l_my_end_chunk * l_frames_per_chunk - 1;
	      float l_chunk;
	      float l_y;
	      int l_offset;
	      for(int l_x = m_note_label_offset; l_x < width() - m_note_label_offset; l_x++)
		{
		  l_chunk = ((l_x + l_window_offset) / m_zoom_factor_X + l_my_start_chunk);
		  if ((l_chunk >= l_my_start_chunk) && (l_chunk <= l_my_end_chunk))
		    {
		      l_offset = toInt(l_chunk * l_frames_per_chunk + l_smooth_delay);
		      if(l_offset > l_end_of_note)
			{
			  l_offset = l_end_of_note;
			}
		      if (l_offset < l_beginning_of_note + 3 * l_smooth_delay)
			{
			  int l_time_at_zero = l_beginning_of_note + l_smooth_delay;
			  float l_scale_X = (l_offset - l_time_at_zero) / float(2 * l_smooth_delay);
			  float l_pitch_at_zero = l_active->dataAtChunk(l_my_start_chunk)->getPitch();
			  int l_smooth_delay_pos3 = std::min(l_beginning_of_note + 3 * l_smooth_delay, static_cast<int>(l_pitch_lookup_used.size()) - 1);
			  if(l_smooth_delay_pos3 >= 0)
			    {
			      float l_pitch_at_2_smooth_delay = l_pitch_lookup_used.at(l_smooth_delay_pos3);
			      l_y = l_half_height + (l_pitch_at_zero + l_scale_X * (l_pitch_at_2_smooth_delay - l_pitch_at_zero) - l_avg_pitch) * l_zoom_factor_Y_x_100;
			    }
			  else
			    {
			      l_y = 0;
			    }
			}
		      else
			{
			  l_offset = std::min(l_offset, l_pitch_lookup_used_size_limit);
			  l_y = l_half_height + (l_pitch_lookup_used.at(l_offset) - l_avg_pitch) * l_zoom_factor_Y_x_100;
			}

		      // Vertical scrollbar offset
		      l_y += m_offset_Y;

		      l_vertices[l_vertices_counter++] = l_x;
		      l_vertices[l_vertices_counter++] = l_y;

		      l_colors[l_colors_counter++] = 127;
		      l_colors[l_colors_counter++] = 0;
		      l_colors[l_colors_counter++] = 0;
		    }
		}
	    }
	  else
	    {
	      // No detailed pitch information available, calculate polyline using the chunkdata
	      l_vertices = new GLfloat[(l_my_end_chunk - l_my_start_chunk) * 2];
	      l_colors = new GLubyte[(l_my_end_chunk - l_my_start_chunk) * 3];

	      float l_x, l_y;
	      for(int l_chunk = l_my_start_chunk; l_chunk < l_my_end_chunk; l_chunk++)
		{
		  l_x = (l_chunk - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x > width() - m_note_label_offset)
		    {
		      break;
		    }
		  l_y = l_half_height + ((l_active->dataAtChunk(l_chunk)->getPitch() - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

		  l_vertices[l_vertices_counter++] = l_x;
		  l_vertices[l_vertices_counter++] = l_y;

		  l_colors[l_colors_counter++] = 127;
		  l_colors[l_colors_counter++] = 0;
		  l_colors[l_colors_counter++] = 0;
		}
	    }
	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_vibrato_polyline, GL_COMPILE);
	  glDrawArrays(GL_LINE_STRIP, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;


	  // Calculate a light grey band indicating which time is being used in the current window
	  l_vertices = new GLfloat[8];
	  l_colors = new GLubyte[16];

	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  const double l_half_window_time = static_cast<double>(l_active->size()) / static_cast<double>(l_active->rate() * 2);
	  int l_pixel_left = toInt((l_active->chunkAtTime(GData::getUniqueInstance().getView().currentTime() - l_half_window_time) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset);
	  int l_pixel_right = toInt((l_active->chunkAtTime(GData::getUniqueInstance().getView().currentTime() + l_half_window_time) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset);

	  l_vertices[l_vertices_counter++] = l_pixel_left;
	  l_vertices[l_vertices_counter++] = 0;
	  l_vertices[l_vertices_counter++] = l_pixel_right;
	  l_vertices[l_vertices_counter++] = 0;
	  l_vertices[l_vertices_counter++] = l_pixel_right;
	  l_vertices[l_vertices_counter++] = height();
	  l_vertices[l_vertices_counter++] = l_pixel_left;
	  l_vertices[l_vertices_counter++] = height();

	  QPalette l_palette;
	  QColor l_foreground = l_palette.color(QPalette::WindowText);
	  for(int l_j = 1; l_j <= 4; l_j++)
	    {
	      l_colors[l_colors_counter++] = l_foreground.red();
	      l_colors[l_colors_counter++] = l_foreground.green();
	      l_colors[l_colors_counter++] = l_foreground.blue();
	      l_colors[l_colors_counter++] = 64;
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_current_window_band, GL_COMPILE);
	  glDrawArrays(GL_QUADS, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;


	  // Calculate the current timeline
	  l_vertices = new GLfloat[4];
	  l_colors = new GLubyte[6];

	  l_vertices_counter = 0;

	  const float l_time_line_X = toInt((l_my_current_chunk - l_my_start_chunk) * m_zoom_factor_X - l_window_offset);

	  l_vertices[l_vertices_counter++] = l_time_line_X;
	  l_vertices[l_vertices_counter++] = 0;
	  l_vertices[l_vertices_counter++] = l_time_line_X;
	  l_vertices[l_vertices_counter++] = height();

	  for(l_colors_counter = 0; l_colors_counter < 6; l_colors_counter++)
	    {
	      l_colors[l_colors_counter] = 0;
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_current_time_line, GL_COMPILE);
	  glDrawArrays(GL_LINES, 0, l_vertices_counter / 2);
	  glEndList();

	  delete[] l_vertices;
	  delete[] l_colors;


	  // Calculate the points of maxima and minima
	  l_vertices = new GLfloat[(l_maxima_size + l_minima_size) * 2];
	  l_colors = new GLubyte[(l_maxima_size + l_minima_size) * 3];

	  l_vertices_counter = 0;
	  l_colors_counter = 0;

	  // Calculate the maxima
	  if ((l_active->doingDetailedPitch()) && (l_active->get_pitch_lookup_smoothed().size() > 0))
	    {
	      float l_x, l_y;
	      for(int l_index = 0; l_index < l_maxima_size; l_index++)
		{
		  l_x = (((static_cast<float>(l_note->get_maxima()->at(l_index)) - l_smooth_delay)/ l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x > width() - m_note_label_offset)
		    {
		      break;
		    }
		  l_y = l_half_height + ((l_pitch_lookup_used.at(l_note->get_maxima()->at(l_index)) - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

		  l_vertices[l_vertices_counter++] = l_x;
		  l_vertices[l_vertices_counter++] = l_y;

		  l_colors[l_colors_counter++] = 255;
		  l_colors[l_colors_counter++] = 255;
		  l_colors[l_colors_counter++] = 0;
		}
	    }
	  // Calculate the minima
	  if ((l_active->doingDetailedPitch()) && (l_active->get_pitch_lookup_smoothed().size() > 0))
	    {
	      float l_x, l_y;
	      for(int l_index = 0; l_index < l_minima_size; l_index++)
		{
		  l_x = (((static_cast<float>(l_note->get_minima()->at(l_index)) - l_smooth_delay) / l_frames_per_chunk) - l_my_start_chunk) * m_zoom_factor_X - l_window_offset;
		  if (l_x < m_note_label_offset)
		    {
		      continue;
		    }
		  if (l_x > width() - m_note_label_offset)
		    {
		      break;
		    }
		  l_y = l_half_height + ((l_pitch_lookup_used.at(l_note->get_minima()->at(l_index)) - l_avg_pitch) * l_zoom_factor_Y_x_100) + m_offset_Y;

		  l_vertices[l_vertices_counter++] = l_x;
		  l_vertices[l_vertices_counter++] = l_y;

		  l_colors[l_colors_counter++] = 0;
		  l_colors[l_colors_counter++] = 255;
		  l_colors[l_colors_counter++] = 0;
		}
	    }

	  glVertexPointer(2, GL_FLOAT, 0, l_vertices);
	  glColorPointer(3, GL_UNSIGNED_BYTE, 0, l_colors);
	  glNewList(m_maxima_minima_points, GL_COMPILE);
	  glDrawArrays(GL_POINTS, 0, l_vertices_counter / 2);
	  glEndList();
      
	  delete[] l_vertices;
	  delete[] l_colors;
	}
    }
}

//------------------------------------------------------------------------------
void VibratoWidget::setZoomFactorX(double p_x)
{
  m_zoom_factor_X = 2 * pow10(log2(p_x / 25));
  update();
}

//------------------------------------------------------------------------------
void VibratoWidget::setZoomFactorY(double p_y)
{
  m_zoom_factor_Y = p_y;
  update();
}

//------------------------------------------------------------------------------
void VibratoWidget::setOffsetY(int p_value)
{
  m_offset_Y = p_value;
  update();
}

//------------------------------------------------------------------------------
QSize VibratoWidget::sizeHint() const
{
  return QSize(300, 100);
}

//------------------------------------------------------------------------------
void VibratoWidget::compose_note_label(QString & p_note_label, const int & p_note)
{
  std::stringstream l_composed_note_label;
  if ((noteOctave(p_note) >= 0) && (noteOctave(p_note) <= 9))
    {
      l_composed_note_label << music_notes::noteName(p_note) << noteOctave(p_note);
    }
  else
    {
      l_composed_note_label << music_notes::noteName(p_note) << " ";
    }
  p_note_label = QString::fromStdString(l_composed_note_label.str());
}

//------------------------------------------------------------------------------
void
VibratoWidget::noteLabelStruct::set( const QString & p_label
                                   , float p_y
                                   )
{
    m_label = p_label;
    m_y = p_y;
}

//------------------------------------------------------------------------------
float
VibratoWidget::noteLabelStruct::get_y() const
{
    return m_y;
}

//------------------------------------------------------------------------------
const QString &
VibratoWidget::noteLabelStruct::get_label() const
{
    return m_label;
}

// EOF
