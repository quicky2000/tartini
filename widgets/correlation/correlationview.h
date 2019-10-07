/***************************************************************************
                          correlationview.h  -  description
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
#ifndef CORRELATIONVIEW_H
#define CORRELATIONVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class CorrelationWidget;

class CorrelationView : public ViewWidget
{
  Q_OBJECT

  public:
    CorrelationView( int p_view_ID
                   , QWidget * p_parent = 0
                   );
    virtual ~CorrelationView(void);

    QSize sizeHint(void) const;

  private:
    CorrelationWidget * m_correlation_widget;
};
#endif // CORRELATIONVIEW_H
// EOF

