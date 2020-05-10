/***************************************************************************
                          htrackwidget.h  -  description
                             -------------------
    begin                : Aug 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef HTRACK_H
#define HTRACK_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>

class Piano3d;

class HTrackWidget : public QGLWidget
{
  Q_OBJECT        // must include this if you use Qt signals/slots

  public:
    HTrackWidget( QWidget *p_parent
                , const char *p_name = nullptr
                );
    ~HTrackWidget();

  public slots:
    inline void setPeakThreshold(float p_peak_threshold);
    inline void setViewAngleHorizontal(const double & p_angle);
    inline void setViewAngleVertical(const double & p_angle);
    inline void setDistanceAway(const double & p_distance);
    void home();

  signals:
    void distanceAwayChanged(double);
    void viewAngleHorizontalChanged(double);
    void viewAngleVerticalChanged(double);

  private:
    void initializeGL();
    void resizeGL( int p_w
                 , int p_h
                 );
    void paintGL();

    void rotateX(const double & p_angle);
    void rotateY(const double & p_angle);
    void translate( float p_x
                  , float p_y
                  , float p_z
                  );
    void mousePressEvent(QMouseEvent * p_event);
    void mouseMoveEvent(QMouseEvent * p_event);
    void mouseReleaseEvent(QMouseEvent * p_event);
    void wheelEvent(QWheelEvent * p_event);
  
    inline float peakThreshold()const;
    inline const double & viewAngleHorizontal()const;
    inline const double & viewAngleVertical()const;

    float m_peak_threshold;
    bool m_mouse_down;
    int m_mouse_X;
    int m_mouse_Y;
    float m_g_current_matrix[16];
    double m_distance_away;
    double m_view_angle_horizontal;
    double m_view_angle_vertical;
    Piano3d * m_piano_3d;
    double m_translate_X;
    double m_translate_Y;
};

#include "htrackwidget.hpp"

#endif // HTRACK_H
// EOF
