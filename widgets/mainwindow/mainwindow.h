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

#include "viewdata.h"
#include <QMainWindow>
#include <QMdiArea>
#include <QDialog>
#include <QKeyEvent>
#include <QEvent>
#include <QLabel>
#include <QSignalMapper>
#include <QIcon>

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

enum VIEW_MENU_TYPES
  { MENU_TYPE_MAIN
  , MENU_TYPE_TECHNICAL
  , MENU_TYPE_EXPERIMENTAL
  };

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

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:

    MainWindow();
    virtual ~MainWindow();

    void keyPressEvent(QKeyEvent * p_event);
    void message(QString p_string, int p_msec);
    void remove_key_type(int p_index);
    void add_key_types(const QStringList & p_list);

  protected:
    bool event(QEvent * p_event);

  signals:
    void changedMusicFolder(const QString &);
    void zoomInPressed();
    void zoomOutPressed();

  public slots:
    void openFile();
    void openFile(const char *p_file_name);

    void openRecord();
    void openPlayRecord();
    void openRecord(bool p_and_play);
    void closeAllWidgets();
    void closeEvent(QCloseEvent *p_event);
    void menuPreferences();

    void windowMenuAboutToShow();
    void windowMenuActivated();
    void newViewAboutToShow();

    /**
     * Opens a view based on a viewId (which should be #defined).
     * All widgets should be created anonymously - ie no saving pointers!
     *
     * @param p_view_id the view to open's ID.
     */
    QWidget *openView(int p_view_id);
    void setTimeLabel(double p_t);
    void setChunkLabel();
    void setNoteLabel();
    void setTimeRange(double p_min, double p_max);

    void rewindPressed();
    void rewindReleased();
    void playStopClicked();
    void fastforwardPressed();
    void fastforwardReleased();
    void setTitle(Channel *p_channel);
    void aboutTartini();
    void aboutGPL();
    void aboutQt();
    void showDocumentation();
    void printPitch();
    void exportChannel(int p_type, QString p_type_string);
    void exportChannelPlainText();
    void exportChannelMatlab();

    bool loadViewGeometry();
    void saveViewGeometry();

    static
    const ViewData & get_view_data(unsigned int p_index);

  private:
    void init_view_data();

    QMdiArea * m_the_workspace;

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

    static const size_t m_view_number = 19;
    static ViewData m_view_data[m_view_number];

};

extern MainWindow * g_main_window;
class MyGLFont;
extern MyGLFont * g_mygl_font;

#endif // MAINWINDOW_H
// EOF
