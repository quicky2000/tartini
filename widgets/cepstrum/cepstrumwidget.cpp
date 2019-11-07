/***************************************************************************
                          cepstrumwidget.cpp  -  description
                             -------------------
    begin                : May 21 2005
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

#include "cepstrumwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
CepstrumWidget::CepstrumWidget(QWidget * p_parent)
: DrawWidget(p_parent, "CepstrumWidget")
{
}

//------------------------------------------------------------------------------
CepstrumWidget::~CepstrumWidget()
{
}

//------------------------------------------------------------------------------
void CepstrumWidget::paintEvent( QPaintEvent * )
{
    Channel * l_active_channel = g_data->getActiveChannel();

    AnalysisData * l_data = NULL;
    double l_pixel_step;
    int l_j;
    int l_x;

    beginDrawing(false);

    if(l_active_channel)
    {
        l_pixel_step = double(l_active_channel->get_cepstrum_data().size()) / double(width());
        if(int(m_point_array.size()) != width())
        {
            m_point_array.resize(width());
        }

        l_active_channel->lock();
        l_data = l_active_channel->dataAtCurrentChunk();
        if(l_data)
        {
            double l_freq = l_data->getFundamentalFreq();
            double l_period = double(l_active_channel->rate()) / l_freq;
            //pixels per period
            double l_scale_X = l_period * double(width()) / double(l_active_channel->get_nsdf_data().size());

            //draw alternating background color indicating l_period
            if(g_data->getView().backgroundShading() && l_period > 4.0 && l_period < double(l_active_channel->get_nsdf_data().size()))
            {
                //number of colored patches
                int l_n = int(ceil(double(width()) / l_scale_X));
                get_painter().setPen(Qt::NoPen);
                QColor l_color_1 = colorBetween(g_data->backgroundColor(), g_data->shading1Color(), l_data->getCorrelation());
                QColor l_color_2 = colorBetween(g_data->backgroundColor(), g_data->shading2Color(), l_data->getCorrelation());
                for(l_j = 0; l_j < l_n; l_j++)
                {
                    l_x = toInt(l_scale_X * double(l_j));
                    get_painter().setBrush((l_j % 2) ? l_color_1 : l_color_2);
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
            QString l_num_periods_text;
            l_num_periods_text.sprintf("Period = %lf", l_period);
            get_painter().setPen(Qt::black);
            get_painter().drawText(5, height() - 8, l_num_periods_text);

            get_painter().drawLine(0, height() / 2, width(), height() / 2);

            //draw the waveform
            double l_ratio = double(height()) / 2.0; //TODO: remove magic number
            get_painter().setPen(QPen(l_active_channel->get_color(), 0));
            for(int l_j = 0; l_j < width(); l_j++)
            {
                //cheap hack to go faster (by drawing less points)
                myassert(int(l_pixel_step * l_j) < l_active_channel->get_cepstrum_data().size());
                m_point_array.setPoint(l_j, l_j, height() / 2 - toInt(l_active_channel->get_cepstrum_data().at(int(l_pixel_step * l_j)) * l_ratio));
            }
            get_painter().drawPolyline(m_point_array);

            if(l_data->getCepstrumIndex() >= 0)
            {
                get_painter().setPen(Qt::blue);
                get_painter().setBrush(Qt::blue);
                int l_x1 = toInt(double(l_data->getCepstrumIndex()) / double(l_active_channel->get_cepstrum_data().size()) * double(width()));
                int l_y1 = height() / 2 - toInt(l_active_channel->get_cepstrum_data().at(l_data->getCepstrumIndex()) * height() / 2);
                get_painter().drawEllipse(l_x1 - 2, l_y1 - 2, 5, 5);
            }
        }
        else
        {
            clearBackground();
        }
        l_active_channel->unlock();
    }
    else
    {
        clearBackground();
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize CepstrumWidget::sizeHint() const
{
    return QSize(500, 128);
}
// EOF
