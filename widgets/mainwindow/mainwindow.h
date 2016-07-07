/***************************************************************************
                          mainwindow.h  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWorkspace>
#include <QDialog>
#include <QKeyEvent>
#include <QEvent>
#include <QLabel>
#include <QSignalMapper>

class Preferences;
class Q3ListView;
class Q3ListViewItem;
class Q3DockWindow;
class QLabel;
class QwtSlider;
class MyScrollBar;
class QTimer;
class QIcon;
class QToolButton;
class Channel;
class MyLabel;
class QComboBox;

struct ViewData
{
  ViewData(QString title_, QString menuName_, QString className_, int menuType_)
  {
    title = title_;
    menuName = menuName_;
    className = className_;
    menuType = menuType_;
  };
  QString title;
  QString menuName;
  QString className;
  int menuType;
};

enum VIEW_MENU_TYPES
  {
    MENU_TYPE_MAIN,
    MENU_TYPE_TECHNICAL,
    MENU_TYPE_EXPERIMENTAL
  };

#define NUM_VIEWS 19
enum VIEW_NAMES
  {
    VIEW_OPEN_FILES,
    VIEW_FREQ,
    VIEW_TUNER,
    VIEW_HTRACK,
    VIEW_VIBRATO,
    VIEW_SCORE,
    VIEW_WAVE,
    VIEW_CORRELATION,
    VIEW_FFT,
    VIEW_CEPSTRUM,
    VIEW_DEBUGVIEW,
    VIEW_HBLOCK,
    VIEW_HSTACK,
    VIEW_HBUBBLE,
    VIEW_HCIRCLE,
    VIEW_PITCH_COMPASS,
    VIEW_PIANO,
    VIEW_SUMMARY,
    VIEW_VOLUME_METER
};

extern ViewData viewData[NUM_VIEWS];

                         
class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  MainWindow(void);
  virtual ~MainWindow(void);

  void keyPressEvent(QKeyEvent * e);
  void message(QString s, int msec);
  Q3ListView * theListView;
  
  QWorkspace * theWorkspace;

  QMenu * newViewMenu;
  QMenu * windowMenu;
  QMenu * optionsMenu;
  QMenu * helpMenu;

  MyLabel * timeLabel;
  MyLabel * chunkLabel;
  MyLabel * noteLabel;
  QwtSlider * timeSlider;
  MyScrollBar * timeScrollBar;
  QComboBox * keyTypeComboBox;
    
  QAction * playStopAction;
  QAction * recordAction;
  QAction * playRecordAction;
  QIcon * playIconSet;
  QIcon * playRecordIconSet;
  QIcon * stopIconSet;
  QIcon * recordIconSet;
  QTimer * rewindTimer;
  QTimer * fastforwardTimer;

  QSignalMapper * createSignalMapper;
  
 protected:
  bool event(QEvent * e);

signals:
  void changedMusicFolder(const QString &);
  void zoomInPressed(void);
  void zoomOutPressed(void);
    
public slots:

  void openFile(void);
  void openFile(const char *filename);

  void openRecord(void);
  void openPlayRecord(void);
  void openRecord(bool andPlay);
  void closeAllWidgets(void);
  void closeEvent(QCloseEvent *event);
  void menuPreferences(void);

  void windowMenuAboutToShow(void);
  void windowMenuActivated(int id);
  void newViewAboutToShow(void);

  /**
   * Opens a view based on a viewId (which should be #defined).
   * All widgets should be created anonymously - ie no saving pointers!
   *
   * @param viewID the view to open's ID.
   */
  QWidget *openView(int viewID);
  void setTimeLabel(double t);
  void setChunkLabel(void);
  void setNoteLabel(void);
  void setTimeRange(double min_, double max_);

  void rewindPressed(void);
  void rewindReleased(void);
  void playStopClicked(void);
  void fastforwardPressed(void);
  void fastforwardReleased(void);
  void setTitle(Channel *ch);
  void aboutTartini(void);
  void aboutGPL(void);
  void aboutQt(void);
  void showDocumentation(void);
  void printPitch(void);
  void exportChannel(int type, QString typeString);
  void exportChannelPlainText(void);
  void exportChannelMatlab(void);

  bool loadViewGeometry(void);
  void saveViewGeometry(void);
};

class TartiniDialog :public QDialog
{
  Q_OBJECT
 
 public:
  TartiniDialog(QWidget *parent = NULL);
  QSize sizeHint(void) const;
};

class GPLDialog :public QDialog
{
  Q_OBJECT
 
 public:
  GPLDialog(QWidget *parent = NULL);
  QSize sizeHint(void) const;
};

extern MainWindow * mainWindow;
class MyGLFont;
extern MyGLFont * mygl_font;

#endif // MAINWINDOW_H
// EOF
