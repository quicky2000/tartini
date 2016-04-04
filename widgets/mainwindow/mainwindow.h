/***************************************************************************
                          mainwindow.h  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qworkspace.h>
#include <qdialog.h>
//#include "audio_stream.h"
//#include "sound_file_stream.h"
//#include "fwinfunc.h"
//#include "audio_thread.h"

class Preferences;
class QListView;
class QListViewItem;
class QDockWindow;
class QLabel;
class QwtSlider;
class MyScrollBar;
class QTimer;
class QIconSet;
class QToolButton;
class Channel;
class MyLabel;

struct ViewData
{
	ViewData(QString title_, QString menuName_, QString className_)
	{
    title = title_;
		menuName = menuName_;
		className = className_;
	};
  QString title;
  QString menuName;
  QString className;
};

#define NUM_VIEWS 13
enum VIEW_NAMES { VIEW_OPEN_FILES, VIEW_FREQ, VIEW_TUNER, VIEW_HTRACK,
  VIEW_PIANO, VIEW_HBLOCK, VIEW_PITCH_COMPASS, VIEW_SUMMARY,
  VIEW_VOLUME_METER/*, VIEW_HSTACK*/, 
  VIEW_WAVE, VIEW_CORRELATION, VIEW_FFT, VIEW_CEPSTRUM };

extern ViewData viewData[NUM_VIEWS];

                         
class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow();
  virtual ~MainWindow();

  void keyPressEvent ( QKeyEvent * e );
  bool close(bool alsoDelete);
  void message(QString s, int msec);
  QListView *theListView;
  
  QDockWindow *listDockWindow;
  QDockWindow *summaryDockWindow;
  QWorkspace *theWorkspace;

  QPopupMenu *newViewMenu;
  QPopupMenu *windowMenu;
  QPopupMenu *optionsMenu;
  QPopupMenu *helpMenu;

  MyLabel *timeLabel;
  MyLabel *chunkLabel;
  MyLabel *noteLabel;
  QwtSlider *timeSlider;
  MyScrollBar *timeScrollBar;
    
  QToolButton *playStopButton;
  QToolButton *recordButton;
  QIconSet *playIconSet;
  QIconSet *stopIconSet;
  QIconSet *recordIconSet;
  QTimer *rewindTimer;
  QTimer *fastforwardTimer;
  
 protected:
  //void customEvent( QCustomEvent * e );
  bool event( QEvent * e );

signals:
  void changedMusicFolder(const QString &);
  //void fileAdded(QString);
  void zoomInPressed();
  void zoomOutPressed();
    
public slots:

  void openFile();
  void openFile(const char *filename);

  void openRecord();
  //void toggleOption(int id);
  void closeAllWidgets();
  void menuPreferences();

  void windowMenuAboutToShow();
  void windowMenuActivated(int id);
  void newViewAboutToShow();
  void openView(int viewID);
  void setTimeLabel(double t);
  void setChunkLabel();
  void setNoteLabel();
  void setTimeRange(double min_, double max_);
  //void setPageStep(double step_);

  void rewindPressed();
  void rewindReleased();
  void playStopClicked();
  void fastforwardPressed();
  void fastforwardReleased();
  void setTitle(Channel *ch);
  void aboutTartini();
  void aboutQt();
};

class TartiniDialog : public QDialog
{
  Q_OBJECT
 
 public:
  TartiniDialog(QWidget *parent = NULL);
  QSize sizeHint() const { return QSize(600, 480); }
};

extern MainWindow *mainWindow;

#endif
