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
ViewData::ViewData( std::string p_title
                  , std::string p_menu_name
                  , std::string p_class_name
                  , int p_menu_type
                  )
: m_title(p_title)
, m_menu_name(p_menu_name)
, m_class_name(p_class_name)
, m_menu_type(p_menu_type)
{
};

//-----------------------------------------------------------------------------
const std::string &
ViewData::get_title() const
{
    return m_title;
}

//-----------------------------------------------------------------------------
const std::string &
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
const std::string &
ViewData::get_class_name() const
{
    return m_class_name;
}

//-----------------------------------------------------------------------------
ViewData::ViewData()
: m_menu_type(0)
{

}

//EOF