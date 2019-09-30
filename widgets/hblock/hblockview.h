/***************************************************************************
                          hblockview.h  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef HBLOCKVIEW_H
#define HBLOCKVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class HBlockWidget;

class HBlockView : public ViewWidget
{
  Q_OBJECT

  public:
    HBlockView( int p_view_id
              , QWidget *p_parent = NULL
              );
    virtual ~HBlockView(void);

    void resizeEvent(QResizeEvent *);

    QSize sizeHint(void) const;

  private:
    HBlockWidget * m_h_block_widget;
};
#endif // HBLOCKVIEW_H
// EOF
