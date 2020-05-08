/***************************************************************************
                          freqdrawwidget.cpp  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
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
#include <QCursor>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPaintEvent>

// zoom cursors
#include "pics/zoomx.xpm"
#include "pics/zoomy.xpm"
#include "pics/zoomxout.xpm"
#include "pics/zoomyout.xpm"

#include "freqwidgetGL.h"
#include "mygl.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "myqt.h"
#include "array1d.h"
#include "musicnotes.h"
#include "music_scale.h"
#include "music_key.h"

#include <sstream>
#include <QtGlobal>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
FreqWidgetGL::FreqWidgetGL( QWidget * /*parent*/
                          , const char* /*name*/
                          )
: m_drag_mode(t_drag_modes::DragNone)
, m_mouse_X(0)
, m_mouse_Y(0)
, m_down_time(0.0)
, m_down_note(0.0)
{
    setMouseTracking(true);

    QSizePolicy l_size_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l_size_policy.setHeightForWidth(false);
    setSizePolicy(l_size_policy);

    setFocusPolicy(Qt::StrongFocus);
    g_data->getView().setPixelHeight(height());
}

//------------------------------------------------------------------------------
void FreqWidgetGL::initializeGL()
{
    qglClearColor(g_data->backgroundColor());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
}

//------------------------------------------------------------------------------
void FreqWidgetGL::resizeGL( int p_w
                           , int p_h
                           )
{
    mygl_resize2d(p_w, p_h);
}

//------------------------------------------------------------------------------
FreqWidgetGL::~FreqWidgetGL()
{
}

//------------------------------------------------------------------------------
void FreqWidgetGL::drawReferenceLinesGL( const double & /* p_left_time*/
                                       , const double & p_current_time
                                       , const double & p_zoom_X
                                       , const double & p_view_bottom
                                       , const double & p_zoom_Y
                                       , int /* p_view_type*/
                                       )
{
    // Draw the lines and notes
    QFontMetrics l_font_metric = fontMetrics();
    int l_font_height_space = l_font_metric.height() / 4;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int l_font_width = l_font_metric.horizontalAdvance("C#0") + 3;
#else // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    int l_font_width = l_font_metric.width("C#0") + 3;
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)

    MusicKey &l_music_key = g_music_keys[g_data->temperedType()];
    MusicScale &l_music_scale = g_music_scales[g_data->musicKeyType()];

    int l_key_root = cycle(g_music_key_root[g_data->musicKey()] + l_music_scale.semitoneOffset(), 12);
    int l_view_bottom_note = (int)p_view_bottom - l_key_root;
    int l_remainder = cycle(l_view_bottom_note, 12);
    int l_low_root = l_view_bottom_note - l_remainder + l_key_root;
    int l_root_octave = l_low_root / 12;
    int l_root_offset = cycle(l_low_root, 12);
    int l_num_octaves = int(ceil(p_zoom_Y * (double)height() / 12.0)) + 1;
    int l_top_octave = l_root_octave + l_num_octaves;
    double l_line_Y;
    double l_cur_root;
    double l_cur_pitch;
    int l_stipple_offset = toInt(p_current_time / p_zoom_X) % 16;
    QString l_note_label;
    int l_name_index;

    //Draw the horizontal reference lines
    for(int l_octave = l_root_octave; l_octave < l_top_octave; ++l_octave)
    {
        l_cur_root = double(l_octave * 12 + l_root_offset);
        for(int l_j = 0; l_j < l_music_key.size(); l_j++)
        {
            if(l_music_scale.hasSemitone(l_music_key.noteType(l_j)))
            {
                //draw it
                l_cur_pitch = l_cur_root + l_music_key.noteOffset(l_j);
                l_line_Y = double(height()) - myround((l_cur_pitch - p_view_bottom) / p_zoom_Y);
                if(l_j == 0)
                {
                    //root note
                    glColor4ub(0, 0, 0, 128);
                    mygl_line(l_font_width, l_line_Y, width() - 1, l_line_Y);
                }
                else if((g_data->musicKeyType() == MusicScale::Chromatic) && !g_music_scales[MusicScale::Major].hasSemitone(l_music_key.noteType(l_j)))
                {
                    glColor4ub(25, 125, 170, 128);
                    glEnable(GL_LINE_STIPPLE);
                    // bitpattern 64716 = 1111110011001100
                    glLineStipple(1, my_wrap_right(0xFCFC, l_stipple_offset));
                    mygl_line(l_font_width, l_line_Y, width() - 1, l_line_Y);
                    glDisable(GL_LINE_STIPPLE);
                }
                else
                {
                    glColor4ub(25, 125, 170, 196);
                    glEnable(GL_LINE_STIPPLE);
                    glLineStipple(1, my_wrap_right(0xFFFC, l_stipple_offset));
                    mygl_line(l_font_width, l_line_Y, width() - 1, l_line_Y);
                    glDisable(GL_LINE_STIPPLE);
                }
            }
            if(p_zoom_Y > 0.1)
            {
                break;
            }
        }
    }

    //Draw the text labels down the left hand side
    for(int l_octave = l_root_octave; l_octave < l_top_octave; ++l_octave)
    {
        l_cur_root = double(l_octave * 12 + l_root_offset);
        for(int l_j = 0; l_j < l_music_key.size(); l_j++)
        {
            if(l_music_scale.hasSemitone(l_music_key.noteType(l_j)))
            {
                //draw it
                l_cur_pitch = l_cur_root + l_music_key.noteOffset(l_j);
                l_line_Y = double(height()) - myround((l_cur_pitch - p_view_bottom) / p_zoom_Y);
                l_name_index = toInt(l_cur_pitch);
                glColor3ub(0, 0, 0);
                std::stringstream l_stream ;
                l_stream << music_notes::noteName(l_name_index) << noteOctave(l_name_index);
                l_note_label = QString(l_stream.str().c_str());
                renderText(2, toInt(l_line_Y) + l_font_height_space, l_note_label);
            }
            if(p_zoom_Y > 0.1)
            {
                break;
            }
        }
    }

    //Draw the vertical reference lines
    //time per 150 pixels
    double l_time_step = timeWidth() / double(width()) * 150.0;
    //round down to the nearest power of 10
    double l_time_scale_base = pow10(floor(log10(l_time_step)));

    //choose a timeScaleStep which is a multiple of 1, 2 or 5 of l_time_scale_base
    int l_large_freq;
    if(l_time_scale_base * 5.0 < l_time_step)
    {
        l_large_freq = 5;
    }
    else if(l_time_scale_base * 2.0 < l_time_step)
    {
        l_large_freq = 2;
    }
    else
    {
        l_large_freq = 2;
        l_time_scale_base /= 2;
    }

    //calc the first one just off the left of the screen
    double l_time_pos = floor(leftTime() / (l_time_scale_base*l_large_freq)) * (l_time_scale_base*l_large_freq);
    int l_x;
    int l_large_counter = -1;
    double l_ratio = double(width()) / timeWidth();
    double l_left_time = leftTime();

    for(; l_time_pos <= rightTime(); l_time_pos += l_time_scale_base)
    {
        if(++l_large_counter == l_large_freq)
        {
            l_large_counter = 0;
            //draw the darker lines
            glColor4ub(25, 125, 170, 128);
        }
        else
        {
            //draw the lighter lines
            glColor4ub(25, 125, 170, 64);
        }
        l_x = toInt((l_time_pos-l_left_time) * l_ratio);
        mygl_line(l_x, 0, l_x, height() - 1);
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::paintGL()
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT
    
    glClear(GL_COLOR_BUFFER_BIT);
    View & l_view = g_data->getView();

    if(l_view.autoFollow() && g_data->getActiveChannel() && g_data->getRunning() == GData::RunningMode::STREAM_FORWARD)
    {
        setChannelVerticalView(g_data->getActiveChannel(), l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY());
    }

    int l_cur_time_pixel = l_view.screenPixelX(l_view.currentTime());

    glLineWidth(3.0f);

    //draw the red/blue background color shading if needed
    if(l_view.backgroundShading() && g_data->getActiveChannel())
    {
        drawChannelFilledGL(g_data->getActiveChannel(), l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);
    }

    glLineWidth(1.0f);
    drawReferenceLinesGL(l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);

    glEnable(GL_LINE_SMOOTH);
    //draw all the visible channels
    for(unsigned int l_i = 0; l_i < g_data->getChannelsSize(); l_i++)
    {
        Channel *l_channel = g_data->getChannelAt(l_i);
        if(!l_channel->isVisible())
        {
            continue;
        }
        drawChannelGL(l_channel, l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);
    }
    glDisable(GL_LINE_SMOOTH);

    QPalette l_palette;
    QColor l_foreground = l_palette.color(QPalette::WindowText);
    // Draw a light grey band indicating which time is being used in the current window
    if(g_data->getActiveChannel())
    {
        QColor l_line_color = l_foreground;
        l_line_color.setAlpha(50);
        Channel *l_channel = g_data->getActiveChannel();
        double l_half_window_time = (double)l_channel->size() / (double)(l_channel->rate() * 2);
        int l_pixel_left = l_view.screenPixelX(l_view.currentTime() - l_half_window_time);
        int l_pixel_right = l_view.screenPixelX(l_view.currentTime() + l_half_window_time);
        qglColor(l_line_color);
        mygl_rect(l_pixel_left, 0, l_pixel_right - l_pixel_left, height() - 1);
    }

    // Draw the current time line
    qglColor(l_foreground);
    glLineWidth(1.0f);
    mygl_line(l_cur_time_pixel, 0, l_cur_time_pixel, height() - 1);

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: FreqWidgetGL::paintGL()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
Channel *FreqWidgetGL::channelAtPixel( int p_x
                                     , int p_y
                                     )const
{
    double l_time = mouseTime(p_x);
    float l_pitch = mousePitch(p_y);
    //10 pixel l_tolerance
    float l_tolerance = 6 * g_data->getView().zoomY();

    std::vector<Channel*> l_channels;

    //loop through l_channels in reverse order finding which one the user clicked on
    unsigned int l_index = 0;
    for(l_index = 0 ; l_index < g_data->getChannelsSize() ; ++l_index)
    {
        Channel * l_channel = g_data->getChannelAt(g_data->getChannelsSize() - 1 - l_index);
        if(l_channel->isVisible())
        {
            const AnalysisData *l_data = l_channel->dataAtTime(l_time);
            if(l_data && within(l_tolerance, l_data->getPitch(), l_pitch))
            {
                return l_channel;
            }
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------
void FreqWidgetGL::mousePressEvent( QMouseEvent *p_mouse_event)
{
    View & l_view = g_data->getView();
    int l_time_X = toInt(l_view.viewOffset() / l_view.zoomX());
    m_drag_mode = t_drag_modes::DragNone;

    //Check if user clicked on center bar, to drag it
    if(within(4, p_mouse_event->x(), l_time_X))
    {
        m_drag_mode = t_drag_modes::DragTimeBar;
        m_mouse_X = p_mouse_event->x();
        return;
    }

    // If the control or alt keys are pressed, zoom in or out on the correct axis, otherwise scroll.
    if(p_mouse_event->QInputEvent::modifiers() & Qt::ControlModifier)
    {
        // Do we zoom in or out?
        if(p_mouse_event->QInputEvent::modifiers() & Qt::ShiftModifier)
        {
        }
        else
        {
        }
    }
    else if(p_mouse_event->QInputEvent::modifiers() & Qt::AltModifier)
    {
        // Do we zoom in or out?
        if(p_mouse_event->QInputEvent::modifiers() & Qt::ShiftModifier)
        {
        }
        else
        {
        }
    }
    else
    {
        //mouseDown = true;
        m_mouse_X = p_mouse_event->x();
        m_mouse_Y = p_mouse_event->y();

        Channel *l_channel = channelAtPixel(p_mouse_event->x(), p_mouse_event->y());
        if(l_channel)
        {
            //Clicked on a channel
            g_data->setActiveChannel(l_channel);
            m_drag_mode = t_drag_modes::DragChannel;
        }
        else
        {
            //Must have clicked on background
            m_drag_mode = t_drag_modes::DragBackground;
            m_down_time = l_view.currentTime();
            m_down_note = l_view.viewBottom();
        }
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::mouseMoveEvent( QMouseEvent *p_mouse_event )
{
    View & l_view = g_data->getView();
    int l_pixel_at_current_time_X = toInt(l_view.viewOffset() / l_view.zoomX());

    switch(m_drag_mode)
    {
        case t_drag_modes::DragNone:
            if(within(4, p_mouse_event->x(), l_pixel_at_current_time_X))
            {
                setCursor(QCursor(Qt::SplitHCursor));
            }
            else if(channelAtPixel(p_mouse_event->x(), p_mouse_event->y()) != NULL)
            {
                setCursor(QCursor(Qt::PointingHandCursor));
            }
            else
            {
                unsetCursor();
            }
            break;
        case t_drag_modes::DragChannel:
            // Nothing implemented
            break;
        case t_drag_modes::DragBackground:
            l_view.setViewBottom(m_down_note - (m_mouse_Y - p_mouse_event->y()) * l_view.zoomY());
            g_data->updateActiveChunkTime(m_down_time - (p_mouse_event->x() - m_mouse_X) * l_view.zoomX());
            l_view.doSlowUpdate();
            break;
        case t_drag_modes::DragTimeBar:
        {
            int l_new_X = l_pixel_at_current_time_X + (p_mouse_event->x() - m_mouse_X);
            l_view.setViewOffset(double(l_new_X) * l_view.zoomX());
            m_mouse_X = p_mouse_event->x();
            l_view.doSlowUpdate();
        }
        break;
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::mouseReleaseEvent( QMouseEvent * )
{
    m_drag_mode = t_drag_modes::DragNone;
}

//------------------------------------------------------------------------------
double FreqWidgetGL::mouseTime(int p_x)const
{
    return g_data->getView().viewLeft() + g_data->getView().zoomX() * p_x;
}

//------------------------------------------------------------------------------
double FreqWidgetGL::mousePitch(int p_y)const
{
    return g_data->getView().viewBottom() + g_data->getView().zoomY() * (height() - p_y);
}

//------------------------------------------------------------------------------
void FreqWidgetGL::wheelEvent(QWheelEvent * p_wheel_event)
{
    View & l_view = g_data->getView();
    double l_amount = double(p_wheel_event->delta()) / WHEEL_DELTA * 0.15;
    bool l_is_zoom = g_data->mouseWheelZooms();
    if(p_wheel_event->QInputEvent::modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
    {
        l_is_zoom = !l_is_zoom;
    }

    if(l_is_zoom)
    {
        if(p_wheel_event->delta() >= 0)
        {
            //zooming in
            double l_before = l_view.logZoomY();
            l_view.setZoomFactorY(l_view.logZoomY() + l_amount, height() - p_wheel_event->y());
            l_amount = l_view.logZoomY() - l_before;
            if(g_data->getRunning() == GData::RunningMode::STREAM_FORWARD)
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount);
            }
            else
            {
                //zoom toward mouse pointer
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount, p_wheel_event->x());
            }
        }
        else
        {
            //zoom out toward center
            double l_before = l_view.logZoomY();
            l_view.setZoomFactorY(l_view.logZoomY() + l_amount, height() / 2);
            l_amount = l_view.logZoomY() - l_before;
            if(g_data->getRunning() == GData::RunningMode::STREAM_FORWARD)
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount);
            }
            else
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount, width() / 2);
            }
        }
    }
    else
    {
        //mouse wheel scrolls
        l_view.setViewBottom(l_view.viewBottom() + l_amount * 0.75 * l_view.viewHeight());
    }
    l_view.doSlowUpdate();

    p_wheel_event->accept();
}

//------------------------------------------------------------------------------
void FreqWidgetGL::resizeEvent (QResizeEvent *p_resize_event)
{
    QGLWidget::resizeEvent(p_resize_event);

    if(p_resize_event->size() == p_resize_event->oldSize())
    {
        return;
    }

    View & l_view = g_data->getView();

    double l_old_view_width = double(l_view.viewWidth());

    l_view.setPixelHeight(height());
    l_view.setPixelWidth(width());
    // Work out what the times/heights of the view should be based on the zoom factors
    float l_new_Y_height = height() * l_view.zoomY();
    float l_new_Y_bottom = l_view.viewBottom() - ((l_new_Y_height - l_view.viewHeight()) / 2.0);

    l_view.setViewOffset(l_view.viewOffset() / l_old_view_width * l_view.viewWidth());

    l_view.setViewBottom(l_new_Y_bottom);
}

//------------------------------------------------------------------------------
void FreqWidgetGL::keyPressEvent( QKeyEvent *p_key_event )
{
    switch (p_key_event->key())
    {
        case Qt::Key_Control:
            setCursor(QCursor(QPixmap(zoomx)));
            break;
        case Qt::Key_Alt:
            setCursor(QCursor(QPixmap(zoomy)));
            break;
        case Qt::Key_Shift:
            if(p_key_event->QInputEvent::modifiers() & Qt::ControlModifier)
            {
                setCursor(QCursor(QPixmap(zoomxout)));
            }
            else if(p_key_event->QInputEvent::modifiers() & Qt::AltModifier)
            {
                setCursor(QCursor(QPixmap(zoomyout)));
            }
            else
            {
                p_key_event->ignore();
            }
            break;
        default:
            p_key_event->ignore();
            break;
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::keyReleaseEvent( QKeyEvent *p_key_event)
{
    switch (p_key_event->key())
    {
        // Unset the cursor if the control or alt keys were released, ignore otherwise
        case Qt::Key_Control:
        case Qt::Key_Alt:
            unsetCursor();
            break;
        case Qt::Key_Shift:
            if(p_key_event->QInputEvent::modifiers() & Qt::ControlModifier)
            {
                setCursor(QCursor(QPixmap(zoomx)));
            }
            else if(p_key_event->QInputEvent::modifiers() & Qt::AltModifier)
            {
                setCursor(QCursor(QPixmap(zoomy)));
            }
            else
            {
                p_key_event->ignore();
            }
            break;
        default:
            p_key_event->ignore();
            break;
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::leaveEvent ( QEvent * p_event)
{
    unsetCursor();
    QWidget::leaveEvent(p_event);
}

//------------------------------------------------------------------------------
void FreqWidgetGL::drawChannelGL( Channel *p_channel
                                , const double & p_left_time
                                , const double & p_current_time
                                , const double & p_zoom_X
                                , double p_view_bottom
                                , const double & p_zoom_Y
                                , int p_view_type
                                )
{
    p_view_bottom += g_data->semitoneOffset();
    float l_line_width = 3.0f;
    float l_line_half_width = l_line_width / 2;
    ZoomLookup *l_zoom_lookup;
    if(p_view_type == DRAW_VIEW_SUMMARY)
    {
        l_zoom_lookup = &p_channel->get_summary_zoom_lookup();
    }
    else
    {
        l_zoom_lookup = &p_channel->get_normal_zoom_lookup();
    }

    ChannelLocker l_channel_locker(p_channel);

    QColor l_current_color = p_channel->get_color();
    qglColor(l_current_color);

    int l_view_bottom_offset = toInt(p_view_bottom / p_zoom_Y);

    // l_base_X is the no. of chunks a pixel must represent.
    double l_base_X = p_zoom_X / p_channel->timePerChunk();

    l_zoom_lookup->setZoomLevel(l_base_X);
  
    double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
    double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());

    double l_frame_time = l_left_frame_time;
    int l_n = 0;
    int l_base_element = int(floor(l_frame_time / l_base_X));
    if(l_base_element < 0)
    {
        l_n -= l_base_element;
        l_base_element = 0;
    }
    int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
  
    Array1d<MyGLfloat2d> l_index_array(width() * 2);

    if(l_base_X > 1)
    {
        // More samples than pixels
        int l_the_width = width();
        if(l_last_base_element > l_zoom_lookup->size())
        {
            l_zoom_lookup->setSize(l_last_base_element);
        }
        for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
        {
            myassert(l_base_element >= 0);
            ZoomElement & l_zoom_element = l_zoom_lookup->at(l_base_element);
            if(!l_zoom_element.isValid())
            {
                if(!calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
                {
                    continue;
                }
            }
            if(l_zoom_element.high() != 0.0f && l_zoom_element.high() - l_zoom_element.low() < 1.0)
            {
                //if range is closer than one semi-tone then draw a line between them
                qglColor(l_zoom_element.color());
                mygl_line(float(l_n), height() - l_line_half_width - (l_zoom_element.high() / p_zoom_Y) + l_view_bottom_offset, l_n, height() + l_line_half_width - (l_zoom_element.low() / p_zoom_Y) + l_view_bottom_offset);
            }
        }
    }
    else
    {
        // More pixels than samples
        float l_err = 0.0;
        float l_pitch = 0.0;
        float l_prev_pitch = 0.0;
        float l_vol;

        // Integer version of frame time
        int l_int_chunk = (int) floor(l_frame_time);
        if(l_int_chunk < 0)
        {
            l_int_chunk = 0;
        }

        // So we skip some pixels
        double l_step_size = 1.0 / l_base_X;
        int l_x = 0;
        int l_y;

        double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
        double l_stop = width() + (2 * l_step_size);
        //make it an odd number
        int l_square_size = (int(sqrt(l_step_size)) / 2) * 2 + 1;
        int l_half_square_size = l_square_size / 2;
        int l_pen_X = 0;
        int l_pen_Y = 0;

        for(double l_double_index = l_start; l_double_index < l_stop && l_int_chunk < (int)p_channel->totalChunks(); l_double_index += l_step_size, l_int_chunk++)
        {
            myassert(l_int_chunk >= 0);
            AnalysisData * l_data = p_channel->dataAtChunk(l_int_chunk);
            l_err = l_data->getCorrelation();
            l_vol = dB2Normalised(l_data->getLogRms(), p_channel->get_rms_ceiling(), p_channel->get_rms_floor());
            glLineWidth(l_line_width);
            if(g_data->pitchContourMode() == 0)
            {
                if(p_view_type == DRAW_VIEW_PRINT)
                {
                    qglColor(colorBetween(QColor(255, 255, 255), p_channel->get_color(), l_err * l_vol));
                }
                else
                {
                    qglColor(colorBetween(g_data->backgroundColor(), p_channel->get_color(), l_err * l_vol));
                }
            }
            else
            {
                qglColor(p_channel->get_color());
            }

            l_x = toInt(l_double_index);
            l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;
            myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
            l_y = height() - 1 - toInt(l_pitch / p_zoom_Y) + l_view_bottom_offset;
            if(l_pitch > 0.0f)
            {
                if(fabs(l_prev_pitch - l_pitch) < 1.0 && l_double_index != l_start)
                {
                    //if closer than one semi-tone from previous then draw a line between them
                    mygl_line((float)l_pen_X, (float)l_pen_Y, (float)l_x, (float)l_y);
                    l_pen_X = l_x;
                    l_pen_Y = l_y;
                }
                else
                {
                    l_pen_X = l_x;
                    l_pen_Y = l_y;
                }
                if(l_step_size > 10)
                {
                    //draw squares on the data points
                    mygl_rect(l_x - l_half_square_size, l_y - l_half_square_size, l_square_size, l_square_size);
                }
            }
            l_prev_pitch = l_pitch;
        }
    }
}

//------------------------------------------------------------------------------
void FreqWidgetGL::drawChannelFilledGL( Channel *p_channel
                                      , const double & p_left_time
                                      , const double & p_current_time
                                      , const double & p_zoom_X
                                      , double p_view_bottom
                                      , const double & p_zoom_Y
                                      , int p_view_type
                                      )
{
    p_view_bottom += g_data->semitoneOffset();
    ZoomLookup * l_zoom_lookup;
    if(p_view_type == DRAW_VIEW_SUMMARY)
    {
        l_zoom_lookup = & p_channel->get_summary_zoom_lookup();
    }
    else
    {
        l_zoom_lookup = & p_channel->get_normal_zoom_lookup();
    }

    ChannelLocker l_channel_locker(p_channel);

    QColor l_current_color = p_channel->get_color();
    QColor l_inverted_color(255 - l_current_color.red(), 255 - l_current_color.green(), 255 - l_current_color.blue());
    qglColor(l_current_color);

    int l_view_bottom_offset = toInt(p_view_bottom / p_zoom_Y);

    // l_base_X is the no. of chunks a pixel must represent.
    double l_base_X = p_zoom_X / p_channel->timePerChunk();

    l_zoom_lookup->setZoomLevel(l_base_X);

    double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
    double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());

    double l_frame_time = l_left_frame_time;
    int l_n = 0;
    int l_base_element = int(floor(l_frame_time / l_base_X));
    if(l_base_element < 0)
    {
        l_n -= l_base_element;
        l_base_element = 0;
    }
    int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));

    int l_first_N = l_n;
    int l_last_N = l_first_N;

    Array1d<MyGLfloat2d> l_bottom_points(width() * 2);
    Array1d<MyGLfloat2d> l_even_mid_points(width() * 2);
    Array1d<MyGLfloat2d> l_odd_mid_points(width() * 2);
    Array1d<MyGLfloat2d> l_even_mid_points_2(width() * 2);
    Array1d<MyGLfloat2d> l_odd_mid_points_2(width() * 2);
    Array1d<MyGLfloat2d> l_note_rect(width() * 2);
    Array1d<MyGLfloat2d> l_note_rect_2(width() * 2);
    std::vector<bool> l_is_note_rect_even(width() * 2);
    int l_point_index = 0;
    int l_even_mid_point_index = 0;
    int l_odd_mid_point_index = 0;
    int l_even_mid_point_index_2 = 0;
    int l_odd_mid_point_index2 = 0;
    int l_rect_index = 0;
    int l_rect_index_2 = 0;

    if(l_base_X > 1)
    {
        // More samples than pixels
        int l_the_width = width();
        if(l_last_base_element > l_zoom_lookup->size()) l_zoom_lookup->setSize(l_last_base_element);
        for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
        {
            myassert(l_base_element >= 0);
            ZoomElement &l_zoom_element = l_zoom_lookup->at(l_base_element);
            if(!l_zoom_element.isValid())
            {
                if(!calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
                {
                    continue;
                }
            }

            int l_y = height() - 1 - toInt(l_zoom_element.high() / p_zoom_Y) + l_view_bottom_offset;
            int l_y2;
            int l_y3;
            if(l_zoom_element.noteIndex() != -1 && p_channel->dataAtChunk(l_zoom_element.midChunk())->getNoteIndex() != -1)
            {
                myassert(l_zoom_element.noteIndex() >= 0);
                myassert(l_zoom_element.noteIndex() < int(p_channel->get_note_data().size()));
                myassert(p_channel->isValidChunk(l_zoom_element.midChunk()));
                AnalysisData * l_data = p_channel->dataAtChunk(l_zoom_element.midChunk());

                if(g_data->showMeanVarianceBars())
                {
                    //longTermMean bars
                    l_y2 = height() - 1 - toInt((l_data->getLongTermMean() + l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_y3 = height() - 1 - toInt((l_data->getLongTermMean() - l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    if(l_zoom_element.noteIndex() % 2 == 0)
                    {
                        l_even_mid_points[l_even_mid_point_index++].set(l_n, l_y2);
                        l_even_mid_points[l_even_mid_point_index++].set(l_n, l_y3);
                    }
                    else
                    {
                        l_odd_mid_points[l_odd_mid_point_index++].set(l_n, l_y2);
                        l_odd_mid_points[l_odd_mid_point_index++].set(l_n, l_y3);
                    }
  
                    //shortTermMean bars
                    l_y2 = height() - 1 - toInt((l_data->getShortTermMean() + l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_y3 = height() - 1 - toInt((l_data->getShortTermMean() - l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    if(l_zoom_element.noteIndex() % 2 == 0)
                    {
                        l_even_mid_points_2[l_even_mid_point_index_2++].set(l_n, l_y2);
                        l_even_mid_points_2[l_even_mid_point_index_2++].set(l_n, l_y3);
                    }
                    else
                    {
                        l_odd_mid_points_2[l_odd_mid_point_index2++].set(l_n, l_y2);
                        l_odd_mid_points_2[l_odd_mid_point_index2++].set(l_n, l_y3);
                    }
                }
            }
            l_bottom_points[l_point_index++].set(l_n, l_y);
            l_bottom_points[l_point_index++].set(l_n, height());
            l_last_N = l_n;
        }
        qglColor(g_data->shading1Color());
        mygl_rect(l_first_N, 0, l_last_N, height());
        qglColor(g_data->shading2Color());
        if(l_point_index > 1)
        {
            glVertexPointer(2, GL_FLOAT, 0, l_bottom_points.begin());
            glDrawArrays(GL_QUAD_STRIP, 0, l_point_index);
        }

        if(g_data->showMeanVarianceBars())
        {
            //shortTermMean bars
            qglColor(Qt::green);
            if(l_even_mid_point_index_2 > 1)
            {
                glVertexPointer(2, GL_FLOAT, 0, l_even_mid_points_2.begin());
                glDrawArrays(GL_LINES, 0, l_even_mid_point_index_2);
            }
            qglColor(Qt::yellow);
            if(l_odd_mid_point_index2 > 1)
            {
                glVertexPointer(2, GL_FLOAT, 0, l_odd_mid_points_2.begin());
                glDrawArrays(GL_LINES, 0, l_odd_mid_point_index2);
            }

            //longTermMean bars
            qglColor(Qt::yellow);
            if(l_even_mid_point_index > 1)
            {
                glVertexPointer(2, GL_FLOAT, 0, l_even_mid_points.begin());
                glDrawArrays(GL_LINES, 0, l_even_mid_point_index);
            }
            qglColor(Qt::green);
            if(l_odd_mid_point_index > 1)
            {
                glVertexPointer(2, GL_FLOAT, 0, l_odd_mid_points.begin());
                glDrawArrays(GL_LINES, 0, l_odd_mid_point_index);
            }
        }
    }
    else
    {
        // More pixels than samples
        float l_err = 0.0;
        float l_pitch = 0.0;
        // Integer version of frame time
        int l_int_chunk = (int) floor(l_frame_time);
        if(l_int_chunk < 0)
        {
            l_int_chunk = 0;
        }
        // So we skip some pixels
        double l_step_size = 1.0 / l_base_X;
        int l_x = 0;
        int l_y;
        int l_y2;
        int l_y3;
        int l_x2;
  
        double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
        double l_stop = width() + (2 * l_step_size);
        l_last_N = l_first_N = toInt(l_start);
        for(double l_double_index = l_start; l_double_index < l_stop && l_int_chunk < (int)p_channel->totalChunks(); l_double_index += l_step_size, l_int_chunk++)
        {
            myassert(l_int_chunk >= 0);
            AnalysisData *l_data = p_channel->dataAtChunk(l_int_chunk);
            l_err = l_data->getCorrelation();

            if(g_data->pitchContourMode() == 0)
            {
                qglColor(colorBetween(QColor(255, 255, 255), p_channel->get_color(), l_err * dB2ViewVal(l_data->getLogRms())));
            }
            else
            {
                qglColor(p_channel->get_color());
            }

            l_x = toInt(l_double_index);
            l_last_N = l_x;
            l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;

            if(l_data->getNoteIndex() >= 0)
            {
                l_is_note_rect_even[l_rect_index] = (l_data->getNoteIndex() % 2) == 0;

                if(g_data->showMeanVarianceBars())
                {
                    l_x2 = toInt(l_double_index + l_step_size);
                    //longTermMean bars
                    l_y2 = height() - 1 - toInt((l_data->getLongTermMean() + l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_y3 = height() - 1 - toInt((l_data->getLongTermMean() - l_data->getLongTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_note_rect[l_rect_index++].set(l_x, l_y2);
                    l_note_rect[l_rect_index++].set(l_x2, l_y3);
  
                    //shortTermMean bars
                    l_y2 = height() - 1 - toInt((l_data->getShortTermMean() + l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_y3 = height() - 1 - toInt((l_data->getShortTermMean() - l_data->getShortTermDeviation()) / p_zoom_Y) + l_view_bottom_offset;
                    l_note_rect_2[l_rect_index_2++].set(l_x, l_y2);
                    l_note_rect_2[l_rect_index_2++].set(l_x2, l_y3);
                }
            }

            myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
            l_y = height() - 1 - toInt(l_pitch / p_zoom_Y) + l_view_bottom_offset;
            l_bottom_points[l_point_index++].set(l_x, l_y);
            l_bottom_points[l_point_index++].set(l_x, height());
        }

        myassert(l_point_index <= width() * 2);
        qglColor(g_data->shading1Color());
        mygl_rect(l_first_N, 0, l_last_N, height());
        qglColor(g_data->shading2Color());
        glVertexPointer(2, GL_FLOAT, 0, l_bottom_points.begin());
        glDrawArrays(GL_QUAD_STRIP, 0, l_point_index);

        if(g_data->showMeanVarianceBars())
        {
            //shortTermMean bars
            for(int l_j = 0; l_j < l_rect_index_2; l_j += 2)
            {
                if(l_is_note_rect_even[l_j])
                {
                    qglColor(Qt::green);
                }
                else
                {
                    qglColor(Qt::yellow);
                }
                mygl_rect(l_note_rect_2[l_j], l_note_rect_2[ l_j + 1]);
            }
            //longTermMean bars
            QColor l_see_through_yellow = Qt::yellow;
            l_see_through_yellow.setAlpha(255);
            QColor l_see_through_green = Qt::green;
            l_see_through_green.setAlpha(255);
            for(int l_j = 0; l_j < l_rect_index; l_j += 2)
            {
                if(l_is_note_rect_even[l_j])
                {
                    qglColor(l_see_through_yellow);
                }
                else
                {
                    qglColor(l_see_through_green);
                }
                mygl_rect(l_note_rect[l_j], l_note_rect[l_j + 1]);
            }
        }
    }
}

//------------------------------------------------------------------------------
bool FreqWidgetGL::calcZoomElement( Channel * p_channel
                                  , ZoomElement & p_zoom_element
                                  , int p_base_element
                                  , const double & p_base_X
                                  )
{
    int l_start_chunk = toInt(double(p_base_element) * p_base_X);
    int l_finish_chunk = toInt(double(p_base_element + 1) * p_base_X) + 1;
    if(l_finish_chunk >= (int)p_channel->totalChunks())
    {
        //dont go off the end
        l_finish_chunk--;
    }
    if(l_finish_chunk >= (int)p_channel->totalChunks())
    {
        //that data doesn't exist yet
        return false;
    }

    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> l_a =
    minMaxElement(p_channel->dataIteratorAtChunk(l_start_chunk), p_channel->dataIteratorAtChunk(l_finish_chunk), lessPitch());
    if(l_a.second == p_channel->dataIteratorAtChunk(l_finish_chunk))
    {
        return false;
    }

    large_vector<AnalysisData>::iterator l_err = std::max_element(p_channel->dataIteratorAtChunk(l_start_chunk), p_channel->dataIteratorAtChunk(l_finish_chunk), lessValue(0));
    if(l_err == p_channel->dataIteratorAtChunk(l_finish_chunk))
    {
        return false;
    }

    float l_low;
    float l_high;
    int l_note_index;
    if(p_channel->isVisibleChunk(&*l_err))
    {
        l_low = l_a.first->getPitch();
        l_high = l_a.second->getPitch();
        l_note_index = l_a.first->getNoteIndex();
    }
    else
    {
        l_low = 0;
        l_high = 0;
        l_note_index = NO_NOTE;
    }
    float l_corr = l_err->getCorrelation() * dB2Normalised(l_err->getLogRms(), p_channel->get_rms_ceiling(), p_channel->get_rms_floor());
    QColor l_the_color = (g_data->pitchContourMode() == 0) ? colorBetween(g_data->backgroundColor(), p_channel->get_color(), l_corr) : p_channel->get_color();

    p_zoom_element.set(l_low, l_high, l_corr, l_the_color, l_note_index, (l_start_chunk + l_finish_chunk) / 2);
    return true;
}

//------------------------------------------------------------------------------
void FreqWidgetGL::setChannelVerticalView( Channel * p_channel
                                         , const double & p_left_time
                                         , const double & p_current_time
                                         , const double & p_zoom_X
                                         , double // p_view_bottom
                                         , const double & // p_zoom_Y
                                         )
{
    ZoomLookup * l_zoom_lookup = &p_channel->get_normal_zoom_lookup();
    
    ChannelLocker l_channel_locker(p_channel);

    std::vector<float> l_ys;
    l_ys.reserve(width());
    std::vector<float> l_weightings;
    l_weightings.reserve(width());
    float l_max_Y = 0.0f;
    float l_min_Y = g_data->topPitch();
    float l_total_Y = 0.0f;
    float l_num_Y = 0.0f;
  
    // l_base_X is the no. of chunks a pixel must represent.
    double l_base_X = p_zoom_X / p_channel->timePerChunk();

    l_zoom_lookup->setZoomLevel(l_base_X);

    double l_current_chunk = p_channel->chunkFractionAtTime(p_current_time);
    double l_left_frame_time = l_current_chunk - ((p_current_time - p_left_time) / p_channel->timePerChunk());

    double l_frame_time = l_left_frame_time;
    int l_n = 0;
    int l_current_base_element = int(floor(l_current_chunk / l_base_X));
    int l_first_base_element = int(floor(l_frame_time / l_base_X));
    int l_base_element = l_first_base_element;
    if(l_base_element < 0)
    {
        l_n -= l_base_element;
        l_base_element = 0;
    }
    int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
    double l_left_base_width = MAX(1.0, double(l_current_base_element - l_first_base_element));
    double l_right_base_width = MAX(1.0, double(l_last_base_element - l_current_base_element));

    /*
     We calculate the auto follow and scale by averaging all the note elements in view.
     We weight the frequency averaging by a triangle weighting function centered on the current time.
     Also it is weighted by the corr value of each frame.
                     /|\
                    / | \
                   /  |  \
                  /   |   \
                    ^   ^
     l_left_base_width l_right_base_width
    */


    if(l_base_X > 1)
    {
        // More samples than pixels
        int l_the_width = width();
        if(l_last_base_element > l_zoom_lookup->size())
        {
            l_zoom_lookup->setSize(l_last_base_element);
        }
        for(; l_n < l_the_width && l_base_element < l_last_base_element; l_n++, l_base_element++)
        {
            myassert(l_base_element >= 0);
            ZoomElement &l_zoom_element = l_zoom_lookup->at(l_base_element);
            if(!l_zoom_element.isValid())
            {
                if(!calcZoomElement(p_channel, l_zoom_element, l_base_element, l_base_X))
                {
                    continue;
                }
            }
            if(l_zoom_element.low() > 0.0f && l_zoom_element.high() > 0.0f)
            {
                float l_weight = l_zoom_element.corr();
                if(l_base_element < l_current_base_element)
                {
                    l_weight *= double(l_current_base_element - l_base_element) / l_left_base_width;
                }
                else if(l_base_element > l_current_base_element)
                {
                    l_weight *= double(l_base_element - l_current_base_element) / l_right_base_width;
                }
                if(l_zoom_element.low() < l_min_Y)
                {
                    l_min_Y = l_zoom_element.low();
                }
                if(l_zoom_element.high() > l_max_Y)
                {
                    l_max_Y = l_zoom_element.high();
                }
                l_total_Y += (l_zoom_element.low() + l_zoom_element.high()) / 2.0f * l_weight;
                l_num_Y += l_weight;
                l_ys.push_back((l_zoom_element.low() + l_zoom_element.high()) / 2.0f);
                l_weightings.push_back(l_weight);
            }
        }
    }
    else
    {
        // More pixels than samples
        float l_pitch = 0.0;
        // Integer version of frame time
        int l_int_chunk = (int) floor(l_frame_time);
        if(l_int_chunk < 0)
        {
            l_int_chunk = 0;
        }
        // So we skip some pixels
        double l_step_size = 1.0 / l_base_X;
        float l_corr;

        double l_start = (double(l_int_chunk) - l_frame_time) * l_step_size;
        double l_stop = width() + (2 * l_step_size);
        for(double l_double_index = l_start; l_double_index < l_stop && l_int_chunk < (int)p_channel->totalChunks(); l_double_index += l_step_size, l_int_chunk++)
        {
            myassert(l_int_chunk >= 0);
            AnalysisData *l_data = p_channel->dataAtChunk(l_int_chunk);

            l_pitch = (p_channel->isVisibleChunk(l_data)) ? l_data->getPitch() : 0.0f;
            myassert(l_pitch >= 0.0 && l_pitch <= g_data->topPitch());
            l_corr = l_data->getCorrelation() * dB2ViewVal(l_data->getLogRms());
            if(l_pitch > 0.0f)
            {
                float l_weight = l_corr;
                if(l_min_Y < l_pitch)
                {
                    l_min_Y = l_pitch;
                }
                if(l_max_Y > l_pitch)
                {
                    l_max_Y = l_pitch;
                }
                l_total_Y += l_pitch * l_weight;
                l_num_Y += l_weight;
                l_ys.push_back(l_pitch);
                l_weightings.push_back(l_weight);
            }
        }
    }

    if(!l_ys.empty() > 0)
    {
        float l_mean_Y = l_total_Y / l_num_Y;
        g_data->getView().setViewBottomRaw(l_mean_Y - g_data->getView().viewHeight() / 2.0);
    }
}
//EOF
