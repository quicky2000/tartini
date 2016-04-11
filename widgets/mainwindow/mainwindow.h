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
    ViewData( QString p_title
            , QString p_menu_name
            , QString p_class_name
            , int p_menu_type
            )
    {
        m_title = p_title;
        m_menu_name = p_menu_name;
        m_class_name = p_class_name;
        m_menu_type = p_menu_type;
    };
    QString m_title;
    QString m_menu_name;
    QString m_class_name;
    int m_menu_type;
};

enum VIEW_MENU_TYPES
  { MENU_TYPE_MAIN
  , MENU_TYPE_TECHNICAL
  , MENU_TYPE_EXPERIMENTAL
  };

#define NUM_VIEWS 19
enum VIEW_NAMES
  { VIEW_OPEN_FILES
  , VIEW_FREQ
  , VIEW_TUNER
  , VIEW_HTRACK
  , VIEW_VIBRATO
  , VIEW_SCORE
  , VIEW_WAVE
  , VIEW_CORRELATION
  , VIEW_FFT
  , VIEW_CEPSTRUM
  , VIEW_DEBUGVIEW
  , VIEW_HBLOCK
  , VIEW_HSTACK
  , VIEW_HBUBBLE
  , VIEW_HCIRCLE
  , VIEW_PITCH_COMPASS
  , VIEW_PIANO
  , VIEW_SUMMARY
  , VIEW_VOLUME_METER
};

extern ViewData g_view_data[NUM_VIEWS];

                         
class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:

    MainWindow(void);
    virtual ~MainWindow(void);

    void keyPressEvent(QKeyEvent * p_event);
    void message(QString p_string, int p_msec);

    QWorkspace * m_the_workspace;

    QMenu * m_new_view_menu;
    QMenu * m_window_menu;
    QMenu * m_options_menu;
    QMenu * m_help_menu;

    MyLabel * m_time_label;
    MyLabel * m_chunk_label;
    MyLabel * m_note_label;
    QwtSlider * m_time_slider;
    MyScrollBar * m_time_scroll_bar;
    QComboBox * m_key_type_combo_box;

    QAction * m_play_stop_action;
    QAction * m_record_action;
    QAction * m_play_record_action;
    QIcon * m_play_icon_set;
    QIcon * m_play_record_icon_set;
    QIcon * m_stop_icon_set;
    QIcon * m_record_icon_set;
    QTimer * m_rewind_timer;
    QTimer * m_fast_forward_timer;

    QSignalMapper * m_create_signal_mapper;

  protected:
    bool event(QEvent * p_event);

  signals:
    void changedMusicFolder(const QString &);
    void zoomInPressed(void);
    void zoomOutPressed(void);

  public slots:
    void openFile(void);
    void openFile(const char *p_file_name);

    void openRecord(void);
    void openPlayRecord(void);
    void openRecord(bool p_and_play);
    void closeAllWidgets(void);
    void closeEvent(QCloseEvent *p_event);
    void menuPreferences(void);

    void windowMenuAboutToShow(void);
    void windowMenuActivated(int p_id);
    void newViewAboutToShow(void);

    /**
     * Opens a view based on a viewId (which should be #defined).
     * All widgets should be created anonymously - ie no saving pointers!
     *
     * @param p_view_id the view to open's ID.
     */
    QWidget *openView(int p_view_id);
    void setTimeLabel(double p_t);
    void setChunkLabel(void);
    void setNoteLabel(void);
    void setTimeRange(double p_min, double p_max);

    void rewindPressed(void);
    void rewindReleased(void);
    void playStopClicked(void);
    void fastforwardPressed(void);
    void fastforwardReleased(void);
    void setTitle(Channel *p_channel);
    void aboutTartini(void);
    void aboutGPL(void);
    void aboutQt(void);
    void showDocumentation(void);
    void printPitch(void);
    void exportChannel(int p_type, QString p_type_string);
    void exportChannelPlainText(void);
    void exportChannelMatlab(void);

    bool loadViewGeometry(void);
    void saveViewGeometry(void);
};

class TartiniDialog :public QDialog
{
  Q_OBJECT

  public:
    TartiniDialog(QWidget *p_parent = NULL);
    QSize sizeHint(void) const;
};

class GPLDialog :public QDialog
{
  Q_OBJECT

  public:
    GPLDialog(QWidget *p_parent = NULL);
    QSize sizeHint(void) const;
};

extern MainWindow * g_main_window;
class MyGLFont;
extern MyGLFont * g_mygl_font;

#endif // MAINWINDOW_H
// EOF
