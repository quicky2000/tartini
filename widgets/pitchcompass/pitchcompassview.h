/***************************************************************************
                          pitchcompassview.h  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef PITCHCOMPASSVIEW_H
#define PITCHCOMPASSVIEW_H

#include "viewwidget.h"
#include <QResizeEvent>

class PitchCompassDrawWidget;

class PitchCompassView: public ViewWidget
{
  Q_OBJECT

  public:

    enum class CompassMode
    { Mode0
    , Mode1
    , Mode2
    };
    
    PitchCompassView( int p_view_id
                    , QWidget * p_parent = nullptr
                    , CompassMode p_mode = CompassMode::Mode2
                    );
    virtual ~PitchCompassView();

    QSize sizeHint() const;

    void resizeEvent(QResizeEvent *);

 private:
    void changeMode(CompassMode p_mode);

    PitchCompassDrawWidget * m_pitch_compass_draw_widget;

};

#endif // PITCHCOMPASSVIEW_H
// EOF
