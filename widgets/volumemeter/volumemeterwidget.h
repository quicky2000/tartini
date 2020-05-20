/***************************************************************************
                          volumemeterwidget.h  -  description
                             -------------------
    begin                : Tue Dec 21 2004
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
#ifndef VOLUMEMETERWIDGET_H
#define VOLUMEMETERWIDGET_H

#include "drawwidget.h"
#include <vector>
#include <QPixmap>
#include <QPaintEvent>

// Forward declarations of classes the h file doesn't need to know specifics about
class QPixmap;
class QFont;


class VolumeMeterWidget: public DrawWidget
{
  Q_OBJECT

  public:
    VolumeMeterWidget(QWidget * p_parent);

    ~VolumeMeterWidget() override;

    void paintEvent(QPaintEvent *) override;

    QSize sizeHint() const override;
    void setFontSize(int fontSize);

  private:

    QFont m_font;
    int m_font_size;

    std::vector<int> m_label_numbers;
};
#endif // VOLUMEMETERWIDGET_H
// EOF
