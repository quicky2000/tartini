/***************************************************************************
                          tunerwidget.cpp  -  description
                             -------------------
    begin                : Mon Jan 10 2005
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
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

#include "tunerwidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "myqt.h"

//------------------------------------------------------------------------------
TunerWidget::TunerWidget(QWidget *p_parent)
: DrawWidget(p_parent)
{
    m_value = 0;
}

//------------------------------------------------------------------------------
TunerWidget::~TunerWidget(void)
{
}

//------------------------------------------------------------------------------
void TunerWidget::paintEvent(QPaintEvent *)
{
    beginDrawing();

    /*
     The tuner is basically just a sector. It has a l_radius of 1.2 * height() or
     1.2 * width() when appropriate, and we draw the largest sector we can fit on screen.

     To calculate this sector, we have to work out the starting angle, and work out how
     much we should draw. From basic trig, we can calculate the starting angle:

     <--- w ---->
     ___________
     \    |    /             sin t = Opposite / Hypotenuse
      \   |   /                    = 0.5 width() / l_radius
       \  |  / r = 1.2h (or 1.2w)  = width() / 2 * l_radius
        \ |t/
         \|/

     So the starting angle is 90 - t. The finishing angle is just the starting angle + 2t.

     To draw the markers, we can just use the equation of a circle to get x and y points
     along it.

     cos(i) = x / r   ->  x = r * cos(i)
     sin(i) = y / r   ->  y = r * sin(i)

     These points have to be adjusted to fit in the widget, so the x co-ordinates get
     l_radius added to them, and the y co-ordinates are taken away from l_radius.

     From there, we just step along the edge, drawing a line at each point we want. The
     lines would eventually meet up with the radius of the circle (width()/2, l_radius).

     */

    QPalette l_palette;
    QColor l_color = l_palette.color(QPalette::WindowText);
    QPen l_pen(l_color, 2);
    // Border
    get_painter().setPen(l_pen);

    double l_half_width = double(width()) / 2.0;
    double l_radius = 1.8 * MAX(height() / 2, l_half_width);
    QPoint l_center(toInt(l_half_width), toInt(l_radius));
    double l_theta = asin(double(width()) / (2 * l_radius));
    double l_theta_deg = l_theta * 180.0 / PI;
    double l_rho = (PI / 2.0) - l_theta;

    {
        // Draw the semicircle
        // Fill colour
        get_painter().setBrush(Qt::white);
        get_painter().drawPie(toInt(l_half_width - l_radius), 0, toInt(2.0 * l_radius), toInt(2.0 * l_radius), toInt((90 - l_theta_deg) * 16), toInt(2 * l_theta_deg * 16));
        get_painter().drawArc(toInt(l_half_width - (l_radius / 2.0)), toInt(l_radius / 2.0), toInt(l_radius), toInt(l_radius), toInt((90 - l_theta_deg) * 16), toInt(2 * l_theta_deg * 16));
    }

    get_painter().setPen(l_color);
    get_painter().setBrush(l_color);
    
    double l_step = (2 * l_theta) / 12.0;
    double l_stop = l_rho + (2 * l_theta) - (l_step / 2);
    {
        // Draw line markings
        for(double l_index = l_rho + l_step; l_index < l_stop; l_index += l_step)
        {
            int l_x = toInt(l_half_width + l_radius * cos(l_index));
            int l_y = toInt(l_radius - l_radius * sin(l_index));
            QPoint l_start(l_x, l_y);
            double l_t = 0.05; //0.025;
            get_painter().drawLine(l_start, l_start + l_t * (l_center - l_start));
        }
    }

    {
        //Draw the text labels
        get_painter().setPen(l_color);

        const char *l_the_names[11] = { "+50", "+40", "+30", "+20", "+10", "0", "-10", "-20", "-30", "-40", "-50" };
        QFontMetrics l_font_metrics = get_painter().fontMetrics();
        int l_half_font_height = l_font_metrics.height() / 2;
        int l_half_font_width;

        for(int l_j = 0; l_j < 11;)
        {
            double l_i = l_rho + l_step *(l_j + 1);
            int l_x = toInt(l_half_width + l_radius * cos(l_i));
            int l_y = toInt(l_radius - l_radius * sin(l_i));
            QPoint l_start(l_x, l_y);
            double l_t = 0.08; //0.025;
            QPoint l_pt = l_start + l_t * (l_center - l_start);
            l_half_font_width = l_font_metrics.width(l_the_names[l_j]) / 2;

            get_painter().drawText(l_pt.x() - l_half_font_width, l_pt.y() + l_half_font_height, l_the_names[l_j]);
            if(l_radius < 300)
            {
                l_j += 2;
            }
            else
            {
                l_j++;
            }
        }
        l_half_font_width = l_font_metrics.width("Cents") / 2;
        get_painter().drawText(l_center.x() - l_half_font_width, toInt(l_center.y() * 0.2) + l_half_font_height, "Cents");
    }

    {
        //draw needle
        double l_cent_angle = (2 * l_theta) / 120;
        double l_note = l_rho + (fabs(m_value - 60) * l_cent_angle);
        get_painter().setPen(colorBetween(Qt::white, Qt::darkRed, m_intensity));

        int l_half_knob_width = MAX(toInt(l_radius * 0.02), 1);

        QPoint l_note_X(toInt(l_half_width + l_radius * cos(l_note))
                       ,toInt(l_radius - l_radius * sin(l_note))
                       );
  
        QPoint l_knob_note(toInt(l_half_width + l_half_knob_width * cos(l_note))
                          ,toInt(height() - l_half_knob_width * sin(l_note))
                          );

        QPoint l_knob_left = l_center - QPoint(l_half_knob_width, 0);
        QPoint l_knob_right = l_center + QPoint(l_half_knob_width, 0);

        if(m_intensity > 0.0)
        {
            get_painter().setBrush(colorBetween(Qt::white, Qt::red, m_intensity));
            QPolygon l_points(3);
            l_points.setPoint(0, l_note_X);
            l_points.setPoint(1, l_knob_right);
            l_points.setPoint(2, l_knob_left);
            get_painter().drawPolygon(l_points);
        }
    }
    endDrawing();
}

//------------------------------------------------------------------------------
void TunerWidget::setValue(float p_value
                          ,float p_intensity
                          )
{
    if(m_value != p_value && p_value >= -60 && p_value <= 60)
    {
        m_value = p_value;
        update();
    }
    if(m_intensity != p_intensity && p_intensity >= 0 && p_intensity <= 1)
    {
        m_intensity = p_intensity;
        update();
    }
}

//------------------------------------------------------------------------------
double TunerWidget::value(void)
{
    return m_value;
}

//------------------------------------------------------------------------------
QSize TunerWidget::minimumSizeHint(void) const
{
    return QSize(100, 75);
}
// EOF
