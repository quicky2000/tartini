/***************************************************************************
                          cepstrumview.h  -  description
                             -------------------
    begin                : May 21 2005
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
#ifndef CEPSTRUMVIEW_H
#define CEPSTRUMVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class CepstrumWidget;

class CepstrumView : public ViewWidget
{
  Q_OBJECT

  public:
    CepstrumView(int p_view_id, QWidget *p_parent = 0);
    virtual ~CepstrumView(void);

    void resizeEvent(QResizeEvent *);

    QSize sizeHint(void) const;

  private:
    CepstrumWidget *m_cepstrum_widget;

};
#endif // CEPSTRUMVIEW_H
// EOF
