/***************************************************************************
                          widget_utils.h
                             -------------------
    begin                : 2020
    copyright            : (C) 2020 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef WIDGET_UTILS_H
#define WIDGET_UTILS_H

class widget_utils
{
  public:
    typedef enum class draw_mode { DRAW_VIEW_NORMAL, DRAW_VIEW_SUMMARY, DRAW_VIEW_PRINT} t_draw_mode;

    inline static
    unsigned int get_wheel_delta();

  private:

    static const unsigned int m_wheel_delta = 120;
};

//-----------------------------------------------------------------------------
unsigned int widget_utils::get_wheel_delta()
{
    return m_wheel_delta;
}
#endif //WIDGET_UTILS_H
// EOF