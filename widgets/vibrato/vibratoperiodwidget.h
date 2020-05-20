/***************************************************************************
                          vibratoperiodwidget.h  -  description
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
#ifndef VIBRATOPERIODWIDGET_H
#define VIBRATOPERIODWIDGET_H

#include <QGLWidget>

class VibratoPeriodWidget: public QGLWidget
{
  Q_OBJECT

  public:
    VibratoPeriodWidget(QWidget * p_parent);
    ~VibratoPeriodWidget() override;

    void initializeGL() override;
    void resizeGL(int p_width, int p_height) override;
    void paintGL() override;

    QSize minimumSizeHint() const override;

  public slots:
    void doUpdate();
    void setSmoothedPeriods(bool p_value);
    void setDrawSineReference(bool p_value);
    void setSineStyle(bool p_value);
    void setDrawPrevPeriods(bool p_value);
    void setPeriodScaling(bool p_value);
    void setDrawComparison(bool p_value);

  private:
    int m_prev_left_minimum_time;
    bool m_smoothed_periods;
    bool m_draw_sine_reference;
    bool m_sine_style;
    bool m_draw_prev_periods;
    bool m_period_scaling;
    bool m_draw_comparison;

    GLuint m_sine_reference;
    GLuint m_previous_poly[5];
    GLuint m_current_period;
    GLuint m_comparison_poly;
    GLuint m_comparison_reference;

    int m_last_period_to_draw;
};
#endif // VIBRATOPERIODWIDGET_H
// EOF
