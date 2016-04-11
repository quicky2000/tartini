/***************************************************************************
                          mylabel.cpp  -  description
                             -------------------
    begin                : 29/6/2005
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

#include "mylabel.h"
#include <QPixmap>
#include <QPaintEvent>

//------------------------------------------------------------------------------
MyLabel::MyLabel(const QString & p_text
                ,QWidget * p_parent
                ,const char * p_name
                )
: DrawWidget(p_parent, p_name)
, m_text(p_text)
{
    QFontMetrics l_font_metric = QFontMetrics(get_painter().font());
    m_font_height = l_font_metric.height();
    m_text_width = l_font_metric.width(m_text);
}

//------------------------------------------------------------------------------
void MyLabel::paintEvent( QPaintEvent * )
{
    beginDrawing(false);
    fillBackground(colorGroup().background());
    get_painter().drawText(4, m_font_height - 2, m_text);
    endDrawing();
}

// EOF
