/***************************************************************************
                          viewdata.cpp  -  description
                             -------------------
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "viewdata.h"

//-----------------------------------------------------------------------------
ViewData::ViewData( QString p_title
                  , QString p_menu_name
                  , QString p_class_name
                  , int p_menu_type
                  )
{
m_title = p_title;
m_menu_name = p_menu_name;
m_class_name = p_class_name;
m_menu_type = p_menu_type;
};

//-----------------------------------------------------------------------------
const QString &
ViewData::get_title() const
{
    return m_title;
}

//-----------------------------------------------------------------------------
const QString &
ViewData::get_menu_name() const
{
    return m_menu_name;
}

//-----------------------------------------------------------------------------
int
ViewData::get_menu_type() const
{
    return m_menu_type;
}

//-----------------------------------------------------------------------------
const QString &
ViewData::get_class_name() const
{
    return m_class_name;
}

//EOF