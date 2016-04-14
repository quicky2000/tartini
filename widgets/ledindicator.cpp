/***************************************************************************
                          ledindicator.cpp  -  description
                             -------------------
    begin                : Tue Jan 11 2005
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

#include "ledindicator.h"

#include <QPixmap>
#include <QPaintEvent>

//------------------------------------------------------------------------------
LEDIndicator::LEDIndicator(QPixmap *p_buffer
                          ,QWidget *p_parent
                          ,const char *p_name
                          ,const QColor & p_on
                          ,const QColor &p_off
                          )
: QWidget(p_parent)
{
    setObjectName(p_name);
    setMinimumSize(sizeHint());
    m_on = p_on;
    m_off = p_off;

    m_active = false;

    // Stop QT from erasing the background all the time
    setAttribute(Qt::WA_OpaquePaintEvent);
}

//------------------------------------------------------------------------------
LEDIndicator::~LEDIndicator(void)
{
}

//------------------------------------------------------------------------------
void LEDIndicator::setOn(bool p_on)
{
    if(m_active != p_on)
    {
        m_active = p_on;
        update();
    }
}

//------------------------------------------------------------------------------
void LEDIndicator::toggle(void)
{
    m_active = !m_active;
}

//------------------------------------------------------------------------------
bool LEDIndicator::lit(void)
{
    return m_active;
}

//------------------------------------------------------------------------------
void LEDIndicator::paintEvent(QPaintEvent *)
{
    QPainter l_painter;
    l_painter.begin(this);

    l_painter.fillRect(0, 0, QWidget::width(), QWidget::height(), (m_active) ? m_on : m_off);

    l_painter.setPen(QPalette::BrightText);

    QFontMetrics l_font_metric = l_painter.fontMetrics();
    int l_font_Height = l_font_metric.height() / 4;
    int l_font_width = l_font_metric.width(objectName()) / 2;

    l_painter.drawText(QWidget::width() / 2 - l_font_width, QWidget::height() / 2 + l_font_Height, objectName());
    l_painter.end();
}

//------------------------------------------------------------------------------
QSize LEDIndicator::sizeHint(void) const
{
    return QSize(15, 25);
}
// EOF
