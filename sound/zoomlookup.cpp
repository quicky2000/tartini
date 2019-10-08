/***************************************************************************
                          zoomlookup.cpp  -  description
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

#include "zoomlookup.h"

//------------------------------------------------------------------------------
ZoomLookup::ZoomLookup(void):
  m_size(0)
{
}

//------------------------------------------------------------------------------
ZoomLookup::~ZoomLookup(void)
{
}

//------------------------------------------------------------------------------
void ZoomLookup::clear(void)
{
  m_table.clear();
  m_size = 0;
}

//------------------------------------------------------------------------------
void ZoomLookup::setZoomLevel(double p_zoom_level)
{
  if(m_zoom_level == p_zoom_level)
    {
      return;
    }
  m_zoom_level = p_zoom_level;
  clear();
}

//------------------------------------------------------------------------------
void ZoomLookup::setSize(int p_size)
{
  m_size = p_size;
  m_table.resize(p_size);
}
// EOF
