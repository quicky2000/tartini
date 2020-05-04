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
#include <QPaintEvent>

#include "hcirclewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
HCircleWidget::HCircleWidget(QWidget *p_parent)
: DrawWidget(p_parent, "HCircleWidget")
{
    //make the widget get updated when the view changes
    m_zoom = 0.001;
    m_threshold = -100;
    m_lowest_value = -110;
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));
}

//------------------------------------------------------------------------------
HCircleWidget::~HCircleWidget()
{
} 

//------------------------------------------------------------------------------
void HCircleWidget::setZoom(double num)
{
    if(m_zoom != num)
    {
        m_zoom = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::setThreshold(double num)
{
    if(m_threshold != num)
    {
        m_threshold = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::setLowestValue(double num)
{
    if(m_lowest_value != num)
    {
        m_lowest_value = num;
    }
}

//------------------------------------------------------------------------------
void HCircleWidget::paintEvent( QPaintEvent * )
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel * l_active_channel = g_data->getActiveChannel();
    int l_num_harmonics = 40;

    beginDrawing();

    double l_scale;

    QFont * l_font = new QFont(get_painter().font());
    l_font->setPointSize(6);
    get_painter().setFont(*l_font);
    QString l_string;

    if(m_threshold > m_lowest_value)
    {
        get_painter().setPen(QPen(colorBetween(g_data->backgroundColor(), qRgb(128,128,128), 0.3), 2));
        int l_radius = toInt((double)height() * m_zoom * (m_threshold - m_lowest_value));
        get_painter().drawEllipse(width() / 2 - l_radius, height() / 2 - l_radius, 2 * l_radius, 2 * l_radius);
        get_painter().drawText(width() / 2 - l_radius + 5, height() / 2, "Threshold");
    }

    get_painter().setPen(QPen(QColor(128,128,128),1));

    double l_step = 10;

    for(l_scale = 0; l_scale > -160; l_scale -= l_step)
    {
        if (l_scale > m_lowest_value)
        {
            int l_radius = toInt((double)height() * m_zoom * (l_scale - m_lowest_value));
            get_painter().drawEllipse(width() / 2 - l_radius,height() / 2 - l_radius, 2 * l_radius, 2 * l_radius);
	        std::stringstream l_stream;
	        l_stream << std::fixed << std::setprecision(1) << l_scale;
	        std::string l_scale_string = l_stream.str();
            get_painter().drawText(width() / 2 + l_radius, height() / 2, QString::fromStdString(l_scale_string));
        }
    }


    l_font = new QFont(get_painter().font());
    l_font->setPointSize(8);
    get_painter().setFont(*l_font);

    if(l_active_channel)
    {
        AnalysisData *l_data = l_active_channel->dataAtCurrentChunk();

        if (l_data)
        {
            int l_i;
            double l_octave = l_data->getFundamentalFreq();
            double l_next_octave = l_octave * 2;
            int l_dot_size = 6;
            int l_half_dot_size = l_dot_size / 2;
            get_painter().setPen(QPen(Qt::black, 2));
            int l_m = MIN(l_data->getHarmonicAmpNoCutOffSize(), (unsigned) l_num_harmonics);
            assert(l_data->getHarmonicAmpNoCutOffSize() == l_data->getHarmonicFreqSize());
            for(l_i = 0; l_i < l_m; l_i++)
            {
                if (l_data->getHarmonicFreqAt(l_i) > l_next_octave)
                {
                    l_octave = l_next_octave;
                    l_next_octave = l_octave * 2;
                }
                if (l_data->getHarmonicAmpNoCutOffAt(l_i) > MAX(m_threshold, m_lowest_value))
                {
                    double l_angle = (l_data->getHarmonicFreqAt(l_i) - l_octave) / (l_next_octave - l_octave) * 2 * PI;
                    double l_size = height() * m_zoom * (l_data->getHarmonicAmpNoCutOffAt(l_i) - m_lowest_value);
                    double l_size1 = height() * m_zoom * (MAX(m_threshold, m_lowest_value) - m_lowest_value);
                    int l_x1 = toInt(sin(l_angle) * l_size1);
                    int l_y1 = toInt(-cos(l_angle) * l_size1);
                    int l_x = toInt(sin(l_angle) * l_size);
                    int l_y = toInt(-cos(l_angle) * l_size);

                    get_painter().drawLine(width() / 2 + l_x1, height() / 2 + l_y1, width() / 2 + l_x, height() / 2 + l_y);
                    get_painter().drawEllipse(width() / 2 + l_x - l_half_dot_size, height() / 2 + l_y - l_half_dot_size, l_dot_size, l_dot_size);
                    get_painter().drawText(width() / 2 + l_x + 5, height() / 2 + l_y, QString::fromStdString(std::to_string(l_i + 1)));
                }
            }
        }
    }
    endDrawing();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: HCircleWidget::paintEvent()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
QSize HCircleWidget::sizeHint() const
{
    return QSize(300, 200);
}
// EOF
