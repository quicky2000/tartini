/***************************************************************************
                          zoomlookup.h  -  description
                             -------------------
    begin                : Wed Feb 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ZOOMLOOKUP_H
#define ZOOMLOOKUP_H

#include <QColor>
//#include "array1d.h"
#include <vector>
#include "myassert.h"

class AnalysisData;

class ZoomElement
{
  public:
    inline ZoomElement();
    inline float low() const;
    inline float high() const;
    inline float corr() const;
    inline const QColor & color() const;
    inline int noteIndex() const;
    inline int midChunk() const;
    inline bool isValid() const;
    inline void set( float p_low
                   , float p_high
                   , float p_corr
                   , const QColor & p_color
                   , int p_note_index
                   , int p_mid_chunk
                   );

  private:
    float m_low;
    float m_high;
    float m_corr;
    QColor m_color;
    int m_note_index;
    int m_mid_chunk;
    bool m_is_valid;
};

class ZoomLookup
{
  public:

    ZoomLookup();
    virtual ~ZoomLookup();

    void clear();
    void setZoomLevel(double p_zoom_level);
    inline int size() const;
    void setSize(int p_size);
    inline ZoomElement & at(int p_x);

  private:
    std::vector<ZoomElement> m_table;
    int m_size;
    double m_zoom_level;
};

#include "zoomlookup.hpp"

#endif // ZOOMLOOKUP_H
// EOF
