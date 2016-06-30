/***************************************************************************
                          myscrollbar.cpp  -  description
                             -------------------
    begin                : Tue Jul 20 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <qscrollbar.h>
#include <QResizeEvent>
#include "myscrollbar.h"
#include <qlayout.h>

//------------------------------------------------------------------------------
MyScrollBar::MyScrollBar(Qt::Orientation orientation, QWidget *parent, const char *name):
  QWidget(parent, name)
{
  bar = new QScrollBar(orientation, this);
  bar->show();
}

//------------------------------------------------------------------------------
MyScrollBar::MyScrollBar(double minValue_,
			 double maxValue_,
			 double lineStep_,
			 double pageStep_,
			 double value_,
			 double step_,
			 Qt::Orientation orientation,
			 QWidget * parent,
			 const char * name):
  QWidget(parent, name)
{
  _minValue = minValue_;
  _maxValue = maxValue_;
  _lineStep = lineStep_;
  _pageStep = pageStep_;
  _value = value_;
  _step = step_;
  
  bar = new QScrollBar(orientation, this);
  bar->setMinimum(toInt(_minValue*_step));
  bar->setMaximum(toInt(_maxValue*_step));
  bar->setSingleStep(MAX(toInt(_lineStep*_step), 1));
  bar->setPageStep(MAX(toInt(_pageStep*_step), 1));
  bar->setValue(toInt(_value*_step));
  setSizePolicy(bar->sizePolicy());
  connect(bar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggering(int)));
}

//------------------------------------------------------------------------------
MyScrollBar::~MyScrollBar(void)
{
  delete bar;
}

//------------------------------------------------------------------------------
QSize MyScrollBar::sizeHint(void) const
{
  return bar->sizeHint();
}

//------------------------------------------------------------------------------
void MyScrollBar::resizeEvent(QResizeEvent * q)
{
  bar->resize(q->size());
}

//------------------------------------------------------------------------------
void MyScrollBar::setMinValue(double minValue_)
{
  _minValue = minValue_;
  bar->setMinimum(toInt(_minValue * _step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setMaxValue(double maxValue_)
{
  _maxValue = maxValue_;
  bar->setMaximum(toInt(_maxValue * _step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setRange(double minValue_, double maxValue_)
{
  setMinValue(minValue_);
  setMaxValue(maxValue_);
}

//------------------------------------------------------------------------------
void MyScrollBar::setLineStep(double lineStep_)
{
  _lineStep = lineStep_;
  bar->setSingleStep(toInt(_lineStep * _step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setPageStep(double pageStep_)
{
  _pageStep = pageStep_;
  bar->setPageStep(toInt(_pageStep * _step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setValue(double value_)
{
  _value = value_;
  bar->setValue(toInt(_value * _step));
}

//------------------------------------------------------------------------------
void MyScrollBar::setIntValue(int value_)
{
  _value = double(value_) / _step;
  emit valueChanged(_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::setStep(double step_)
{
  _step = step_;
}

//------------------------------------------------------------------------------
void MyScrollBar::sliderMoving(int value_)
{
  _value = double(value_) / _step;
  emit sliderMoved(_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::sliderMoving()
{
  _value = double(bar->value()) / _step;
  emit sliderMoved(_value);
}

//------------------------------------------------------------------------------
void MyScrollBar::actionTriggering(int /*action*/)
{
  setIntValue(bar->sliderPosition());
  emit sliderMoved(_value);
}

// EOF
