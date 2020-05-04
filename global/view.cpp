/***************************************************************************
                          view.cpp  -  description
                             -------------------
    begin                : Mon Jul 26 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
***************************************************************************/

#include "view.h"

#include "useful.h"
#include "gdata.h"
#include "channel.h"
#include "conversions.h"

//------------------------------------------------------------------------------
View::View()
: m_current_time(1.0) //to force a change in the setCurrentTime call
, m_view_bottom(0.0)
, m_view_offset(0.0) //to force a change in the setViewOffset call
, m_log_zoom_X(0.0)
, m_log_zoom_Y(0.0)
, m_pixel_height(0) //to force a change in the setPixelHeight call
, m_pixel_width(0) //to force a change in the setPixelWidth call
, m_zoom_X(0.0)
, m_zoom_Y(0.0)
, m_auto_follow(g_data->getSettingsValue("View/autoFollow", true))
, m_background_shading(g_data->getSettingsValue("View/backgroundShading", true))
, m_very_fast_update_timer(new QTimer(this))
, m_fast_update_timer(new QTimer(this))
, m_slow_update_timer(new QTimer(this))
, m_need_slow_update(false)
, m_need_fast_update(false)
, m_need_very_fast_update(false)
{
    setCurrentTime(0.0); //in seconds

    setLogZoomX(1.0);
    setLogZoomY(1.0);
    setPixelWidth(400);
    setPixelHeight(350);
    setZoomFactorY(3.2);
    setViewBottom(62.0); //the lowest note visible (in semitones) from C0

    connect(m_very_fast_update_timer, SIGNAL(timeout()), this, SLOT(nextVeryFastUpdate()));
    connect(m_fast_update_timer, SIGNAL(timeout()), this, SLOT(nextFastUpdate()));
    connect(m_slow_update_timer, SIGNAL(timeout()), this, SLOT(nextSlowUpdate()));
    connect(this, SIGNAL(viewChanged()), this, SLOT(newUpdate()));
}

//------------------------------------------------------------------------------
View::~View()
{
    delete m_very_fast_update_timer;
    delete m_fast_update_timer;
    delete m_slow_update_timer;
}

// please call this after a window has been created
//------------------------------------------------------------------------------
void View::init()
{
    setViewOffset(viewWidth() / 2.0);
}

//------------------------------------------------------------------------------
void View::doUpdate()
{
    doSlowUpdate();
    doFastUpdate();
}

//------------------------------------------------------------------------------
void View::doSlowUpdate()
{
    m_need_slow_update = false;
    if(!m_slow_update_timer->isActive())
    {
        m_slow_update_timer->start(g_data->slowUpdateSpeed());
    }
    emit onSlowUpdate(m_current_time);
}

//------------------------------------------------------------------------------
void View::doFastUpdate()
{
    m_need_fast_update = false;
    if(!m_fast_update_timer->isActive())
    {
        m_fast_update_timer->start(g_data->fastUpdateSpeed());
    }
    emit onFastUpdate(m_current_time);
}

//------------------------------------------------------------------------------
void View::doVeryFastUpdate()
{
    m_need_very_fast_update = false;
    if(!m_very_fast_update_timer->isActive())
    {
        m_very_fast_update_timer->start(g_data->veryFastUpdateSpeed());
    }
    emit onVeryFastUpdate(m_current_time);
}

//------------------------------------------------------------------------------
void View::newUpdate()
{
    if(m_slow_update_timer->isActive())
    {
        m_need_slow_update = true;
    }
    else
    {
        m_need_slow_update = false;
        m_slow_update_timer->start(g_data->slowUpdateSpeed());
        emit onSlowUpdate(m_current_time);
    }
    if(m_fast_update_timer->isActive())
    {
        m_need_fast_update = true;
    }
    else
    {
        m_need_fast_update = false;
        m_fast_update_timer->start(g_data->fastUpdateSpeed());
        emit onFastUpdate(m_current_time);
    }
    if(m_very_fast_update_timer->isActive())
    {
        m_need_very_fast_update = true;
    }
    else
    {
        m_need_very_fast_update = false;
        m_very_fast_update_timer->start(g_data->veryFastUpdateSpeed());
        emit onVeryFastUpdate(m_current_time);
    }
}

//------------------------------------------------------------------------------
void View::nextVeryFastUpdate()
{
    if(m_need_very_fast_update)
    {
        m_need_very_fast_update = false;
        m_very_fast_update_timer->start(g_data->veryFastUpdateSpeed());
        emit onVeryFastUpdate(m_current_time);
    }
}

//------------------------------------------------------------------------------
void View::nextFastUpdate()
{
    if(m_need_fast_update)
    {
        m_need_fast_update = false;
        m_fast_update_timer->start(g_data->fastUpdateSpeed());
        emit onFastUpdate(m_current_time);
    }
}

//------------------------------------------------------------------------------
void View::nextSlowUpdate()
{
    if(m_need_slow_update)
    {
        m_need_slow_update = false;
        m_slow_update_timer->start(g_data->slowUpdateSpeed());
        emit onSlowUpdate(m_current_time);
    }
}

//------------------------------------------------------------------------------
void View::setCurrentTimeRaw(double p_x)
{
    if(p_x != m_current_time)
    {
        Channel * l_active_channel = g_data->getActiveChannel();
        if(l_active_channel)
        {
            p_x = l_active_channel->timeAtChunk(l_active_channel->chunkAtTime(p_x)); //align time to an integer sample step
        }
        m_current_time = p_x;
    }
}

//------------------------------------------------------------------------------
void View::setCurrentTime(double p_x)
{
    if(p_x != m_current_time)
    {
        Channel * l_active_channel = g_data->getActiveChannel();
        if(l_active_channel)
        {
            p_x = l_active_channel->timeAtChunk(l_active_channel->chunkAtTime(p_x)); //align time to an integer sample step
        }

        m_current_time = p_x;
        emit currentTimeChanged(p_x);
        emit timeViewRangeChanged(viewLeft(), viewRight());
        emit viewChanged();
    }
}

//------------------------------------------------------------------------------
void View::setViewOffset(double p_x)
{
    if(p_x < 0)
    {
        p_x = 0;
    }

    if(p_x > viewWidth())
    {
        p_x = viewWidth();
    }
    if(p_x != m_view_offset)
    {
        m_view_offset = p_x;
        emit timeViewRangeChanged(viewLeft(), viewRight());
        emit viewChanged();
    }
}

//------------------------------------------------------------------------------
void View::setViewBottomRaw(const double & p_y)
{
    if(p_y != m_view_bottom)
    {
        m_view_bottom = p_y;
        emit viewBottomChanged(g_data->topPitch() - viewHeight() - p_y);
    }
}

//------------------------------------------------------------------------------
void View::setViewBottom(const double & p_y)
{
    if(p_y != m_view_bottom)
    {
        m_view_bottom = p_y;
        emit viewBottomChanged(g_data->topPitch() - viewHeight() - p_y);
        emit viewChanged();
    }
}


// Changes the view without using a step value
//------------------------------------------------------------------------------
void View::changeViewX(const double & p_x)
{
    setCurrentTime(p_x);
    emit viewChanged();
}

//------------------------------------------------------------------------------
void View::changeViewY(const double & p_y)
{
    setViewBottom(g_data->topPitch() - viewHeight() - p_y);
    emit viewChanged();
}

//------------------------------------------------------------------------------
void View::setPixelHeight(int p_h)
{
    if(p_h != m_pixel_height)
    {
        m_pixel_height = p_h;
        if(viewHeight() > g_data->topPitch())
        {
            setLogZoomY(log(double(m_pixel_height) / g_data->topPitch()));
            emit logZoomYChanged(logZoomY());
        }
        emit scrollableYChanged(g_data->topPitch() - viewHeight());
        emit viewHeightChanged(viewHeight());
    }
}

//------------------------------------------------------------------------------
void View::setPixelWidth(int p_w)
{
    if(p_w != m_pixel_width)
    {
        m_pixel_width = p_w;
        if(viewWidth() > g_data->totalTime() * 2.0)
        {
            setLogZoomX(log(double(m_pixel_width) / (g_data->totalTime() * 2.0)));
            emit logZoomXChanged(logZoomX());
        }
        emit viewWidthChanged(viewWidth());
    }
}

//------------------------------------------------------------------------------
void View::setZoomFactorX(const double & p_x)
{
    if(p_x != logZoomX())
    {
        double l_old_view_width = viewWidth();
        setLogZoomX(p_x);
        emit logZoomXChanged(logZoomX());
        emit viewWidthChanged(viewWidth());
    
        setViewOffset(viewOffset() * (viewWidth() / l_old_view_width));
    }
}

//------------------------------------------------------------------------------
void View::setZoomFactorX( const double & p_x
                         , int p_fixed_x
                         )
{
    if(p_x != logZoomX())
    {
        double l_fixed_time = viewLeft() + zoomX() * double(p_fixed_x);
        double l_old_view_width = viewWidth();
        setLogZoomX(p_x);
        emit logZoomXChanged(logZoomX());
        emit viewWidthChanged(viewWidth());
    
        double l_ratio = viewWidth() / l_old_view_width;
        setViewOffset(viewOffset() * l_ratio);
    
        //shift the current time to keep the keep the time fixed at the mouse pointer
        double l_new_time = viewLeft() + zoomX() * double(p_fixed_x);
        g_data->updateActiveChunkTime(currentTime() - (l_new_time - l_fixed_time));
    }
}

//------------------------------------------------------------------------------
void View::setZoomFactorY(const double & p_y)
{
    if(p_y != logZoomY())
    {
        double l_prev_center_Y = viewBottom() + viewHeight() / 2.0;
        setLogZoomY(p_y);
        emit logZoomYChanged(logZoomY());
        emit scrollableYChanged(bound(g_data->topPitch() - viewHeight(), 0.0, g_data->topPitch()));
        emit viewHeightChanged(viewHeight());
    
        setViewBottom(l_prev_center_Y - viewHeight() / 2.0);
        emit viewBottomChanged(g_data->topPitch() - viewHeight() - viewBottom());
    }
}

//------------------------------------------------------------------------------
void View::setZoomFactorY( const double & p_y
                         , int p_fixed_y
                         )
{
    if(p_y != logZoomY())
    {
        double l_fixed_note = viewBottom() + zoomY() * p_fixed_y;
        setLogZoomY(p_y);
        emit logZoomYChanged(logZoomY());
        emit scrollableYChanged(bound(g_data->topPitch() - viewHeight(), 0.0, g_data->topPitch()));
        emit viewHeightChanged(viewHeight());
    
        double l_new_note = viewBottom() + zoomY() * p_fixed_y;
        setViewBottom(viewBottom() - (l_new_note - l_fixed_note));
        emit viewBottomChanged(g_data->topPitch() - viewHeight() - viewBottom());
    }
}

//------------------------------------------------------------------------------
void View::doAutoFollowing()
{
    if(!autoFollow())
    {
        return;
    }
    double l_time = currentTime();
    // We want the average note value for the time period currentTime to viewRight
 
    Channel * l_active_channel = g_data->getActiveChannel();

    if(l_active_channel == NULL)
    {
        return;
    }
  

    double l_start_time = l_time - (viewWidth() / 8.0);
    double l_stop_time = l_time + (viewWidth() / 8.0);
  
    int l_start_frame = bound(toInt(l_start_time / l_active_channel->timePerChunk()), 0, l_active_channel->totalChunks() - 1);
    int l_stop_frame = bound(toInt(l_stop_time / l_active_channel->timePerChunk()), 0, l_active_channel->totalChunks() - 1);

    float l_pitch = l_active_channel->averagePitch(l_start_frame, l_stop_frame);

    if(l_pitch < 0)
    {
        return; // There were no good notes detected
    }

    float l_new_bottom = l_pitch - (viewHeight() / 2.0);

    setViewBottom(l_new_bottom);
}

//------------------------------------------------------------------------------
void View::setAutoFollow(bool p_is_checked)
{
    m_auto_follow = p_is_checked;
    g_data->setSettingsValue("View/autoFollow", p_is_checked);
}

//------------------------------------------------------------------------------
void View::setBackgroundShading(bool p_is_checked)
{
    m_background_shading = p_is_checked;
    g_data->setSettingsValue("View/backgroundShading", p_is_checked);
    emit onSlowUpdate(m_current_time);
}
