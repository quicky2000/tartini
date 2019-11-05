/***************************************************************************
                          vibratospeedwidget.h  -  description
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
#ifndef VIBRATOSPEEDWIDGET_H
#define VIBRATOSPEEDWIDGET_H

#include <QGLWidget>

class VibratoSpeedWidget: public QGLWidget
{
  Q_OBJECT

  public:
    VibratoSpeedWidget(QWidget * p_parent);
    virtual ~VibratoSpeedWidget(void);

    void initializeGL(void);
    void resizeGL(int p_width
                 ,int p_height
                 );
    void paintGL(void);

    QSize minimumSizeHint(void) const;

 public slots:
    void doUpdate(void);
    void setUseProny(bool p_value);

 private:
    //data goes here
    float m_speed_value_to_draw;
    float m_width_value_to_draw;
    float m_prev_vibrato_speed;
    float m_prev_vibrato_width;

    bool m_use_prony;

    QFont m_speed_width_font;

    int m_width_limit;
    int m_prev_note_number;

    float m_hz_label_X;
    float m_hz_label_Y;
    float m_cents_label_X;
    float m_cents_label_Y;

    class labelStruct
    {
      public:

        void set( const QString & p_label
                , float p_x
                , float p_y
                );

        const QString & get_label() const;
        float get_x() const;
        float get_y() const;

      private:

        QString m_label;
        float m_x;
        float m_y;
    };

    int m_speed_label_counter;
    labelStruct m_speed_labels[100];

    int m_width_label_counter;
    labelStruct m_width_labels[100];

    GLuint m_speed_dial;
    GLuint m_speed_needle;
    GLuint m_width_dial;
    GLuint m_width_needle;
};
#endif // VIBRATOSPEEDWIDGET_H
// EOF
