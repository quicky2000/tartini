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

static double g_keyOffsetTable[12] =
        { 0.0
        , 15.5
        , 23.5
        , 43.5
        , 47.0
        , 70.5
        , 85.5
        , 94.0
        , 112.5
        , 117.5
        , 139.0
        , 141.0
        };

//------------------------------------------------------------------------------
Piano3d::Piano3d( int p_num_keys
                , int p_first_key
                )
{
  init(p_num_keys, p_first_key);
}

//------------------------------------------------------------------------------
Piano3d::~Piano3d()
{
}

//------------------------------------------------------------------------------
double Piano3d::pianoWidth()
{
    return (isBlackNote(m_num_keys - 1) + m_first_key) ? m_key_offsets[m_num_keys - 1] + BLACK_KEY_WIDTH : m_key_offsets[m_num_keys - 1] + WHITE_KEY_WIDTH;
}

//------------------------------------------------------------------------------
double Piano3d::offsetAtKey(int p_key_num)
{
    return (noteOctave(p_key_num) + 1) * OCTAVE_WIDTH + g_keyOffsetTable[cycle(p_key_num, 12)] - m_first_key_offset;
}

//------------------------------------------------------------------------------
void Piano3d::init( int p_num_keys
                  , int p_first_key
                  )
{
    m_num_keys = p_num_keys;
    m_first_key = p_first_key;
    m_key_states.resize(m_num_keys, false);
    m_key_offsets.resize(m_num_keys);
    m_first_key_offset = (noteOctave(m_first_key) + 1) * OCTAVE_WIDTH + g_keyOffsetTable[cycle(m_first_key, 12)];
    int l_cur_key;
    for(int l_j = 0; l_j < m_num_keys; l_j++)
    {
        l_cur_key = l_j + m_first_key;
        m_key_offsets[l_j] = offsetAtKey(l_cur_key);
    }

    //build display lists for a white and a black key (for fast rendering later)
    m_a_white_key = glGenLists(1);
    glNewList(m_a_white_key, GL_COMPILE);
    drawWhiteKey();
    glEndList();
    m_a_black_key = glGenLists(1);
    glNewList(m_a_black_key, GL_COMPILE);
    drawBlackKey();
    glEndList();
}

//------------------------------------------------------------------------------
void Piano3d::setAllKeyStatesOff()
{
    std::fill(m_key_states.begin(), m_key_states.end(), false);
}

//------------------------------------------------------------------------------
void Piano3d::drawWhiteKey()
{
    glBegin(GL_QUAD_STRIP);
    //top surface
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(WHITE_KEY_WIDTH, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);

    glVertex3f(WHITE_KEY_WIDTH, 0.0, WHITE_KEY_LENGTH);
    glVertex3f(0.0, 0.0, WHITE_KEY_LENGTH);

    //front surface
    // todo: fix normal
    glNormal3f(0.0f, -1.0f, 0.2f);
    glVertex3f(WHITE_KEY_WIDTH, -2.0f, WHITE_KEY_LENGTH_INSIDE);
    glVertex3f(0.0f, -2.0f, WHITE_KEY_LENGTH_INSIDE);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(WHITE_KEY_WIDTH, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
    glVertex3f(0.0f, -WHITE_KEY_HEIGHT, WHITE_KEY_LENGTH_INSIDE);
    glEnd();
}

//------------------------------------------------------------------------------
void Piano3d::drawBlackKey()
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
void Piano3d::setMaterialWhiteKey()
{
    GLfloat l_piano_key_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, l_piano_key_diffuse);
}

//------------------------------------------------------------------------------
void Piano3d::setMaterialBlackKey()
{
    GLfloat l_piano_key_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, l_piano_key_diffuse);
}

//------------------------------------------------------------------------------
void Piano3d::draw()
{
    int l_cur_key;
    int l_j;

    glShadeModel(GL_FLAT);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glPolygonMode(GL_FRONT, GL_FILL);

    //draw white keys filled
    setMaterialSpecular( 0.2f, 0.2f, 0.2f, 50.0f);
    setMaterialColor(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    for(l_j = 0; l_j < m_num_keys; l_j++)
    {
        l_cur_key = l_j + m_first_key;
        if(!isBlackNote(l_cur_key))
        {
            if(m_key_states[l_j])
            {
                setMaterialColor(0.9f, 0.9f, 0.0f);
                glPushMatrix();
                glRotated(5.0, 1.0, 0.0, 0.0);
                glCallList(m_a_white_key);
                glPopMatrix();
                setMaterialColor(0.9f, 0.9f, 0.9f);
            }
            else
            {
                glCallList(m_a_white_key);
            }
            glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
        }
    }
    glPopMatrix();
  
    //draw black keys filled
    setMaterialColor(0.15f, 0.15f, 0.15f);
    for(l_j = 0; l_j < m_num_keys; l_j++)
    {
        l_cur_key = l_j + m_first_key;
        if(isBlackNote(l_cur_key))
        {
            glPushMatrix();
            glTranslatef(m_key_offsets[l_j], 0.0, 0.0);
            if(m_key_states[l_j])
            {
                glRotated(5.0, 1.0, 0.0, 0.0);
                setMaterialColor(0.5, 0.5, 0.0);
                glCallList(m_a_black_key);
                setMaterialColor(0.15f, 0.15f, 0.15f);
            }
            else
            {
                glCallList(m_a_black_key);
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
    for(l_j = 0; l_j < m_num_keys; l_j++)
    {
        l_cur_key = l_j + m_first_key;
        if(!isBlackNote(l_cur_key))
        {
            if(m_key_states[l_j])
            {
                glPushMatrix();
                glRotated(5.0, 1.0, 0.0, 0.0);
                glCallList(m_a_white_key);
                glPopMatrix();
            }
            else
            {
                glCallList(m_a_white_key);
            }
            glTranslatef(WHITE_KEY_WIDTH, 0.0, 0.0);
        }
    }
    glPopMatrix();

    //draw black key outline
    setMaterialColor(0.35f, 0.35f, 0.35f);
    for(l_j = 0; l_j < m_num_keys; l_j++)
    {
        l_cur_key = l_j + m_first_key;
        if(isBlackNote(l_cur_key))
        {
            glPushMatrix();
            glTranslatef(m_key_offsets[l_j], 0.0, 0.0);
            if(m_key_states[l_j])
            {
                glRotated(5.0, 1.0, 0.0, 0.0);
                glCallList(m_a_black_key);
            }
            else
            {
                glCallList(m_a_black_key);
            }
            glPopMatrix();
        }
    }
}

// EOF
