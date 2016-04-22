/***************************************************************************
                          fftwidget.cpp  -  description
                             -------------------
    begin                : May 18 2005
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

#include "fftwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
FFTWidget::FFTWidget(QWidget *p_parent)
: DrawWidget(p_parent, "FFTWidget")
{
}

//------------------------------------------------------------------------------
FFTWidget::~FFTWidget(void)
{
}

//------------------------------------------------------------------------------
void FFTWidget::paintEvent( QPaintEvent * )
{
    Channel *l_active_channel = g_data->getActiveChannel();

    AnalysisData *l_data;
    double l_pixel_step;
    int l_j;
    int l_x;

    beginDrawing(false);

    if(l_active_channel)
    {
        l_pixel_step = double(l_active_channel->get_fft_data1().size()) / double(width());
        if(int(m_point_array.size()) != width())
        {
            m_point_array.resize(width());
        }

        l_active_channel->lock();
        l_data = l_active_channel->dataAtCurrentChunk();

        if(l_data)
        {
            double l_freq = l_data->getFundamentalFreq();
            double l_normalised_freq = l_freq / double(l_active_channel->rate()) * 2.0;
            //pixels per fundamental l_freq
            double l_scale_X = l_normalised_freq * double(width());
      
            //draw alternating background color indicating period
            if(g_data->getView().backgroundShading() && l_scale_X > 4.0 && l_scale_X < double(width()))
            {
                //number of colored patches
                int l_n = int(ceil(double(width()) / l_scale_X));
                get_painter().setPen(Qt::NoPen);
                QColor l_color1 = colorBetween(g_data->backgroundColor(), g_data->shading1Color(), l_data->getCorrelation());
                QColor l_color2 = colorBetween(g_data->backgroundColor(), g_data->shading2Color(), l_data->getCorrelation());
                for(l_j = 0; l_j < l_n; l_j++)
                {
                    l_x = toInt(l_scale_X * double(l_j));
                    get_painter().setBrush((l_j % 2) ? l_color1 : l_color2);
                    get_painter().drawRect(l_x, 0, toInt(l_scale_X * double(l_j + 1)) - toInt(l_scale_X * double(l_j)), height());
                }
                get_painter().setPen(colorBetween(g_data->backgroundColor(), Qt::black, 0.3 * l_data->getCorrelation()));
                for(l_j = 0; l_j < l_n; l_j++)
                {
                    l_x = toInt(l_scale_X * double(l_j));
                    get_painter().drawLine(l_x, 0, l_x, height());
                }
            }
            else
            {
                clearBackground();
            }
            QString l_fund_freq_text;
            l_fund_freq_text.sprintf("Fundamental Frequency = %lf", l_freq);
            get_painter().setPen(Qt::black);
            get_painter().drawText(5, 15, l_fund_freq_text);
        }
        else
        {
            clearBackground();
        }

        //draw the waveform
        get_painter().setPen(QPen(Qt::red, 0));
        for(l_j = 0; l_j < width(); l_j++)
        {
            //cheap hack to go faster (by drawing less points)
            myassert(int(l_pixel_step * l_j) < l_active_channel->get_fft_data2().size());
            m_point_array.setPoint(l_j, l_j, height() - 1 - toInt(l_active_channel->get_fft_data2().at(int(l_pixel_step * l_j))*double(height())));
        }
        get_painter().drawPolyline(m_point_array);
        l_active_channel->unlock();
    }
    else
    {
        clearBackground();
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize FFTWidget::sizeHint(void) const
{
    return QSize(500, 128);
}

// EOF
