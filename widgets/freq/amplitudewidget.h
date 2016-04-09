/***************************************************************************
                          amplitudewidget.h  -  description
                             -------------------
    begin                : 19 Mar 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef AMPLITUDEWIDGET_H
#define AMPLITUDEWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QPixmap>
#include <QWheelEvent>
#include <QPaintEvent>
#include "channel.h"

struct ZoomElement;
class AnalysisData;

class AmplitudeWidget : public QGLWidget
{
  Q_OBJECT

  public:
    enum DragModes
    { DragNone = 0
    , DragChannel = 1
    , DragBackground = 2
    , DragTimeBar = 3
    , DragNoiseThreshold = 4
    };

    AmplitudeWidget( QWidget * p_parent
                   , const char * p_name = 0
                   );
    virtual ~AmplitudeWidget(void);

    inline QSize sizeHint(void) const;
    inline const double & range(void)const;

  public slots:
    void setRange(double p_new_range);
    void setOffset(double p_new_offset);

  signals:
    void rangeChanged(double);
    void offsetChanged(double);
    void offsetInvChanged(double);

  private:
    void initializeGL(void);
    void resizeGL( int p_width
                 , int p_height
                 );
    void paintGL(void);

    void setLineWidth(float p_width);
    void drawVerticalRefLines(void);
    double getCurrentThreshold(int p_index);
    QString getCurrentThresholdString(void)const;
    void drawChannelAmplitudeGL(Channel * p_channel);
    void drawChannelAmplitudeFilledGL(Channel * p_channel);

    /**
       This function has the side effect of changing zoom element
    */
    bool calcZoomElement( ZoomElement & p_zoom_element
                        , Channel * p_channel
                        , int p_base_element
                        , double p_base_X
                        );
    double calculateElement(AnalysisData * p_data);
    void setCurrentThreshold( double p_new_threshold
                            , int p_index
                            );

    void mousePressEvent( QMouseEvent * p_mouse_event);
    void mouseMoveEvent( QMouseEvent * p_mouse_event);
    void mouseReleaseEvent( QMouseEvent * p_mouse_event);
    void wheelEvent(QWheelEvent * p_mouse_event);

    inline double leftTime(void)const;
    inline double rightTime(void)const;
    inline double timeWidth(void)const;

    inline double maxOffset(void)const;
    inline const double & offset(void)const;
    inline const double & offsetInv(void)const;

    int m_drag_mode;
    int m_mouse_X;
    int m_mouse_Y;
    double m_down_time;
    double m_down_offset;
    int m_threshold_index;
    double m_range;
    double m_offset;
    double m_offset_inv;
    float m_line_width;
    float m_half_line_width;

};

#include "amplitudewidget.hpp"

#endif
//EOF
