/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
bool Piano3d::keyState(int keyNum)
{
  return (keyNum >= 0 && keyNum < _numKeys) ? keyStates[keyNum] : false;
}

//------------------------------------------------------------------------------
bool Piano3d::midiKeyState(int keyNum)
{
  return keyState(keyNum-_firstKey);
}

//------------------------------------------------------------------------------
void Piano3d::setKeyState(int keyNum, bool state)
{
  if(keyNum >= 0 && keyNum < _numKeys)
    {
      keyStates[keyNum] = state;
    }
}

//------------------------------------------------------------------------------
void Piano3d::setMidiKeyState(int keyNum, bool state)
{
  setKeyState(keyNum - _firstKey, state);
}

//------------------------------------------------------------------------------
int Piano3d::numKeys(void)
{
  return _numKeys;
}

//------------------------------------------------------------------------------
int Piano3d::firstKey(void)
{
  return _firstKey;
}

//------------------------------------------------------------------------------
void setMaterialColor(float r, float g, float b)
{
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, theColor);
}

//------------------------------------------------------------------------------
void setMaterialSpecular(float r, float g, float b, float shiney)
{
  GLfloat theSpecular[4] = { r, g, b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, theSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiney);
}

//------------------------------------------------------------------------------
void setLightPosition(float x, float y, float z)
{
  GLfloat light_position[4] = { x, y, z, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//------------------------------------------------------------------------------
void setLightDirection(float x, float y, float z)
{
  GLfloat light_position[4] = { x, y, z, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//------------------------------------------------------------------------------
void setLightAmbient(float r, float g, float b)
{
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, theColor);
}

//------------------------------------------------------------------------------
void setLightDiffuse(float r, float g, float b)
{
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, theColor);
}

//------------------------------------------------------------------------------
void setLightSpecular(float r, float g, float b)
{
  GLfloat theColor[4] = { r, g, b, 1.0 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, theColor);
}

// EOF
