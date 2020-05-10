/***************************************************************************
                          mylabel.h  -  description
                             -------------------
    begin                : 29/6/2005
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

#ifndef MYLABEL_H
#define MYLABEL_H

#include "drawwidget.h"
#include <QString>
#include <QPaintEvent>
#include <QtGlobal>

/**
   MyLabel uses double buffering to draw simple text.
   So is faster (espicially on the Mac) than using QLabel.
   It only supports 1 line of text.
*/
class MyLabel : public DrawWidget
{
  public:
    MyLabel(const std::string & p_text
           ,QWidget * p_parent
           ,const char * p_name = nullptr
           );

    inline
    const std::string & text() const;

    inline
    void setText(const std::string & p_text);

    void paintEvent( QPaintEvent * );

    inline
    QSize sizeHint() const;

  private:

    inline
    void compute_text_width();

    std::string m_text;
    int m_font_height;
    int m_text_width;
    bool m_size_computed;
};

#include "mylabel.hpp"

#endif // MYLABEL_H
// EOF
