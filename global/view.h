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

#include <qobject.h>
#include <qtimer.h>
#include "useful.h"

class Channel;

class View : public QObject
{
  Q_OBJECT

public:
  View(void);
  virtual ~View(void);

  void init(void); //please call this after a window has been created



  // Relates to the views

  /**
     Returns the current time in seconds
  */
  inline const double & currentTime(void)const;

  /**
     Returns how long the view is in seconds
  */
  inline double viewWidth(void)const;

  /**
     Returns how many semitones fit in the view
  */
  inline double viewHeight(void)const;

  /**
     Returns how many seconds are visible before the current time
  */
  inline const double & viewOffset(void)const;

  /**
     Allows you to specify how many seconds should be visible before the current time
  */
  void setViewOffset(double x);

  /**
     Returns the lowest note visible in the view
  */
  inline const double & viewBottom(void)const;

  /**
     Allows you to specify the lowest note that should be visible in the view
  */
  void setViewBottomRaw(const double & y);
  void setViewBottom(const double & y);

  /**
     Returns the time at the left edge of the view
  */
  inline double viewLeft(void)const;

  /**
     Returns the time at the right edge of the view
  */
  inline double viewRight(void)const;

  /**
     Returns how long the view is in seconds
  */
  inline double viewTotalTime(void)const;

  /**
     Returns how many semitones are visible in the view
  */
  inline double viewTop(void)const;

  /**
     Returns the screen pixel for the time t will be drawn
  */
  inline int screenPixelX(double t)const;
  
  // Relates to where the files start/stop
  
  /**
     Returns the x zoom factor in seconds per pixel
  */
  inline const double & zoomX(void)const;

  /**
     Returns the y zoom factor in semitones per pixel
  */
  inline const double & zoomY(void)const;
  
  inline const double & logZoomX(void)const;
  inline const double & logZoomY(void)const;

  void setPixelHeight(int h);
  void setPixelWidth(int w);
  inline void setLogZoomYRaw(double y);
    
public slots:
  void doUpdate(void);
  void doSlowUpdate(void);
  void doFastUpdate(void);
  void newUpdate(void);
  void nextFastUpdate(void);
  void nextSlowUpdate(void);
  void setCurrentTimeRaw(double x);
  void setCurrentTime(double x);
  void changeViewX(const double & x);
  void changeViewY(const double & y);
  void setZoomFactorX(const double & x);
  void setZoomFactorX(const double & x, int fixedX);
  void setZoomFactorY(const double & y);
  void setZoomFactorY(const double & y, int fixedY);
  
  void doAutoFollowing(void);
  bool autoFollow(void) { return _autoFollow; }
  void setAutoFollow(bool);
  void setBackgroundShading(bool);
  bool backgroundShading(void) { return _backgroundShading; }
  
signals:
  void onSlowUpdate(double currentTime_);
  void onFastUpdate(double currentTime_);
  void viewChanged(void);
  void scrollableYChanged(double x);
  void currentTimeChanged(double x);
  void viewBottomChanged(double x);
  void viewWidthChanged(double x);
  void viewHeightChanged(double x);
  void viewOffsetChanged(int x);
  void zoomXChanged(double x);
  void zoomYChanged(double x);
  void timeViewRangeChanged(double leftView_, double rightView_);
  void logZoomXChanged(double x);
  void logZoomYChanged(double x);

private:
  /**
     in seconds
  */
  double _currentTime;

  /**
     the lowest note visible
  */
  double _viewBottom;

  /**
     the amount of time visible before the current time (in seconds)
  */
  double _viewOffset;

  /**
     the log of the number of pixels per second
  */
  double _logZoomX;

  /**
     the log of the number of pixels per semitone
  */
  double _logZoomY;

  /**
     the height of the view window (in pixels)
  */
  int _pixelHeight;

  /**
     the width of the view window (in pixels)
  */
  int _pixelWidth;
  
  /**
     How many seconds of data each pixel must represent (on the x axis)
  */
  double _zoomX;

  /**
     How many semitones each pixel must represent (on the y axis)
  */
  double _zoomY;
  
  /**
     Whether the view should autofollow or not
  */
  bool _autoFollow;
  bool _backgroundShading;

  inline void setLogZoomX(double x);
  inline void setLogZoomY(double y);

  QTimer *fastUpdateTimer;
  QTimer *slowUpdateTimer;
  bool needSlowUpdate;
  bool needFastUpdate;

};

#include "view.hpp"

#endif
