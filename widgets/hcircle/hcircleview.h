/***************************************************************************
                          hstackview.h  -  description
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
#ifndef HCIRCLEVIEW_H
#define HCIRCLEVIEW_H

#include "viewwidget.h"

class HCircleWidget;

class HCircleView: public ViewWidget
{
  Q_OBJECT

  public:
    HCircleView( int p_view_id
               , QWidget *p_parent = NULL
               );
    virtual ~HCircleView(void);

    QSize sizeHint(void) const;

  private:
    void resizeEvent(QResizeEvent *);

    HCircleWidget *m_h_circle_widget;
};
#endif // HCIRCLEVIEW_H
// EOF

