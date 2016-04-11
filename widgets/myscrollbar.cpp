/***************************************************************************
                          myscrollbar.cpp  -  description
                             -------------------
    begin                : Tue Jul 20 2004
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

#include <QScrollBar>
#include <QResizeEvent>
#include "myscrollbar.h"
#include <QLayout>

//------------------------------------------------------------------------------
MyScrollBar::MyScrollBar(Qt::Orientation p_orientation
                        ,QWidget * p_parent
                        ,const char * p_name
                        )
: QWidget(p_parent, p_name)
, m_bar(new QScrollBar(p_orientation, this))
{
    m_bar->show();
}

//------------------------------------------------------------------------------
MyScrollBar::MyScrollBar(double p_min_value
                        ,double p_max_value
                        ,double p_line_step
                        ,double p_page_step
                        ,double p_value
                        ,double p_step
                        ,Qt::Orientation p_orientation
                        ,QWidget * p_parent
                        ,const char * p_name
                        )
: QWidget(p_parent, p_name)
, m_min_value(p_min_value)
, m_max_value(p_max_value)
, m_value(p_value)
, m_line_step(p_line_step),
  m_page_step(p_page_step),
  m_step(p_step),
  m_bar(new QScrollBar(p_orientation, this))
{
    m_bar->setMinimum(toInt(m_min_value * m_step));
    m_bar->setMaximum(toInt(m_max_value * m_step));
    m_bar->setSingleStep(MAX(toInt(m_line_step * m_step), 1));
    m_bar->setPageStep(MAX(toInt(m_page_step * m_step), 1));
    m_bar->setValue(toInt(m_value * m_step));
    setSizePolicy(m_bar->sizePolicy());
    connect(m_bar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggering(int)));
}

//------------------------------------------------------------------------------
MyScrollBar::~MyScrollBar(void)
{
    delete m_bar;
}

//------------------------------------------------------------------------------
QSize MyScrollBar::sizeHint(void) const
{
    return m_bar->sizeHint();
}

//------------------------------------------------------------------------------
void MyScrollBar::resizeEvent(QResizeEvent * q)
{
    m_bar->resize(q->size());
}

//------------------------------------------------------------------------------
void MyScrollBar::setMinValue(double p_min_value)
{
    m_min_value = p_min_value;
    m_bar->setMinimum(toInt(m_min_value * m_step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setMaxValue(double p_max_value_)
{
    m_max_value = p_max_value_;
    m_bar->setMaximum(toInt(m_max_value * m_step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setRange(double p_min_value, double p_max_value)
{
    setMinValue(p_min_value);
    setMaxValue(p_max_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::setLineStep(double p_line_step)
{
    m_line_step = p_line_step;
    m_bar->setSingleStep(toInt(m_line_step * m_step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setPageStep(double p_page_step)
{
    m_page_step = p_page_step;
    m_bar->setPageStep(toInt(m_page_step * m_step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setValue(double p_value)
{
    m_value = p_value;
    m_bar->setValue(toInt(m_value * m_step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setIntValue(int p_value)
{
    m_value = double(p_value) / m_step;
    emit valueChanged(m_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::setStep(double p_step)
{
    m_step = p_step;
}

//------------------------------------------------------------------------------
void MyScrollBar::sliderMoving(int p_value)
{
    m_value = double(p_value) / m_step;
    emit sliderMoved(m_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::sliderMoving()
{
    m_value = double(m_bar->value()) / m_step;
    emit sliderMoved(m_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::actionTriggering(int /*action*/)
{
    setIntValue(m_bar->sliderPosition());
    emit sliderMoved(m_value);
}

// EOF
