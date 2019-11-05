/***************************************************************************
                          vibratospeedwidget.cpp  -  description
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
#include "vibratospeedwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include <glu.h>
#include <gl.h>

//------------------------------------------------------------------------------
VibratoSpeedWidget::VibratoSpeedWidget(QWidget * p_parent)
: QGLWidget(p_parent)
{
    m_speed_value_to_draw = 0;
    m_width_value_to_draw = 0;
    m_prev_vibrato_speed = 0;
    m_prev_vibrato_width = 0;
    m_use_prony = true;
    m_width_limit = 50;
    m_prev_note_number = -1;
    m_speed_label_counter = 0;
    m_width_label_counter = 0;
    for (int l_index = 0; l_index < 100; l_index++)
    {
        m_speed_labels[l_index].set( QString(8,' '), 0.0f, 0.0f);
        m_width_labels[l_index].set( QString(8,' '), 0.0f, 0.0f);
    }
    m_speed_width_font = QFont();
    m_speed_width_font.setPointSize(9);
}

//------------------------------------------------------------------------------
VibratoSpeedWidget::~VibratoSpeedWidget(void)
{
    // Remove display lists
    makeCurrent();

    glDeleteLists(m_speed_dial, 1);
    glDeleteLists(m_speed_needle, 1);
    glDeleteLists(m_width_dial, 1);
    glDeleteLists(m_width_needle, 1);
}

//------------------------------------------------------------------------------
void VibratoSpeedWidget::initializeGL(void)
{
    QColor l_bg = g_data->backgroundColor();
    glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    m_speed_dial = glGenLists(1);
    m_speed_needle = glGenLists(1);
    m_width_dial = glGenLists(1);
    m_width_needle = glGenLists(1);
}

//------------------------------------------------------------------------------
void VibratoSpeedWidget::resizeGL(int p_width
                                 ,int p_height
                                 )
{
    glViewport(0, 0, (GLint)p_width, (GLint)p_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, p_width, 0, p_height);

    const float l_half_width = 0.5 * width();
    const float l_half_height = 0.5 * height();
    const float l_radius = 1.8 * MAX(l_half_height, l_half_width);
    const float l_theta = asin(float(width()) / (2 * l_radius));
    const float l_rho = (PI / 2.0) - l_theta;

    // Calculate the speed dial with the new width & height
    const float l_speed_center_X = l_half_width;
    const float l_speed_center_Y = height() - l_radius;

    glNewList(m_speed_dial, GL_COMPILE);

    // Calculate the coordinates of the points on the upper arc
    float *l_speed_upper_arc_X;
    float *l_speed_upper_arc_Y;
    l_speed_upper_arc_X = new float[width()];
    l_speed_upper_arc_Y = new float[width()];
    float l_speed_upper_arc_theta = -l_theta;
    float l_speed_upper_arc_theta_step = (2 * l_theta) / width();
    for(int l_x = 0; l_x < width(); l_x++)
    {
        l_speed_upper_arc_X[l_x] = l_x;
        l_speed_upper_arc_Y[l_x] = l_speed_center_Y + (sin(0.5 * PI - l_speed_upper_arc_theta) * l_radius);
        l_speed_upper_arc_theta += l_speed_upper_arc_theta_step;
    }

    // Calculate the white semicircle of the dial
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 255);
    glVertex2f(l_speed_center_X, l_speed_center_Y);
    for(int l_x = 0; l_x < width(); l_x++)
    {
        glVertex2f(l_speed_upper_arc_X[l_x], l_speed_upper_arc_Y[l_x]);
    }
    glEnd();

    // Calculate the black outline of the dial
    glBegin(GL_LINE_LOOP);
    glColor3ub(0, 0, 0);
    glVertex2f(l_speed_center_X, l_speed_center_Y);
    for (int l_x = 0; l_x < width(); l_x++)
    {
        glVertex2f(l_speed_upper_arc_X[l_x], l_speed_upper_arc_Y[l_x]);
    }
    glEnd();

    // Calculate the black arc inside the dial
    glBegin(GL_LINE_STRIP);
    glColor3ub(0, 0, 0);
    float l_speed_lower_arc_theta = -l_theta;
    float l_speed_lower_arc_theta_step = (2 * l_theta) / l_half_width;
    for(float l_x = 0.25 * width(); l_x < 0.75 * width(); l_x++)
    {
        glVertex2f(l_x, l_speed_center_Y + (0.5 * sin(0.5 * PI - l_speed_lower_arc_theta) * l_radius));
        l_speed_lower_arc_theta += l_speed_lower_arc_theta_step;
    }
    glEnd();

    // Calculate line markings & text labels
    float l_speed_step = (2 * l_theta) / 14.0;
    m_speed_label_counter = 0;
    const char * l_speed_label_lookup[13] =
    {
        "12   ",
        "11",
        "10",
        "9",
        "8",
        "7",
        "6",
        "5",
        "4",
        "3",
        "2",
        "1",
        "0"
    };

    for(int l_j = 0; l_j < 13; l_j++)
    {
        float l_i = l_rho + l_speed_step * (l_j + 1);
        float l_start_X = l_speed_center_X + l_radius * cos(l_i);
        float l_start_Y = l_speed_center_Y + l_radius * sin(l_i);
        if(width() < 175)
        {
            // Small dial
            if((l_j % 4) == 0)
            {
                // Bigger marking + text label
                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.05 * (l_speed_center_X - l_start_X)
                          ,l_start_Y + 0.05 * (l_speed_center_Y - l_start_Y)
                          );
                glEnd();

                m_speed_labels[m_speed_label_counter++].set( l_speed_label_lookup[l_j]
                                                           , l_start_X + 0.08 * (l_speed_center_X - l_start_X)
                                                           , l_start_Y + 0.08 * (l_speed_center_Y - l_start_Y)
                                                           );
            }
            else
            {
                // Smaller marking, no text label
                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.03 * (l_speed_center_X - l_start_X)
                          ,l_start_Y + 0.03 * (l_speed_center_Y - l_start_Y)
                          );
                glEnd();
            }
        }
        else if(width() < 300)
        {
            // Bigger dial
            if((l_j % 2) == 0)
            {
                // Bigger marking + text label
                l_speed_label_lookup[0] = "12";

                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.05 * (l_speed_center_X - l_start_X)
                          ,l_start_Y + 0.05 * (l_speed_center_Y - l_start_Y)
                          );
                glEnd();

                m_speed_labels[m_speed_label_counter++].set( l_speed_label_lookup[l_j]
                                                           , l_start_X + 0.08 * (l_speed_center_X - l_start_X)
                                                           , l_start_Y + 0.08 * (l_speed_center_Y - l_start_Y)
                                                           );
            }
            else
            {
                // Smaller marking, no text label
                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.03 * (l_speed_center_X - l_start_X)
                          ,l_start_Y + 0.03 * (l_speed_center_Y - l_start_Y)
                          );
                glEnd();
            }
        }
        else
        {
            // Big dial
            // Bigger marking + text label
            l_speed_label_lookup[0] = "12";

            glBegin(GL_LINES);
            glColor3ub(0, 0, 0);
            glVertex2f(l_start_X, l_start_Y);
            glVertex2f(l_start_X + 0.05 * (l_speed_center_X - l_start_X)
                      ,l_start_Y + 0.05 * (l_speed_center_Y - l_start_Y)
                      );
            glEnd();

            m_speed_labels[m_speed_label_counter++].set( l_speed_label_lookup[l_j]
                                                       , l_start_X + 0.08 * (l_speed_center_X - l_start_X)
                                                       , l_start_Y + 0.08 * (l_speed_center_Y - l_start_Y)
                                                       );
        }
    }

    // Calculate the "Hz" label
    m_hz_label_X = l_speed_center_X;
    m_hz_label_Y = l_speed_center_Y + 0.7 * l_radius;

    glEndList();

    delete[] l_speed_upper_arc_X;
    delete[] l_speed_upper_arc_Y;

    // Calculate the width dial with the new width & height
    const float l_width_center_X = l_half_width;
    const float l_width_center_Y = l_half_height - l_radius;

    glNewList(m_width_dial, GL_COMPILE);

    // Calculate the openGL-coordinates of the points on the upper arc
    float *l_width_upper_arc_X;
    float *l_width_upper_arc_Y;
    l_width_upper_arc_X = new float[width()];
    l_width_upper_arc_Y = new float[width()];
    float l_width_upper_arc_theta = -l_theta;
    float l_width_upper_arc_theta_step = (2 * l_theta) / width();
    for(int l_x = 0; l_x < width(); l_x++)
    {
        l_width_upper_arc_X[l_x] = l_x;
        l_width_upper_arc_Y[l_x] = l_width_center_Y + (sin(0.5 * PI - l_width_upper_arc_theta) * l_radius);
        l_width_upper_arc_theta += l_width_upper_arc_theta_step;
    }

    // Calculate the white semicircle of the dial
    glBegin(GL_TRIANGLE_FAN);
    glColor3ub(255, 255, 255);
    glVertex2f(l_width_center_X, l_width_center_Y);
    for(int x = 0; x < width(); x++)
    {
        glVertex2f(l_width_upper_arc_X[x], l_width_upper_arc_Y[x]);
    }
    glEnd();

    // Calculate the black outline of the dial
    glBegin(GL_LINE_LOOP);
    glColor3ub(0, 0, 0);
    glVertex2f(l_width_center_X, l_width_center_Y);
    for(int x = 0; x < width(); x++)
    {
        glVertex2f(l_width_upper_arc_X[x], l_width_upper_arc_Y[x]);
    }
    glEnd();

    // Calculate the black arc inside the dial
    glBegin(GL_LINE_STRIP);
    glColor3ub(0, 0, 0);
    float l_width_lower_arc_theta = -l_theta;
    float l_width_lower_arc_theta_step = (2 * l_theta) / l_half_width;
    for(float l_x = 0.25 * width(); l_x < 0.75 * width(); l_x++)
    {
        glVertex2f(l_x, l_width_center_Y + (0.5 * sin(0.5 * PI - l_width_lower_arc_theta) * l_radius));
        l_width_lower_arc_theta += l_width_lower_arc_theta_step;
    }
    glEnd();

    // Calculate line markings & text labels
    float l_width_step = (2 * l_theta) / 12.0;
    m_width_label_counter = 0;
    char l_width_label[5];

    for(int l_j = 0; l_j < 11; l_j++)
    {
        float l_i = l_rho + l_width_step * (l_j + 1);
        float l_start_X = l_width_center_X + l_radius * cos(l_i);
        float l_start_Y = l_width_center_Y + l_radius * sin(l_i);
        if(width() < 250)
        {
            if((l_j % 2) == 0)
            {
                // Bigger marking + text label
                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.05 * (l_width_center_X - l_start_X)
                          ,l_start_Y + 0.05 * (l_width_center_Y - l_start_Y)
                          );
                glEnd();

                sprintf(l_width_label, "%d", m_width_limit - ((m_width_limit / 10) * l_j));
                m_width_labels[m_width_label_counter++].set( l_width_label
                                                           , l_start_X + 0.08 * (l_width_center_X - l_start_X)
                                                           , l_start_Y + 0.08 * (l_width_center_Y - l_start_Y)
                                                           );
            }
            else
            {
                // Smaller marking, no text label
                glBegin(GL_LINES);
                glColor3ub(0, 0, 0);
                glVertex2f(l_start_X, l_start_Y);
                glVertex2f(l_start_X + 0.03 * (l_width_center_X - l_start_X)
                          ,l_start_Y + 0.03 * (l_width_center_Y - l_start_Y)
                          );
                glEnd();
            }
        }
        else
        {
            // Bigger marking + text label
            glBegin(GL_LINES);
            glColor3ub(0, 0, 0);
            glVertex2f(l_start_X, l_start_Y);
            glVertex2f(l_start_X + 0.05 * (l_width_center_X - l_start_X)
                      ,l_start_Y + 0.05 * (l_width_center_Y - l_start_Y)
                      );
            glEnd();

            sprintf(l_width_label, "%d", m_width_limit - ((m_width_limit / 10) * l_j));
            m_width_labels[m_width_label_counter++].set( l_width_label
                                                       , l_start_X + 0.08 * (l_width_center_X - l_start_X)
                                                       , l_start_Y + 0.08 * (l_width_center_Y - l_start_Y)
                                                       );
        }
    }

    // Calculate the "Cents" label
    m_cents_label_X = l_width_center_X;
    m_cents_label_Y = l_width_center_Y + 0.7 * l_radius;

    glEndList();

    delete[] l_width_upper_arc_X;
    delete[] l_width_upper_arc_Y;

    // Do forced update on resize
    m_prev_vibrato_speed = -999;
    m_prev_vibrato_width = -999;
    doUpdate();
}

//------------------------------------------------------------------------------
void VibratoSpeedWidget::paintGL(void)
{
    QColor l_bg = g_data->backgroundColor();
    glClearColor( double(l_bg.red()) / 256.0, double(l_bg.green()) / 256.0, double(l_bg.blue()) / 256.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT);

    QFontMetrics l_font_metric = QFontMetrics(m_speed_width_font);

    // Draw the speed dial
    glLineWidth(1.5);
    glCallList(m_speed_dial);

    // Draw the labels for the speed dial
    glColor3ub(0,0,0);
    renderText(m_hz_label_X - (0.5 * l_font_metric.width("Hz")), m_hz_label_Y, 0, "Hz", m_speed_width_font);
    for (int l_index = 0; l_index < m_speed_label_counter; l_index++)
    {
        renderText(m_speed_labels[l_index].get_x() - (0.5 * l_font_metric.width(m_speed_labels[l_index].get_label()))
                  ,m_speed_labels[l_index].get_y() - 8
                  ,0
                  ,m_speed_labels[l_index].get_label()
                  ,m_speed_width_font
                  );
    }

    // Draw the speed needle
    glLineWidth(1.0);
    glCallList(m_speed_needle);

    // Draw the width dial
    glLineWidth(1.5);
    glCallList(m_width_dial);

    // Draw the labels for the width dial
    glColor3ub(0,0,0);
    renderText(m_cents_label_X - (0.5 * l_font_metric.width("Cents"))
              ,m_cents_label_Y
              ,0
              ,"Cents"
              ,m_speed_width_font
              );
    for(int l_index = 0; l_index < m_width_label_counter; l_index++)
    {
        renderText(m_width_labels[l_index].get_x() - (0.5 * l_font_metric.width(m_width_labels[l_index].get_label()))
                  ,m_width_labels[l_index].get_y() - 8
                  ,0
                  ,m_width_labels[l_index].get_label()
                  ,m_speed_width_font
                  );
    }

    // Draw the width needle
    glLineWidth(1.0);
    glCallList(m_width_needle);
}

//------------------------------------------------------------------------------
void VibratoSpeedWidget::doUpdate(void)
{
    Channel * l_active_channel = g_data->getActiveChannel();

    float l_vibrato_speed = 0;
    float l_vibrato_width = 0;
    int l_current_note_number = -1;

    if(l_active_channel)
    {
        AnalysisData * l_data;
        if(g_data->getSoundMode() & SOUND_REC)
        {
            l_data = l_active_channel->dataAtChunk(l_active_channel->chunkAtCurrentTime() - l_active_channel->pronyDelay());
        }
        else
        {
            l_data = l_active_channel->dataAtCurrentChunk();
        }
        if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
        {
            const NoteData * l_note = new NoteData();
            l_note = &(l_active_channel->get_note_data()[l_data->getNoteIndex()]);

            l_current_note_number = l_data->getNoteIndex();
            if (m_use_prony)
            {
                l_vibrato_speed = l_data->getVibratoSpeed();
                l_vibrato_width = 200 * l_data->getVibratoWidth();
            }
            else if((l_active_channel->doingDetailedPitch()) && (l_active_channel->get_pitch_lookup_smoothed().size() > 0))
            {
                large_vector<float> l_pitch_lookup_used = l_active_channel->get_pitch_lookup_smoothed();
                int l_smooth_delay = l_active_channel->get_pitch_big_smoothing_filter().delay();

                int l_current_time = l_active_channel->chunkAtCurrentTime() * l_active_channel->framesPerChunk() + l_smooth_delay;
                int l_maximum_time = 0;
                int l_minimum_time = 0;
                int l_maxima_size = l_note->get_maxima()->size();
                int l_minima_size = l_note->get_minima()->size();

                // Find the most recent maximum
                for (int l_index = 1; l_index < l_maxima_size; l_index++)
                {
                    if((l_current_time > l_note->get_maxima()->at(l_index - 1)) && (l_current_time <= l_note->get_maxima()->at(l_index)))
                    {
                        l_maximum_time = l_note->get_maxima()->at(l_index - 1);
                        break;
                    }
                }
                if((l_maxima_size > 0) && (l_current_time > l_note->get_maxima()->at(l_maxima_size - 1)))
                {
                    l_maximum_time = l_note->get_maxima()->at(l_maxima_size - 1);
                }

                // Find the most recent minimum
                for(int l_index = 1; l_index < l_note->get_minima()->size(); l_index++)
                {
                    if((l_current_time > l_note->get_minima()->at(l_index - 1)) && (l_current_time <= l_note->get_minima()->at(l_index)))
                    {
                        l_minimum_time = l_note->get_minima()->at(l_index - 1);
                        break;
                    }
                }
                if ((l_note->get_minima()->size() > 0) && (l_current_time > l_note->get_minima()->at(l_minima_size - 1)))
                {
                    l_minimum_time = l_note->get_minima()->at(l_minima_size - 1);
                }
                if ((l_maximum_time !=0 ) && (l_minimum_time != 0))
                {
                    // The speed and width can be calculated
                    l_vibrato_speed = l_active_channel->rate() / (float)(2 * abs(l_maximum_time - l_minimum_time));
                    l_vibrato_width = fabs(100 * (l_pitch_lookup_used.at(l_maximum_time) - l_pitch_lookup_used.at(l_minimum_time)));
                }
            }
        }
    }

    if((fabs(m_prev_vibrato_speed - l_vibrato_speed) < 0.05) && (fabs(m_prev_vibrato_width - l_vibrato_width) < 0.05))
    {
        // Needle values haven't changed (much) , no update needed
    }
    else
    {
        // Needle values have changed
        makeCurrent();

        glNewList(m_speed_needle, GL_COMPILE);
        glEndList();

        glNewList(m_width_needle, GL_COMPILE);
        glEndList();

        if((l_vibrato_speed == 0) && (l_vibrato_width == 0))
        {
            // No needle values, don't draw the needles this update
            m_prev_vibrato_speed = 0;
            m_prev_vibrato_width = 0;

            m_speed_value_to_draw = 0;
            m_width_value_to_draw = 0;
            m_prev_note_number = -1;

            updateGL();

        }
        else
        {
            // Needle values, draw the needles this update
            m_prev_vibrato_speed = l_vibrato_speed;
            m_prev_vibrato_width = l_vibrato_width;

            m_speed_value_to_draw = l_vibrato_speed;
            m_width_value_to_draw = l_vibrato_width;

            const float l_half_width = 0.5 * width();
            const float l_half_height = 0.5 * height();
            const float l_radius = 1.8 * MAX(0.5 * height(), l_half_width);
            const float l_theta = asin(float(width()) / (2 * l_radius));
            const float l_rho = (PI * 0.5) - l_theta;
            const int halfKnobWidth = MAX(toInt(l_radius * 0.02), 1);

            // Calculate the speed needle, if m_speed_value_to_draw is set to sensible value
            if((m_speed_value_to_draw > 0) && (m_speed_value_to_draw <= 12))
            {
                const float l_center_X = l_half_width;
                const float l_center_Y = height() - l_radius;

                float l_hz_angle = (2 * l_theta) / 14.0;
                float l_note = PI - l_rho - (m_speed_value_to_draw * l_hz_angle) - l_hz_angle;

                float l_note_X = l_center_X + l_radius * cos(l_note);
                float l_note_Y = l_center_Y + l_radius * sin(l_note);
                float l_knob_left_X = l_center_X - halfKnobWidth;
                float l_knob_left_Y = l_center_Y;
                float l_knob_right_X = l_center_X + halfKnobWidth;
                float l_knob_right_Y = l_center_Y;

                glNewList(m_speed_needle, GL_COMPILE);

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

            // Determine whether the scale needs adjusting
            int l_old_width_imit = m_width_limit;
            if((m_width_value_to_draw > 0) && (l_current_note_number == m_prev_note_number))
            {
                if(m_width_value_to_draw > 50)
                {
                    m_width_limit = std::max(m_width_limit, 100);
                }
                if(m_width_value_to_draw > 100)
                {
                    m_width_limit = std::max(m_width_limit, 200);
                }
                if(m_width_value_to_draw > 200)
                {
                    m_width_limit = std::max(m_width_limit, 300);
                }
            }
            if(m_prev_note_number != l_current_note_number)
            {
                m_width_limit = 50;
            }
            if(m_width_limit != l_old_width_imit)
            {
                // Do resize and redraw the dial if the scale is adjusted
                resizeGL(width(), height());
            }
            m_prev_note_number = l_current_note_number;

            // Calculate the width needle, if m_width_value_to_draw is set to sensible value
            if((m_width_value_to_draw > 0) && (m_width_value_to_draw <= m_width_limit))
            {
                const float l_center_X = l_half_width;
                const float l_center_Y = l_half_height - l_radius;

                float l_cent_angle = (2 * l_theta) / (m_width_limit + 2 * (m_width_limit / 10));
                float l_note = PI - l_rho - (m_width_value_to_draw * l_cent_angle) - (m_width_limit / 10) * l_cent_angle;
                int l_half_knob_width = MAX(toInt(l_radius * 0.02), 1);

                float l_note_X = l_center_X + l_radius * cos(l_note);
                float l_note_Y = l_center_Y + l_radius * sin(l_note);
                float l_knob_left_X = l_center_X - l_half_knob_width;
                float l_knob_left_Y = l_center_Y;
                float l_knob_right_X = l_center_X + l_half_knob_width;
                float l_knob_right_Y = l_center_Y;

                glNewList(m_width_needle, GL_COMPILE);

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
void VibratoSpeedWidget::setUseProny(bool p_value)
{
    m_use_prony = p_value;
    m_prev_vibrato_speed = -999;
    m_prev_vibrato_width = -999;
    doUpdate();
}

//------------------------------------------------------------------------------
QSize VibratoSpeedWidget::minimumSizeHint(void) const
{
    return QSize(100, 75);
}

//------------------------------------------------------------------------------
void
VibratoSpeedWidget::labelStruct::set( const QString & p_label
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
VibratoSpeedWidget::labelStruct::get_label() const
{
    return m_label;
}

//------------------------------------------------------------------------------
float
VibratoSpeedWidget::labelStruct::get_x() const
{
    return m_x;
}

//------------------------------------------------------------------------------
float
VibratoSpeedWidget::labelStruct::get_y() const
{
    return m_y;
}
// EOD
