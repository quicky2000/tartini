/***************************************************************************
                          vibratotunerwidget.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOTUNERWIDGET_H
#define VIBRATOTUNERWIDGET_H

#include <QGLWidget>

class VibratoTunerWidget: public QGLWidget
{
  Q_OBJECT

  public:
    VibratoTunerWidget(QWidget *p_parent);

    virtual ~VibratoTunerWidget(void);

    void initializeGL(void);
    void resizeGL(int p_width, int p_height);
    void paintGL(void);

    QSize minimumSizeHint(void) const;

  signals:

    void ledSet(int index, bool value);

  public slots:

    void doUpdate(double p_pitch);

  private:

    //data goes here

    float m_needle_value_to_draw;
    float m_prev_needle_value;
    int m_prev_close_pitch;
    bool m_prev_is_pitch_black_note;
    double m_cur_pitch;

    QFont m_tuner_font;

    float m_cents_label_X;
    float m_cents_label_Y;

    int m_tuner_label_counter;
    struct tunerLabelStruct
    {
        QString m_label;
        float m_x;
        float m_y;
    };
    tunerLabelStruct tunerLabels[100];

    void resetLeds(void);

    GLuint m_dial;
    GLuint m_needle;
};
#endif // VIBRATOTUNERWIDGET_H
// EOF
