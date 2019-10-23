/***************************************************************************
                          vibratoperiodwidget.cpp  -  description
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
#include "vibratoperiodwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include <glu.h>
#include <gl.h>

//------------------------------------------------------------------------------
VibratoPeriodWidget::VibratoPeriodWidget(QWidget *p_parent)
: QGLWidget(p_parent)
{
    m_prev_left_minimum_time = -1;
    m_last_period_to_draw = -1;

    m_smoothed_periods = true;
    m_draw_sine_reference = false;
    m_sine_style = false;
    m_draw_prev_periods = false;
    m_period_scaling = false;
    m_draw_comparison = false;
}

//------------------------------------------------------------------------------
VibratoPeriodWidget::~VibratoPeriodWidget(void)
{
    // Remove display lists
    makeCurrent();

    glDeleteLists(m_sine_reference, 1);
    for(int l_index = 0; l_index < 5; l_index++)
    {
        glDeleteLists(m_previous_poly[l_index], 1);
    }
    glDeleteLists(m_current_period, 1);
    glDeleteLists(m_comparison_poly, 1);
    glDeleteLists(m_comparison_reference, 1);
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::initializeGL(void)
{
    QColor l_bg = g_data->backgroundColor();
    glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    m_sine_reference = glGenLists(1);
    for(int l_index = 0; l_index < 5; l_index++)
    {
    	m_previous_poly[l_index] = glGenLists(1);
    }
    m_current_period = glGenLists(1);
    m_comparison_poly = glGenLists(1);
    m_comparison_reference = glGenLists(1);
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::resizeGL(int p_width
		                          ,int p_height
		                          )
{
	glViewport(0, 0, (GLint)p_width, (GLint)p_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, p_width, 0, p_height);

	// Calculate the horizontal reference line
	const float halfHeight = 0.5 * height();

	glNewList(m_comparison_reference, GL_COMPILE);
	glColor4ub(0, 0, 0, 64);
	glBegin(GL_LINES);
	glVertex2f(0, halfHeight);
	glVertex2f(p_width, halfHeight);
	glEnd();
	glEndList();

	// Do forced update on resize
	m_prev_left_minimum_time = -1;
	doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::paintGL(void)
{
	QColor l_bg = g_data->backgroundColor();
	glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the horizontal reference line
	glLineWidth(1.5);
	glCallList(m_comparison_reference);

	// Draw the sinewave
	glLineWidth(2.0);
	glCallList(m_sine_reference);

	// Draw the comparison
	glCallList(m_comparison_poly);

	// Draw the previous periods
	for(int l_index = 4; l_index >= 0; l_index--)
    {
		if (m_last_period_to_draw >= l_index)
		{
			glCallList(m_previous_poly[l_index]);
		}
    }

    // Draw the current period
    glCallList(m_current_period);

}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::doUpdate(void)
{
	Channel *l_active_channel = g_data->getActiveChannel();

	int l_left_minimum_time = -1;
	int l_maximum_time = -1;
	int l_right_minimum_time = -1;
	int l_left_minimum_at = -1;

	if((l_active_channel) && (l_active_channel->doingDetailedPitch()) && (l_active_channel->get_pitch_lookup_smoothed().size() > 0))
    {
		AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
		if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
		{
			const NoteData * l_note = new NoteData();
			l_note = &(l_active_channel->get_note_data()[l_data->getNoteIndex()]);

			int l_smooth_delay = l_active_channel->get_pitch_big_smoothing_filter().delay();
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
					if((l_current_time >= l_note->get_minima()->at(l_index-1)) && (l_current_time <= l_note->get_minima()->at(l_index)))
					{
						l_left_minimum_time  = l_note->get_minima()->at(l_index-2);
						l_right_minimum_time = l_note->get_minima()->at(l_index-1);
						l_left_minimum_at = l_index-2;
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
			    if ((l_note->get_maxima()->at(l_index) >= l_left_minimum_time) && (l_note->get_maxima()->at(l_index) <= l_right_minimum_time))
			    {
			        l_maximum_time = l_note->get_maxima()->at(l_index);
			        break;
			    }
			}
		}
    }

    if(m_prev_left_minimum_time == l_left_minimum_time)
    {
        // Same period as previous, don't change the polys & no update needed
    }
    else
    {
        // Period has changed
        makeCurrent();

        if(l_left_minimum_time == -1)
        {
            // No period to draw, erase polys & update
            glNewList(m_sine_reference, GL_COMPILE);
            glEndList();

            for(int l_index = 0; l_index < 5; l_index++)
            {
                glNewList(m_previous_poly[l_index], GL_COMPILE);
                glEndList();
            }

            glNewList(m_current_period, GL_COMPILE);
            glEndList();

            glNewList(m_comparison_poly, GL_COMPILE);
            glEndList();

            m_prev_left_minimum_time = -1;

            updateGL();
        }
        else
        {
            // New period, calculate new polys & update
            glNewList(m_sine_reference, GL_COMPILE);
            glEndList();

            for(int l_index = 0; l_index < 5; l_index++)
            {
                glNewList(m_previous_poly[l_index], GL_COMPILE);
                glEndList();
            }

            glNewList(m_current_period, GL_COMPILE);
            glEndList();

            glNewList(m_comparison_poly, GL_COMPILE);
            glEndList();

            AnalysisData * l_data = l_active_channel->dataAtCurrentChunk();
            const NoteData * l_note = new NoteData();
            l_note = &(l_active_channel->get_note_data()[l_data->getNoteIndex()]);

            large_vector<float> l_the_pitch_lookup;
            int l_the_delay;
            if(m_smoothed_periods)
            {
                l_the_pitch_lookup = l_active_channel->get_pitch_lookup_smoothed();
                l_the_delay = 0;
            }
            else
            {
                l_the_pitch_lookup = l_active_channel->get_pitch_lookup();
                l_the_delay = l_active_channel->get_pitch_big_smoothing_filter().delay() - l_active_channel->get_pitch_small_smoothing_filter().delay();
            }

            int l_the_left_minimum_time = l_left_minimum_time - l_the_delay;
            int l_the_right_minimum_time = l_right_minimum_time - l_the_delay;
            int l_the_maximum_time = l_maximum_time - l_the_delay;
            int l_period_duration = l_the_right_minimum_time - l_the_left_minimum_time;

            float l_minimum_pitch = (l_the_pitch_lookup.at(l_the_left_minimum_time) > l_the_pitch_lookup.at(l_the_right_minimum_time))
				        ? l_the_pitch_lookup.at(l_the_right_minimum_time)
				        : l_the_pitch_lookup.at(l_the_left_minimum_time)
				        ;
            float l_maximum_pitch = l_the_pitch_lookup.at(l_the_maximum_time);
            float l_period_width = l_maximum_pitch - l_minimum_pitch;

            const float l_half_height = 0.5 * height();

            // Display either sine- or cosine-style periods
            int l_the_sine_delay = 0;
            if(m_sine_style)
            {
                l_the_sine_delay = toInt((l_right_minimum_time - l_left_minimum_time) * 0.25);
            }

            GLfloat * l_vertices;
            l_vertices = new GLfloat[(width() + 1) * 2];

            GLfloat * l_sine_vertices;
            l_sine_vertices = new GLfloat[(width() + 1) * 2];

            GLubyte * l_colors;
            l_colors = new GLubyte[(width() + 1) * 4];
            glColorPointer(4, GL_UNSIGNED_BYTE, 0, l_colors);


            // Calculate the sinewave
            if(m_draw_sine_reference && (width() > 0) && (height() > 0))
            {
                uint l_v = 0;
                uint l_c = 0;
                if(m_sine_style)
                {
                    for(float l_xx = 0; l_xx < width(); l_xx++)
                    {
                        l_sine_vertices[l_v++] = 0.05 * width() + 0.9 * l_xx;
                        l_sine_vertices[l_v++] = l_half_height + l_half_height * 0.9 * sin((l_xx / width()) * 2 * PI);
                        l_colors[l_c++] = 255;
                        l_colors[l_c++] = 255;
                        l_colors[l_c++] = 0;
                        l_colors[l_c++] = 255;
                    }
                }
                else
                {
                    for(float l_xx = 0; l_xx < width(); l_xx++)
                    {
                        l_sine_vertices[l_v++] = 0.05 * width() + 0.9 * l_xx;;
                        l_sine_vertices[l_v++] = l_half_height + l_half_height * -0.9 * cos((l_xx/width()) * 2 * PI);
                        l_colors[l_c++] = 255;
                        l_colors[l_c++] = 255;
                        l_colors[l_c++] = 0;
                        l_colors[l_c++] = 255;
                    }
                }
                glVertexPointer(2, GL_FLOAT, 0, l_sine_vertices);
                glNewList(m_sine_reference, GL_COMPILE);
                glDrawArrays(GL_LINE_STRIP, 0, width());
                glEndList();
            }

            // Calculate the previous period(s)
            if(m_draw_prev_periods && (width() > 0) && (height() > 0))
            {
                for(int l_index = 0; l_index < 5; l_index++)
                {
                    if (l_left_minimum_at - (l_index+1) < 0)
                    {
                        m_last_period_to_draw = l_index - 1;
                        break;
                    }

                    uint l_v = 0;
                    uint l_c = 0;
                    int l_this_prev_left_minimum_time = l_note->get_minima()->at(l_left_minimum_at - (l_index + 1)) - l_the_delay;
                    int l_this_prev_right_minimum_time = l_note->get_minima()->at(l_left_minimum_at - l_index) - l_the_delay;
                    int l_this_prev_duration = l_this_prev_right_minimum_time - l_this_prev_left_minimum_time;
                    int l_this_prev_maximum_time = 0;
                    for(int l_j = 0; l_j < l_note->get_maxima()->size(); l_j++)
                    {
                        if((l_note->get_maxima()->at(l_j) >= l_this_prev_left_minimum_time) && (l_note->get_maxima()->at(l_j) <= l_this_prev_right_minimum_time))
                        {
                            l_this_prev_maximum_time = l_note->get_maxima()->at(l_j) - l_the_delay;
                            break;
                        }
                    }

                    float l_this_prev_minimum_pitch = (l_the_pitch_lookup.at(l_this_prev_left_minimum_time) > l_the_pitch_lookup.at(l_this_prev_right_minimum_time))
                            ? l_the_pitch_lookup.at(l_this_prev_right_minimum_time)
                            : l_the_pitch_lookup.at(l_this_prev_left_minimum_time)
                            ;
                    float l_this_prev_maximum_pitch = l_the_pitch_lookup.at(l_this_prev_maximum_time);
                    float l_this_prev_width = l_this_prev_maximum_pitch - l_this_prev_minimum_pitch;

                    if(m_period_scaling)
                    {
                        for (float l_xx = 0; l_xx < width(); l_xx++)
                        {
                            int l_offset = toInt((l_xx / width()) * l_this_prev_duration + l_this_prev_left_minimum_time + l_the_sine_delay);
                            l_vertices[l_v++] = 0.05 * width() + 0.9 * l_xx;
                            l_vertices[l_v++] = 0.05 * height() + 0.9 * ((l_the_pitch_lookup.at(l_offset) - l_this_prev_minimum_pitch) / l_this_prev_width) * height();
                            l_colors[l_c++] = 127;
                            l_colors[l_c++] = 0;
                            l_colors[l_c++] = 0;
                            l_colors[l_c++] = toInt(float(1/pow(2,l_index + 1)) * 255);
                        }
                        glVertexPointer(2, GL_FLOAT, 0, l_vertices);
                        glNewList(m_previous_poly[l_index], GL_COMPILE);
                        glDrawArrays(GL_LINE_STRIP, 0, width());
                        glEndList();
                    }
                    else
                    {
                        for(float l_xx = 0; l_xx < width(); l_xx++)
                        {
                            int l_offset = toInt((l_xx / width()) * l_this_prev_duration + l_this_prev_left_minimum_time + l_the_sine_delay);
                            float l_xxx = l_xx * (float(l_this_prev_duration) / l_period_duration);
                            l_xxx = l_xxx + ((0.5 * (l_period_duration - l_this_prev_duration) / l_period_duration) * width());
                            l_vertices[l_v++] = 0.05 * width() + 0.9 * l_xxx;
                            l_vertices[l_v++] = 0.05 * height() + 0.9 * ((l_the_pitch_lookup.at(l_offset) - l_minimum_pitch) / l_period_width) * height();
                            l_colors[l_c++] = 127;
                            l_colors[l_c++] = 0;
                            l_colors[l_c++] = 0;
                            l_colors[l_c++] = toInt(float(1/pow(2, l_index + 1)) * 255);
                        }

                        glVertexPointer(2, GL_FLOAT, 0, l_vertices);
                        glNewList(m_previous_poly[l_index], GL_COMPILE);
                        glDrawArrays(GL_LINE_STRIP, 0, width());
                        glEndList();
                    }
                    m_last_period_to_draw = l_index;
                }
            }

            // Calculate the current period
            if((width() > 0) && (height() > 0))
            {
                uint l_v = 0;
                uint l_c = 0;
                for(float l_xx = 0; l_xx < width(); l_xx++)
                {
                    l_vertices[l_v++] = 0.05 * width() + 0.9 * l_xx;
                    l_vertices[l_v++] = 0.05 * height() + 0.9 * ((l_the_pitch_lookup.at(toInt((l_xx / width()) * l_period_duration + l_the_left_minimum_time + l_the_sine_delay)) - l_minimum_pitch) / l_period_width) * height();
                    l_colors[l_c++] = 127;
                    l_colors[l_c++] = 0;
                    l_colors[l_c++] = 0;
                    l_colors[l_c++] = 255;
                }
                glVertexPointer(2, GL_FLOAT, 0, l_vertices);
                glNewList(m_current_period, GL_COMPILE);
                glDrawArrays(GL_LINE_STRIP, 0, width());
                glEndList();
            }

            // Calculate the comparison
            if(m_draw_comparison && m_draw_sine_reference && (width() > 0) && (height() > 0))
            {
                uint l_v = 0;
                uint l_c = 0;
                for(int l_xx = 0; l_xx < width(); l_xx++)
                {
                    l_vertices[l_v++] = 0.05 * width() + 0.9 * l_xx;
                    l_vertices[l_v] = l_half_height + l_vertices[l_v] - l_sine_vertices[l_v];
                    ++l_v;
                    l_colors[l_c++] = 0;
                    l_colors[l_c++] = 255;
                    l_colors[l_c++] = 0;
                    l_colors[l_c++] = 255;
                }
                glVertexPointer(2, GL_FLOAT, 0, l_vertices);
                glNewList(m_comparison_poly, GL_COMPILE);
                glDrawArrays(GL_LINE_STRIP, 0, width());
                glEndList();
            }

            delete[] l_vertices;
            delete[] l_sine_vertices;
            delete[] l_colors;

            m_prev_left_minimum_time = l_left_minimum_time;

            updateGL();
        }
    }
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setSmoothedPeriods(bool p_value)
{
    m_smoothed_periods = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setDrawSineReference(bool p_value)
{
    m_draw_sine_reference = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setSineStyle(bool p_value)
{
    m_sine_style = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setDrawPrevPeriods(bool p_value)
{
    m_draw_prev_periods = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setPeriodScaling(bool p_value)
{
    m_period_scaling = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoPeriodWidget::setDrawComparison(bool p_value)
{
    m_draw_comparison = p_value;
    m_prev_left_minimum_time = -1;
    doUpdate();
}

//------------------------------------------------------------------------------
QSize VibratoPeriodWidget::minimumSizeHint(void) const
{
    return QSize(100, 75);
}
// EOF
