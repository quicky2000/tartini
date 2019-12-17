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
#include <QPalette>

// zoom cursors
#include "pics/zoomx.xpm"
#include "pics/zoomy.xpm"
#include "pics/zoomxout.xpm"
#include "pics/zoomyout.xpm"

#include "freqdrawwidget.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "musicnotes.h"

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//------------------------------------------------------------------------------
FreqDrawWidget::FreqDrawWidget( QWidget *p_parent
                              , const char* p_name
                              )
: DrawWidget(p_parent, p_name)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose);

    m_drag_mode = DragNone;

    QSizePolicy l_size_policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l_size_policy.setHeightForWidth(false);
    setSizePolicy(l_size_policy);

    setFocusPolicy(Qt::StrongFocus);
    g_data->getView().setPixelHeight(height());
}

//------------------------------------------------------------------------------
FreqDrawWidget::~FreqDrawWidget()
{
}

//------------------------------------------------------------------------------
void FreqDrawWidget::drawReferenceLines( QPaintDevice & p_paint_device
                                       , QPainter & p_painter
                                       , double p_current_time
                                       , double p_zoom_X
                                       , double p_view_bottom
                                       , double p_zoom_Y
                                       , int p_view_type
                                       )
{
    // Draw the lines and notes
    QFontMetrics l_font_metric = p_painter.fontMetrics();
    int l_font_height_space = l_font_metric.height() / 4;
    int l_font_width = l_font_metric.width("C#0") + 3;
  
    //number of pixels to jump between each line
    double l_step = 1.0 / p_zoom_Y;
    //in semi-tones
    int l_note_jump;
    if(l_step > 10.0)
    {
        l_note_jump = 1;
    }
    else if(l_step > 5.0)
    {
        l_note_jump = 6;
    }
    else
    {
        l_note_jump = 12;
    }
    double l_remainder = cycle(p_view_bottom, double(l_note_jump));
    double l_to_first_note = double(l_note_jump) - l_remainder;
    double l_start = l_to_first_note * l_step;
    double l_stop = double(p_paint_device.height());
    int l_name_index = toInt(p_view_bottom + l_to_first_note);
    l_step *= l_note_jump;

    //draw the note names and reference lines
    QString l_note_label;
    int l_line_Y = 0;
    for(double l_y = l_start; l_y < l_stop; l_y += l_step, l_name_index+=l_note_jump)
    {
        l_line_Y = p_paint_device.height() - toInt(l_y);
        if(!isBlackNote(l_name_index))
        {
            p_painter.setPen(Qt::black);
            l_note_label.sprintf("%s%d", music_notes::noteName(l_name_index).c_str(), noteOctave(l_name_index));
            p_painter.drawText(2, l_line_Y + l_font_height_space, l_note_label);
            if(noteValue(l_name_index) == 0)
            {
                p_painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));
            }
            else
            {
                //transperenct colors don't seem to work on the printer
                if(p_view_type == DRAW_VIEW_PRINT)
                {
                    p_painter.setPen(QPen(QColor(128, 128, 128), 1, Qt::SolidLine));
                }
                else
                {
                    p_painter.setPen(QPen(QColor(144, 156, 170), 1, Qt::DashDotDotLine));
                }
            }
        }
        else
        {
            if(p_view_type == DRAW_VIEW_PRINT)
            {
                p_painter.setPen(QPen(QColor(196, 196, 196), 1, Qt::SolidLine));
            }
            else
            {
                p_painter.setPen(QPen(QColor(136, 161, 180), 1, Qt::DotLine));
            }
        }
        int l_offset = toInt(p_current_time / p_zoom_X) % 32;
        p_painter.setClipRect(l_font_width, 0, p_paint_device.width() - l_font_width, p_paint_device.height());
        p_painter.drawLine(l_font_width - l_offset, l_line_Y, p_paint_device.width() - 1, l_line_Y);
        p_painter.setClipRect(0, 0, p_paint_device.width(), p_paint_device.height());
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::paintEvent(QPaintEvent *)
{
    View & l_view = g_data->getView();

    if(l_view.autoFollow() && g_data->getActiveChannel() && g_data->getRunning() == STREAM_FORWARD)
    {
        setChannelVerticalView(g_data->getActiveChannel(), l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY());
    }

    int l_cur_time_pixel = l_view.screenPixelX(l_view.currentTime());

    beginDrawing();

    DrawWidget::setLineWidth(3);

    //draw the red/blue background color shading if needed
    if(l_view.backgroundShading() && g_data->getActiveChannel())
    {
        drawChannelFilled(g_data->getActiveChannel(), get_painter(), l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);
    }
  

    drawReferenceLines(*this, get_painter(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);

    //draw all the visible channels
    for(uint l_i = 0; l_i < g_data->getChannelsSize(); l_i++)
    {
        Channel * l_channel = g_data->getChannelAt(l_i);
        if(!l_channel->isVisible())
        {
            continue;
        }
        drawChannel(*this, l_channel, get_painter(), l_view.viewLeft(), l_view.currentTime(), l_view.zoomX(), l_view.viewBottom(), l_view.zoomY(), DRAW_VIEW_NORMAL);
    }

    QPalette l_palette;
    QColor l_foreground = l_palette.color(QPalette::WindowText);
    // Draw a light grey band indicating which time is being used in the current window
    if(g_data->getActiveChannel())
    {
        QColor l_line_color = l_foreground;
        l_line_color.setAlpha(50);
        Channel * l_channel = g_data->getActiveChannel();
        double l_half_window_time = (double)l_channel->size() / (double)(l_channel->rate() * 2);
        int l_pixel_left = l_view.screenPixelX(l_view.currentTime() - l_half_window_time);
        int l_pixel_right = l_view.screenPixelX(l_view.currentTime() + l_half_window_time);
        get_painter().fillRect(l_pixel_left, 0, l_pixel_right - l_pixel_left, height() - 1, l_line_color);
    }

    // Draw the current time line
    get_painter().setPen(QPen(l_foreground, 1));
    get_painter().drawLine(l_cur_time_pixel, 0, l_cur_time_pixel, height() - 1);

    endDrawing();
}

//------------------------------------------------------------------------------
Channel * FreqDrawWidget::channelAtPixel( int p_x
                                        , int p_y
                                        )
{
    double l_time = mouseTime(p_x);
    float l_pitch = mousePitch(p_y);
    //10 pixel l_tolerance
    float l_tolerance = 6 * g_data->getView().zoomY();

    std::vector<Channel*> channels;

    //loop through channels in reverse order finding which one the user clicked on
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
QSize FreqDrawWidget::sizeHint() const
{
    return QSize(400, 350);
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mousePressEvent( QMouseEvent * p_mouse_event)
{
    View & l_view = g_data->getView();
    int l_time_X = toInt(l_view.viewOffset() / l_view.zoomX());
    m_drag_mode = DragNone;
  
    //Check if user clicked on center bar, to drag it
    if(within(4, p_mouse_event->x(), l_time_X))
    {
        m_drag_mode = DragTimeBar;
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

        Channel * l_channel = channelAtPixel(p_mouse_event->x(), p_mouse_event->y());
        if(l_channel)
        {
            //Clicked on a channel
            g_data->setActiveChannel(l_channel);
            m_drag_mode = DragChannel;
        }
        else
        {
            //Must have clicked on background
            m_drag_mode = DragBackground;
            m_down_time = l_view.currentTime();
            m_down_note = l_view.viewBottom();
        }
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mouseMoveEvent( QMouseEvent * p_mouse_event)
{
    View & l_view = g_data->getView();
    int l_pixel_at_current_time_X = toInt(l_view.viewOffset() / l_view.zoomX());
  
    switch(m_drag_mode)
    {
        case DragTimeBar:
        {
            int l_new_X = l_pixel_at_current_time_X + (p_mouse_event->x() - m_mouse_X);
            l_view.setViewOffset(double(l_new_X) * l_view.zoomX());
            m_mouse_X = p_mouse_event->x();
            l_view.doSlowUpdate();
        }
        break;
        case DragBackground:
            l_view.setViewBottom(m_down_note - (m_mouse_Y - p_mouse_event->y()) * l_view.zoomY());
            g_data->updateActiveChunkTime(m_down_time - (p_mouse_event->x() - m_mouse_X) * l_view.zoomX());
            l_view.doSlowUpdate();
            break;
        case DragNone:
            if(within(4, p_mouse_event->x(), l_pixel_at_current_time_X)) \
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
    }
}

//------------------------------------------------------------------------------
void FreqDrawWidget::mouseReleaseEvent( QMouseEvent *)
{
    m_drag_mode = DragNone;
}

//------------------------------------------------------------------------------
double FreqDrawWidget::mouseTime(int p_x)
{
    return g_data->getView().viewLeft() + g_data->getView().zoomX() * p_x;
}

//------------------------------------------------------------------------------
double FreqDrawWidget::mousePitch(int p_y)
{
    return g_data->getView().viewBottom() + g_data->getView().zoomY() * (height() - p_y);
}

//------------------------------------------------------------------------------
void FreqDrawWidget::wheelEvent(QWheelEvent * p_wheel_event)
{
    View & l_view = g_data->getView();
    double l_amount = double(p_wheel_event->delta() / WHEEL_DELTA);
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
            if(g_data->getRunning() == STREAM_FORWARD)
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount * 0.3);
            }
            else
            {
                //zoom toward mouse pointer
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount * 0.3, p_wheel_event->x());
            }
            l_view.setZoomFactorY(l_view.logZoomY() + l_amount * 0.2, height() - p_wheel_event->y());
        }
        else
        {
            //zoom out toward center
            if(g_data->getRunning() == STREAM_FORWARD)
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount * 0.3);
            }
            else
            {
                l_view.setZoomFactorX(l_view.logZoomX() + l_amount * 0.3, width() / 2);
            }
            l_view.setZoomFactorY(l_view.logZoomY() + l_amount * 0.2, height() / 2);
        }
    }
    else
    {
        //mouse wheel scrolls
        l_view.setViewBottom(l_view.viewBottom() + l_amount * l_view.viewHeight() * 0.1);
    }
    l_view.doSlowUpdate();

    p_wheel_event->accept();
}

//------------------------------------------------------------------------------
void FreqDrawWidget::resizeEvent (QResizeEvent * p_resize_event)
{
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
void FreqDrawWidget::keyPressEvent( QKeyEvent * p_key_event )
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
void FreqDrawWidget::keyReleaseEvent( QKeyEvent * p_key_event)
{
    switch (p_key_event->key())
    {
        case Qt::Key_Control: // Unset the cursor if the control or alt keys were released, ignore otherwise
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
void FreqDrawWidget::leaveEvent ( QEvent * p_event)
{
    unsetCursor();
    QWidget::leaveEvent(p_event);
}

// EOF
