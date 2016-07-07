/***************************************************************************
                          scorewidget.h  -  description
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
#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include "drawwidget.h"
#include "qfont.h"
#include <QMouseEvent>
#include <QPaintEvent>


class ScoreWidget : public DrawWidget
{
  Q_OBJECT

  enum StemType { StemNone, StemUp, StemDown };
  enum FillType { FilledNote, EmptyNote };
  enum NoteType { DemiSemiQuaver, SemiQuaver, Quaver, Crotchet, Minum, SemiBreve };

 public:
  ScoreWidget(QWidget * parent);
  virtual ~ScoreWidget(void);

  /**
   * Draw a crotchet at x, y
   * @param x The lefthand-side x pixel
   * @param y The center y pixel
   * @param stemUp true if the stem is to be drawn upwards, else its drawn downwards
   */
  void drawNote(int x, int y, StemType stepType, FillType fillType, int numFlicks);

  /**
     Draw a single note
     @param x The x pos in pixels
     @param y The y pos of the center of the scale (Middle C)
     @param pitch The midi number of the note (60 == Middle C)
     @param noteLength in seconds
  */
  void drawNoteAtPitch(int x, int y, int pitch, double noteLength, double volume=1.0);

  /** Draws a segment of the musical score
   * @param ch The channel to get the notes from.
   * @param leftX In pixels (even though a doule)
   * @param lineCenterY In pixels
   * @param leftTime The time is seconds at which is at the lefthand side of the scoreSegment to draw
   */
  void drawScoreSegment(Channel * ch, double leftX, int lineCenterY, double leftTime, double rightTime);

  /** Draw a segment of stave lines
   * @param leftX The pixel at the left of the segment
   * @param lineCenterY The y pixel to put middle C on
   * @param widthX The width of the stave segment
   */
  void drawStaveSegment(int leftX, int lineCenterY, int widthX);
  void paintEvent(QPaintEvent * );
  int getStaveHeight(void)
  {
    return toInt(_scaleY * (_showBaseClef ? 16 : 8));
  }
  int getStaveCenterY(void)
  {
    return toInt(_scaleY * (_showBaseClef ? 8 : 7));
  }
  QSize sizeHint(void) const
  {
    return QSize(300, 200);
  }
  NoteType getNoteType(double noteLength);

  double scaleX(void)
  {
    return _scaleX;
  }
  double scaleY(void)
  {
    return _scaleY;
  }

  public slots:
  void setScaleX(double x)
  {
    _scaleX = x;
  }
  void setScaleY(double y)
  {
    _scaleY = y;
  }
  void setSharpsMode(int mode)
  {
    _useFlats = (mode!=0);
  }
  void setNotesMode(int mode)
  {
    _showNotes = (mode==0);
  }
  void setClefMode(int mode)
  {
    _showBaseClef = (mode==0);
  }
  void setOpaqueMode(int mode)
  {
    _showOpaqueNotes = (mode!=0);
  }
  void setTransposeLevel(int index)
  {
    _pitchOffset = (index - 2) * -12;
  }
  void setShowAllMode(int mode)
  {
    _showAllMode = (mode!=0);
  }

 private:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  //data goes here
  //num. pixels between lines
  double _scaleY;
  //num. pixels per second
  double _scaleX;
  double _boarderX;
  double _boarderY;
  bool _showBaseClef;
  bool _showNotes;
  bool _showOpaqueNotes;
  bool _useFlats;
  bool _showAllMode;
  int _pitchOffset;
  int sharpsLookup[129];
  int flatsLookup[129];
  int _fontHeight;
  int _fontWidth;
  QFont _font;
  //in fractions of a page
  double _lookAhead;
  //in fractions of a page
  double _lookAheadGap;
  bool _mouseDown;
  friend class ScoreSegmentIterator;
};

#endif // SCOREWIDGET_H
// EOF
