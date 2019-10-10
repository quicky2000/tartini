/***************************************************************************
                          correlationwidget.cpp  -  description
                             -------------------
    begin                : May 2 2005
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
#include <qpixmap.h>
#include <qpainter.h>
#include <q3memarray.h>
#include <QPaintEvent>

#include "correlationwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"
#include "myalgo.h"

//------------------------------------------------------------------------------
CorrelationWidget::CorrelationWidget(QWidget * p_parent)
: DrawWidget(p_parent)
{
    m_aggregate_mode = 0;
}

//------------------------------------------------------------------------------
CorrelationWidget::~CorrelationWidget(void)
{
}

//------------------------------------------------------------------------------
void CorrelationWidget::paintEvent( QPaintEvent * )
{
    Channel *l_active_channel = gdata->getActiveChannel();

    AnalysisData * l_data = NULL;
    int l_chunk = 0;
    double l_dh2 = double(height() - 1) / 2.0;
    int l_x;
    int l_y;

    beginDrawing(false);

    if(l_active_channel)
    {
        l_active_channel->lock();
        l_chunk = l_active_channel->currentChunk();
        l_data = l_active_channel->dataAtChunk(l_chunk);

        if(l_data)
        {
            double l_freq = l_data->getFundamentalFreq();
            double l_period = double(l_active_channel->rate()) / l_freq;
            //pixels per l_period
            double l_scale_X = l_period * double(width()) / double(l_active_channel->get_nsdf_data().size());
      
            //draw alternating background color indicating l_period
            if(gdata->getView().backgroundShading() && l_period > 4.0 && l_period < double(l_active_channel->get_nsdf_data().size()))
            {
                //number of colored patches
                int l_n = int(ceil(double(width()) / l_scale_X));
                get_painter().setPen(Qt::NoPen);
                QColor l_color_1 = colorBetween(gdata->backgroundColor(), gdata->shading1Color(), l_data->getCorrelation());
                QColor l_color_2 = colorBetween(gdata->backgroundColor(), gdata->shading2Color(), l_data->getCorrelation());
                for(int l_j = 0; l_j<l_n; l_j++)
                {
                    l_x = toInt(l_scale_X*double(l_j));
                    get_painter().setBrush((l_j%2) ? l_color_1 : l_color_2);
                    get_painter().drawRect(l_x, 0, toInt(l_scale_X * double(l_j + 1)) - toInt(l_scale_X * double(l_j)), height());
                }
                get_painter().setPen(colorBetween(gdata->backgroundColor(), Qt::black, 0.3 * l_data->getCorrelation()));
                for(int l_j = 0; l_j < l_n; l_j++)
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

    //draw the horizontal center line
    get_painter().setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
    get_painter().drawLine(0, toInt(l_dh2), width(), toInt(l_dh2));

    if(l_active_channel)
    {
        if(gdata->doingFreqAnalysis())
        {
            //only do every second pixel (for speed)
            int l_w = width() / 2;
            //draw the waveform
            if(int(m_point_array.size()) != l_w)
            {
                m_point_array.resize(l_w);
            }
            if(m_lookup.size() != l_w)
            {
                m_lookup.resize(l_w);
            }

            const NoteData *l_current_note = l_active_channel->getCurrentNote();
            const Array1d<float> *l_input = &(l_active_channel->get_nsdf_data());
            if(l_current_note)
            {
                if(m_aggregate_mode == 1)
                {
                    l_input = &l_current_note->get_nsdf_aggregate_data();
                }
                else if(m_aggregate_mode == 2)
                {
                    l_input = &l_current_note->get_nsdf_aggregate_data_scaled();
                }
            }
            maxAbsDecimate1d(*l_input, m_lookup);
            for(int l_j = 0; l_j < l_w; l_j++)
            {
                m_point_array.setPoint(l_j, l_j * 2, toInt(l_dh2 - m_lookup[l_j] * l_dh2));
            }

            get_painter().setPen(QPen(l_active_channel->get_color(), 0));
            get_painter().drawPolyline(m_point_array);
        }
        if(l_data && (m_aggregate_mode == 0))
        {
            //pixels per index
            double ratio = double(width()) / double(l_active_channel->get_nsdf_data().size());
      
            //draw a dot at all the period estimates
            get_painter().setPen(Qt::blue);
            get_painter().setBrush(Qt::blue);
            for(int l_j = 0; l_j < int(l_data->getPeriodEstimatesSize()); l_j++)
            {
                l_x = toInt(double(l_data->getPeriodEstimatesAt(l_j)) * ratio);
                l_y = toInt(l_dh2 - l_data->getPeriodEstimatesAmpAt(l_j) * l_dh2);
                get_painter().drawEllipse(l_x - 2, l_y - 2, 5, 5);
            }

            if(l_data->getHighestCorrelationIndex() >= 0)
            {
                float l_highest = l_data->getPeriodEstimatesAmpAt(l_data->getHighestCorrelationIndex());
                //draw threshold line
                get_painter().setPen(QPen(colorBetween(colorGroup().background(), Qt::black, 0.3), 0));
                l_y = toInt(l_dh2 - (l_highest * l_active_channel->threshold()) * l_dh2);
                get_painter().drawLine(0, l_y, width(), l_y);
      
                //draw a dot at the highest correlation period
                get_painter().setPen(Qt::black);
                get_painter().setBrush(Qt::black);
                l_x = toInt(double(l_data->getPeriodEstimatesAt(l_data->getHighestCorrelationIndex())) * ratio);
                l_y = toInt(l_dh2 - l_highest * l_dh2);
                get_painter().drawEllipse(l_x - 2, l_y - 2, 5, 5);
            }
      
            //draw a dot at the chosen correlation period
            if(l_data->getChosenCorrelationIndex() >= 0)
            {
                get_painter().setPen(Qt::red);
                get_painter().setBrush(Qt::red);
                l_x = toInt(double(l_data->getPeriodEstimatesAt(l_data->getChosenCorrelationIndex())) * ratio);
                l_y = toInt(l_dh2 - l_data->getPeriodEstimatesAmpAt(l_data->getChosenCorrelationIndex()) * l_dh2);
                get_painter().drawEllipse(l_x - 2, l_y - 2, 5, 5);
            }

            //draw a line at the chosen correlation period
            if(l_data->getChosenCorrelationIndex() >= 0)
            {
                get_painter().setPen(Qt::green);
                get_painter().setBrush(Qt::green);
                l_x = toInt(double(l_active_channel->periodOctaveEstimate(l_chunk)) * ratio);
                get_painter().drawLine(l_x, 0, l_x, height());
            }
        }
        l_active_channel->unlock();
    }
    endDrawing();
}

//------------------------------------------------------------------------------
void CorrelationWidget::setAggregateMode(int p_mode)
{
    m_aggregate_mode = p_mode;
    update();
}

//------------------------------------------------------------------------------
QSize CorrelationWidget::sizeHint(void) const
{
    return QSize(500, 128);
}

// EOF
