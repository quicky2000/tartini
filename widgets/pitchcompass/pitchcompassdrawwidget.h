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

class QwtCompass;

class PitchCompassDrawWidget: public QWidget
{

  Q_OBJECT

  public:
    PitchCompassDrawWidget( QWidget * p_parent
                          , const std::string & p_name = ""
                          , int p_mode = 0
                          );
    virtual ~PitchCompassDrawWidget();

    void resizeEvent(QResizeEvent *);

  public slots:

    void updateCompass(double p_time);

  private:

    void blank();

    QwtCompass * m_compass;

    int m_blank_count;

    int m_mode;
};
#endif // PITCHCOMPASSDRAWWIDGET_H
// EOF
