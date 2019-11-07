/***************************************************************************
                          scoreview.h  -  description
                             -------------------
    begin                : Mar 26 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SCOREVIEW_H
#define SCOREVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class ScoreWidget;

class ScoreView: public ViewWidget
{
  Q_OBJECT

  public:
  ScoreView(int p_view_id
           ,QWidget *p_parent = NULL
           );
  virtual ~ScoreView();

  void resizeEvent(QResizeEvent *);

  QSize sizeHint() const;

  private:
  ScoreWidget * m_score_widget;

};
#endif // SCOREVIEW_H
// EOF
