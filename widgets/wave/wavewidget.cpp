/***************************************************************************
                          wavewidget.cpp  -  description
                             -------------------
    begin                : Mon Mar 14 2005
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
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

#include "wavewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
WaveWidget::WaveWidget(QWidget *parent)
: DrawWidget(parent)
{
    setZoomY(1.0);
}

//------------------------------------------------------------------------------
WaveWidget::~WaveWidget(void)
{
}

//------------------------------------------------------------------------------
void WaveWidget::setZoomY(const double & p_zoom_Y)
{
    if(m_zoom_Y != p_zoom_Y)
    {
        m_zoom_Y = p_zoom_Y;
        emit zoomYChanged(m_zoom_Y);
    }
}

//------------------------------------------------------------------------------
void WaveWidget::paintEvent(QPaintEvent *)
{
    Channel * l_active = g_data->getActiveChannel();

    beginDrawing(false);

    if(l_active)
    {
        l_active->lock();
        AnalysisData * l_data = l_active->dataAtCurrentChunk();
        int l_center_X = width() / 2;
        if(l_data)
        {
            double l_freq = l_data->getFundamentalFreq();
            double l_period = double(l_active->rate()) / l_freq;
            double l_num_periods = double(l_active->size()) / l_period;
            //pixels per period
            double l_scale_X = l_period * double(width()) / double(l_active->size());

            //draw altinating background color indicating period
            if(g_data->getView().backgroundShading() && l_period > 4.0 && l_period < double(l_active->get_nsdf_data().size()))
            {
                int l_n = int(ceil(double(l_center_X) / l_scale_X));
                get_painter().setPen(Qt::NoPen);
                QColor l_color1 = colorBetween(g_data->backgroundColor(), g_data->shading1Color(), l_data->getCorrelation());
                QColor l_color2 = colorBetween(g_data->backgroundColor(), g_data->shading2Color(), l_data->getCorrelation());
                for(int l_j = -l_n; l_j < l_n; l_j++)
                {
                    int l_x = l_center_X + toInt(l_scale_X * double(l_j));
                    get_painter().setBrush((l_j % 2) ? l_color1 : l_color2);
                    get_painter().drawRect(l_x, 0, toInt(l_scale_X * double(l_j + 1)) - toInt(l_scale_X * double(l_j)), height());
                }
                get_painter().setPen(colorBetween(g_data->backgroundColor(), Qt::black, 0.3 * l_data->getCorrelation()));
                for(int l_j = -l_n; l_j < l_n; l_j++)
                {
                    int l_x = l_center_X + toInt(l_scale_X * l_j);
                    get_painter().drawLine(l_x, 0, l_x, height());
                }
            }
            else
            {
                clearBackground();
            }
            QString l_num_periods_text;
            l_num_periods_text.sprintf("# Periods = %lf", l_num_periods);
            get_painter().setPen(Qt::black);
            get_painter().drawText(5, 15, l_num_periods_text);
        }
        else
        {
            clearBackground();
        }
    }
    else
    {
        clearBackground();
    }

    double l_dh2 = double(height() - 1) / 2.0;

    //draw the center line
    QPalette l_palette;
    get_painter().setPen(QPen(colorBetween(l_palette.color(QPalette::Window), Qt::black, 0.3), 0));
    get_painter().drawLine(0, toInt(l_dh2), width(), toInt(l_dh2));

    if(l_active)
    {
        //draw the waveform
        // only do every second pixel (for speed)
        int l_w = width() / 2;
        if(int(m_point_array.size()) != l_w)
        {
            m_point_array.resize(l_w);
        }
        double l_scale_Y = l_dh2 * zoomY();

        //Use Bresenham's algorithm in 1d to choose the points to draw
        const Array1d<float> & l_filtered_data = l_active->get_filtered_input();

        int l_int_step = int(l_filtered_data.size() / l_w);
        int l_remainder_step = l_filtered_data.size() - (l_int_step * l_w);
        int l_pos = 0;
        int l_remainder = 0;
        for(int l_j = 0; l_j < l_w; l_j++, l_pos += l_int_step, l_remainder += l_remainder_step)
        {
            if(l_remainder >= l_w)
            {
                l_pos++;
                l_remainder -= l_w;
            }
            myassert(l_pos < l_filtered_data.size());
            if(l_pos >= l_filtered_data.size())
            {
                printf("pos = %d, l_filtered_data.size()=%d\n", l_pos, l_filtered_data.size());
            }
            m_point_array.setPoint(l_j, l_j * 2, toInt(l_dh2 - (l_filtered_data.at(l_pos)) * l_scale_Y));
        }
        get_painter().setPen(QPen(l_active->get_color(), 0));
        get_painter().drawPolyline(m_point_array);
        l_active->unlock();
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize WaveWidget::sizeHint(void) const
{
    return QSize(500, 128);
}

//------------------------------------------------------------------------------
const double & WaveWidget::zoomY(void) const
{
    return m_zoom_Y;
}

// EOF
