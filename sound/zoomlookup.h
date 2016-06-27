/***************************************************************************
                          zoomlookup.h  -  description
                             -------------------
    begin                : Wed Feb 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ZOOMLOOKUP_H
#define ZOOMLOOKUP_H

#include <qcolor.h>
//#include "array1d.h"
#include <vector>
#include "myassert.h"

class AnalysisData;

class ZoomElement
{
public:
  inline ZoomElement(void);
  inline float low(void) const;
  inline float high(void) const;
  inline float corr(void) const;
  inline const QColor & color(void) const;
  inline int noteIndex(void) const;
  inline int midChunk(void) const;
  inline bool isValid(void) const;
  inline void set(float low_,
		  float high_,
		  float corr_,
		  const QColor & color_,
		  int noteIndex_,
		  int midChunk_
		  );

private:
  float _low;
  float _high;
  float _corr;
  QColor _color;
  int _noteIndex;
  int _midChunk;
  bool _isValid;

};

class ZoomLookup
{
private:
  int _size;
  double _zoomLevel;
public:
  std::vector<ZoomElement> _table;

  ZoomLookup(void);
  virtual ~ZoomLookup(void);

  void clear(void);
  void setZoomLevel(double zoomLevel_);
  inline int size(void) const;
  void setSize(int size_);
  inline ZoomElement & at(int x);
};

#include "zoomlookup.hpp"

#endif // ZOOMLOOKUP_H
// EOF
