/***************************************************************************
                          pitchcompassdrawwidget.h  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 ***************************************************************************/

#ifndef PITCHCOMPASSDRAWWIDGET_H
#define PITCHCOMPASSDRAWWIDGET_H

#include "drawwidget.h"
#include <QResizeEvent>

class QwtCompass;

class PitchCompassDrawWidget: public QWidget
{

  Q_OBJECT

  public:
  PitchCompassDrawWidget(QWidget * parent, const char * name = NULL, int mode = 0);
  virtual ~PitchCompassDrawWidget(void);

  void resizeEvent(QResizeEvent *);

  public slots:
  void updateCompass(double time);

  private:
  QwtCompass * compass;
  bool moveNeedle;
  void blank(void);
  int blankCount;
  int mode;
};
#endif // PITCHCOMPASSDRAWWIDGET_H
// EOF
