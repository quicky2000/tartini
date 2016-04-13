/***************************************************************************
                          viewwidget.h  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>

class ViewWidget : public QWidget
{
  Q_OBJECT
 public:
  ViewWidget(int p_view_ID, QWidget * p_parent = 0, Qt::WFlags p_flags = 0);

 private:
  int m_view_ID;
};

#endif // VIEWWIDGET_H
// EOF
