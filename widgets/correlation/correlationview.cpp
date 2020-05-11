/***************************************************************************
                          correlationview.cpp  -  description
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
#include "correlationview.h"
#include "correlationwidget.h"
#include "gdata.h"
#include "channel.h"
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

//------------------------------------------------------------------------------
CorrelationView::CorrelationView( int p_view_ID
                                , QWidget * p_parent
                                )
: ViewWidget( p_view_ID, p_parent)
{
    GData::getUniqueInstance().setDoingActiveAnalysis(true);

    Channel * l_active_channel = GData::getUniqueInstance().getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->lock();
        l_active_channel->processChunk(l_active_channel->currentChunk());
        l_active_channel->unlock();
    }

    m_correlation_widget = new CorrelationWidget(this);
    QStringList l_string_list;
    l_string_list << "Chunk correlation" << "Note Aggregate Correlation" << "Note Aggregate Correlation Scaled";
    QComboBox *l_aggregate_mode_combo_box = new QComboBox(this);
    l_aggregate_mode_combo_box->setObjectName("aggregateModeComboBox");
    l_aggregate_mode_combo_box->addItems(l_string_list);
    QHBoxLayout *l_h_layout = new QHBoxLayout();
    l_h_layout->setMargin(0);
    l_h_layout->addWidget(l_aggregate_mode_combo_box);
    l_h_layout->addStretch(1);
    connect(l_aggregate_mode_combo_box, SIGNAL(activated(int)), m_correlation_widget, SLOT(setAggregateMode(int)));

    QVBoxLayout *l_main_layout = new QVBoxLayout(this);
    l_main_layout->setMargin(0);
    l_main_layout->addWidget(m_correlation_widget);
    l_main_layout->addLayout(l_h_layout);

    //make the widget get updated when the view changes
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onSlowUpdate(double)), m_correlation_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
CorrelationView::~CorrelationView()
{
    GData::getUniqueInstance().setDoingActiveAnalysis(false);
    delete m_correlation_widget;
}

//------------------------------------------------------------------------------
QSize CorrelationView::sizeHint() const
{
    return QSize(400, 128);
}

// EOF
