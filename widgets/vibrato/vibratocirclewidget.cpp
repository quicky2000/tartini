/***************************************************************************
                          vibratocirclewidget.cpp  -  description
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
#include "vibratocirclewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include <glu.h>
#include <gl.h>

//------------------------------------------------------------------------------
VibratoCircleWidget::VibratoCircleWidget(QWidget *p_parent)
: QGLWidget(p_parent)
{
    m_accuracy = 1.0;
    m_type = 5;
    m_last_period_to_draw = -1;
}

//------------------------------------------------------------------------------
VibratoCircleWidget::~VibratoCircleWidget(void)
{
	// Remove display lists
	makeCurrent();

	glDeleteLists(m_reference_circle, 1);
	glDeleteLists(m_current_period, 1);
	for(int l_index = 0; l_index < 6; l_index++)
    {
		glDeleteLists(m_prev_periods[l_index], 1);
    }
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::initializeGL(void)
{
	QColor l_bg = gdata->backgroundColor();
	glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	m_reference_circle = glGenLists(1);
	m_current_period = glGenLists(1);
	for(int l_index = 0; l_index < 6; l_index++)
    {
		m_prev_periods[l_index] = glGenLists(1);
    }
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::resizeGL(int p_width
		                          ,int p_height
		                          )
{
	glViewport(0, 0, (GLint)p_width, (GLint)p_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, p_width, 0, p_height);


	// Calculate reference circle + lines
	glNewList(m_reference_circle, GL_COMPILE);

	const float l_half_width = 0.5 * width();
	const float l_half_height = 0.5 * height();

	// The horizontal line
	glColor4ub(0, 0, 0, 64);
	glBegin(GL_LINES);
	glVertex2f(0, l_half_height);
	glVertex2f(p_width, l_half_height);
	glEnd();

	// The vertical line
	glColor4ub(0, 0, 0, 64);
	glBegin(GL_LINES);
	glVertex2f(l_half_width, 0);
	glVertex2f(l_half_width, p_height);
	glEnd();

	// The circle
	const float l_DEG2RAD = PI / 180;
	glColor3ub(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	for(int l_index = 0; l_index < 360; l_index = l_index + 1)
	{
		float l_deg_in_rad = (float)l_index * l_DEG2RAD;
		glVertex2f(l_half_width + 0.8 * l_half_width * cos(l_deg_in_rad), l_half_height + 0.8 * l_half_height * sin(l_deg_in_rad));
	}
	glEnd();

	glEndList();

	// Do forced update on resize
	doUpdate();
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::paintGL(void)
{
	QColor l_bg = gdata->backgroundColor();
	glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the reference circle
	glLineWidth(1.5);
	glCallList(m_reference_circle);

	if((m_type == 1) || (m_type == 2) || (m_type == 4) || (m_type == 5))
    {
		// Draw the current period
		glLineWidth(2.0);
		glCallList(m_current_period);
    }
    else if(m_type == 3)
    {
    	// Draw the previous periods
    	for(int l_index = 5; l_index >= 0; l_index--)
    	{
    		if (m_last_period_to_draw >= l_index)
    		{
    			glCallList(m_prev_periods[l_index]);
    		}
    	}
    }
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::doUpdate(void)
{
	makeCurrent();

	glNewList(m_current_period, GL_COMPILE);
	glEndList();

	for(int l_index = 0; l_index < 6; l_index++)
    {
		glNewList(m_prev_periods[l_index], GL_COMPILE);
		glEndList();
    }

    Channel * l_active_channel = gdata->getActiveChannel();

	int l_left_minimum_time = -1;
	int l_maximum_time = -1;
	int l_right_minimum_time = -1;
	int l_left_minimum_at = -1;
	int l_maximum_at = -1;

	if((l_active_channel) && (l_active_channel->doingDetailedPitch()) && (l_active_channel->get_pitch_lookup_smoothed().size() > 0))
    {
		AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
		if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
		{
			const NoteData * l_note = new NoteData();
			l_note = &(l_active_channel->get_note_data()[l_data->getNoteIndex()]);

			// Determine which delay to use
			int l_smooth_delay = l_active_channel->get_pitch_big_smoothing_filter().delay();
			large_vector<float> l_pitch_lookup_used = l_active_channel->get_pitch_lookup_smoothed();

			int l_current_time = l_active_channel->chunkAtCurrentTime() * l_active_channel->framesPerChunk() + l_smooth_delay;
			int l_maxima_size = l_note->get_maxima()->size();
			int l_minima_size = l_note->get_minima()->size();

			// Determine which period to show: the 2 rightmost minima + the maximum in between
			if((l_maxima_size >= 1 ) && (l_minima_size == 2))
			{
				// Only 2 minima
				if(l_current_time >= l_note->get_minima()->at(1))
				{
					l_left_minimum_time  = l_note->get_minima()->at(0);
					l_right_minimum_time = l_note->get_minima()->at(1);
					l_left_minimum_at = 0;
				}
			}
			else if((l_maxima_size >= 1) && (l_minima_size > 2))
			{
				// More than 2 minima
				for(int l_index = 2; l_index < l_minima_size; l_index++)
				{
					if((l_current_time >= l_note->get_minima()->at(l_index - 1)) && (l_current_time <= l_note->get_minima()->at(l_index)))
					{
						l_left_minimum_time  = l_note->get_minima()->at(l_index - 2);
						l_right_minimum_time = l_note->get_minima()->at(l_index - 1);
						l_left_minimum_at = l_index - 2;
						break;
					}
				}
				if(l_current_time > l_note->get_minima()->at(l_minima_size - 1))
				{
					l_left_minimum_time  = l_note->get_minima()->at(l_minima_size - 2);
					l_right_minimum_time = l_note->get_minima()->at(l_minima_size - 1);
					l_left_minimum_at = l_minima_size - 2;
				}
			}
			// The maximum in between
			for(int l_index = 0; l_index < l_maxima_size; l_index++)
			{
				if((l_note->get_maxima()->at(l_index) >= l_left_minimum_time) && (l_note->get_maxima()->at(l_index) <= l_right_minimum_time))
				{
					l_maximum_time = l_note->get_maxima()->at(l_index);
					l_maximum_at = l_index;
					break;
				}
			}

			if(l_maximum_time > 0)
			{
				// There is at least one full period before this one
				const float l_half_width = 0.5 * width();
				const float l_half_height = 0.5 * height();

				uint l_vertices_counter, l_colors_counter;

				if((m_type == 1) || (m_type == 2))
				{
					// Draw element for every 0.001s
					const int l_step_size = l_active_channel->rate() / 1000;
					const int l_prev_period_duration = l_right_minimum_time - l_left_minimum_time;
					const int l_current_chunk = l_active_channel->chunkAtCurrentTime();

					// Minimum pitch of previous period
					float l_prev_minimum_pitch = (l_pitch_lookup_used.at(l_left_minimum_time) > l_pitch_lookup_used.at(l_right_minimum_time))
							? l_pitch_lookup_used.at(l_right_minimum_time)
							: l_pitch_lookup_used.at(l_left_minimum_time);

					// Maximum pitch of previous period
					float l_prev_maximum_pitch = l_pitch_lookup_used.at(l_maximum_time);
					float l_prev_avg_pitch = (l_prev_minimum_pitch + l_prev_maximum_pitch) * 0.5;
					float l_prev_width = l_prev_maximum_pitch - l_prev_minimum_pitch;

					int l_array_size = 1 + (((l_current_chunk + 1) * l_active_channel->framesPerChunk() + l_smooth_delay - 1) - l_right_minimum_time) / l_step_size;

					GLfloat * l_vertices;
					l_vertices = new GLfloat[l_array_size * 2];

					GLubyte *l_colors;
					l_colors = new GLubyte[l_array_size * 4];

					l_vertices_counter = 0;
					l_colors_counter = 0;

					int l_end_i = std::min((l_current_chunk + 1) * l_active_channel->framesPerChunk() + l_smooth_delay, (int)l_pitch_lookup_used.size());
					for(int l_index = l_right_minimum_time; l_index < l_end_i; l_index += l_step_size)
					{
						if(l_index > (int)l_pitch_lookup_used.size())
						{
							// Break out of loop when end of smoothed pitchlookup is reached
							break;
						}

						if((l_index - l_right_minimum_time) > l_prev_period_duration)
						{
							// Break out of loop when current period is longer than previous period, 'circle' is complete then
							break;
						}

						float l_current_pitch = l_pitch_lookup_used.at(l_index);
						float l_prev_pitch = l_pitch_lookup_used.at(l_index - l_prev_period_duration);

						switch (m_type)
						{
							case 1:
								{
									float l_phase = float(l_index - l_right_minimum_time) / l_prev_period_duration;
									float l_cos_phase = cos(l_phase * twoPI);
									float l_sin_phase = sin(l_phase * twoPI);
									float l_comparison = (l_current_pitch - l_prev_pitch) / l_prev_width;

									l_vertices[l_vertices_counter++] = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
									l_vertices[l_vertices_counter++] = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);

									l_colors[l_colors_counter++] = 255;
									l_colors[l_colors_counter++] = 0;
									l_colors[l_colors_counter++] = 0;
									l_colors[l_colors_counter++] = 255;
									break;
								}
							case 2:
								{
									float l_phase = float(l_index - l_right_minimum_time) / l_prev_period_duration;
									float l_cos_phase = cos(l_phase * twoPI);
									float l_sin_phase = sin(l_phase * twoPI);
									float l_comparison = (l_current_pitch - (l_prev_avg_pitch + (-l_cos_phase * l_prev_width * 0.5))) / l_prev_width;

									l_vertices[l_vertices_counter++] = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
									l_vertices[l_vertices_counter++] = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);

									l_colors[l_colors_counter++] = 255;
									l_colors[l_colors_counter++] = 0;
									l_colors[l_colors_counter++] = 0;
									l_colors[l_colors_counter++] = 255;
									break;
								}
							default:
								{
									break;
								}
						}
					}

					glVertexPointer(2, GL_FLOAT, 0, l_vertices);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
					glNewList(m_current_period, GL_COMPILE);
					glDrawArrays(GL_LINE_STRIP, 0, l_vertices_counter / 2);
					glEndList();

					delete[] l_vertices;
					delete[] l_colors;
				}
				else if(m_type == 3)
				{
					// Draw element for every 0.001s
					const int l_step_size = l_active_channel->rate() / 1000;
					for(int l_p = 0; l_p < 6; l_p++)
					{
						if (l_left_minimum_at - l_p < 0)
						{
							m_last_period_to_draw = l_p - 1;
							break;
						}

						l_left_minimum_time = l_note->get_minima()->at(l_left_minimum_at - l_p);
						l_right_minimum_time = l_note->get_minima()->at(l_left_minimum_at - (l_p - 1));
						for(int l_j = 0; l_j < l_note->get_maxima()->size(); l_j++)
						{
							if ((l_note->get_maxima()->at(l_j) >= l_left_minimum_time) && (l_note->get_maxima()->at(l_j) <= l_right_minimum_time))
							{
								l_maximum_time = l_note->get_maxima()->at(l_j);
								break;
							}
						}

						float l_prev_minimumPitch = (l_pitch_lookup_used.at(l_left_minimum_time) > l_pitch_lookup_used.at(l_right_minimum_time))
								? l_pitch_lookup_used.at(l_right_minimum_time)
								: l_pitch_lookup_used.at(l_left_minimum_time);
						float l_prev_maximum_pitch = l_pitch_lookup_used.at(l_maximum_time);
						float l_prev_avg_pitch = (l_prev_minimumPitch + l_prev_maximum_pitch) * 0.5;
						float l_prev_width = l_prev_maximum_pitch - l_prev_minimumPitch;

						int l_array_size = 1 + (l_right_minimum_time - l_left_minimum_time) / l_step_size;

						GLfloat * l_vertices;
						l_vertices = new GLfloat[l_array_size * 2];

						GLubyte * l_colors;
						l_colors = new GLubyte[l_array_size * 4];

						l_vertices_counter = 0;
						l_colors_counter = 0;

						for(int l_index = l_left_minimum_time; l_index <= l_right_minimum_time; l_index += l_step_size)
						{
							float l_pitch = l_pitch_lookup_used.at(l_index);
							float l_phase = float(l_index - l_left_minimum_time) / float(l_right_minimum_time - l_left_minimum_time);
							float l_cos_phase = cos(l_phase * twoPI);
							float l_sin_phase = sin(l_phase * twoPI);
							float l_comparison = (l_pitch - (l_prev_avg_pitch + (-l_cos_phase * l_prev_width * 0.5))) / l_prev_width;

							l_vertices[l_vertices_counter++] = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
							l_vertices[l_vertices_counter++] = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);

							l_colors[l_colors_counter++] = 255;
							l_colors[l_colors_counter++] = 0;
							l_colors[l_colors_counter++] = 0;
							l_colors[l_colors_counter++] = toInt(float(1/pow(2,l_p)) * 255);
						}

						glVertexPointer(2, GL_FLOAT, 0, l_vertices);
						glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
						glNewList(m_prev_periods[l_p], GL_COMPILE);
						glDrawArrays(GL_LINE_STRIP, 0, l_vertices_counter / 2);
						glEndList();

						delete[] l_vertices;
						delete[] l_colors;

						m_last_period_to_draw = l_p;
					}
				}
				else if(m_type == 4)
				{
					const int l_period_duration = l_right_minimum_time - l_left_minimum_time;
					int l_array_size = 362;

					GLfloat *l_vertices;
					l_vertices = new GLfloat[l_array_size * 2];

					GLubyte *l_colors;
					l_colors = new GLubyte[l_array_size * 4];

					l_vertices_counter = 0;

					l_vertices[l_vertices_counter++] = l_half_width;
					l_vertices[l_vertices_counter++] = l_half_height;

					for(int l_j = 0; l_j < 360; l_j++)
					{
						int l_i = l_left_minimum_time + toInt(floor((l_j / float(360)) * l_period_duration));
						if(l_i > l_right_minimum_time)
						{
							l_i = l_right_minimum_time;
						}

						float l_pitch = l_pitch_lookup_used.at(l_i);
						float l_phase = float(l_i - l_left_minimum_time) / float(l_period_duration);
						float l_cos_phase = cos(l_phase * twoPI);
						float l_sin_phase = sin(l_phase * twoPI);
						float l_min_step = float(l_i - l_left_minimum_time) * ((l_pitch_lookup_used.at(l_right_minimum_time) - l_pitch_lookup_used.at(l_left_minimum_time)) / l_period_duration);
						float l_this_minimum_pitch = l_pitch_lookup_used.at(l_left_minimum_time) + l_min_step;
						float l_this_maximum_pitch = l_pitch_lookup_used.at(l_maximum_time);
						float l_this_avg_pitch = (l_this_minimum_pitch + l_this_maximum_pitch) * 0.5;
						float l_this_width = l_this_maximum_pitch - l_this_minimum_pitch;
						float l_comparison = (l_pitch - (l_this_avg_pitch + (-l_cos_phase * l_this_width * 0.5))) / l_this_width;

						l_vertices[l_vertices_counter++] = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
						l_vertices[l_vertices_counter++] = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);
					}

					l_vertices[l_vertices_counter++] = 1.8 * l_half_width;
					l_vertices[l_vertices_counter++] = l_half_height;

					for(int l_i = 0; l_i < 4 * 362; l_i += 4)
					{
						l_colors[l_i] = 255;
						l_colors[l_i+1] = 0;
						l_colors[l_i+2] = 0;
						l_colors[l_i+3] = 200;
					}

					glVertexPointer(2, GL_FLOAT, 0, l_vertices);
					glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
					glNewList(m_current_period, GL_COMPILE);
					glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
					glEndList();

					delete[] l_vertices;
					delete[] l_colors;
				}
				else if(m_type == 5)
				{
					// Fade in first period
					if (l_left_minimum_at == 0)
					{
						const int l_period_duration = l_right_minimum_time - l_left_minimum_time;
						const int l_fade_alpha_int = 200 * (l_current_time - l_right_minimum_time) / l_period_duration;
						const GLubyte l_fade_alpha = (l_fade_alpha_int > 255) ? 255 : l_fade_alpha_int;

						int l_array_size = 362;
						GLfloat *l_vertices;
						l_vertices = new GLfloat[l_array_size * 2];

						GLubyte *l_colors;
						l_colors = new GLubyte[l_array_size * 4];

						l_vertices_counter = 0;

						l_vertices[l_vertices_counter++] = l_half_width;
						l_vertices[l_vertices_counter++] = l_half_height;

						for(int l_j = 0; l_j < 360; l_j++)
						{
							int l_i = l_left_minimum_time + toInt(floor((l_j / float(360)) * l_period_duration));
							if(l_i > l_right_minimum_time)
							{
								l_i = l_right_minimum_time;
							}

							float l_pitch = l_pitch_lookup_used.at(l_i);

							float l_phase = float(l_i - l_left_minimum_time) / float(l_period_duration);
							float l_cos_phase = cos(l_phase * twoPI);
							float l_sin_phase = sin(l_phase * twoPI);
							float l_min_step = float(l_i - l_left_minimum_time) * ((l_pitch_lookup_used.at(l_right_minimum_time) - l_pitch_lookup_used.at(l_left_minimum_time)) / l_period_duration);
							float l_this_minimum_pitch = l_pitch_lookup_used.at(l_left_minimum_time) + l_min_step;
							float l_this_maximum_pitch = l_pitch_lookup_used.at(l_maximum_time);
							float l_this_avg_pitch = (l_this_minimum_pitch + l_this_maximum_pitch) * 0.5;
							float l_this_width = l_this_maximum_pitch - l_this_minimum_pitch;
							float l_comparison = (l_pitch - (l_this_avg_pitch + (-l_cos_phase * l_this_width * 0.5))) / l_this_width;

							l_vertices[l_vertices_counter++] = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
							l_vertices[l_vertices_counter++] = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);
						}

						l_vertices[l_vertices_counter++] = 1.8 * l_half_width;
						l_vertices[l_vertices_counter++] = l_half_height;

						for(int l_index = 0; l_index < 4 * 362; l_index += 4)
						{
							l_colors[l_index] = 255;
							l_colors[l_index+1] = 0;
							l_colors[l_index+2] = 0;
							l_colors[l_index+3] = l_fade_alpha;
						}

						glVertexPointer(2, GL_FLOAT, 0, l_vertices);
						glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
						glNewList(m_current_period, GL_COMPILE);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
						glEndList();

						delete[] l_vertices;
						delete[] l_colors;
					}
					else if(l_left_minimum_at > 0)
					{
						// Morph from previous period to current period
						const int l_prev_left_minimum_time = l_note->get_minima()->at(l_left_minimum_at - 1);
						const int l_prev_right_minimum_time = l_left_minimum_time;
						const int l_prev_maximum_time = l_note->get_maxima()->at(l_maximum_at - 1);
						const int l_prev_period_duration = l_prev_right_minimum_time - l_prev_left_minimum_time;
						float l_prev_maximum_pitch = l_pitch_lookup_used.at(l_prev_maximum_time);
						const int l_period_duration = l_right_minimum_time - l_left_minimum_time;
						float l_this_maximum_pitch = l_pitch_lookup_used.at(l_maximum_time);
						float l_distance_ratio = (l_current_time - l_right_minimum_time) / float(l_period_duration);
						if(l_distance_ratio > 1)
						{
							l_distance_ratio = 1;
						}

						int l_array_size = 362;

						GLfloat * l_vertices;
						l_vertices = new GLfloat[l_array_size * 2];

						GLubyte * l_colors;
						l_colors = new GLubyte[l_array_size * 4];

						l_vertices_counter = 0;

						l_vertices[l_vertices_counter++] = l_half_width;
						l_vertices[l_vertices_counter++] = l_half_height;

						for(int l_j = 0; l_j < 360; l_j++)
						{
							int l_prev_I = l_prev_left_minimum_time + toInt(floor((l_j / float(360)) * l_prev_period_duration));
							if(l_prev_I > l_prev_right_minimum_time)
							{
								l_prev_I = l_prev_right_minimum_time;
							}
							int l_i = l_left_minimum_time + toInt(floor((l_j / float(360)) * l_period_duration));
							if(l_i > l_right_minimum_time)
							{
								l_i = l_right_minimum_time;
							}

							float l_prev_pitch = l_pitch_lookup_used.at(l_prev_I);
							float l_prev_phase = float(l_prev_I - l_prev_left_minimum_time) / float(l_prev_period_duration);
							float l_prev_cos_phase = cos(l_prev_phase * twoPI);
							float l_prev_sin_phase = sin(l_prev_phase * twoPI);
							float l_prev_min_step = float(l_prev_I - l_prev_left_minimum_time) * ((l_pitch_lookup_used.at(l_prev_right_minimum_time) - l_pitch_lookup_used.at(l_prev_left_minimum_time)) / l_prev_period_duration);
							float l_prev_minimum_pitch = l_pitch_lookup_used.at(l_prev_left_minimum_time) + l_prev_min_step;
							float l_prev_avg_pitch = (l_prev_minimum_pitch + l_prev_maximum_pitch) * 0.5;
							float l_prev_width = l_prev_maximum_pitch - l_prev_minimum_pitch;
							float l_prev_comparison = (l_prev_pitch - (l_prev_avg_pitch + (-l_prev_cos_phase * l_prev_width * 0.5))) / l_prev_width;
							float l_prev_X = l_half_width + l_half_width * (0.8 * l_prev_cos_phase + m_accuracy * l_prev_comparison * l_prev_cos_phase);
							float l_prev_Y = l_half_height + l_half_height * (0.8 * l_prev_sin_phase + m_accuracy * l_prev_comparison * l_prev_sin_phase);

							float l_pitch = l_pitch_lookup_used.at(l_i);
							float l_phase = float(l_i - l_left_minimum_time) / float(l_period_duration);
							float l_cos_phase = cos(l_phase * twoPI);
							float l_sin_phase = sin(l_phase * twoPI);
							float l_min_step = float(l_i - l_left_minimum_time) * ((l_pitch_lookup_used.at(l_right_minimum_time) - l_pitch_lookup_used.at(l_left_minimum_time)) / l_period_duration);
							float l_this_minimum_pitch = l_pitch_lookup_used.at(l_left_minimum_time) + l_min_step;
							float l_this_avg_pitch = (l_this_minimum_pitch + l_this_maximum_pitch) * 0.5;
							float l_this_width = l_this_maximum_pitch - l_this_minimum_pitch;
							float l_comparison = (l_pitch - (l_this_avg_pitch + (-l_cos_phase * l_this_width * 0.5))) / l_this_width;
							float l_this_X = l_half_width + l_half_width * (0.8 * l_cos_phase + m_accuracy * l_comparison * l_cos_phase);
							float l_this_Y = l_half_height + l_half_height * (0.8 * l_sin_phase + m_accuracy * l_comparison * l_sin_phase);

							l_vertices[l_vertices_counter++] = l_prev_X + l_distance_ratio * (l_this_X - l_prev_X);
							l_vertices[l_vertices_counter++] = l_prev_Y + l_distance_ratio * (l_this_Y - l_prev_Y);
						}

						l_vertices[l_vertices_counter++] = 1.8 * l_half_width;
						l_vertices[l_vertices_counter++] = l_half_height;

						for(int l_index = 0; l_index < 4 * 362; l_index += 4)
						{
							l_colors[l_index] = 255;
							l_colors[l_index+1] = 0;
							l_colors[l_index+2] = 0;
							l_colors[l_index+3] = 200;
						}

						glVertexPointer(2, GL_FLOAT, 0, l_vertices);
						glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);
						glNewList(m_current_period, GL_COMPILE);
						glDrawArrays(GL_TRIANGLE_FAN, 0, 362);
						glEndList();

						delete[] l_vertices;
						delete[] l_colors;
					}
				}
			}
		}
    }
    updateGL();
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::setAccuracy(int p_value)
{
	m_accuracy = p_value / 10.0;
	doUpdate();
}

//------------------------------------------------------------------------------
void VibratoCircleWidget::setType(int p_value)
{
	m_type = p_value;
	doUpdate();
}

//------------------------------------------------------------------------------
QSize VibratoCircleWidget::minimumSizeHint(void) const
{
	return QSize(100, 75);
}

//------------------------------------------------------------------------------
int VibratoCircleWidget::getType(void)
{
	return m_type;
}
// EOF
