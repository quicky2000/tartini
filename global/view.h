/***************************************************************************
                          view.h  -  description
                             -------------------
    begin                : Mon Jul 26 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QTimer>
#include "useful.h"

class Channel;

class View : public QObject
{
  Q_OBJECT

  public:

    View();
    virtual ~View();

    void init(); //please call this after a window has been created

    // Relates to the views

    /**
       Returns the current time in seconds
    */
    inline const double & currentTime()const;

    /**
       Returns how long the view is in seconds
    */
    inline double viewWidth()const;

    /**
       Returns how many semitones fit in the view
    */
    inline double viewHeight()const;

    /**
       Returns how many seconds are visible before the current time
    */
    inline const double & viewOffset()const;

    /**
       Allows you to specify how many seconds should be visible before the current time
    */
    void setViewOffset(double p_x);

    /**
       Returns the lowest note visible in the view
    */
    inline const double & viewBottom()const;

    /**
       Allows you to specify the lowest note that should be visible in the view
    */
    void setViewBottomRaw(const double & p_y);
    void setViewBottom(const double & p_y);

    /**
       Returns the time at the left edge of the view
    */
    inline double viewLeft()const;

    /**
       Returns the time at the right edge of the view
    */
    inline double viewRight()const;

    /**
       Returns how long the view is in seconds
    */
    inline double viewTotalTime()const;

    /**
       Returns how many semitones are visible in the view
    */
    inline double viewTop()const;

    /**
       Returns the screen pixel for the time t will be drawn
    */
    inline int screenPixelX(double p_t)const;
  
    // Relates to where the files start/stop
  
    /**
       Returns the x zoom factor in seconds per pixel
    */
    inline const double & zoomX()const;

    /**
       Returns the y zoom factor in semitones per pixel
    */
    inline const double & zoomY()const;
  
    inline const double & logZoomX()const;
    inline const double & logZoomY()const;

    void setPixelHeight(int p_h);
    void setPixelWidth(int p_w);
    inline void setLogZoomYRaw(double p_y);
    
  public slots:

    void doUpdate();
    void doSlowUpdate();
    void doFastUpdate();
    void doVeryFastUpdate();
    void newUpdate();
    void nextVeryFastUpdate();
    void nextFastUpdate();
    void nextSlowUpdate();
    void setCurrentTimeRaw(double p_x);
    void setCurrentTime(double p_x);
    void changeViewX(const double & p_x);
    void changeViewY(const double & p_y);
    void setZoomFactorX(const double & p_x);
    void setZoomFactorX( const double & p_x
                       , int p_fixed_x
                       );
    void setZoomFactorY(const double & p_y);
    void setZoomFactorY( const double & p_y
                       , int p_fixed_y
                       );
  
    void doAutoFollowing();
    inline bool autoFollow()const;
    void setAutoFollow(bool);
    void setBackgroundShading(bool);
    inline bool backgroundShading()const;
  
  signals:

    void onSlowUpdate(double p_current_time);
    void onFastUpdate(double p_current_time);
    void onVeryFastUpdate(double p_current_time);
    void viewChanged();
    void scrollableYChanged(double p_x);
    void currentTimeChanged(double p_x);
    void viewBottomChanged(double p_x);
    void viewWidthChanged(double p_x);
    void viewHeightChanged(double p_x);
    void viewOffsetChanged(int p_x);
    void zoomXChanged(double p_x);
    void zoomYChanged(double p_x);
    void timeViewRangeChanged( double p_left_view
                             , double p_right_view
                             );
    void logZoomXChanged(double p_x);
    void logZoomYChanged(double p_x);

  private:
    /**
       in seconds
    */
    double m_current_time;

    /**
       the lowest note visible
    */
    double m_view_bottom;

    /**
       the amount of time visible before the current time (in seconds)
    */
    double m_view_offset;

    /**
       the log of the number of pixels per second
    */
    double m_log_zoom_X;

    /**
       the log of the number of pixels per semitone
    */
    double m_log_zoom_Y;

    /**
       the height of the view window (in pixels)
    */
    int m_pixel_height;

    /**
       the width of the view window (in pixels)
    */
    int m_pixel_width;
  
    /**
       How many seconds of data each pixel must represent (on the x axis)
    */
    double m_zoom_X;

    /**
       How many semitones each pixel must represent (on the y axis)
    */
    double m_zoom_Y;
  
    /**
       Whether the view should autofollow or not
    */
    bool m_auto_follow;
    bool m_background_shading;

    inline void setLogZoomX(double p_x);
    inline void setLogZoomY(double p_y);

    QTimer * m_very_fast_update_timer;
    QTimer * m_fast_update_timer;
    QTimer * m_slow_update_timer;
    bool m_need_slow_update;
    bool m_need_fast_update;
    bool m_need_very_fast_update;

};

#include "view.hpp"

#endif
