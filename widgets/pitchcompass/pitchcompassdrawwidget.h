/***************************************************************************
                          pitchcompassdrawwidget.h  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

 ***************************************************************************/

#ifndef PITCHCOMPASSDRAWWIDGET_H
#define PITCHCOMPASSDRAWWIDGET_H

#include <QResizeEvent>
#include "pitchcompassview.h"

class QwtCompass;

class PitchCompassDrawWidget: public QWidget
{

  Q_OBJECT

  public:
    PitchCompassDrawWidget( QWidget * p_parent
                          , const std::string & p_name = ""
                          , PitchCompassView::CompassMode p_mode = PitchCompassView::CompassMode::Mode2
                          );

    ~PitchCompassDrawWidget() override;

    void resizeEvent(QResizeEvent *) override;

  public slots:

    void updateMusicKey(int p_music_key);
    void updateMusicScale(int p_music_scale);
    void updateCompass(double p_time);

  private:

    void setCompassScale();
    void blank(bool p_force = false);

    QwtCompass * m_compass;

    int m_blank_count;

    PitchCompassView::CompassMode m_mode;
};
#endif // PITCHCOMPASSDRAWWIDGET_H
// EOF
