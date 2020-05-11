/***************************************************************************
                          amplitudewidget.cpp  -  description
                             -------------------
    begin                : 19 Mar 2005
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


#include "mygl.h"
#include "amplitudewidget.h"
#include "gdata.h"
#include "channel.h"
#include "zoomlookup.h"
#include "qcursor.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include "analysisdata.h"
#include "conversions.h"

#include <QPixmap>
#include <sstream>
#include <iomanip>

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
AmplitudeWidget::AmplitudeWidget(QWidget * p_parent, const std::string & p_name)
// Replace call to setRange(0.8); and setOffset(0.0) by final value assignations
// to avoid issue with non initialised values. We don't care about signal emissions
// because at construction time there are still not bound to slots
: QGLWidget(p_parent)
, m_drag_mode(DragModes::DragNone)
, m_mouse_X(0)
, m_mouse_Y(0)
, m_down_time(0.0)
, m_down_offset(0.0)
, m_threshold_index(0)
, m_range(0.8)
, m_offset(0.0)
, m_offset_inv(1.0 - m_range)
, m_line_width(0.0)
, m_half_line_width(0.0)
{
    setObjectName(p_name.c_str());
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::initializeGL()
{
    qglClearColor(GData::getUniqueInstance().backgroundColor());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::resizeGL( int p_width
                              , int p_height
                              )
{
    mygl_resize2d(p_width, p_height);
}

//------------------------------------------------------------------------------
AmplitudeWidget::~AmplitudeWidget()
{
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setRange(double p_new_range)
{
    if(m_range != p_new_range)
    {
        m_range = bound(p_new_range, 0.0, 1.0);
        setOffset(offset());
        emit rangeChanged(m_range);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setOffset(double p_new_offset)
{
    p_new_offset = bound(p_new_offset, 0.0, maxOffset());
    m_offset = p_new_offset;
    m_offset_inv = maxOffset() - m_offset;
    emit offsetChanged(m_offset);
    emit offsetInvChanged(offsetInv());
}

//------------------------------------------------------------------------------
void AmplitudeWidget::paintGL()
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    glClear(GL_COLOR_BUFFER_BIT);

    setLineWidth(3.0);
    glEnable(GL_LINE_SMOOTH);

    View & l_view = GData::getUniqueInstance().getView();

    //draw the red/blue background color shading if needed
    if(l_view.backgroundShading() && GData::getUniqueInstance().getActiveChannel())
    {
        drawChannelAmplitudeFilledGL(GData::getUniqueInstance().getActiveChannel());
    }

    setLineWidth(1.0);
    glDisable(GL_LINE_SMOOTH);

    drawVerticalRefLines();

    setLineWidth(3.0);
    glEnable(GL_LINE_SMOOTH);

    //draw all the visible channels
    for(unsigned int l_i = 0; l_i < GData::getUniqueInstance().getChannelsSize(); l_i++)
    {
        Channel *l_channel = GData::getUniqueInstance().getChannelAt(l_i);
        if(!l_channel->isVisible())
        {
            continue;
        }
        drawChannelAmplitudeGL(l_channel);
    }

    // Draw the current time line
    glDisable(GL_LINE_SMOOTH);
    QPalette l_palette;
    qglColor(l_palette.color(QPalette::WindowText));
    glLineWidth(1.0);
    double l_cur_screen_time = (l_view.currentTime() - l_view.viewLeft()) / l_view.zoomX();
    mygl_line(l_cur_screen_time, 0, l_cur_screen_time, height() - 1);

    // Draw a horizontal line at the current threshold.
    int l_y;
    double l_height_ratio = double(height()) / range();

    qglColor(Qt::black);
    l_y = height() - 1 - toInt((getCurrentThreshold(0) - offsetInv()) * l_height_ratio);
    mygl_line(0, l_y, width(), l_y);

    qglColor(Qt::red);
    l_y = height() - 1 - toInt((getCurrentThreshold(1) - offsetInv()) * l_height_ratio);
    mygl_line(0, l_y, width(), l_y);

    qglColor(Qt::black);
    renderText(2, height() - 3, getCurrentThresholdString());

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: AmplitudeWidget::paintGL()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawVerticalRefLines()
{
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
    else if (l_time_scale_base * 2.0 < l_time_step)
    {
        l_large_freq = 2;
    }
    else
    {
        l_large_freq = 2;
        l_time_scale_base /= 2;
    }

    //calc the first one just off the left of the screen
    double l_time_pos = floor(leftTime() / (l_time_scale_base * l_large_freq)) * (l_time_scale_base * l_large_freq);
    int l_x;
    int l_large_counter = -1;
    double l_ratio = double(width()) / timeWidth();
    double l_lTime = leftTime();

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
        l_x = toInt((l_time_pos-l_lTime) * l_ratio);
        mygl_line(l_x, 0, l_x, height() - 1);
    }
}

//------------------------------------------------------------------------------
bool AmplitudeWidget::calcZoomElement(ZoomElement &p_zoom_element, Channel *p_channel, int p_base_element, double p_base_X)
{
    int l_start_chunk = int(floor(double(p_base_element) * p_base_X));
    myassert(l_start_chunk <= p_channel->totalChunks());
    int l_finish_chunk = int(floor(double(p_base_element + 1) * p_base_X)) + 1;
    myassert(l_finish_chunk <= p_channel->totalChunks());
    if(l_start_chunk == l_finish_chunk)
    {
        return false;
    }

    myassert(l_start_chunk < l_finish_chunk);

    int l_mode = GData::getUniqueInstance().amplitudeMode();
    std::pair<large_vector<AnalysisData>::iterator, large_vector<AnalysisData>::iterator> a =
    minMaxElement(p_channel->dataIteratorAtChunk(l_start_chunk), p_channel->dataIteratorAtChunk(l_finish_chunk), lessValue(l_mode));
    myassert(a.second != p_channel->dataIteratorAtChunk(l_finish_chunk));
    float l_low = (*g_amp_mode_func[l_mode])(a.first->getValue(l_mode), GData::getUniqueInstance());
    float l_high = (*g_amp_mode_func[l_mode])(a.second->getValue(l_mode), GData::getUniqueInstance());

    p_zoom_element.set(l_low, l_high, 0, p_channel->get_color(), NO_NOTE, (l_start_chunk + l_finish_chunk) / 2);
    return true;
}

//------------------------------------------------------------------------------
double AmplitudeWidget::calculateElement(AnalysisData *p_data)
{
    double l_val = (*g_amp_mode_func[GData::getUniqueInstance().amplitudeMode()])(p_data->getValue(GData::getUniqueInstance().amplitudeMode()), GData::getUniqueInstance());
    return l_val;
}

//------------------------------------------------------------------------------
double AmplitudeWidget::getCurrentThreshold(int p_index)
{
    return (*g_amp_mode_func[GData::getUniqueInstance().amplitudeMode()])(GData::getUniqueInstance().ampThreshold(GData::getUniqueInstance().amplitudeMode(), p_index), GData::getUniqueInstance());
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setCurrentThreshold(double p_new_threshold, int p_index)
{
    p_new_threshold = bound(p_new_threshold, 0.0, 1.0);
    if(p_new_threshold < offsetInv())
    {
        setOffset(maxOffset() - p_new_threshold);
    }
    else if(p_new_threshold > offsetInv() + range())
    {
        setOffset(maxOffset() - (p_new_threshold - range()));
    }

    GData::getUniqueInstance().setAmpThreshold(GData::getUniqueInstance().amplitudeMode(), p_index, (*g_amp_mode_inv_func[GData::getUniqueInstance().amplitudeMode()])(p_new_threshold, GData::getUniqueInstance()));
}

//------------------------------------------------------------------------------
QString AmplitudeWidget::getCurrentThresholdString()const
{
    std::stringstream l_stream;
    l_stream << std::fixed << std::setprecision(2) << GData::getUniqueInstance().ampThreshold(GData::getUniqueInstance().amplitudeMode(), 0) << " " << GData::getUniqueInstance().ampThreshold(GData::getUniqueInstance().amplitudeMode(), 1);
    std::string l_threshold_str = g_amp_display_string[GData::getUniqueInstance().amplitudeMode()] + " = " + l_stream.str();
    return QString::fromStdString(l_threshold_str);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::setLineWidth(float p_width)
{
    m_line_width = p_width;
    m_half_line_width = p_width / 2;
    glLineWidth(m_line_width);
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawChannelAmplitudeGL(Channel *p_channel)
{
    View & l_view = GData::getUniqueInstance().getView();

    ChannelLocker l_channel_locker(p_channel);
    ZoomLookup *l_zoom_lookup = &p_channel->get_amplitude_zoom_lookup();
  
    // l_base_X is the no. of chunks a pixel must represent.
    double l_base_X = l_view.zoomX() / p_channel->timePerChunk();

    l_zoom_lookup->setZoomLevel(l_base_X);
  
    double l_current_chunk = p_channel->chunkFractionAtTime(l_view.currentTime());
    double l_left_frame_time = l_current_chunk - ((l_view.currentTime() - l_view.viewLeft()) / p_channel->timePerChunk());
    int l_n = 0;
    int l_base_element = int(floor(l_left_frame_time / l_base_X));
    if(l_base_element < 0)
    {
        l_n -= l_base_element;
        l_base_element = 0;
    }
    int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
    double l_height_ratio = double(height()) / range();

    Array1d<MyGLfloat2d> l_vertex_array(width() * 2);
    int l_point_index = 0;

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
                if(!calcZoomElement(l_zoom_element, p_channel, l_base_element, l_base_X))
                {
                    continue;
                }
            }

            l_vertex_array[l_point_index++] = MyGLfloat2d(l_n, height() - 1 - ((l_zoom_element.high() - offsetInv()) * l_height_ratio) - m_half_line_width);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_n, height() - 1 - ((l_zoom_element.low() - offsetInv()) * l_height_ratio) + m_half_line_width);

        }
        myassert(l_point_index <= width() * 2);
        qglColor(p_channel->get_color());
        glLineWidth(1.0f);
        glVertexPointer(2, GL_FLOAT, 0, l_vertex_array.begin());
        glDrawArrays(GL_LINES, 0, l_point_index);
    }
    else
    {
        //l_base_X <= 1
        float l_val = 0.0;
        // Integer version of frame time
        int l_int_chunk = static_cast<int>(floor(l_left_frame_time));
        // So we skip some pixels
        double l_step_size = 1.0 / l_base_X;
        float l_x = 0.0f, l_y;

        double l_start = (double(l_int_chunk) - l_left_frame_time) * l_step_size;
        double l_stop = width() + (2 * l_step_size);

        double l_dn = l_start;
        int l_total_chunks = p_channel->totalChunks();
        if(l_int_chunk < 0)
        {
            l_dn += l_step_size * -l_int_chunk;
            l_int_chunk = 0;
        }
        for(; l_dn < l_stop && l_int_chunk < l_total_chunks; l_dn += l_step_size, l_int_chunk++)
        {

            AnalysisData *l_data = p_channel->dataAtChunk(l_int_chunk);
            myassert(l_data);

            if(!l_data)
            {
                continue;
            }

            l_val = calculateElement(l_data);

            l_x = l_dn;
            l_y = height() - 1 - ((l_val - offsetInv()) * l_height_ratio);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_x, l_y);
        }
        myassert(l_point_index <= width() * 2);
        qglColor(p_channel->get_color());
        glLineWidth(3.0f);
        glVertexPointer(2, GL_FLOAT, 0, l_vertex_array.begin());
        glDrawArrays(GL_LINE_STRIP, 0, l_point_index);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::drawChannelAmplitudeFilledGL(Channel *p_channel)
{
    View & l_view = GData::getUniqueInstance().getView();

    ChannelLocker l_channel_locker(p_channel);
    ZoomLookup *l_zoom_lookup = &p_channel->get_amplitude_zoom_lookup();

    // l_base_X is the no. of chunks a pixel must represent.
    double l_base_X = l_view.zoomX() / p_channel->timePerChunk();

    l_zoom_lookup->setZoomLevel(l_base_X);
  
    double l_current_chunk = p_channel->chunkFractionAtTime(l_view.currentTime());
    double l_left_frame_time = l_current_chunk - ((l_view.currentTime() - l_view.viewLeft()) / p_channel->timePerChunk());
    int l_n = 0;
    int l_base_element = int(floor(l_left_frame_time / l_base_X));
    if(l_base_element < 0)
    {
        l_n -= l_base_element;
        l_base_element = 0;
    }
    int l_last_base_element = int(floor(double(p_channel->totalChunks()) / l_base_X));
    double l_height_ratio = double(height()) / range();

    Array1d<MyGLfloat2d> l_vertex_array(width() * 2);
    int l_point_index = 0;

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
                if(!calcZoomElement(l_zoom_element, p_channel, l_base_element, l_base_X))
                {
                    continue;
                }
            }

            int l_y = height() - 1 - toInt((l_zoom_element.high() - offsetInv()) * l_height_ratio);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_n, l_y);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_n, height());
        }
        qglColor(GData::getUniqueInstance().shading2Color());
        glVertexPointer(2, GL_FLOAT, 0, l_vertex_array.begin());
        glDrawArrays(GL_QUAD_STRIP, 0, l_point_index);
    }
    else
    {
        //l_base_X <= 1
        float l_val = 0.0;
        // Integer version of frame time
        int l_int_chunk = static_cast<int>(floor(l_left_frame_time));
        // So we skip some pixels
        double l_step_size = 1.0 / l_base_X;
        float l_x = 0.0f, l_y;

        double l_start = (double(l_int_chunk) - l_left_frame_time) * l_step_size;
        double l_stop = width() + (2 * l_step_size);

        double l_dn = l_start;
        int l_total_chunks = p_channel->totalChunks();
        if(l_int_chunk < 0)
        {
            l_dn += l_step_size * -l_int_chunk;
            l_int_chunk = 0;
        }

        for(; l_dn < l_stop && l_int_chunk < l_total_chunks; l_dn += l_step_size, l_int_chunk++)
        {
            AnalysisData *l_data = p_channel->dataAtChunk(l_int_chunk);
            myassert(l_data);

            if(!l_data)
            {
                continue;
            }

            l_val = calculateElement(l_data);

            l_x = l_dn;
            l_y = height() - 1 - ((l_val - offsetInv()) * l_height_ratio);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_x, l_y);
            l_vertex_array[l_point_index++] = MyGLfloat2d(l_x, height());
        }
        myassert(l_point_index <= width() * 2);
        qglColor(GData::getUniqueInstance().shading2Color());
        glVertexPointer(2, GL_FLOAT, 0, l_vertex_array.begin());
        glDrawArrays(GL_QUAD_STRIP, 0, l_point_index);
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mousePressEvent(QMouseEvent * p_mouse_event)
{
    View & l_view = GData::getUniqueInstance().getView();
    int l_time_X = toInt(l_view.viewOffset() / l_view.zoomX());
    int l_pixel_at_current_noise_threshold_Y;
    m_drag_mode = DragModes::DragNone;

    //Check if user clicked on center bar, to drag it
    if(within(4, p_mouse_event->x(), l_time_X))
    {
        m_drag_mode = DragModes::DragTimeBar;
        m_mouse_X = p_mouse_event->x();
        return;
    }
    //Check if user clicked on a threshold bar
    for(int l_j = 0; l_j < 2; l_j++)
    {
        l_pixel_at_current_noise_threshold_Y = height() - 1 - toInt((getCurrentThreshold(l_j) - offsetInv()) / range() * double(height()));
        if(within(4, p_mouse_event->y(), l_pixel_at_current_noise_threshold_Y))
        {
            m_drag_mode = DragModes::DragNoiseThreshold;
            //remember which m_threshold_index the user clicked
            m_threshold_index = l_j;
            m_mouse_Y = p_mouse_event->y();
            return;
        }
    }
    //Otherwise user has clicked on background
    {
        m_mouse_X = p_mouse_event->x();
        m_mouse_Y = p_mouse_event->y();
        m_drag_mode = DragModes::DragBackground;
        m_down_time = l_view.currentTime();
        m_down_offset = offset();
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mouseMoveEvent(QMouseEvent *p_mouse_event)
{
    View & l_view = GData::getUniqueInstance().getView();
    int l_pixel_at_current_time_X = toInt(l_view.viewOffset() / l_view.zoomX());
    int l_pixel_at_current_noise_threshold_Y;
  
    switch(m_drag_mode)
    {
        case DragModes::DragTimeBar:
        {
            int l_new_X = l_pixel_at_current_time_X + (p_mouse_event->x() - m_mouse_X);
            l_view.setViewOffset(double(l_new_X) * l_view.zoomX());
            m_mouse_X = p_mouse_event->x();
            l_view.doSlowUpdate();
        }
        break;
        case DragModes::DragNoiseThreshold:
        {
            int l_new_Y = p_mouse_event->y();
            setCurrentThreshold(double(height() - 1 - l_new_Y) / double(height()) * range() + offsetInv(), m_threshold_index);
            m_mouse_Y = p_mouse_event->y();
            GData::getUniqueInstance().getView().doSlowUpdate();
        }
        break;
        case DragModes::DragBackground:
            GData::getUniqueInstance().updateActiveChunkTime(m_down_time - (p_mouse_event->x() - m_mouse_X) * l_view.zoomX());
            setOffset(m_down_offset - (double(p_mouse_event->y() - m_mouse_Y) / double(height()) * range()));
            l_view.doSlowUpdate();
        break;
        case DragModes::DragNone:
            if(within(4, p_mouse_event->x(), l_pixel_at_current_time_X))
            {
                setCursor(QCursor(Qt::SplitHCursor));
            }
            else
            {
                bool l_over_threshold = false;
                for(int l_j = 0; l_j < 2; l_j++)
                {
                    l_pixel_at_current_noise_threshold_Y = height() - 1 - toInt((getCurrentThreshold(l_j) - offsetInv()) / range() * double(height()));
                    if(within(4, p_mouse_event->y(), l_pixel_at_current_noise_threshold_Y))
                    {
                        l_over_threshold = true;
                    }
                }
                if(l_over_threshold)
                {
                    setCursor(QCursor(Qt::SplitVCursor));
                }
                else
                {
                    unsetCursor();
                }
            }
        break;
        case DragModes::DragChannel:
        break;
    }
}

//------------------------------------------------------------------------------
void AmplitudeWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_drag_mode = DragModes::DragNone;
}

//------------------------------------------------------------------------------
void AmplitudeWidget::wheelEvent(QWheelEvent * p_mouse_event)
{
    View & l_view = GData::getUniqueInstance().getView();
    if(!(p_mouse_event->QInputEvent::modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)))
    {
        if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_FORWARD)
        {
            l_view.setZoomFactorX(l_view.logZoomX() + double(p_mouse_event->delta() / WHEEL_DELTA) * 0.3);
        }
        else
        {
            if(p_mouse_event->delta() < 0)
            {
                l_view.setZoomFactorX(l_view.logZoomX() + double(p_mouse_event->delta() / WHEEL_DELTA) * 0.3, width() / 2);
            }
            else
            {
                l_view.setZoomFactorX(l_view.logZoomX() + double(p_mouse_event->delta() / WHEEL_DELTA) * 0.3, p_mouse_event->x());
            }
        }
        l_view.doSlowUpdate();
    }
    p_mouse_event->accept();
}
// EOF
