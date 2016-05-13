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

private:
  double _currentTime; /**< in seconds */
  double _viewBottom; /**< the lowest note visible */
  double _viewOffset; /**< the amount of time visible before the current time (in seconds) */

  double _logZoomX; /**< the log of the number of pixels per second */
  double _logZoomY; /**< the log of the number of pixels per semitone */
  int _pixelHeight; /**< the height of the view window (in pixels) */
  int _pixelWidth; /**< the width of the view window (in pixels) */
  
  double _zoomX; /**< How many seconds of data each pixel must represent (on the x axis) */
  double _zoomY; /**< How many semitones each pixel must represent (on the y axis) */
  
  bool _autoFollow; /**< Whether the view should autofollow or not */
  bool _backgroundShading;

  inline void setLogZoomX(double x);
  inline void setLogZoomY(double y);

  QTimer *fastUpdateTimer;
  QTimer *slowUpdateTimer;
  bool needSlowUpdate;
  bool needFastUpdate;

public:
  View();
  virtual ~View();

  void init(); //please call this after a window has been created



  // Relates to the views

  /** Returns the current time in seconds */
  inline const double & currentTime(void)const;

  /** Returns how long the view is in seconds */
  inline double viewWidth(void);

  /** Returns how many semitones fit in the view */
  inline double viewHeight(void);

  /** Returns how many seconds are visible before the current time */
  inline double viewOffset(void);

  /** Allows you to specify how many seconds should be visible before the current time */
  void setViewOffset(double x);

  /** Returns the lowest note visible in the view */
  inline double viewBottom(void);

  /** Allows you to specify the lowest note that should be visible in the view */
  void setViewBottomRaw(double y);
  void setViewBottom(double y);

  inline double viewLeft(void); /**< Returns the time at the left edge of the view */
  inline double viewRight(void); /**< Returns the time at the right edge of the view */
  inline double viewTotalTime(void); /**< Returns how long the view is in seconds */
  inline double viewTop(void); /**< Returns how many semitones are visible in the view*/
  /**
     Returns the screen pixel for the time t will be drawn
  */
  inline int screenPixelX(double t);
  
  // Relates to where the files start/stop
  
  /** Returns the x zoom factor in seconds per pixel */
  inline double zoomX(void);
  /** Returns the y zoom factor in semitones per pixel */
  inline double zoomY(void);
  
  inline double logZoomX(void);
  inline double logZoomY(void);

  void setPixelHeight(int h);
  void setPixelWidth(int w);
  inline void setLogZoomYRaw(double y);
    
public slots:
  void doUpdate();
  void doSlowUpdate();
  void doFastUpdate();
  void newUpdate();
  void nextFastUpdate();
  void nextSlowUpdate();
  void setCurrentTimeRaw(double x);
  void setCurrentTime(double x);
  void changeViewX(double x);
  void changeViewY(double y);
  void setZoomFactorX(double x);
  void setZoomFactorX(double x, int fixedX);
  void setZoomFactorY(double y);
  void setZoomFactorY(double y, int fixedY);
  
  void doAutoFollowing();
  bool autoFollow() { return _autoFollow; }
  void setAutoFollow(bool);
  void setBackgroundShading(bool);
  bool backgroundShading() { return _backgroundShading; }
  
signals:
  void onSlowUpdate(double currentTime_);
  void onFastUpdate(double currentTime_);
  void viewChanged();
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
};

#include "view.hpp"

#endif
