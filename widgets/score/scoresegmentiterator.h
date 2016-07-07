/***************************************************************************
                          scoresegmentiterator.h  -  description
                             -------------------
    begin                : Mar 29 2006
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
#ifndef SCORESEGMENTITERATOR_H
#define SCORESEGMENTITERATOR_H

#include "channel.h"

class ScoreWidget;

class ScoreSegmentIterator
{
  ScoreWidget * sw;

  Channel * ch;
  int staveHeight;
  int halfStaveHeight;
  int numRows;
  double totalRowTime;
  double totalPageTime;
  double _curTime;

  int _curPage;
  double lookAheadGapTime;
  double lookAheadTime;
  double lookBehindTime;
  double startOfPageTime;
  double curRelPageTime;
  double endTime;
  int _numPages;
  double endOfLastPageTime;
  double lookAheadTime2;
  double lookBehindTime2;
  double lookBehindTime3;
  int curRow;
  int rowCounter;
  int subRowCounter;

  double _leftTime;
  double _rightTime;
  double _leftX;
  int _lineCenterY;
  bool _isValid;

 public :
  ScoreSegmentIterator(ScoreWidget *scoreWidget, Channel *channel)
    {
      reset(scoreWidget, channel);
    }
  /**
     Initalise the iterator to just before the first segment
     @param scoreWidget The ScoreWidget to iterate through it's segments
  */
  void reset(ScoreWidget * scoreWidget, Channel * channel);

  /**
     move to the next score segment
     @return true, if their was another score segment
  */
  bool next(void);
  bool isValid(void)
  {
    return _isValid;
  }

  double leftTime(void)
  {
    return _leftTime;
  }
  double rightTime(void)
  {
    return _rightTime;
  }
  double leftX(void)
  {
    return _leftX;
  }
  int lineCenterY(void)
  {
    return _lineCenterY;
  }
  int staveTop(void)
  {
    return _lineCenterY - halfStaveHeight;
  }
  int staveBottom(void)
  {
    return staveTop() + staveHeight;
  }
  double widthX(void);
  double curTime(void)
  {
    return _curTime;
  }
  int curPage(void)
  {
    return _curPage;
  }
  int numPages(void)
  {
    return _numPages;
  }
  bool contains(double t)
  {
    return (t >= _leftTime && t <= _rightTime);
  }
};
#endif // SCORESEGMENTITERATOR_H
// EOF
