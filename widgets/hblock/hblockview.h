/***************************************************************************
                          hblockview.h  -  description
                             -------------------
    begin                : Mon Jan 10 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
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
  HBlockView(int viewID_, QWidget *parent = NULL);
  virtual ~HBlockView(void);

  void resizeEvent(QResizeEvent *);

  QSize sizeHint(void) const;

 private:
  HBlockWidget *hBlockWidget;
};
#endif // HBLOCKVIEW_H
// EOF
