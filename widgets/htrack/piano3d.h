/***************************************************************************
                          piano3d.h  -  description
                             -------------------
    begin                : 31 March 2005
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

#ifndef PIANO3D_H
#define PIANO3D_H

#include <vector>
#include <stdio.h>
#include <qgl.h>
#include "myassert.h"

#define WHITE_KEY_WIDTH            23.5f
#define WHITE_KEY_LENGTH           148.0f
#define WHITE_KEY_LENGTH_INSIDE    142.0f
#define WHITE_KEY_HEIGHT           16.0f
#define BLACK_KEY_WIDTH            11.0f
#define BLACK_KEY_LENGTH_TOP       94.0f
#define BLACK_KEY_LENGTH_BOTTOM    100.0f
#define BLACK_KEY_HEIGHT           10.0f
#define OCTAVE_WIDTH               164.5f

class Piano3d
{
public:
  Piano3d(int numKeys_ = 85, int firstKey_ = 21);
  ~Piano3d(void);
  
  static void drawWhiteKey(void);
  static void drawBlackKey(void);
  
  void draw(void);
  
  /** Initialises the piano.
      This can be called again to reinitialise the piano to a different size
      @param numKeys_ The number of keys the piano is to have
      @param firstKey_ The midi note number that will be the lowest key on the piano
  */
  void init(int numKeys_ = 85, int firstKey_ = 21);
  
  void setMaterialWhiteKey(void);
  void setMaterialBlackKey(void);

  
  /**
     @return The distance the in mm away from where the 0 midi note would be
  */
  double offsetAtKey(int keyNum);
  inline bool keyState(int keyNum);
  inline bool midiKeyState(int keyNum);
  inline void setKeyState(int keyNum, bool state);
  inline void setMidiKeyState(int keyNum, bool state);
  void setAllKeyStatesOff(void);

  /**
     @return The width of the current piano keyboard in mm
  */
  double pianoWidth(void);
  inline int numKeys(void);
  inline int firstKey(void);
  double firstKeyOffset;
  
private:
  std::vector<bool> keyStates;
  std::vector<float> keyOffsets;
  int _numKeys;
  /**
     on the midi scale (ie middle C = 60)
  */
  int _firstKey;

  /**
     aWhiteKeyLine;
  */
  GLint aWhiteKey;
  GLint aBlackKey;
};

inline void setMaterialColor(float r, float g, float b);
inline void setMaterialSpecular(float r, float g, float b, float shiney);
inline void setLightPosition(float x, float y, float z);
inline void setLightDirection(float x, float y, float z);
inline void setLightAmbient(float r, float g, float b);
inline void setLightDiffuse(float r, float g, float b);
inline void setLightSpecular(float r, float g, float b);

#include "piano3d.hpp"

#endif // PIANO3D_H
// EOF
