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
bool Piano3d::keyState(int p_key_num)
{
  return (p_key_num >= 0 && p_key_num < m_num_keys) ? m_key_states[p_key_num] : false;
}

//------------------------------------------------------------------------------
bool Piano3d::midiKeyState(int p_key_num)
{
  return keyState(p_key_num-m_first_key);
}

//------------------------------------------------------------------------------
void Piano3d::setKeyState(int p_key_num, bool p_state)
{
  if(p_key_num >= 0 && p_key_num < m_num_keys)
    {
      m_key_states[p_key_num] = p_state;
    }
}

//------------------------------------------------------------------------------
void Piano3d::setMidiKeyState(int p_key_num, bool p_state)
{
  setKeyState(p_key_num - m_first_key, p_state);
}

//------------------------------------------------------------------------------
int Piano3d::numKeys(void)
{
  return m_num_keys;
}

//------------------------------------------------------------------------------
int Piano3d::firstKey(void)
{
  return m_first_key;
}

//------------------------------------------------------------------------------
void setMaterialColor(float p_r, float p_g, float p_b)
{
  GLfloat l_the_color[4] = { p_r, p_g, p_b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, l_the_color);
}

//------------------------------------------------------------------------------
void setMaterialSpecular(float p_r, float p_g, float p_b, float p_shiney)
{
  GLfloat l_the_specular[4] = { p_r, p_g, p_b, 1.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, l_the_specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, p_shiney);
}

//------------------------------------------------------------------------------
void setLightPosition(float p_x, float p_y, float p_z)
{
  GLfloat l_light_position[4] = { p_x, p_y, p_z, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, l_light_position);
}

//------------------------------------------------------------------------------
void setLightDirection(float p_x, float p_y, float p_z)
{
  GLfloat l_light_position[4] = { p_x, p_y, p_z, 0.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, l_light_position);
}

//------------------------------------------------------------------------------
void setLightAmbient(float p_r, float p_g, float p_b)
{
  GLfloat l_the_color[4] = { p_r, p_g, p_b, 1.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, l_the_color);
}

//------------------------------------------------------------------------------
void setLightDiffuse(float p_r, float p_g, float p_b)
{
  GLfloat l_the_color[4] = { p_r, p_g, p_b, 1.0 };
  glLightfv(GL_LIGHT0, GL_DIFFUSE, l_the_color);
}

//------------------------------------------------------------------------------
void setLightSpecular(float p_r, float p_g, float p_b)
{
  GLfloat l_the_color[4] = { p_r, p_g, p_b, 1.0 };
  glLightfv(GL_LIGHT0, GL_SPECULAR, l_the_color);
}

// EOF
