/***************************************************************************
                          myglfonts.h
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MYGLFONTS_H
#define MYGLFONTS_H

#include <QGLWidget>
#include <QString>
#include <glu.h>
#include <gl.h>

/** 
  Simple use of MyGLFont

  //in initialiseGL()
  MyGLFont myglf(QFont());
  //in paintGL()
  myglf.drawGLtext(10, 10, "Hello World");
  
  
  To draw lots of text faster, use

  //in initialiseGL()
  MyGLFont myglf(QFont());
  //in paintGL()
  myglf.beginGLtext(width(), height());
  for(int j=1; j<100; j++) {
    myglf.drawGLtextRaw(10, 10*j, "Hello World");
  }
  myglf.endGLtext();
*/

class MyGLChar
{
public:
  inline MyGLChar(int p_width, int p_height, GLubyte *p_data, int p_width_raw);
  inline const GLubyte * data(void) const;
  inline int w(void) const;
  inline int h(void) const;
  inline int w_raw(void) const;

 private:
  int m_width;
  int m_height;
  int m_width_raw;
  GLubyte *m_data;
};

class MyGLFont
{
 public:
  inline MyGLFont(void);
  inline MyGLFont(const QFont p_font);
  ~MyGLFont(void);

  void beginGLtext(int p_width, int p_height) const;
  void drawGLtextRaw(float p_x, float p_y, QString p_string) const;
  void endGLtext(void) const;

 private:
  void init(const QFont p_font);
  void drawGLtext(float p_x, float p_y, QString p_string, int p_width, int p_height) const;

 private:
  MyGLChar * m_chars[256];
  bool m_been_init;
};

#include "myglfonts.hpp"

#endif // MYGLFONTS_H
// EOF
