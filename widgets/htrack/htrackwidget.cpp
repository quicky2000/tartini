/***************************************************************************
                          htrackwidget.cpp  -  description
                             -------------------
    begin                : Aug 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <QMouseEvent>
#include <QWheelEvent>

#include "htrackwidget.h"
#include "gdata.h"
#include "useful.h"
#include "channel.h"
#include "array1d.h"
#include "piano3d.h"
#include "musicnotes.h"
#include <glu.h>
#include <gl.h>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
HTrackWidget::HTrackWidget( QWidget *p_parent
                          , const std::string & p_name
                          )
: QGLWidget(p_parent)
{
    setObjectName(p_name.c_str());
}

//------------------------------------------------------------------------------
HTrackWidget::~HTrackWidget()
{
    delete m_piano_3d;
}

//------------------------------------------------------------------------------
void HTrackWidget::initializeGL()
{
    setPeakThreshold(0.05f);
    setDistanceAway(1500.0);
    setViewAngleVertical(-35.0);
    setViewAngleHorizontal(20.0);
    m_translate_X = 0.0;
    m_translate_Y = -60.0;

    m_piano_3d = new Piano3d();
    glEnable(GL_NORMALIZE);
    // Set up the rendering context, define display lists etc.:
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, m_g_current_matrix);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    setLightSpecular(0.5, 0.5, 0.5);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

//------------------------------------------------------------------------------
void HTrackWidget::resizeGL( int p_w
                           , int p_h
                           )
{
    // setup viewport, projection etc.:
    glViewport( 0, 0, static_cast<GLint>(p_w), static_cast<GLint>(p_h) );
}

//------------------------------------------------------------------------------
void HTrackWidget::paintGL()
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();

    QColor l_background = GData::getUniqueInstance().backgroundColor();
    glClearColor( double(l_background.red()) / 256.0, double(l_background.green()) / 256.0, double(l_background.blue()) / 256.0, 0.0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double l_near_plane = 100.0;
    double l_w2 = double(width()) / 2.0;
    double l_h2 = double(height()) / 2.0;
    double l_ratio = l_near_plane / double(width());
    glFrustum((-l_w2 - m_translate_X) * l_ratio, (l_w2 - m_translate_X) * l_ratio, (-l_h2 - m_translate_Y) * l_ratio, (l_h2 - m_translate_Y) * l_ratio, l_near_plane, 10000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double l_center_x = 0.0;
    double l_center_z = 0.0;
    gluLookAt(0.0, 0.0/*m_distance_away*/, m_distance_away, l_center_x, 0.0, l_center_z, 0.0, 1.0, 0.0);
    glRotated(-viewAngleVertical(), 1.0, 0.0, 0.0);
    glRotated(viewAngleHorizontal(), 0.0, 1.0, 0.0);

    setLightPosition(0.0, 2000.0, -1600.0);
    setLightAmbient(0.4f, 0.4f, 0.4f);
    setLightDiffuse(0.9f, 0.9f, 0.9f);

    glPushMatrix();

    double l_piano_width = m_piano_3d->pianoWidth();
    //set center of keyboard at origin
    glTranslatef(-l_piano_width / 2.0, 0.0, 0.0);

    m_piano_3d->setAllKeyStatesOff();
    if(l_active_channel)
    {
        AnalysisData *l_data = l_active_channel->dataAtCurrentChunk();
        if(l_data && l_active_channel->isVisibleChunk(l_data))
        {
            m_piano_3d->setMidiKeyState(toInt(l_data->getPitch()), true);
        }
    }
    glLineWidth(0.01f);
    m_piano_3d->draw();

    setLightDirection(-1.0, 0.0, 0.0);
    setLightAmbient(0.2f, 0.2f, 0.2f);
    setLightDiffuse(0.9f, 0.9f, 0.9f);

    glTranslatef(-m_piano_3d->get_first_key_offset(), 0.0, 0.0);
    //set a scale of 1 semitime = 1 unit
    glScaled(m_piano_3d->octaveWidth() / 12.0, 200.0, 5.0);

    glColor4f( 0.3f, 0.3f, 0.3f, 1.0f );
    glLineWidth(1.0);


    if(l_active_channel)
    {
        l_active_channel->lock();
        double l_pos;
        int l_j;
        unsigned int l_num_harmonics = 40;
        int l_visible_chunks = 512; //128;
        Array2d<float> l_pitches(l_num_harmonics, l_visible_chunks);
        Array2d<float> l_amps(l_num_harmonics, l_visible_chunks);
    
        AnalysisData *l_data;
        int l_finish_chunk = l_active_channel->currentChunk();
        int l_start_chunk = l_finish_chunk - l_visible_chunks;
        unsigned int l_harmonic;
        int l_chunk_offset;
    
        //draw the time ref lines
        glBegin(GL_LINES);
        for(l_j = roundUp(l_start_chunk, 16); l_j<l_finish_chunk; l_j += 16)
        {
            if(l_active_channel->isValidChunk(l_j))
            {
                glVertex3f(m_piano_3d->firstKey(), 0.0, double(l_j - l_finish_chunk));
                glVertex3f(m_piano_3d->firstKey() + m_piano_3d->numKeys(), 0.0, double(l_j - l_finish_chunk));
            }
        }
        glEnd();
    
        //build a table of frequencies and amplitudes for faster drawing
        for(l_chunk_offset = 0; l_chunk_offset < l_visible_chunks; l_chunk_offset++)
        {
            l_data = l_active_channel->dataAtChunk(l_start_chunk + l_chunk_offset);
            if(l_data && l_data->getHarmonicFreqSize() > 0)
            {
                for(l_harmonic=0; l_harmonic < l_num_harmonics; l_harmonic++)
                {
                    l_pitches(l_harmonic, l_chunk_offset) = freq2pitch(l_data->getHarmonicFreqAt(l_harmonic));
                    l_amps(l_harmonic, l_chunk_offset) = l_data->getHarmonicAmpAt(l_harmonic);
                }
            }
            else
                {
                for(l_harmonic=0; l_harmonic < l_num_harmonics; l_harmonic++)
                {
                    l_pitches(l_harmonic, l_chunk_offset) = 0.0;
                    l_amps(l_harmonic, l_chunk_offset) = 0.0;
                }
                }
        }

        bool l_inside_line;
        float l_cur_pitch=0.0, l_cur_amp=0.0, l_prev_pitch, l_diff_note;
    
        //draw the outlines
        setMaterialSpecular(0.0, 0.0, 0.0, 0.0);
        setMaterialColor(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0);
        for(l_harmonic = 0; l_harmonic < l_num_harmonics; l_harmonic++)
        {
            l_inside_line = false;
            l_pos = -double(l_visible_chunks - 1);
            for(l_chunk_offset = 0; l_chunk_offset < l_visible_chunks; l_chunk_offset++, l_pos++)
            {
                l_cur_amp = l_amps(l_harmonic, l_chunk_offset) - m_peak_threshold;
                if(l_cur_amp > 0.0)
                {
                    l_prev_pitch = l_cur_pitch;
                    l_cur_pitch = l_pitches(l_harmonic, l_chunk_offset);
                    l_diff_note = l_prev_pitch - l_cur_pitch;
                    if(fabs(l_diff_note) < 1.0)
                    {
                        if(!l_inside_line)
                        {
                            glBegin(GL_LINE_STRIP);
                            glVertex3f(l_cur_pitch, 0, l_pos);
                            l_inside_line = true;
                        }
                        glVertex3f(l_cur_pitch, l_cur_amp, l_pos);
                    }
                    else
                    {
                        glEnd();
                        l_inside_line = false;
                    }
                }
                else
                {
                    if(l_inside_line)
                    {
                        glVertex3f(l_cur_pitch, 0, l_pos - 1);
                        glEnd();
                        l_inside_line = false;
                    }
                }
            }
            if(l_inside_line)
            {
                glVertex3f(l_cur_pitch, 0, l_pos - 1);
                glEnd();
                l_inside_line = false;
            }
        }

        //draw the faces
        glShadeModel(GL_FLAT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for(l_harmonic = 0; l_harmonic < l_num_harmonics; l_harmonic++)
        {
            if(l_harmonic % 2 == 0)
            {
                setMaterialColor(0.5f, 0.5f, 0.9f);
            }
            else
            {
                setMaterialColor(0.5f, 0.9f, 0.5f);
            }
            l_inside_line = false;
            l_pos = -double(l_visible_chunks - 1);
            for(l_chunk_offset = 0; l_chunk_offset < l_visible_chunks; l_chunk_offset++, l_pos++)
            {
                if(l_amps(l_harmonic, l_chunk_offset) > m_peak_threshold)
                {
                    if(!l_inside_line)
                    {
                        glBegin(GL_QUAD_STRIP);
                        l_inside_line = true;
                        l_cur_pitch = l_pitches(l_harmonic, l_chunk_offset);
                        glVertex3f(l_cur_pitch, (l_amps(l_harmonic, l_chunk_offset) - m_peak_threshold), l_pos);
                        glVertex3f(l_cur_pitch, 0, l_pos);
                    }
                    else
                    {
                        l_prev_pitch = l_cur_pitch;
                        l_cur_pitch = l_pitches(l_harmonic, l_chunk_offset);
                        l_diff_note = l_prev_pitch - l_cur_pitch;
                        if(fabs(l_diff_note) < 1.0)
                        {
                            glNormal3f(l_diff_note ,0.0 ,1.0);
                            glVertex3f(l_cur_pitch, (l_amps(l_harmonic, l_chunk_offset) - m_peak_threshold), l_pos);
                            glVertex3f(l_cur_pitch, 0, l_pos);
                        }
                        else
                        {
                            glEnd();
                            l_inside_line = false;
                        }
                    }
                }
                else
                {
                    if(l_inside_line)
                    {
                        glEnd();
                        l_inside_line = false;
                    }
                }
            }
            if(l_inside_line)
            {
                glEnd();
                l_inside_line = false;
            }
        }
        l_active_channel->unlock();
    }
    glPopMatrix();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: HTrackWidget::paintGL()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
void HTrackWidget::rotateX(const double & p_angle)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();
    glRotatef(p_angle, 1.0, 0.0, 0.0);
    glMultMatrixf(m_g_current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_g_current_matrix);

    glPopMatrix();
}

//------------------------------------------------------------------------------
void HTrackWidget::home()
{
    setPeakThreshold(0.05f);
    setDistanceAway(1500.0);
    setViewAngleVertical(-35.0);
    setViewAngleHorizontal(20.0);
    m_translate_X = 0.0;
    m_translate_Y = -60.0;
}

//------------------------------------------------------------------------------
void HTrackWidget::rotateY(const double & p_angle)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();
    glRotatef(p_angle, 0.0, 1.0, 0.0);
    glMultMatrixf(m_g_current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_g_current_matrix);

    glPopMatrix();
}

//------------------------------------------------------------------------------
void HTrackWidget::translate( float p_x
                            , float p_y
                            , float p_z
                            )
{
    m_translate_X += p_x;
    m_translate_Y += p_y;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();
    glTranslatef(p_x, p_y, p_z);
    glMultMatrixf(m_g_current_matrix);
    glGetFloatv(GL_MODELVIEW_MATRIX, m_g_current_matrix);

    glPopMatrix();
}

//------------------------------------------------------------------------------
void HTrackWidget::mousePressEvent( QMouseEvent *p_event)
{
    m_mouse_down = true;
    m_mouse_X = p_event->x();
    m_mouse_Y = p_event->y();
}

//------------------------------------------------------------------------------
void HTrackWidget::mouseMoveEvent( QMouseEvent *p_event)
{
    if(m_mouse_down)
    {
        translate(float(p_event->x() - m_mouse_X), -float(p_event->y() - m_mouse_Y), 0.0);

        m_mouse_X = p_event->x();
        m_mouse_Y = p_event->y();
        update();
    }
}

//------------------------------------------------------------------------------
void HTrackWidget::mouseReleaseEvent( QMouseEvent * )
{
    m_mouse_down = false;
}

//------------------------------------------------------------------------------
void HTrackWidget::wheelEvent(QWheelEvent * p_event)
{
    setDistanceAway(m_distance_away * pow(2.0, -(double(p_event->delta()) / double(WHEEL_DELTA)) / 20.0));
    update();
    p_event->accept();
}

//EOF
