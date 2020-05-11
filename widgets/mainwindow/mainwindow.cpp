/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <QStatusBar>
#include <QMenuBar>
#include <QSplitter>
#include <QLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QSpinBox>
#include <QPixmap>
#include <QTimer>
#include <QMetaObject>
#include <QSizeGrip>
#include <QFileInfo>
#include <QLabel>
#include <QToolBar>
#include <QToolTip>
#include <QMimeData>
#include <QTextEdit>
#include <QTextStream>
#include <QTextBrowser>
#include <QIcon>
#include <QMdiArea>
#include <QMdiSubWindow>

#include <QAction>
#include <QKeyEvent>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QDesktopServices>

#include "gdata.h"
#include "mainwindow.h"

#include "soundfile.h"
#include "channel.h"
#include "myscrollbar.h"
#include "mylabel.h"
#include "music_scale.h"
#include "music_key.h"
#include "tartinidialog.h"
#include "gpldialog.h"

// Views
#include "openfiles.h"
#include "freqview.h"
#include "summaryview.h"
#include "pitchcompassview.h"
#include "volumemeterview.h"
#include "tunerview.h"
#include "hblockview.h"
#include "hstackview.h"
#include "hbubbleview.h"
#include "hcircleview.h"
#include "waveview.h"
#include "pianoview.h"
#include "htrackview.h"
#include "correlationview.h"
#include "fftview.h"
#include "cepstrumview.h"
#include "debugview.h"
#include "scoreview.h"
#include "vibratoview.h"
#include "musicnotes.h"

#include "savedialog.h"
#include "opendialog.h"
#include "tartinisettingsdialog.h"
#include <QLineEdit>
#include <QMessageBox>
#include <qwt_slider.h>
#include <QLabel>
#include <QWhatsThis>

#include "wave_stream.h"
#ifdef USE_OGG
#include "ogg_stream.h"
#endif // USE_OGG
#ifdef USE_SOX
#include "sox_stream.h"
#endif // USE_SOX

#include <math.h>
#include "useful.h"
#include <string.h>

#include "pics/record32x32.xpm"
#include "pics/open.xpm"
#include "pics/close32x32.xpm"
#include "pics/save32x32.xpm"

#include "pics/beginning32x32.xpm"
#include "pics/rewind32x32.xpm"
#include "pics/play32x32.xpm"
#include "pics/playrecord32x32.xpm"
#include "pics/stop32x32.xpm"
#include "pics/fastforward32x32.xpm"
#include "pics/end32x32.xpm"

#include "pics/autofollow32x32.xpm"
#include "pics/shadingon32x32.xpm"
#include "pics/shadingoff32x32.xpm"

#include "myglfonts.h"

#ifdef MACX
#include "main.h"
#endif // MACX

#include "myassert.h"

MainWindow *g_main_window;
MyGLFont *g_mygl_font;

ViewData MainWindow::m_view_data[MainWindow::m_view_number];

//------------------------------------------------------------------------------
MainWindow::MainWindow()
: QMainWindow(nullptr)
{
    init_view_data();
    setAttribute(Qt::WA_DeleteOnClose);
    m_create_signal_mapper = new QSignalMapper(this);
    connect(m_create_signal_mapper, SIGNAL(mapped(int)), SLOT(openView(int)));

    QFont l_my_font = QFont();
    l_my_font.setPointSize(9);
    l_my_font.setStyleStrategy(QFont::PreferAntialias);
    g_mygl_font = new MyGLFont(l_my_font);

#ifndef WINDOWS
    //for running multi-threaded profiling
    getitimer(ITIMER_PROF, &g_profiler_value);
#endif // WINDOWS
    //6 pixel boarder
    resize(1024 - 6, 768 - 6);

#ifdef MYDEBUG
    setWindowTitle("Tartini (Debug)");
#else // MYDEBUG
    setWindowTitle("Tartini");
#endif // MYDEBUG

    //Create the main Workspace for the view widgets to go in (for MDI)
    m_the_workspace = new QMdiArea(this);
    m_the_workspace->setObjectName("TheWorkspace");
    setCentralWidget( m_the_workspace );
  
    //Create the file Toolbar
    QToolBar * l_file_tool_bar = new QToolBar("File Actions", this);
    addToolBar(Qt::TopToolBarArea, l_file_tool_bar);
    l_file_tool_bar->setIconSize(QSize(32, 32));

    QIcon l_open_icon;
    l_open_icon.addPixmap(QPixmap(iconOpen), QIcon::Normal);
    QAction * l_open_action = new QAction(l_open_icon, tr("&Open"), this);
    l_open_action->setShortcut(tr("Ctrl+O"));
    l_open_action->setWhatsThis(tr("Open a sound from file and process it using the current preferences"));
    l_file_tool_bar->addAction(l_open_action);
    connect(l_open_action, SIGNAL(triggered()), this, SLOT(openFile()));

    QIcon l_save_icon;
    l_save_icon.addPixmap(QPixmap(save32x32_xpm), QIcon::Normal);
    QAction * l_save_action = new QAction(l_save_icon, tr("&Save"), this);
    l_save_action->setShortcut(tr("Ctrl+S"));
    l_save_action->setWhatsThis(tr("Save the active sound to a file"));
    l_file_tool_bar->addAction(l_save_action);
    connect(l_save_action, SIGNAL(triggered()), &GData::getUniqueInstance(), SLOT(saveActiveFile()));

    QIcon l_close_icon;
    l_close_icon.addPixmap(QPixmap(close32x32_xpm), QIcon::Normal);
    QAction * l_close_action = new QAction(l_close_icon, tr("&Close"), this);
    l_close_action->setShortcut(tr("Ctrl+W"));
    l_close_action->setWhatsThis(tr("Close the active sound. If unsaved will ask to save. Note: Asking can be disabled in the preferences"));
    l_file_tool_bar->addAction(l_close_action);
    connect(l_close_action, SIGNAL(triggered()), &GData::getUniqueInstance(), SLOT(closeActiveFile()));

    QAction * l_close_all_action = new QAction(tr("Close All"), this);
    l_close_all_action->setWhatsThis(tr("Close all the sounds. If any sounds are unsaved, it will ask to save. Note: Asking can be disabled in the preferences"));
    connect(l_close_all_action, SIGNAL(triggered()), &GData::getUniqueInstance(), SLOT(closeAllFiles()));
  
    QAction * l_print_action = new QAction(tr("Print"), this);
    l_print_action->setShortcut(tr("Ctrl+P"));
    l_print_action->setWhatsThis(tr("Print the Pitch Contour, fitting the its current view onto a page"));
    connect(l_print_action, SIGNAL(triggered()), this, SLOT(printPitch()));

    //Create the sound Toolbar
    QToolBar * l_sound_tool_bar = new QToolBar("Sound Actions", this);
    addToolBar(Qt::BottomToolBarArea, l_sound_tool_bar);
    l_sound_tool_bar->setIconSize(QSize(32, 32));

    QIcon l_beginning_icon;
    l_beginning_icon.addPixmap(QPixmap(beginning32x32_xpm), QIcon::Normal);
    QAction * l_beginning_action = new QAction(l_beginning_icon,tr("Beginning"),l_sound_tool_bar);
    l_beginning_action->setToolTip(tr("Rewind to the beginning"));
    l_beginning_action->setWhatsThis(tr("Jump to the beginning of the sound"));
    l_sound_tool_bar->addAction(l_beginning_action);
    connect(l_beginning_action, SIGNAL(triggered()), &GData::getUniqueInstance(), SLOT(beginning()));

    m_rewind_timer = new QTimer(this);
    connect(m_rewind_timer, SIGNAL(timeout()), &GData::getUniqueInstance(), SLOT(rewind()));
    QIcon l_rewind_icon;
    l_rewind_icon.addPixmap(QPixmap(rewind32x32_xpm), QIcon::Normal);
    QAction * l_rewind_action = new QAction(l_rewind_icon, tr("Rewind"), l_sound_tool_bar);
    l_rewind_action->setToolTip(tr("Rewind"));
    l_rewind_action->setWhatsThis(tr("Rewind the sound"));
    QToolButton * l_rewind_button = new QToolButton(l_sound_tool_bar);
    l_rewind_button->setDefaultAction(l_rewind_action);
    l_sound_tool_bar->addWidget(l_rewind_button);
    connect(l_rewind_button, SIGNAL(pressed()), this, SLOT(rewindPressed()));
    connect(l_rewind_button, SIGNAL(released()), this, SLOT(rewindReleased()));

    m_play_icon_set = new QIcon();
    m_play_icon_set->addPixmap(QPixmap(play32x32_xpm), QIcon::Normal);
    m_play_record_icon_set = new QIcon();
    m_play_record_icon_set->addPixmap(QPixmap(playrecord32x32_xpm), QIcon::Normal);
    m_stop_icon_set = new QIcon();
    m_stop_icon_set->addPixmap(QPixmap(stop32x32_xpm), QIcon::Normal);
    m_play_stop_action = new QAction(*m_play_icon_set, "&Play", this);
    m_play_stop_action->setShortcut(tr("Space"));
    m_play_stop_action->setWhatsThis(tr("Play/Stop the active sound"));
    l_sound_tool_bar->addAction(m_play_stop_action);
    connect(m_play_stop_action, SIGNAL(triggered()), this, SLOT(playStopClicked()));

    m_fast_forward_timer = new QTimer(this);
    connect(m_fast_forward_timer, SIGNAL(timeout()), &GData::getUniqueInstance(), SLOT(fastforward()));
    QIcon l_fastforward_icon;
    l_fastforward_icon.addPixmap(QPixmap(fastforward32x32_xpm), QIcon::Normal);
    QAction * l_fast_forward_action = new QAction(l_fastforward_icon, tr("Fast-forward"), l_sound_tool_bar);
    l_fast_forward_action->setToolTip(tr("Fast-forward"));
    l_fast_forward_action->setWhatsThis(tr("Fastfoward the sound"));
    QToolButton * l_fast_forward_button = new QToolButton(l_sound_tool_bar);
    l_fast_forward_button->setDefaultAction(l_fast_forward_action);
    l_sound_tool_bar->addWidget(l_fast_forward_button);
    connect(l_fast_forward_button, SIGNAL(pressed()), this, SLOT(fastforwardPressed()));
    connect(l_fast_forward_button, SIGNAL(released()), this, SLOT(fastforwardReleased()));

    QIcon l_end_icon;
    l_end_icon.addPixmap(QPixmap(end32x32_xpm), QIcon::Normal);
    QAction * l_end_action = new QAction(l_end_icon, tr("&End"), this);
    l_end_action->setWhatsThis(tr("Jump to the end of the sound"));
    l_end_action->setShortcut(tr("Ctrl+E"));
    connect(l_end_action, SIGNAL(triggered()), &GData::getUniqueInstance(), SLOT(end()));
    l_sound_tool_bar->addAction(l_end_action);

    //Create the Actions, to be used in Menus and Toolbars
    m_record_icon_set = new QIcon();
    m_record_icon_set->addPixmap(QPixmap(record32x32_xpm), QIcon::Normal);
    m_record_action = new QAction(*m_record_icon_set, tr("&Record"), this);
    m_record_action->setShortcut(tr("Return"));
    m_record_action->setWhatsThis(tr("Record a new sound, using the input device and settings selected in the preferences"));
    l_file_tool_bar->addAction(m_record_action);
    connect(m_record_action, SIGNAL(triggered()), this, SLOT(openRecord()));

    m_play_record_action = new QAction(*m_play_record_icon_set, tr("Play and Record"), this);
    m_play_record_action->setShortcut(tr("Shift+Return"));
    m_play_record_action->setWhatsThis(tr("Play the active sound and record a new one at the same time!"));
    l_file_tool_bar->addAction(m_play_record_action);
    connect(m_play_record_action, SIGNAL(triggered()), this, SLOT(openPlayRecord()));

    QAction * l_quit = new QAction(tr("&Quit"), this);
    l_quit->setShortcut(tr("Ctrl+Q"));
    l_quit->setWhatsThis(tr("Quit the Tartini application"));
    connect(l_quit, SIGNAL(triggered()), this, SLOT( close() ));

    //Create the File Menu
    QMenu * l_file_menu = menuBar()->addMenu(tr("&File"));
    l_file_menu->addAction(l_open_action);
    l_file_menu->addAction(l_save_action);
    l_file_menu->addAction(l_close_action);
    l_file_menu->addAction(l_close_all_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(m_play_stop_action);
    l_file_menu->addAction(m_record_action);
    l_file_menu->addAction(m_play_record_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(l_print_action);
    l_file_menu->addSeparator();
    l_file_menu->addAction(l_quit);

    //Channel Menu
    QAction * l_export_action_1 = new QAction(tr("&Export to plain text"), this);
    connect(l_export_action_1, SIGNAL(triggered()), this, SLOT(exportChannelPlainText()));
    QAction * l_export_action_2 = new QAction(tr("&Export to matlab"), this);
    connect(l_export_action_2, SIGNAL(triggered()), this, SLOT(exportChannelMatlab()));

    QMenu * l_channel_menu = menuBar()->addMenu(tr("C&hannel"));
    l_channel_menu->addAction(l_export_action_1);
    l_channel_menu->addAction(l_export_action_2);

    // Create actions for the new view menu
    m_new_view_menu = menuBar()->addMenu(tr("&Create"));
#ifdef MACX
    // On Mac, Qt's automatic "menu bar merging" feature will not create any menu that is empty.
    // See https://doc.qt.io/qt-5/qmenubar.html#qmenubar-as-a-global-menu-bar
    // So, load the menu with actions now.
    newViewAboutToShow();
#endif // MACX
    connect(m_new_view_menu, SIGNAL(aboutToShow()), this, SLOT(newViewAboutToShow()));

    //Create the Window Menu
    m_window_menu = menuBar()->addMenu(tr("&Windows"));
#ifdef MACX
    windowMenuAboutToShow();
#endif // MACX
    connect(m_window_menu, SIGNAL( aboutToShow() ), this, SLOT( windowMenuAboutToShow() ) );

    //Create the Options Menu
    m_options_menu = menuBar()->addMenu(tr("&Options"));
    m_options_menu->addAction(tr("&Preferences"), this, SLOT(menuPreferences()));

    QAction * l_whats_this = QWhatsThis::createAction(this);
    l_whats_this->setToolTip(tr("What's this?"));
    l_whats_this->setWhatsThis(tr("Click this button, then click something to learn more about it"));

    m_help_menu = menuBar()->addMenu(tr("&Help"));
    m_help_menu->addAction(l_whats_this);
    m_help_menu->addSeparator();
    m_help_menu->addAction(tr("Documentation"), this, SLOT(showDocumentation()));
    m_help_menu->addSeparator();
    m_help_menu->addAction(tr("About Tartini"), this, SLOT(aboutTartini()));
    m_help_menu->addAction(tr("About Qt"), this, SLOT(aboutQt()));
  
    //Create the other toolbar, which contains some option stuff
    QToolBar * l_analysis_tool_bar = new QToolBar(tr("Analysis Toolbar"), this);
    addToolBar(l_analysis_tool_bar);
    l_analysis_tool_bar->setIconSize(QSize(32, 32));

    QIcon l_auto_follow_icon_set;
    l_auto_follow_icon_set.addPixmap(QPixmap(autofollow32x32_xpm), QIcon::Normal);
    QAction * l_auto_follow_action = new QAction(l_auto_follow_icon_set, tr("Auto Follow"),l_analysis_tool_bar);
    l_auto_follow_action->setToolTip(tr("Moves the view up and down automaticlly with the active channel when playing or recording"));
    l_auto_follow_action->setCheckable(true);
    l_auto_follow_action->setChecked(GData::getUniqueInstance().getView().autoFollow());
    QToolButton * l_auto_follow_button = new QToolButton(l_analysis_tool_bar);
    l_auto_follow_button->setDefaultAction(l_auto_follow_action);
    l_auto_follow_button->setAutoRaise(true);
    l_auto_follow_button->setWhatsThis(tr("Scrolls the Pitch Contour view up and down automaticlly with the active channel when playing or recording. Note: Manual scrolling (vertically) will be disabled during this time."));
    l_analysis_tool_bar->addWidget(l_auto_follow_button);
    connect(l_auto_follow_action, SIGNAL(toggled(bool)), &(GData::getUniqueInstance().getView()), SLOT(setAutoFollow(bool)));

    QIcon l_background_shading_icon_set;
    l_background_shading_icon_set.addPixmap(QPixmap(shadingon32x32_xpm), QIcon::Normal, QIcon::On);
    l_background_shading_icon_set.addPixmap(QPixmap(shadingoff32x32_xpm), QIcon::Normal, QIcon::Off);

    QAction * l_background_shading_action = new QAction(l_background_shading_icon_set, "Background Shading",l_analysis_tool_bar);
    l_background_shading_action->setToolTip("Toggle background shading on/off");
    l_background_shading_action->setCheckable(true);
    l_background_shading_action->setChecked(GData::getUniqueInstance().getView().backgroundShading());
    QToolButton * l_background_shading_button = new QToolButton(l_analysis_tool_bar);
    l_background_shading_button->setDefaultAction(l_background_shading_action);
    l_background_shading_button->setAutoRaise(true);
    l_background_shading_button->setWhatsThis(tr("Draw solid color underneath the Pitch Contour, to help you find the line"));
    l_analysis_tool_bar->addWidget(l_background_shading_button);
    connect(l_background_shading_action, SIGNAL(toggled(bool)), &(GData::getUniqueInstance().getView()), SLOT(setBackgroundShading(bool)));

    l_analysis_tool_bar->addAction(l_whats_this);

    View & l_view = GData::getUniqueInstance().getView();
    QToolBar * l_time_bar_dock = new QToolBar(tr("Time-axis Slider"), this);
    addToolBar(Qt::BottomToolBarArea, l_time_bar_dock);
    l_time_bar_dock->setIconSize(QSize(32, 32));

#if QWT_VERSION >= 0x060000
    m_time_slider = new QwtSlider(Qt::Horizontal,l_time_bar_dock);
    m_time_slider->setScalePosition(QwtSlider::NoScale);
    // To define background style for Qwt slider there are some setters to control display of groove and trough
    // m_time_slider->setBackgroundStyle(QwtSlider::BgBoth) ;
    // Ensure that both Groove and Trough are displayed as it was the case with Qwt 5.x
    m_time_slider->setGroove(true);
    m_time_slider->setTrough(true);

    // Due to changes in Qwt desing it seems that the notion of range has been relaced by the notion of Scale defined in qwt_abstract_scale class
    //  m_time_slider->setRange(GData::getUniqueInstance().leftTime(), GData::getUniqueInstance().rightTime(), 1.0/10000.0, 1000);
    m_time_slider->setScale(GData::getUniqueInstance().leftTime(), GData::getUniqueInstance().rightTime());

    // the parameters defining how the slider behave inside the scale is now defined in class qwt_abstract_slider
    double l_range_wdith = ( GData::getUniqueInstance().leftTime() < GData::getUniqueInstance().rightTime() ? GData::getUniqueInstance().rightTime() - GData::getUniqueInstance().leftTime() : GData::getUniqueInstance().leftTime() - GData::getUniqueInstance().rightTime());
    unsigned int l_nb_steps = (static_cast<unsigned int>(l_range_wdith * 10000.0));
    m_time_slider->setTotalSteps(l_nb_steps);
    m_time_slider->setPageSteps(1000);

    // Graphical parameters
    // setThumbWidth and setThumbLength has been replaced by setHandleSize
    m_time_slider->setHandleSize(QSize(60,20));

    // Don't know how to deal with margins in Qwt 6.x

#else // QWT_VERSION >= 0x060000
#if QWT_VERSION >= 0x050000
    m_time_slider = new QwtSlider(l_time_bar_dock, Qt::Horizontal, QwtSlider::NoScale, QwtSlider::BgBoth);
#else // QWT_VERSION >= 0x050000
    m_time_slider = new QwtSlider(l_time_bar_dock, Qt::Horizontal, QwtSlider::None, QwtSlider::BgBoth);
#endif // QWT_VERSION >= 0x050000
    m_time_slider->setThumbWidth(20);
    m_time_slider->setThumbLength(60);
    m_time_slider->setMargins(2, 2);
    m_time_slider->setRange(GData::getUniqueInstance().leftTime(), GData::getUniqueInstance().rightTime(), 1.0/10000.0, 1000);
#endif // QWT_VERSION >= 0x060000
    m_time_slider->setValue(l_view.currentTime());
    m_time_slider->setTracking(true);
    m_time_slider->setBorderWidth(4);
    m_time_slider->setMinimumWidth(200);
    m_time_slider->setWhatsThis("Drag the time slider to move back and forward through the sound file");
    connect(m_time_slider, SIGNAL(sliderMoved(double)), &GData::getUniqueInstance(), SLOT(updateActiveChunkTime(double)));
    // This is no longer needed, since View::setCurrentTime() now calls doUpdate() to update all views (fast and slow ones).
    // connect(m_time_slider, SIGNAL(sliderMoved(double)), &l_view, SLOT(doSlowUpdate()));
    connect(&l_view, SIGNAL(onSlowUpdate(double)), m_time_slider, SLOT(setValue(double)));
    connect(&GData::getUniqueInstance(), SIGNAL(timeRangeChanged(double, double)), this, SLOT(setTimeRange(double, double)));
    l_time_bar_dock->addWidget(m_time_slider);

    QToolBar * l_volume_meter_tool_bar = new QToolBar(tr("Volume Meter"), this);
    addToolBar(l_volume_meter_tool_bar);
    l_volume_meter_tool_bar->setIconSize(QSize(32, 32));
    VolumeMeterView * volumeMeterView = new VolumeMeterView(VIEW_VOLUME_METER, l_volume_meter_tool_bar);
    volumeMeterView->setWhatsThis(tr("Shows the volume (in dB) of the left and right channels of the active sound. Note: If a mono sound the both are the same"));
    l_volume_meter_tool_bar->addWidget(volumeMeterView);


    QToolBar * l_key_tool_bar = new QToolBar(tr("Key Toolbar"), this);
    l_key_tool_bar->setWhatsThis(tr("Used to set which reference lines are drawn in the Pitch Contour View."));
    addToolBar(l_key_tool_bar);
    QLabel * l_key_label = new QLabel(tr("Key"), l_key_tool_bar);
    l_key_tool_bar->addWidget(l_key_label);

    QComboBox * l_key_combo_box = new QComboBox(l_key_tool_bar);
    l_key_combo_box->setWindowTitle(tr("Key"));
    QStringList l_string_list;
    for(int l_j = 0; l_j < NUM_MUSIC_KEYS; l_j++)
    {
        l_string_list << g_music_key_name[l_j].c_str();
    }
    l_key_combo_box->addItems(l_string_list);
    l_key_combo_box->setCurrentIndex(GData::getUniqueInstance().musicKey());
    l_key_tool_bar->addWidget(l_key_combo_box);
    connect(l_key_combo_box, SIGNAL(activated(int)), &GData::getUniqueInstance(), SLOT(setMusicKey(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(musicKeyChanged(int)), l_key_combo_box, SLOT(setCurrentIndex(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(musicKeyChanged(int)), &(GData::getUniqueInstance().getView()), SLOT(doUpdate()));

    m_key_type_combo_box = new QComboBox(l_key_tool_bar);
    m_key_type_combo_box->setWindowTitle(tr("Scale type"));
    l_string_list.clear();
    for(unsigned int l_j = 0; l_j < g_music_scales.size(); l_j++)
    {
        l_string_list << g_music_scales[l_j].name().c_str();
    }
    m_key_type_combo_box->addItems(l_string_list);
    m_key_type_combo_box->setCurrentIndex(GData::getUniqueInstance().musicKeyType());
    l_key_tool_bar->addWidget(m_key_type_combo_box);
    connect(m_key_type_combo_box, SIGNAL(activated(int)), &GData::getUniqueInstance(), SLOT(setMusicKeyType(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(musicKeyTypeChanged(int)), m_key_type_combo_box, SLOT(setCurrentIndex(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(musicKeyTypeChanged(int)), &(GData::getUniqueInstance().getView()), SLOT(doUpdate()));

    QComboBox * l_tempered_combo_box = new QComboBox(l_key_tool_bar);
    l_tempered_combo_box->setWindowTitle(tr("Tempered type"));
    l_string_list.clear();
    for(unsigned int j = 0; j < g_music_keys.size(); j++)
    {
        l_string_list << g_music_keys[j].name().c_str();
    }
    l_tempered_combo_box->addItems(l_string_list);
    l_tempered_combo_box->setCurrentIndex(GData::getUniqueInstance().temperedType());
    l_key_tool_bar->addWidget(l_tempered_combo_box);
    connect(l_tempered_combo_box, SIGNAL(activated(int)), &GData::getUniqueInstance(), SLOT(setTemperedType(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(temperedTypeChanged(int)), l_tempered_combo_box, SLOT(setCurrentIndex(int)));
    connect(&GData::getUniqueInstance(), SIGNAL(temperedTypeChanged(int)), &(GData::getUniqueInstance().getView()), SLOT(doUpdate()));

    QToolBar * l_freq_A_tool_bar = new QToolBar(tr("Frequency Offset Toolbar"), this);
    l_freq_A_tool_bar->setWhatsThis(tr("The frequency of an even-tempered 'A' used for reference lines in the Pitch Contour View. Default 440 Hz."
                                       "Note: For other scales the root note is chosen from the even-tempered scale with that 'A'."
                                      )
                                   );
    addToolBar(l_freq_A_tool_bar);

    QSpinBox * l_freq_A_spin_box = new QSpinBox(l_freq_A_tool_bar);
    l_freq_A_spin_box->setRange(400,600);
    l_freq_A_spin_box->setSingleStep(1);
    l_freq_A_spin_box->setPrefix("A=");
    l_freq_A_spin_box->setSuffix(" Hz");
    l_freq_A_spin_box->setFocusPolicy(Qt::NoFocus);
    l_freq_A_spin_box->setValue(toInt(GData::getUniqueInstance().freqA()));
    l_freq_A_tool_bar->addWidget(l_freq_A_spin_box);
    connect(l_freq_A_spin_box, SIGNAL(valueChanged(int)), &GData::getUniqueInstance(), SLOT(setFreqA(int)));
    connect(l_freq_A_spin_box, SIGNAL(valueChanged(int)), &(GData::getUniqueInstance().getView()), SLOT(doUpdate()));

    m_note_label = new MyLabel("Note: 9999", statusBar(), "notelabel");
    statusBar()->addPermanentWidget(m_note_label, 0);
    setNoteLabel();
    m_note_label->setToolTip("The current note number in the active file");
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setNoteLabel()));


    m_chunk_label = new MyLabel("Chunk: 999999", statusBar(), "chunklabel");
    statusBar()->addPermanentWidget(m_chunk_label, 0);
    setChunkLabel();
    m_chunk_label->setToolTip("The current chunk number in the active file");
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setChunkLabel()));

    m_time_label = new MyLabel("Time: -00:00.000", statusBar(), "timelabel");
    statusBar()->addPermanentWidget(m_time_label, 0);
    setTimeLabel(0);
    m_time_label->setToolTip(tr("The current time positon for all files (mins:sec)"));
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onSlowUpdate(double)), this, SLOT(setTimeLabel(double)));

    statusBar()->showMessage( "Ready", 2000 );
    
    connect(&GData::getUniqueInstance(), SIGNAL(activeChannelChanged(Channel*)), this, SLOT(setTitle(Channel*)));
}

//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete m_rewind_timer;
    delete m_fast_forward_timer;
    delete m_play_record_icon_set;
    delete m_play_icon_set;
    delete m_stop_icon_set;
    delete m_record_icon_set;
}

//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *p_event)
{
    if(GData::getUniqueInstance().closeAllFiles())
    {
        saveViewGeometry();
        p_event->accept();
    }
    else
    {
        p_event->ignore();
    }
}

//------------------------------------------------------------------------------
bool MainWindow::event(QEvent * p_event)
{
    if(p_event->type() == UPDATE_FAST || p_event->type() == UPDATE_SLOW)
    {
        SoundFile * l_sound_file = GData::getUniqueInstance().getAudioThread().curSoundFile();
        if(l_sound_file)
        {
            l_sound_file->lock();
            GData::getUniqueInstance().getView().setCurrentTime(l_sound_file->timeAtCurrentChunk());
            l_sound_file->unlock();
        }
        GData::getUniqueInstance().updateViewLeftRightTimes();
        GData::getUniqueInstance().setNeedUpdate(false);
        return true;
    }
    else if(p_event->type() == SOUND_STARTED)
    {
        m_play_stop_action->setIcon(*m_stop_icon_set);
        m_play_stop_action->setText("Stop");
        m_record_action->setIcon(*m_stop_icon_set);
        m_record_action->setText("Stop");
        m_play_record_action->setIcon(*m_stop_icon_set);
        m_play_record_action->setText("Stop");
    }
    else if(p_event->type() == SOUND_STOPPED)
    {
        m_play_stop_action->setIcon(*m_play_icon_set);
        m_play_stop_action->setText("Play");
        m_record_action->setIcon(*m_record_icon_set);
        m_record_action->setText("Record");
        m_play_record_action->setIcon(*m_play_record_icon_set);
        m_play_record_action->setText("Play and Record");
    }
    else if(p_event->type() == SETTINGS_CHANGED)
    {
        GData::getUniqueInstance().updateQuickRefSettings();
    }
    return QMainWindow::event(p_event);
}

//------------------------------------------------------------------------------
void MainWindow::keyPressEvent (QKeyEvent * p_event)
{
    View & l_view = GData::getUniqueInstance().getView();

    double l_speed = 1;
    if(p_event->QInputEvent::modifiers() & Qt::ShiftModifier)
    {
        l_speed *= 4;
    }
    if(p_event->QInputEvent::modifiers() & Qt::ControlModifier)
    {
        l_speed *= 10;
    }
    double l_new_time;

    switch(p_event->key())
    {
#ifdef MYDEBUG
        case Qt::Key_Escape:
            GData::getUniqueInstance().stopAndWaitAudioThread();
            close();
        break;
#endif // MYDEBUG
        case Qt::Key_Left:
            if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_FORWARD)
            {
                GData::getUniqueInstance().rewind();
            }
            else
            {
                if(GData::getUniqueInstance().getActiveChannel())
                {
                    l_new_time = l_view.currentTime() - GData::getUniqueInstance().getActiveChannel()->timePerChunk() * l_speed;
                }
                else
                {
                    //move 1/5th of a second back
                    l_new_time = l_view.currentTime() - 0.10 * l_speed;
                }
                GData::getUniqueInstance().updateActiveChunkTime(l_new_time);
            }
        break;
        case Qt::Key_Right:
            if(GData::getUniqueInstance().getRunning() == GData::RunningMode::STREAM_FORWARD)
            {
                GData::getUniqueInstance().fastforward();
            }
            else
            {
                if(GData::getUniqueInstance().getActiveChannel())
                {
                    l_new_time = l_view.currentTime() + GData::getUniqueInstance().getActiveChannel()->timePerChunk()*l_speed;
                }
                else
                {
                    //move 1/5th of a second forward
                    l_new_time = l_view.currentTime() + 0.10 * l_speed;
                }
                GData::getUniqueInstance().updateActiveChunkTime(l_new_time);
            }
        break;
        case Qt::Key_Up:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() + 0.2 * l_speed);
            GData::getUniqueInstance().getView().doSlowUpdate();
        break;
        case Qt::Key_Down:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() - 0.2 * l_speed);
            GData::getUniqueInstance().getView().doSlowUpdate();
        break;
        case Qt::Key_PageUp:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() + 1.0 * l_speed);
            GData::getUniqueInstance().getView().doSlowUpdate();
        break;
        case Qt::Key_PageDown:
            //move 1/5 of a semi-tone
            l_view.setViewBottom(l_view.viewBottom() - 1.0 * l_speed);
            GData::getUniqueInstance().getView().doSlowUpdate();
        break;
        case Qt::Key_Home:
            GData::getUniqueInstance().updateActiveChunkTime(GData::getUniqueInstance().leftTime());
        break;
        case Qt::Key_End:
            GData::getUniqueInstance().updateActiveChunkTime(GData::getUniqueInstance().rightTime());
        break;
        case Qt::Key_Comma:
            l_view.setViewOffset(l_view.viewOffset() - l_view.viewWidth()/20.0);
            l_view.doSlowUpdate();
        break;
        case Qt::Key_Period:
            l_view.setViewOffset(l_view.viewOffset() + l_view.viewWidth()/20.0);
            l_view.doSlowUpdate();
        break;
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            emit zoomInPressed();
        break;
        case Qt::Key_Minus:
            emit zoomOutPressed();
        break;
        default:
            p_event->ignore();
    }
}

//------------------------------------------------------------------------------
void MainWindow::openFile()
{
    QString l_last_folder = QDir::toNativeSeparators(QString::fromStdString(GData::getUniqueInstance().getSettingsValue("Dialogs/openFilesFolder", QDir::currentPath().toStdString())));
    QString l_file_name = QFileDialog::getOpenFileName(this, "Open File", l_last_folder, "Sounds (*.wav)");
    if(l_file_name.isEmpty())
    {
        return;
    }
    l_file_name = QDir::toNativeSeparators(l_file_name);
    GData::getUniqueInstance().setSettingsValue("Dialogs/openFilesFolder", l_file_name.toStdString());
    openFile(l_file_name.toStdString().c_str());
}

//------------------------------------------------------------------------------
void MainWindow::openFile(const char *p_file_name)
{
    SoundFile * l_new_sound_file = new SoundFile();
    if(!l_new_sound_file->openRead(p_file_name))
    {
        fprintf(stderr, "Error opening %s\n", p_file_name);
        delete l_new_sound_file;
        return;
    }

#ifdef TIME_PREPROCESS
    QElapsedTimer l_timer;
    l_timer.start();
#endif // TIME_PREPROCESS
    l_new_sound_file->preProcess();
#ifdef TIME_PREPROCESS
    double l_ms_per_chunk = static_cast<double>(l_timer.elapsed()) / l_new_sound_file->totalChunks();
    double l_frames_per_sec = 1000 * l_new_sound_file->totalChunks() * l_new_sound_file->framesPerChunk() / static_cast<double>(l_timer.elapsed());
    std::cout << "Pre-processing took " << l_timer.elapsed() << " ms for " << l_new_sound_file->totalChunks() << " chunks (" <<
        l_ms_per_chunk << " ms per chunk, " << l_frames_per_sec << " frames/sec)" << std::endl;
#endif // TIME_PREPROCESS
    GData::getUniqueInstance().updateViewLeftRightTimes();

    GData::getUniqueInstance().addFileToList(l_new_sound_file);
    GData::getUniqueInstance().setActiveChannel(&(l_new_sound_file->getChannel(0)));
    QApplication::postEvent(g_main_window, new QEvent(static_cast<QEvent::Type>(UPDATE_SLOW)));
    GData::getUniqueInstance().getView().doUpdate();
}

//------------------------------------------------------------------------------
void MainWindow::openRecord()
{
    openRecord(false);
}

//------------------------------------------------------------------------------
void MainWindow::openPlayRecord()
{
    openRecord(true);
}

//------------------------------------------------------------------------------
void MainWindow::openRecord(bool p_and_play)
{
    if(GData::getUniqueInstance().getRunning() != GData::RunningMode::STREAM_STOP)
    {
        GData::getUniqueInstance().stop();
        return;
    }

    SoundFile * l_play_sound_file = (p_and_play) ? GData::getUniqueInstance().getActiveSoundFile() : nullptr;

    int l_rate = GData::getUniqueInstance().getSettingsValue("Sound/sampleRate", 44100);
    std::string l_number_of_channels = GData::getUniqueInstance().getSettingsValue("Sound/numberOfChannels", std::string("mono"));
    int l_channels;
    if(QString::fromStdString(l_number_of_channels).toLower() == "mono")
    {
        l_channels = 1;
    }
    else
    {
        l_channels = 2;
    }
    int l_bits = GData::getUniqueInstance().getSettingsValue("Sound/bitsPerSample", 16);
    int l_window_size = GData::getUniqueInstance().getAnalysisBufferSize(l_rate);
    int l_step_size = GData::getUniqueInstance().getAnalysisStepSize(l_rate);

    //If playing and recording then overide the record settings to the same as the playing file
    if(l_play_sound_file)
    {
        l_rate = l_play_sound_file->rate();
        l_channels = l_play_sound_file->numChannels();
        l_bits = l_play_sound_file->bits();
        l_window_size = l_play_sound_file->bufferSize();
        l_step_size = l_play_sound_file->bufferSize()/2;
    }

    std::string l_temp_file_folder = GData::getUniqueInstance().getSettingsValue("General/tempFilesFolder", QDir::toNativeSeparators(QDir::currentPath()).toStdString());
    QDir l_dir = QDir(QString::fromStdString(l_temp_file_folder));
    QFileInfo l_file_info;
    QString l_file_name;
    bool l_file_exists;
    do
    {
        l_file_exists = false;
        l_file_name = GData::getUniqueInstance().getFilenameString();
        l_file_info.setFile(l_dir, l_file_name);
        if(l_file_info.exists())
        {
            l_file_exists = true;
            int l_file_generating_number = GData::getUniqueInstance().getSettingsValue("General/fileGeneratingNumber", 1);\
            GData::getUniqueInstance().setSettingsValue("General/fileGeneratingNumber", l_file_generating_number + 1);
        }
    }
    while(l_file_exists);

    SoundFile * l_new_sound_file = new SoundFile();
    QString l_new_filename(l_file_info.absoluteFilePath());
    l_new_filename = QDir::toNativeSeparators(l_new_filename);
    if(!l_new_sound_file->openWrite(l_new_filename.toStdString().c_str(), l_rate, l_channels, l_bits, l_window_size, l_step_size))
    {
        delete l_new_sound_file; l_new_sound_file = nullptr;
        return;
    }

    if(!GData::getUniqueInstance().openPlayRecord(l_new_sound_file, l_play_sound_file))
    {
        GData::getUniqueInstance().closeFile(l_new_sound_file, GData::SavingModes::NEVER_SAVE);
        QMessageBox::warning(g_main_window, "Error", QString("Error opening sound device for recording"), QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    GData::getUniqueInstance().addFileToList(l_new_sound_file);
    GData::getUniqueInstance().getView().setCurrentTime(0.0);
    GData::getUniqueInstance().setActiveChannel(&(l_new_sound_file->getChannel(0)));

    int l_file_generating_number = GData::getUniqueInstance().getSettingsValue("General/fileGeneratingNumber", 1);
    GData::getUniqueInstance().setSettingsValue("General/fileGeneratingNumber", l_file_generating_number + 1);
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuAboutToShow()
{
    m_window_menu->clear();

    QList<QMdiSubWindow *> l_windows = m_the_workspace->subWindowList();
    for(int l_i = 0; l_i < int(l_windows.count()); ++l_i )
    {
        QAction * l_action = m_window_menu->addAction(l_windows.at(l_i)->windowTitle(),this, SLOT( windowMenuActivated() ) );
        l_action->setData(l_i);
        l_action->setChecked(m_the_workspace->activeSubWindow() == l_windows.at(l_i));
    }

    m_window_menu->addSeparator();
    QAction * l_cascade = m_window_menu->addAction( "&Cascade", m_the_workspace, SLOT( cascadeSubWindows() ));
    QAction * l_close = m_window_menu->addAction( "Close &All", this, SLOT( closeAllWidgets() ));

    if(l_windows.isEmpty())
    {
        l_cascade->setEnabled(false);
        l_close->setEnabled(false);
    }
}

//------------------------------------------------------------------------------
void MainWindow::windowMenuActivated()
{
    int l_id = static_cast<QAction*>(sender())->data().toInt();
    std::cout << "windowMenuActivated " << l_id << std::endl ;
    QWidget* l_widget = m_the_workspace->subWindowList().at( l_id );
    if( l_widget )
    {
        l_widget->showNormal();
        l_widget->setFocus();
    }
}

//------------------------------------------------------------------------------
void MainWindow::message( QString p_string
                        , int p_msec
                        )
{
    QStatusBar *l_status_bar = statusBar();
    if(l_status_bar)
    {
        l_status_bar->showMessage(p_string, p_msec);
    }
}

//------------------------------------------------------------------------------
void MainWindow::closeAllWidgets()
{
    QList<QMdiSubWindow *> l_opened = m_the_workspace->subWindowList();
    for(QList<QMdiSubWindow *>::iterator l_iterator = l_opened.begin(); l_iterator < l_opened.end(); l_iterator++)
    {
        (*l_iterator)->close();
    }
}

//------------------------------------------------------------------------------
void MainWindow::menuPreferences()
{
    TartiniSettingsDialog *l_settings = new TartiniSettingsDialog(this);
    l_settings->show();
}


//------------------------------------------------------------------------------
QWidget * MainWindow::openView(int p_view_id)
{
    QWidget * l_widget = nullptr;
    int l_use_top_level_widgets = GData::getUniqueInstance().getSettingsValue("Display/useTopLevelWidgets", false);
    QWidget * l_parent = (l_use_top_level_widgets) ? nullptr : m_the_workspace;

    switch(p_view_id)
    {
        case VIEW_OPEN_FILES:
            l_widget = new OpenFiles(p_view_id, l_parent);
        break;
        case VIEW_FREQ:
        {
            FreqView * l_freq_view = new FreqView(p_view_id, l_parent);
            connect(this, SIGNAL(zoomInPressed()), l_freq_view, SLOT(zoomIn()));
            connect(this, SIGNAL(zoomOutPressed()), l_freq_view, SLOT(zoomOut()));
            l_widget = l_freq_view;
        }
        break;
        case VIEW_SUMMARY:
            l_widget = new SummaryView( p_view_id, l_parent);
        break;
        case VIEW_PITCH_COMPASS:
            l_widget = new PitchCompassView(p_view_id, l_parent);
        break;
        case VIEW_VOLUME_METER:
            l_widget = new VolumeMeterView(p_view_id, l_parent);
        break;
        case VIEW_TUNER:
            l_widget = new TunerView(p_view_id, l_parent);
        break;
        case VIEW_HBLOCK:
            l_widget = new HBlockView(p_view_id, l_parent);
        break;
        case VIEW_HSTACK:
            l_widget = new HStackView(p_view_id, l_parent);
        break;
        case VIEW_HBUBBLE:
            l_widget = new HBubbleView(p_view_id, l_parent);
        break;
        case VIEW_HCIRCLE:
            l_widget = new HCircleView(p_view_id, l_parent);
        break;
        case VIEW_WAVE:
            l_widget = new WaveView(p_view_id, l_parent);
        break;
        case VIEW_PIANO:
            l_widget = new PianoView(p_view_id, l_parent);
        break;
        case VIEW_HTRACK:
            l_widget = new HTrackView(p_view_id, l_parent);
        break;
        case VIEW_CORRELATION:
            l_widget = new CorrelationView(p_view_id, l_parent);
        break;
        case VIEW_FFT:
            l_widget = new FFTView(p_view_id, l_parent);
        break;
        case VIEW_CEPSTRUM:
            l_widget = new CepstrumView(p_view_id, l_parent);
        break;
        case VIEW_DEBUGVIEW:
            l_widget = new DebugView(p_view_id, l_parent);
        break;
        case VIEW_SCORE:
            l_widget = new ScoreView(p_view_id, l_parent);
        break;
        case VIEW_VIBRATO:
            l_widget = new VibratoView(p_view_id, l_parent);
        break;
    }
    if(l_widget)
    {
        if(l_parent)
        {
            m_the_workspace->addSubWindow(l_widget);
        }
        l_widget->show();
    }
    return l_widget;
}

//------------------------------------------------------------------------------
void MainWindow::newViewAboutToShow()
{
    m_new_view_menu->clear();

    QMenu * l_technical_menu = new QMenu("Technical");
    QMenu * l_experimental_menu = new QMenu("Experimental");
    QMenu * l_other_menu = new QMenu("Other");

    QList<QMdiSubWindow *> l_opened = m_the_workspace->subWindowList();

    for(unsigned int l_j = 0; l_j < m_view_number; l_j++)
    {
        QAction * l_action;
        switch(m_view_data[l_j].get_menu_type())
        {
            case ViewData::t_view_menu::MENU_TYPE_MAIN:
                l_action = m_new_view_menu->addAction(QString::fromStdString(m_view_data[l_j].get_menu_name()));
                break;
            case ViewData::t_view_menu::MENU_TYPE_TECHNICAL:
                l_action = l_technical_menu->addAction(QString::fromStdString(m_view_data[l_j].get_menu_name()));
                break;
            case ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL:
                l_action = l_experimental_menu->addAction(QString::fromStdString(m_view_data[l_j].get_menu_name()));
                break;
            case ViewData::t_view_menu::MENU_TYPE_OTHERS:
                l_action = l_other_menu->addAction(QString::fromStdString(m_view_data[l_j].get_menu_name()));
                break;
            default:
                throw std::out_of_range("Unknown menu type");
        }

        connect(l_action, SIGNAL(triggered()), m_create_signal_mapper, SLOT(map()));
        m_create_signal_mapper->setMapping(l_action, l_j);
        for(QList<QMdiSubWindow *>::iterator l_iterator=l_opened.begin(); l_iterator<l_opened.end(); l_iterator++)
        {
            const std::string l_widget_class_name = (*l_iterator)->widget()->metaObject()->className();
            const std::string l_view_class_name = m_view_data[l_j].get_class_name();
            if(l_widget_class_name == l_view_class_name)
            {
                l_action->setEnabled(false);
                break;
            }
        }
    }
    m_new_view_menu->addSeparator();
    m_new_view_menu->addMenu(l_technical_menu);
    m_new_view_menu->addMenu(l_experimental_menu);
    m_new_view_menu->addMenu(l_other_menu);
}

//------------------------------------------------------------------------------
void MainWindow::setTimeLabel(double p_t)
{
    char l_temp[128];
    if(m_time_label)
    {
        int l_sign = (p_t < 0) ? -1 : 1;
        p_t = fabs(p_t);
        int l_mins = int(floor(p_t)) / 60;
        int l_secs = int(floor(p_t - double(l_mins * 60)));
        int l_milli_secs = int(floor((p_t - double(l_mins * 60) - double(l_secs))*1000.0));
        sprintf(l_temp, "Time: %s%02d:%02d.%03d", (l_sign == -1) ? "-" : " ", l_mins, l_secs, l_milli_secs);
        m_time_label->setText(l_temp);
    }
}

//------------------------------------------------------------------------------
void MainWindow::setChunkLabel()
{
    char l_temp[128];
    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();
    if(l_active_channel)
    {
        sprintf(l_temp, "Chunk: %d", l_active_channel->currentChunk());
    }
    else
    {
        sprintf(l_temp, "Chunk: 0");
    }
    m_chunk_label->setText(l_temp);
}

//------------------------------------------------------------------------------
void MainWindow::setNoteLabel()
{
    char l_temp[128];
    Channel *l_active_channel = GData::getUniqueInstance().getActiveChannel();
    if(l_active_channel)
    {
        l_active_channel->lock();
        AnalysisData *l_data = l_active_channel->dataAtCurrentChunk();
        if(l_data && l_active_channel->isVisibleNote(l_data->getNoteIndex()) && l_active_channel->isLabelNote(l_data->getNoteIndex()))
        {
            sprintf(l_temp, "Note: %d", l_data->getNoteIndex());
        }
        else
        {
            sprintf(l_temp, "Note:    ");
        }
        l_active_channel->unlock();
    }
    else
    {
        sprintf(l_temp, "Note:    ");
    }
    m_note_label->setText(l_temp);
}

//------------------------------------------------------------------------------
void MainWindow::setTimeRange( double p_min
                             , double p_max
                             )
{
    if(m_time_slider)
    {
#if QWT_VERSION >= 0x060000

        // In Qwt 6.x range has been replaced by scale
        m_time_slider->setScale(p_min, p_max);
        // and steps and pages are managed by qwt_abstract_slider class
        double l_range_wdith = ( p_min < p_max ? p_max - p_min : p_min - p_max);
        unsigned int l_nb_steps = (static_cast<unsigned int>(l_range_wdith * 10000.0));
        m_time_slider->setTotalSteps(l_nb_steps > m_time_slider->totalSteps() ? l_nb_steps : m_time_slider->totalSteps());
        m_time_slider->setPageSteps(1000);
#else // QWT_VERSION >= 0x060000
        m_time_slider->setRange(p_min, p_max, m_time_slider->step(),1000);
#endif // QWT_VERSION >= 0x060000
    }
}

//------------------------------------------------------------------------------
void MainWindow::rewindPressed()
{
    GData::getUniqueInstance().rewind();
    //every 0.2 seconds
    m_rewind_timer->start(GData::getUniqueInstance().fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::rewindReleased()
{
    //every 0.2 seconds
    m_rewind_timer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::playStopClicked()
{
    if(GData::getUniqueInstance().getRunning() != GData::RunningMode::STREAM_STOP)
    {
        GData::getUniqueInstance().stop();
    }
    else
    {
        GData::getUniqueInstance().play();
    }  
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardPressed()
{
    GData::getUniqueInstance().fastforward();
    //every 0.2 seconds
    m_fast_forward_timer->start(GData::getUniqueInstance().fastUpdateSpeed());
}

//------------------------------------------------------------------------------
void MainWindow::fastforwardReleased()
{
    //every 0.2 seconds
    m_fast_forward_timer->stop();
}

//------------------------------------------------------------------------------
void MainWindow::setTitle(Channel *p_channel)
{
#ifdef MYDEBUG
    if(p_channel)
    {
        setWindowTitle(QString("Tartini (debug) - ") + p_channel->getUniqueFilename());
    }
    else
    {
        setWindowTitle(QString("Tartini (debug)"));
    }
#else // MYDEBUG
    if(p_channel)
    {
        setWindowTitle(QString("Tartini - ") + p_channel->getUniqueFilename());
    }
    else
    {
        setWindowTitle(QString("Tartini"));
    }
#endif // MYDEBUG
}

//------------------------------------------------------------------------------
void MainWindow::aboutTartini()
{
    TartiniDialog *l_tartini_dialog = new TartiniDialog(this);
    l_tartini_dialog->exec();
    delete l_tartini_dialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutGPL()
{
    GPLDialog *l_gpl_dialog = new GPLDialog(this);
    l_gpl_dialog->exec();
    delete l_gpl_dialog;
}

//------------------------------------------------------------------------------
void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, "About Qt");
}

//------------------------------------------------------------------------------
void MainWindow::showDocumentation()
{
    QDesktopServices::openUrl(QUrl("http://www.tartini.net/doc"));
}

#include "freqwidgetGL.h"
#include "drawwidget.h"

//------------------------------------------------------------------------------
void MainWindow::printPitch()
{
    QPrinter l_printer(QPrinter::HighResolution);
    l_printer.setOrientation(QPrinter::Landscape);
    QPrintDialog l_print_dialog(&l_printer, this);
    if(l_print_dialog.exec() == QDialog::Accepted)
    {
        View & l_view = GData::getUniqueInstance().getView();
        QPainter l_painter;
        l_painter.begin(&l_printer);
        int l_width = l_printer.width();
        int l_height = l_printer.height();
        double l_left_time = l_view.viewLeft();
        double l_right_time = l_view.viewRight();
        double l_view_bottom = l_view.viewBottom();
        double l_view_top = l_view.viewTop();
        double l_zoom_X = (l_right_time-l_left_time) / double(l_width);
        double l_zoom_Y = (l_view_top-l_view_bottom) / double(l_height);
        FreqWidgetGL::drawReferenceLines(l_printer, l_painter, 0.0, l_zoom_X, l_view_bottom, l_zoom_Y, widget_utils::t_draw_mode::DRAW_VIEW_PRINT);

        double l_dots_per_line_step_Y = 1.0 / l_zoom_Y;
        double l_dots_per_MM = double(l_printer.height()) / double(l_printer.heightMM());
        //1mm thick line
        DrawWidget::setLineWidth(toInt(std::min(l_dots_per_MM * 1.0, l_dots_per_line_step_Y * 0.2)));
        //draw all the visible channels
        for (unsigned int l_i = 0; l_i < GData::getUniqueInstance().getChannelsSize(); l_i++)
        {
            Channel *l_channel = GData::getUniqueInstance().getChannelAt(l_i);
            if(!l_channel->isVisible())
            {
                continue;
            }
            DrawWidget::drawChannel(l_printer, l_channel, l_painter, l_left_time, 0.0, l_zoom_X, l_view_bottom, l_zoom_Y, widget_utils::t_draw_mode::DRAW_VIEW_PRINT);
            if(l_channel == GData::getUniqueInstance().getActiveChannel())
            {
                l_painter.setPen(Qt::black);
                QString l_file_name = l_channel->getUniqueFilename();
                QFont l_font = l_painter.font();
                l_font.setPointSize(18);
                l_painter.setFont(l_font);
                l_painter.drawText(QRect(0, 0, l_printer.width(), l_printer.height()), Qt::AlignHCenter | Qt::AlignTop, l_file_name);
            }
        }
        l_painter.end();
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannel(int p_type, QString p_type_string)
{
    Channel *l_channel = GData::getUniqueInstance().getActiveChannel();
    if(l_channel)
    {
        l_channel->exportChannel(p_type, p_type_string);
    }
    else
    {
        fprintf(stderr, "No channel to export.\n");
    }
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelPlainText()
{
    exportChannel(0, "Text (*.txt)");
}

//------------------------------------------------------------------------------
void MainWindow::exportChannelMatlab()
{
    exportChannel(1, "Matlab code (*.m)");
}

//------------------------------------------------------------------------------
bool MainWindow::loadViewGeometry()
{
    QPoint l_pos;
    QSize l_size;
    int l_counter = 0;

    for(unsigned int l_j = 0; l_j < m_view_number; l_j++)
    {
        QString l_base = QString("geometry/") + QString::fromStdString(m_view_data[l_j].get_class_name());
        QString l_key = l_base+"/visible";
        if(GData::getUniqueInstance().settingsContains(l_key.toStdString()) && GData::getUniqueInstance().getSettingsValue(l_key.toStdString(), false) == true)
        {
            l_pos = GData::getUniqueInstance().getSettingsValue((l_base + "/pos").toStdString(), QPoint(0, 0));
            l_size = GData::getUniqueInstance().getSettingsValue((l_base + "/size").toStdString(), QSize(100, 100));
            QWidget * l_widget = openView(l_j);
            //get the subwindow frame
            QWidget * l_parent_widget = static_cast<QWidget*>(l_widget->parent());
            if(l_parent_widget)
            {
                l_parent_widget->resize(l_size);
                l_parent_widget->move(l_pos);
            }
            l_counter++;
        }
    }
    if(l_counter == 0)
    {
        openView(VIEW_OPEN_FILES);
        openView(VIEW_FREQ);
        openView(VIEW_TUNER);
        openView(VIEW_VIBRATO);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
void MainWindow::saveViewGeometry()
{
    QList<QMdiSubWindow *> l_opened = m_the_workspace->subWindowList();

    for(unsigned int l_j = 0; l_j < m_view_number; l_j++)
    {
        QString l_base = QString("geometry/") + QString::fromStdString(m_view_data[l_j].get_class_name());

        bool l_found = false;
        for(QList<QMdiSubWindow *>::iterator l_iterator = l_opened.begin(); l_iterator < l_opened.end(); l_iterator++)
        {
            const std::string l_widget_class_name = (*l_iterator)->widget()->metaObject()->className();
            const std::string l_view_class_name = m_view_data[l_j].get_class_name();
            if(l_widget_class_name == l_view_class_name)
            {
                GData::getUniqueInstance().setSettingsValue((l_base + "/visible").toStdString(), true);
                GData::getUniqueInstance().setSettingsValue((l_base + "/pos").toStdString(), (*l_iterator)->pos());
                GData::getUniqueInstance().setSettingsValue((l_base + "/size").toStdString(), (*l_iterator)->size());
                l_found = true;
                break;
            }
        }
        if(!l_found)
        {
            GData::getUniqueInstance().setSettingsValue((l_base + "/visible").toStdString(), false);
        }
    }
}

//------------------------------------------------------------------------------
void
MainWindow::init_view_data()
{
    m_view_data[0] = ViewData(QObject::tr("File List").toStdString(),         QObject::tr("&File List").toStdString(),          "OpenFiles",         ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[1] = ViewData(QObject::tr("Pitch Contour").toStdString(),     QObject::tr("&Pitch Contour").toStdString(),      "FreqView",          ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[2] = ViewData(QObject::tr("Chromatic Tuner").toStdString(),   QObject::tr("&Chromatic Tuner").toStdString(),    "TunerView",         ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[3] = ViewData(QObject::tr("Harmonic Track").toStdString(),    QObject::tr("3D Harmonic &Track").toStdString(),  "HTrackView",        ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[4] = ViewData(QObject::tr("Vibrato View").toStdString(),      QObject::tr("V&ibrato View").toStdString(),       "VibratoView",       ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[5] = ViewData(QObject::tr("Musical Score").toStdString(),     QObject::tr("&Musical Score").toStdString(),      "ScoreView",         ViewData::t_view_menu::MENU_TYPE_MAIN);
    m_view_data[6] = ViewData(QObject::tr("Oscilloscope").toStdString(),      QObject::tr("&Oscilloscope").toStdString(),       "WaveView",          ViewData::t_view_menu::MENU_TYPE_TECHNICAL);
    m_view_data[7] = ViewData(QObject::tr("Correlation View").toStdString(),  QObject::tr("Corre&lation View").toStdString(),   "CorrelationView",   ViewData::t_view_menu::MENU_TYPE_TECHNICAL);
    m_view_data[8] = ViewData(QObject::tr("FFT View").toStdString(),          QObject::tr("FF&T View").toStdString(),           "FFTView",           ViewData::t_view_menu::MENU_TYPE_TECHNICAL);
    m_view_data[9] = ViewData(QObject::tr("Cepstrum View").toStdString(),     QObject::tr("C&epstrum View").toStdString(),      "CepstrumView",      ViewData::t_view_menu::MENU_TYPE_TECHNICAL);
    m_view_data[10] = ViewData(QObject::tr("Debug View").toStdString(),        QObject::tr("&Debug View").toStdString(),         "DebugView",        ViewData::t_view_menu::MENU_TYPE_TECHNICAL);
    m_view_data[11] = ViewData(QObject::tr("Harmonic Block").toStdString(),    QObject::tr("Harmonic &Block").toStdString(),     "HBlockView",       ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL);
    m_view_data[12] = ViewData(QObject::tr("Harmonic Stack").toStdString(),    QObject::tr("&Harmonic Stack").toStdString(),     "HStackView",       ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL);
    m_view_data[13] = ViewData(QObject::tr("Harmonic Bubbles").toStdString(),  QObject::tr("H&armonic Bubbles").toStdString(),   "HBubbleView",      ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL);
    m_view_data[14] = ViewData(QObject::tr("Harmonic Circle").toStdString(),   QObject::tr("Ha&rmonic Circle").toStdString(),    "HCircleView",      ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL);
    m_view_data[15] = ViewData(QObject::tr("Pitch Compass").toStdString(),     QObject::tr("Pitch &Compass").toStdString(),      "PitchCompassView", ViewData::t_view_menu::MENU_TYPE_EXPERIMENTAL);
    m_view_data[16] = ViewData(QObject::tr("Piano Keyboard").toStdString(),    QObject::tr("2D Piano &Keyboard").toStdString(),  "PianoView",        ViewData::t_view_menu::MENU_TYPE_OTHERS);
    m_view_data[17] = ViewData(QObject::tr("Summary View").toStdString(),      QObject::tr("&Summary View").toStdString(),       "SummaryView",      ViewData::t_view_menu::MENU_TYPE_OTHERS);
    m_view_data[18] = ViewData(QObject::tr("Volume Meter").toStdString(),      QObject::tr("&Volume Meter").toStdString(),       "VolumeMeterView",  ViewData::t_view_menu::MENU_TYPE_OTHERS);
}

//------------------------------------------------------------------------------
const ViewData &
MainWindow::get_view_data(unsigned int p_index)
{
    assert(p_index < m_view_number);
    return m_view_data[p_index];
}

//------------------------------------------------------------------------------
void
MainWindow::remove_key_type(int p_index)
{
    assert(p_index < m_key_type_combo_box->count());
    m_key_type_combo_box->removeItem(p_index);
}

//------------------------------------------------------------------------------
void
MainWindow::add_key_types(const QStringList & p_list)
{
    m_key_type_combo_box->addItems(p_list);
}

// EOF
