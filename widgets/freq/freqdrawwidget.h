/***************************************************************************
                          freqdrawwidget.h  -  description
                             -------------------
    begin                : Fri Dec 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef FREQDRAWWIDGET_H
#define FREQDRAWWIDGET_H

#include "drawwidget.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QPaintEvent>
#include <QPaintDevice>

class FreqDrawWidget : public DrawWidget
{

Q_OBJECT

  public:
    enum DragModes
    { DragNone = 0
    , DragChannel = 1
    , DragBackground = 2
    , DragTimeBar = 3
    };

    FreqDrawWidget( QWidget * p_parent
                  , const char* p_name = 0
                  );
    virtual ~FreqDrawWidget(void);

    static
    void drawReferenceLines( QPaintDevice & p_paint_device
                           , QPainter & p_painter
                           , double p_current_time
                           , double p_zoom_X
                           , double p_view_bottom
                           , double p_zoom_Y
                           , int p_view_type
                           );
    void paintEvent(QPaintEvent *);
    QSize sizeHint(void) const;

  private:

    /*
     * Changes the cursor icon to be one of the zoom ones depending on if the control or alt keys were pressed.
     * Otherwise, ignores the event.
     *
     * @param k the QKeyEvent to respond to.
     */
    void keyPressEvent( QKeyEvent *p_key_event );

    /*
     * Unsets the cursor icon if the control or alt key was released. Otherwise, ignores the event.
     *
     * @param k the QKeyEvent to respond to.
     */
    void keyReleaseEvent( QKeyEvent *p_key_event);
    void leaveEvent ( QEvent * p_event);
  
    /*
     * If control or alt is pressed, zooms. If shift is also pressed, it 'reverses' the zoom: ie ctrl+shift zooms x
     * out, alt+shift zooms y out. Otherwise, does internal processing.
     *
     * @param e the QMouseEvent to respond to.
     */
    void mousePressEvent( QMouseEvent *p_mouse_event );
    void mouseMoveEvent( QMouseEvent *p_mouse_event );
    void mouseReleaseEvent( QMouseEvent *p_mouse_event );
    void wheelEvent(QWheelEvent * p_wheel_event);
    void resizeEvent (QResizeEvent *p_resize_event);
  
    /**
       Calculates at what time the mouse is.
       @param p_x the mouse's x co-ordinate
       @return the time the mouse is positioned at.
    */
    double mouseTime(int p_x);

    /**
       Calculates at what note pitch the mouse is at.
       @param x the mouse's y co-ordinate
       @return the pitch the mouse is positioned at.
    */
    double mousePitch(int p_y);
    Channel *channelAtPixel(int p_x, int p_y);

    int m_drag_mode;
    int m_mouse_X;
    int m_mouse_Y;
    double m_down_time;
    double m_down_note;

};

#endif // FREQDRAWWIDGET_H
// EOF
