/***************************************************************************
                          hstackwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#include <QPolygon>
#include <QPaintEvent>

#include "hstackwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
HStackWidget::HStackWidget(QWidget *p_parent)
: DrawWidget(p_parent, "HStackWidget")
{
    //make the widget get updated when the view changes
    m_window_size = 128;
    m_view_height = 100;
    m_top = 0;

    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

//------------------------------------------------------------------------------
void HStackWidget::setWindowSize(double p_window_size)
{ 
    if(m_window_size != p_window_size)
    {
        m_window_size = toInt(p_window_size);
        emit windowSizeChanged(static_cast<float>(m_window_size));
    }
}

//------------------------------------------------------------------------------
void HStackWidget::setDBRange(double p_range)
{ 
    if(m_view_height != p_range)
    { 
        m_view_height = p_range;
        emit dBRangeChanged(m_view_height);
    }
}


//------------------------------------------------------------------------------
HStackWidget::~HStackWidget()
{
}

//------------------------------------------------------------------------------
std::string HStackWidget::format_label(float p_label)
{
    std::stringstream l_stream;
    l_stream << std::fixed;
    if(m_view_height < 10)
    {
        l_stream << std::setprecision(2);
    }
    else
    {
        l_stream << std::setprecision(1);
    }
    l_stream << p_label;
    return l_stream.str();
}

//------------------------------------------------------------------------------
void HStackWidget::paintEvent(QPaintEvent *)
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel * l_active_channel = GData::getUniqueInstance().getActiveChannel();

    beginDrawing();
    
    if(l_active_channel) 
    { 
        AnalysisData *l_data;
        l_active_channel->lock();

        int l_start_chunk = l_active_channel->currentChunk() - m_window_size / 2;
        int l_i;
        int l_j;
        int l_num_harmonics = 16;
        
        float l_scale_Y = height() / m_view_height;
        float l_scale_X = static_cast<float>(width()) / static_cast<float>(m_window_size);
        int l_not_on_graph = height() + 10;

        QPolygon l_points[l_num_harmonics];
        
        for(l_j = 0; l_j < l_num_harmonics; l_j++) 
        { 
            l_points[l_j].resize(m_window_size + 5); 
        }

        QString l_string;
        get_painter().setPen(qRgb(128,128,128));

        float l_lbl;

        float l_increase = 10;

        if(m_view_height < 50) 
        { 
            l_increase = 5; 
        }
        if (m_view_height < 10) 
        { 
            l_increase = 1;
        }
        
        for(l_lbl = 0; l_lbl < m_view_height; l_lbl += l_increase)
        {
            get_painter().drawLine(0, -toInt((-m_top + l_lbl) * l_scale_Y), width(), -toInt((-m_top + l_lbl) * l_scale_Y));
            get_painter().drawText(0, -toInt((-m_top + l_lbl) * l_scale_Y), QString::fromStdString(format_label(l_lbl)));

            get_painter().drawLine(0, -toInt((-m_top - l_lbl) * l_scale_Y), width(), -toInt((-m_top - l_lbl) * l_scale_Y));
            get_painter().drawText(0, -toInt((-m_top - l_lbl) * l_scale_Y), QString::fromStdString(format_label(-l_lbl)));
        }
        
        for(l_i = -1; l_i <= m_window_size + 1; l_i++) 
        { 
            l_data = l_active_channel->dataAtChunk(l_start_chunk + l_i);
            if (l_data != nullptr) 
            { 
                int l_m = MIN(l_data->getHarmonicAmpNoCutOffSize(), static_cast<unsigned>(l_num_harmonics));
                for(l_j = 0; l_j < l_m;l_j++) 
                { 
                    if (!isinf(l_data->getHarmonicAmpNoCutOffAt(l_j))) 
                    { 
                        l_points[l_j].setPoint(l_i + 2, toInt(l_scale_X * static_cast<float>(l_i)), -toInt((-m_top + l_data->getHarmonicAmpNoCutOffAt(l_j)) * l_scale_Y));
                    } 
                    else 
                    { 
                        l_points[l_j].setPoint(l_i + 2, toInt(l_scale_X * static_cast<float>(l_i)), l_not_on_graph);
                    } 
                }
                for (l_j = l_m; l_j < l_num_harmonics; l_j++) 
                { 
                    l_points[l_j].setPoint(l_i + 2, toInt(l_scale_X * static_cast<float>(l_i)), l_not_on_graph);
                } 
            } 
            else 
            { 
                for(l_j = 0; l_j < l_num_harmonics;l_j++) 
                { 
                    l_points[l_j].setPoint(l_i + 2,toInt(l_scale_X * static_cast<float>(l_i)),l_not_on_graph);
                } 
            } 
        }
        
        for(l_i = 0; l_i < l_num_harmonics; l_i++) 
        { 
            l_points[l_i].setPoint(0, -1, l_not_on_graph);
            l_points[l_i].setPoint(m_window_size + 4, width() + 1, l_not_on_graph);
            get_painter().setBrush(colorBetween(qRgb(255,255,255), qRgb(0, 255, 0), float(l_i) / l_num_harmonics));
            get_painter().setPen(colorBetween(qRgb(128,128,128), qRgb(0, 128, 0), float(l_i) / l_num_harmonics));
            get_painter().drawPolygon(l_points[l_i]); 
        }
        
        get_painter().drawLine(width() / 2, 0, width() / 2, height());
        get_painter().drawLine(width() - 1, 0, width() - 1, height());
        
        l_active_channel->unlock(); 
    } 
    
    endDrawing();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: HStackWidget::paintEvent()" << std::endl;
#endif // TIME_PAINT
}

// EOF
