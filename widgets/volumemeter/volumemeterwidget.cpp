/***************************************************************************
                          volumemeterwidget.cpp  -  description
                             -------------------
    begin                : Tue Dec 21 2004
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
#include <QPaintEvent>

#include <vector>

#include "volumemeterwidget.h"
#include "gdata.h"
#include "channel.h"
#include "soundfile.h"
#include "analysisdata.h"
#include "useful.h"

//------------------------------------------------------------------------------
VolumeMeterWidget::VolumeMeterWidget(QWidget * p_parent)
: DrawWidget(p_parent, "VolumeMeterWidget")
{
    //make the widget get updated when the view changes
    connect(&(g_data->getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));

    // Define the number of labels to use for available places
    m_label_numbers.push_back(1);
    m_label_numbers.push_back(2);
    m_label_numbers.push_back(3);
    m_label_numbers.push_back(3);
    m_label_numbers.push_back(3);
    m_label_numbers.push_back(6);

    setFontSize(9);
}

//------------------------------------------------------------------------------
VolumeMeterWidget::~VolumeMeterWidget()
{
}

//------------------------------------------------------------------------------
void VolumeMeterWidget::setFontSize(int p_font_size)
{
    m_font_size = p_font_size;
    m_font = QFont("AnyStyle", m_font_size);
}

//------------------------------------------------------------------------------
void VolumeMeterWidget::paintEvent(QPaintEvent *)
{
    Channel * l_active = g_data->getActiveChannel();

    beginDrawing(false);

    QPalette l_palette;
    fillBackground(l_palette.color(QPalette::Window));
    get_painter().setFont(m_font);

    // Work out how many labels we can draw
    QFontMetrics l_font_metric = get_painter().fontMetrics();
    // Since we'll have two characters normally
    int l_label_width = l_font_metric.width("-60");
    int l_half_label_width = l_label_width / 2;
    int l_stopWidth = l_font_metric.width("0dB") / 2;
    // The actual width of the data
    int l_real_width = width() - l_stopWidth - l_half_label_width;
    // How many labels we should have
    int l_places = m_label_numbers[MIN(l_real_width / 20, 6) - 1];
    // How many pixels to jump between labels/markers
    int l_pixel_step = toInt(double(l_real_width) / double(l_places));
    int l_line_Y = height() - m_font_size - 2;

    int l_label_step = 10;

    // Determine how many dB we should jump by each time
    switch (l_places)
    {
        case 1:
            l_label_step = 60;
        break;
        case 2:
            l_label_step = 30;
        break;
        case 3:
            l_label_step = 20;
        break;
        case 6:
            l_label_step = 10;
        break;
        default:
            // This should never happen!
            myassert(false);
        break;
    }

    // Draw horizontal line
    get_painter().setPen(Qt::black);

    int l_y = height() - (l_line_Y / 2) - 1;

    for (int l_index = 0; l_index < l_places; l_index++)
    {
        int l_x = l_index * l_pixel_step;
        get_painter().drawText(l_x, l_y, QString::number(-60 + (l_index * l_label_step)));
    }
    get_painter().drawText(l_places * l_pixel_step - 2, l_y, "0dB");

    QColor l_colour;
    float l_the_val[2];
    if (l_active != NULL && l_active->isValidChunk(l_active->currentChunk()))
    {
        int l_chunk = l_active->currentChunk();
        if(l_active->getParent()->numChannels() > 1)
        {
            l_the_val[0] = l_active->getParent()->getChannel(0).dataAtChunk(l_chunk)->getMaxIntensityDB();
            l_the_val[1] = l_active->getParent()->getChannel(1).dataAtChunk(l_chunk)->getMaxIntensityDB();
        }
        else
        {
            l_the_val[0] = l_the_val[1] = l_active->dataAtChunk(l_chunk)->getMaxIntensityDB();
        }
    }
    else
    {
        l_the_val[0] = l_the_val[1] = 0.0;
    }

    for(int l_channel = 0; l_channel < 2; l_channel++)
    {
        double l_decibels = l_the_val[l_channel];
        // We'll show 60 dB
        int l_val = toInt((double(l_real_width / 60.0) * l_decibels) + l_real_width);

        // Not right - needs to be based on l_real_width
        int l_bar_width = 5;

        for(int l_j = l_half_label_width; l_j <= l_real_width; l_j += 10)
        {
            if(l_j < l_val)
            {
                if( l_j < (width() * 0.5))
                {
                    l_colour = Qt::blue;
                }
                else if( l_j < (width() * 0.85))
                {
                    l_colour = QColor(255, 120, 60);
                }
                else
                {
                    l_colour = Qt::red;
                }
            }
            else
            {
                l_colour = QColor(198, 198, 198);
            }

            if(l_channel == 0)
            {
                get_painter().fillRect(l_j, 2, l_bar_width, l_line_Y / 2 - 3, l_colour);
            }
            else
            {
                get_painter().fillRect(l_j, height() - (l_line_Y / 2) + 1, l_bar_width, l_line_Y / 2 - 3, l_colour);
            }
        }
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize VolumeMeterWidget::sizeHint() const
{
    return QSize(256, 30);
}
// EOF
