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
  inline MyGLChar(int width, int height, GLubyte *theData, int width_raw);
  inline const GLubyte * data(void) const;
  inline int w(void) const;
  inline int h(void) const;
  inline int w_raw(void) const;

 private:
  int _w;
  int _h;
  int _w_raw;
  GLubyte *_data;
};

class MyGLFont
{
 public:
  inline MyGLFont(void);
  inline MyGLFont(const QFont f);
  ~MyGLFont(void);

  void beginGLtext(int w, int h) const;
  void drawGLtextRaw(float x, float y, QString s) const;
  void endGLtext(void) const;

 private:
  void init(const QFont f);
  void drawGLtext(float x, float y, QString s, int w, int h) const;

 private:
  MyGLChar * c[256];
  bool beenInit;
};

#include "myglfonts.hpp"

#endif // MYGLFONTS_H
// EOF
