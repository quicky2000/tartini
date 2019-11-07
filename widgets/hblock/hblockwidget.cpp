/***************************************************************************
                          hblockwidget.cpp  -  description
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

#include "hblockwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
HBlockWidget::HBlockWidget(QWidget *p_parent)
: DrawWidget(p_parent, "HBlockWidget")
{
}

//------------------------------------------------------------------------------
HBlockWidget::~HBlockWidget()
{
}

//------------------------------------------------------------------------------
void HBlockWidget::paintEvent( QPaintEvent * )
{
    Channel *l_active_channel = g_data->getActiveChannel();

    beginDrawing();

    if(l_active_channel)
    {
        AnalysisData *l_the_data = l_active_channel->dataAtCurrentChunk();
        if(l_the_data)
        {
            //get a copy of l_the_data so we don't hold the mutex for too long
            l_active_channel->lock();
            AnalysisData l_data = *l_the_data;
            l_active_channel->unlock();
    
            // Get the frame's fundamental frequency
            float l_fund = l_data.getFundamentalFreq();
  
            // Work out the bar height for each harmonic
            double l_bar_height = double(height()) / double(l_data.getHarmonicFreqSize());
            QPalette l_palette;
            QColor l_color = l_palette.color(QPalette::Window);
            QColor l_fill_color = colorBetween(l_color, l_active_channel->get_color(), l_data.getVolumeValue(*g_data));
            QColor l_outline_color = colorBetween(l_color, Qt::black, l_data.getVolumeValue(*g_data));
            get_painter().setBrush(l_fill_color);
  
            int l_bar_start = 0;
            float l_bar_width = 0;
            int l_diff = 0;

            /*
             * Each note has a fundamental frequency f, which comes from the lookup table.
             * The harmonic frequencies are defined as f, 2f, 3f, 4f, 5f...
             * m_harmonic_freq stores what the harmonics have been calculated to be.
             */
            for(uint l_index = 0; l_index < l_data.getHarmonicFreqSize(); l_index++)
            {
                get_painter().setPen(l_outline_color);
                get_painter().setBrush(colorBetween(l_fill_color, Qt::black, l_data.getHarmonicNoiseAt(l_index)));
                // Work out how many pixels wide the harmonic should be
                l_bar_width = (l_data.getHarmonicAmpAt(l_index)) * width();
                // Work out how many pixels the harmonic should be offset from where it would be
                // if it were exactly (l_index+1)f
                l_diff = toInt( (l_data.getHarmonicFreqAt(l_index) - (l_index + 1) * l_fund) / l_fund * l_bar_width );
                // Work out the starting position, and draw the bar
                l_bar_start = toInt( ((width() / 2) + l_diff) - l_bar_width / 2);
                int l_bar_bottom = height() - toInt(l_bar_height * l_index);
                get_painter().drawRect(l_bar_start, l_bar_bottom, toInt(l_bar_width), -toInt(l_bar_height));
                // Draw the centre line on the bar
                get_painter().setPen(Qt::white);
                get_painter().drawLine((width() / 2) + l_diff, l_bar_bottom, (width() / 2) + l_diff, l_bar_bottom - toInt(l_bar_height));
            }
            // Draw the exact line (f, 2f, 3f...)
            get_painter().setPen(Qt::white);
            get_painter().drawLine(width() / 2, 0, width() / 2, height());
        }
    }
    endDrawing();
}

//------------------------------------------------------------------------------
QSize HBlockWidget::sizeHint() const
{
    return QSize(300, 200);
}
// EOF
