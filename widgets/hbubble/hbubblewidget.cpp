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

#include "hbubblewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
HBubbleWidget::HBubbleWidget(QWidget *p_parent)
: DrawWidget(p_parent, "HBubbleWidget")
{
    fprintf(stderr,"Initializing\n");
    m_history_chunks = 32;
    m_num_harmonics = 40;

    //make the widget get updated when the view changes
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onFastUpdate(double)), this, SLOT(update()));
    fprintf(stderr,"Done\n");
}

//------------------------------------------------------------------------------
HBubbleWidget::~HBubbleWidget()
{
} 

//------------------------------------------------------------------------------
void HBubbleWidget::setNumHarmonics(double num)
{
    if (m_num_harmonics != toInt(num))
    {
        m_num_harmonics = toInt(num);
        emit numHarmonicsChanged(static_cast<double>(num));
    }
}


//------------------------------------------------------------------------------
void HBubbleWidget::setHistoryChunks(double num)
{
    if (m_history_chunks != toInt(num))
    {
        m_history_chunks = toInt(num);
        emit historyChunksChanged(static_cast<double>(num));
    }
}

//------------------------------------------------------------------------------
void HBubbleWidget::paintEvent( QPaintEvent * )
{
#ifdef TIME_PAINT
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PAINT

    Channel * l_active_channel = GData::getUniqueInstance().getActiveChannel();
    AnalysisData * l_data;
    int l_i;
    int l_j;
    beginDrawing();

    get_painter().setPen(Qt::NoPen);
    if(l_active_channel)
    {
        for(l_j = 0; l_j < m_history_chunks; l_j++)
        {
            l_data = l_active_channel->dataAtChunk(l_active_channel->currentChunk() - m_history_chunks + l_j + 1);
            if(l_data != nullptr)
            {
                if(l_data->getHarmonicFreqSize() != 0 && l_data->getFundamentalFreq() != 0)
                {
                    for(l_i = 0; l_i < m_num_harmonics; l_i++)
                    {
                        int l_radius = toInt((l_data->getHarmonicAmpNoCutOffAt(l_i) + 160.0) / 160.0 * static_cast<float>(height()) / m_num_harmonics / 2);
                        if(l_radius > 0)
                        {
                            float l_flat_sharp = (l_data->getHarmonicFreqAt(l_i) / l_data->getFundamentalFreq() - (l_i + 1)) * 10;
                            QColor l_color;
                            if(l_flat_sharp > 0)
                            {
                                l_color = colorBetween(qRgb(255,255,255), qRgb(255,0,0),l_flat_sharp);
                            }
                            else
                            {
                                l_color = colorBetween(qRgb(255,255,255), qRgb(0,0,255),-l_flat_sharp);
                            }
                            get_painter().setBrush(colorBetween(GData::getUniqueInstance().backgroundColor(),l_color,((l_j == (m_history_chunks - 1)) ? 1.0 : static_cast<float>(l_j) / m_history_chunks * 0.5)));
                            get_painter().drawEllipse( toInt(width() / 8 * 3 + l_j * static_cast<float>(width()) / 8 / m_history_chunks - l_radius)
                                                     , toInt(height() - static_cast<float>((l_i + 1) * height()) / (m_num_harmonics + 2) - l_radius)
                                                     , l_radius * 2
                                                     , l_radius * 2
                                                     );
                        }
                    }
                }
            }
        }
    }
    endDrawing();

#ifdef TIME_PAINT
    std::cout << l_timer.elapsed() << " ms: HBubbleWidget::paintEvent()" << std::endl;
#endif // TIME_PAINT
}

//------------------------------------------------------------------------------
QSize HBubbleWidget::sizeHint() const
{
    return QSize(300, 200);
}

// EOF
