/***************************************************************************
                          viewdata.h  -  description
                             -------------------
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef VIEWDATA_H
#define VIEWDATA_H

#include <string>

class ViewData
{
  public:

    ViewData( std::string p_title
            , std::string p_menu_name
            , std::string p_class_name
            , int p_menu_type
            );

    ViewData();

    const std::string & get_title() const;
    const std::string & get_menu_name() const;
    const std::string & get_class_name() const;
    int get_menu_type() const;

  private:
    std::string m_title;
    std::string m_menu_name;
    std::string m_class_name;
    int m_menu_type;
};


#endif // VIEWDATA_H
// EOF