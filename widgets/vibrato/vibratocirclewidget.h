/***************************************************************************
                          vibratocirclewidget.h  -  description
                             -------------------
    begin                : May 18 2005
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
#ifndef VIBRATOCIRCLEWIDGET_H
#define VIBRATOCIRCLEWIDGET_H

#include <QGLWidget>

class VibratoCircleWidget: public QGLWidget
{
  Q_OBJECT

  public:
    VibratoCircleWidget(QWidget * p_parent);
    virtual ~VibratoCircleWidget(void);

    void initializeGL(void);
    void resizeGL(int p_width
                 ,int p_height
                 );
    void paintGL(void);

    QSize minimumSizeHint(void) const;
    int getType(void);

  public slots:

    void doUpdate(void);
    void setAccuracy(int p_value);
    void setType(int p_value);

  private:

    float m_accuracy;
    int m_type;
    int m_last_period_to_draw;

    GLuint m_reference_circle;
    GLuint m_current_period;
    GLuint m_prev_periods[6];
};

#endif // VIBRATOCIRCLEWIDGET_H
// EOF
