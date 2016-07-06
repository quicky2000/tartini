/***************************************************************************
                          piano3d.cpp  -  description
                             -------------------
    begin                : 31 Mar 2005
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

#include <algorithm>

#include "piano3d.h"
#include "useful.h"
#include "musicnotes.h"

double keyOffsetTable[12] =
  {
    0.0,
    15.5,
    23.5,
    43.5,
    47.0,
    70.5,
    85.5,
    94.0,
    112.5,
    117.5,
    139.0,
    141.0
  };

//------------------------------------------------------------------------------
Piano3d::Piano3d(int numKeys_, int firstKey_)
{
  init(numKeys_, firstKey_);
}

//------------------------------------------------------------------------------
Piano3d::~Piano3d(void)
{
}

//------------------------------------------------------------------------------
double Piano3d::pianoWidth(void)
{
  return (isBlackNote(_numKeys - 1) + _firstKey) ? keyOffsets[_numKeys - 1] + BLACK_KEY_WIDTH : keyOffsets[_numKeys - 1] + WHITE_KEY_WIDTH;
}

//------------------------------------------------------------------------------
double Piano3d::offsetAtKey(int keyNum)
{
  return (noteOctave(keyNum) + 1) * OCTAVE_WIDTH + keyOffsetTable[cycle(keyNum, 12)] - firstKeyOffset;
}

//------------------------------------------------------------------------------
void Piano3d::init(int numKeys_, int firstKey_)
{
  _numKeys = numKeys_;
  _firstKey = firstKey_;
  keyStates.resize(_numKeys, false);
  keyOffsets.resize(_numKeys);
  firstKeyOffset = (noteOctave(_firstKey) + 1) * OCTAVE_WIDTH + keyOffsetTable[cycle(_firstKey, 12)];
  int curKey;
  for(int j = 0; j < _numKeys; j++)
    {
      curKey = j + _firstKey;
      keyOffsets[j] = offsetAtKey(curKey);
    }
  
  //build display lists for a white and a black key (for fast rendering later)
  aWhiteKey = glGenLists(1);
  glNewList(aWhiteKey, GL_COMPILE);
  drawWhiteKey();
  glEndList();
  aBlackKey = glGenLists(1);
  glNewList(aBlackKey, GL_COMPILE);
  drawBlackKey();
  glEndList();
}

//------------------------------------------------------------------------------
void Piano3d::setAllKeyStatesOff(void)
{
  std::fill(keyStates.begin(), keyStates.end(), false);
}

//------------------------------------------------------------------------------
void Piano3d::drawWhiteKey(void)
{
  glBegin(GL_QUAD_STRIP);
  //top surface
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(WHITE_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  
  glVertex3f(WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
  glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH);
  
  //front surface
  //todo: fix normal
  glNormal3f(0.0f, -1.0f, 0.2f);
  glVertex3f(WHITE_KEY_WIDTH, -2.0f, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(0.0f, -2.0f, WHITE_KEY_LENGTH_INSIDE);
  
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glVertex3f(0.0f, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
  glEnd();
}

//------------------------------------------------------------------------------
void Piano3d::drawBlackKey(void)
{
  glBegin(GL_QUAD_STRIP);
  glNormal3f(-1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glNormal3f(1.0, 0.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, 0.0, 0.0);
  glVertex3f(BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
  
  glBegin(GL_QUADS);
  //todo: fix normal
  glNormal3f(0.0, 3.0, 1.0);
  glVertex3f(BLACK_KEY_WIDTH, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glVertex3f(BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(0.0, BLACK_KEY_HEIGHT, BLACK_KEY_LENGTH_TOP);
  glVertex3f(0.0, 0.0, BLACK_KEY_LENGTH_BOTTOM);
  glEnd();
}

//------------------------------------------------------------------------------
void Piano3d::setMaterialWhiteKey(void)
{
  GLfloat pianoKeyDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pianoKeyDiffuse);
}

//------------------------------------------------------------------------------
void Piano3d::setMaterialBlackKey(void)
{
  GLfloat pianoKeyDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pianoKeyDiffuse);
}

//------------------------------------------------------------------------------
void Piano3d::draw(void)
{
  int curKey;
  int j;

  glShadeModel(GL_FLAT);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glPolygonMode(GL_FRONT, GL_FILL);
  
  //draw white keys filled
  setMaterialSpecular( 0.2f, 0.2f, 0.2f, 50.0f);
  setMaterialColor(0.9f, 0.9f, 0.9f);
  glPushMatrix();
  for(j = 0; j < _numKeys; j++)
    {
      curKey = j + _firstKey;
      if(!isBlackNote(curKey))
	{
	  if(keyStates[j])
	    {
	      setMaterialColor(0.9f, 0.9f, 0.0f);
	      glPushMatrix();
	      glRotated(5.0, 1.0, 0.0, 0.0);
	      glCallList(aWhiteKey);
	      glPopMatrix();
	      setMaterialColor(0.9f, 0.9f, 0.9f);
	    }
	  else
	    {
	      glCallList(aWhiteKey);
	    }
	  glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
	}
    }
  glPopMatrix();
  
  //draw black keys filled
  setMaterialColor(0.15f, 0.15f, 0.15f);
  for(j = 0; j < _numKeys; j++)
    {
      curKey = j + _firstKey;
      if(isBlackNote(curKey))
	{
	  glPushMatrix();
	  glTranslatef(keyOffsets[j], 0.0, 0.0);
	  if(keyStates[j])
	    {
	      glRotated(5.0, 1.0, 0.0, 0.0);
	      setMaterialColor(0.5, 0.5, 0.0);
	      glCallList(aBlackKey);
	      setMaterialColor(0.15f, 0.15f, 0.15f);
	    }
	  else
	    {
	      glCallList(aBlackKey);
	    }
	  glPopMatrix();
	}
    }
  glDisable(GL_POLYGON_OFFSET_FILL);
  
  //draw white key outline
  setMaterialColor(0.0, 0.0, 0.0);
  glLineWidth(1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glPushMatrix();
  for(j = 0; j < _numKeys; j++)
    {
      curKey = j + _firstKey;
      if(!isBlackNote(curKey))
	{
	  if(keyStates[j])
	    {
	      glPushMatrix();
	      glRotated(5.0, 1.0, 0.0, 0.0);
	      glCallList(aWhiteKey);
	      glPopMatrix();
	    }
	  else
	    {
	      glCallList(aWhiteKey);
	    }
	  glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
	}
    }
  glPopMatrix();

  //draw black key outline
  setMaterialColor(0.35f, 0.35f, 0.35f);
  for(j = 0; j < _numKeys; j++)
    {
      curKey = j + _firstKey;
      if(isBlackNote(curKey))
	{
	  glPushMatrix();
	  glTranslatef(keyOffsets[j], 0.0, 0.0);
	  if(keyStates[j])
	    {
	      glRotated(5.0, 1.0, 0.0, 0.0);
	      glCallList(aBlackKey);
	    }
	  else
	    {
	      glCallList(aBlackKey);
	    }
	  glPopMatrix();
	}
    }
}

// EOF
