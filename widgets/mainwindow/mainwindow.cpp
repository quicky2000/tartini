/***************************************************************************
                          mainwindow.cpp  -  description
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

//#include <qapplication.h>
#include <qstatusbar.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qworkspace.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpixmap.h>
//#include <qlistbox.h>
//#include <qpixmap.h>
//#include <qgrid.h>
//#include <qlayout.h>
#include <qtimer.h>
#include <qheader.h>
#include <qmetaobject.h>
#include <qsizegrip.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qmime.h>
#include <qtextedit.h>

#include <qaction.h>

#include "gdata.h"
#include "mainwindow.h"

#include "freqpair.h"
//#include "process.h"
#include "soundfile.h"
#include "channel.h"
#include "myscrollbar.h"
#include "mylabel.h"

// Views
#include "openfiles.h"
#include "freqview.h"
#include "summaryview.h"
#include "pitchcompassview.h"
#include "volumemeterview.h"
#include "tunerview.h"
#include "hblockview.h"
#include "hstackview.h"
#include "waveview.h"
#include "pianoview.h"
#include "htrackview.h"
#include "correlationview.h"
#include "fftview.h"
#include "cepstrumview.h"

#include "savedialog.h"
#include "opendialog.h"
//#include "preferences.h"
#include "settingsdialog.h"
#include <qlineedit.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qwt_slider.h>

#include "wave_stream.h"
#ifdef USE_OGG
#include "ogg_stream.h"
#endif
#ifdef USE_SOX
#include "sox_stream.h"
#endif

#include <math.h>
#include "useful.h"
#include "mystring.h"
//#include <unistd.h>
#include <string.h>

//include the icon .xpm pictures into the program directly
//#include "pics/mic.xpm"
#include "pics/record32x32.xpm"
#include "pics/open.xpm"
#include "pics/close32x32.xpm"
#include "pics/save32x32.xpm"
//#include "pics/pause.xpm"

#include "pics/beginning32x32.xpm"
#include "pics/rewind32x32.xpm"
#include "pics/play32x32.xpm"
#include "pics/stop32x32.xpm"
#include "pics/fastforward32x32.xpm"
#include "pics/end32x32.xpm"

#include "pics/background.xpm"
#include "pics/autofollow32x32.xpm"
#include "pics/shadingon32x32.xpm"
#include "pics/shadingoff32x32.xpm"
//#include "pics/chain_open.xpm"
//#include "pics/chain_closed.xpm"
//#include "pics/tartinilogo.xpm"

#include "myassert.h"

MainWindow *mainWindow;

ViewData viewData[NUM_VIEWS] = { ViewData("File List",       "&File List",       "OpenFiles"),
                                 ViewData("Pitch Contour",   "&Pitch Contour",   "FreqView"),
                                 ViewData("Chromatic Tuner", "&Chromatic Tuner", "TunerView"),
                                 ViewData("Harmonic Track",  "3D Harmonic &Track",  "HTrackView"),
                                 ViewData("Piano Keyboard",  "2D Piano &Keyboard",  "PianoView"),
                                 ViewData("Harmonic Block",  "Harmonic &Block",  "HBlockView"),
                                 ViewData("Pitch Compass",   "Pitch &Compass",   "PitchCompassView"),
                                 ViewData("Summary View",    "&Summary View",    "SummaryView"),
                                 ViewData("Volume Meter",    "&Volume Meter",    "VolumeMeterView"),
                                 //ViewData("Harmonic Stack",  "&Harmonic Stack",  "HStackView"),
                                 ViewData("Oscilloscope",    "&Oscilloscope",    "WaveView"),
                                 ViewData("Correlation View",  "Corre&lation View",  "CorrelationView"),
                                 ViewData("FFT View",  "FF&T View",  "FFTView"),
                                 ViewData("Cepstrum View",  "C&epstrum View",  "CepstrumView")
                               };

MainWindow::MainWindow()
  : QMainWindow( 0, "Tartini", WDestructiveClose )
{
#ifndef WINDOWS
  getitimer(ITIMER_PROF, &profiler_value); //for running multi-threaded profiling
#endif
  //resize(1024+10, 790);
  resize(1024-6, 768-6); //6 pixel boarder
  setUsesBigPixmaps(true);

  //gdata->inputFile = copy_string(inputFile_);
  //if(inputFile_) setCaption(inputFile_);
  //else setCaption("<No File>");
  setCaption("Tartini");

  //QMimeSourceFactory::defaultFactory()->setPixmap("chain_open.png", QPixmap(chain_open_xpm));
  //QMimeSourceFactory::defaultFactory()->setPixmap("chain_closed.png", QPixmap(chain_closed_xpm));
  
  //Create the main Workspace for the view widgets to go in (for MDI)
  theWorkspace = new QWorkspace(this, "TheWorkspace");
  theWorkspace->setPaletteBackgroundPixmap ( QPixmap(background_xpm ));

  
  
/*  
  QPalette thePalette = qApp->palette(this);
  thePalette.setColor(QPalette::Active, QColorGroup::Background, gdata->getBackgroundColor());
  //thePalette.setColor(QPalette::Active, QColorGroup::Background, QColor(210, 218, 232));
  //thePalette.setBrush(QPalette::Active, QColorGroup::Background, QBrush(QColor(255, 120, 64), Qt::CrossPattern));
  //thePalette.setBrush(QPalette::Active, QColorGroup::Background, QBrush(QColor(188, 200, 213), QPixmap(background_xpm)));
  this->setPalette(thePalette);
*/
  
  
  //Create the file Toolbar
  QToolBar *fileToolBar = new QToolBar("File Actions", this, QMainWindow::Top, FALSE);
  
  QAction *openAction = new QAction("Open", QIconSet( QPixmap(iconOpen), QIconSet::Large ), "&Open", CTRL+Key_O, this);
  openAction->addTo(fileToolBar);
  connect(openAction, SIGNAL(activated()), this, SLOT(openFile()));

  QAction *saveAction = new QAction("Save", QIconSet( QPixmap(save32x32_xpm), QIconSet::Large ), "&Save", CTRL+Key_S, this);
  saveAction->addTo(fileToolBar);
  connect(saveAction, SIGNAL(activated()), gdata, SLOT(saveActiveFile()));
  
  QAction *closeAction = new QAction("Close", QIconSet( QPixmap(close32x32_xpm), QIconSet::Large ), "&Close", CTRL+Key_W, this);
  closeAction->addTo(fileToolBar);
  connect(closeAction, SIGNAL(activated()), gdata, SLOT(closeActiveFile()));
  
  QAction *closeAllAction = new QAction("Close All", "Close All", QKeySequence(), this);
  connect(closeAllAction, SIGNAL(activated()), gdata, SLOT(closeAllFiles()));
  
  //Create the sound Toolbar
  QToolBar *soundToolBar = new QToolBar("Sound Actions", this, QMainWindow::Bottom, FALSE);
  
  QToolButton *beginningButton = new QToolButton(QIconSet(QPixmap(beginning32x32_xpm), QIconSet::Small), "Beginning", "Rewind to the beginning", NULL, NULL, soundToolBar, "Beginning");
  //QAction *beginningAction = new QAction("Beginning", QIconSet( QPixmap(beginning32x32_xpm), QIconSet::Small ), "&Beginning", CTRL+Key_B, this);
  connect(beginningButton, SIGNAL(pressed()), gdata, SLOT(beginning()));
  
  rewindTimer = new QTimer(this);
  connect(rewindTimer, SIGNAL(timeout()), gdata, SLOT(rewind()));
  //QAction *rewindAction = new QAction("Rewind", QIconSet( QPixmap(rewind32x32_xpm), QIconSet::Small ), "&Rewind", CTRL+Key_R, this);
  QToolButton *rewindButton = new QToolButton(QIconSet(QPixmap(rewind32x32_xpm), QIconSet::Small), "Rewind", "Rewind", NULL, NULL, soundToolBar, "rewind");
  connect(rewindButton, SIGNAL(pressed()), this, SLOT(rewindPressed()));
  connect(rewindButton, SIGNAL(released()), this, SLOT(rewindReleased()));
  
  //QAction *play = new QAction("Play Sound", QIconSet( QPixmap(iconPlay), QIconSet::Small ), "&Play", CTRL+Key_P, this);
  //QAction *playAction = new QAction("Play", QIconSet( QPixmap(play32x32_xpm), QIconSet::Small ), "&Play", CTRL+Key_P, this);
  playIconSet = new QIconSet(QPixmap(play32x32_xpm), QIconSet::Small);
  stopIconSet = new QIconSet(QPixmap(stop32x32_xpm), QIconSet::Small);
  playStopButton = new QToolButton(*playIconSet, "Play", "Play the active sound", NULL, NULL, soundToolBar, "play");
  connect(playStopButton, SIGNAL(clicked()), this, SLOT(playStopClicked()));

  //QAction *stop = new QAction("Stop", QIconSet( QPixmap(iconStop), QIconSet::Small ), "&Stop", CTRL+Key_S, this);
  //connect(stop, SIGNAL(activated()), gdata, SLOT(stopAllSound()));
  
  fastforwardTimer = new QTimer(this);
  connect(fastforwardTimer, SIGNAL(timeout()), gdata, SLOT(fastforward()));
  //QAction *fastforwardAction = new QAction("Fast-forward", QIconSet( QPixmap(fastforward32x32_xpm), QIconSet::Small ), "&Fast-forward", CTRL+Key_F, this);
  QToolButton *fastforwardButton = new QToolButton(QIconSet(QPixmap(fastforward32x32_xpm), QIconSet::Small), "Fast-forward", "Fast-forward", NULL, NULL, soundToolBar, "fastforward");
  connect(fastforwardButton, SIGNAL(pressed()), this, SLOT(fastforwardPressed()));
  connect(fastforwardButton, SIGNAL(released()), this, SLOT(fastforwardReleased()));

  QAction *endAction = new QAction("End", QIconSet( QPixmap(end32x32_xpm), QIconSet::Small ), "&End", CTRL+Key_E, this);
  connect(endAction, SIGNAL(activated()), gdata, SLOT(end()));
  endAction->addTo(soundToolBar);
  
  //QAction *pause = new QAction("Pause", QIconSet( QPixmap(iconPause), QIconSet::Small ), "Pa&use", CTRL+Key_U, this);
  //connect(pause, SIGNAL(activated()), gdata, SLOT(pauseSound()));

  //Create the Actions, to be used in Menus and Toolbars
  recordIconSet = new QIconSet(QPixmap(record32x32_xpm), QIconSet::Large);
  //QAction *recordAction = new QAction("Record", *recordIconSet, "&Record", CTRL+Key_R, this);
  //connect(recordAction, SIGNAL(activated()), this, SLOT(openRecord()));
  //recordAction->addTo(fileToolBar);
  recordButton = new QToolButton(*recordIconSet, "Record", "Record a new sound", NULL, NULL, fileToolBar, "record");
  connect(recordButton, SIGNAL(clicked()), this, SLOT(openRecord()));
  
  QAction *quit = new QAction("Quit", "&Quit", CTRL+Key_Q, this);
  connect(quit, SIGNAL(activated()), qApp, SLOT( closeAllWindows() ));

  //Create the File Menu
  QPopupMenu *fileMenu = new QPopupMenu( this );
  menuBar()->insertItem( "&File", fileMenu );
  openAction->addTo(fileMenu);
  saveAction->addTo(fileMenu);
  closeAction->addTo(fileMenu);
  closeAllAction->addTo(fileMenu);
  //recordAction->addTo(fileMenu);
  //play->addTo(fileMenu);
  //stop->addTo(fileMenu);
  //pause->addTo(fileMenu);
  fileMenu->insertSeparator();
  quit->addTo(fileMenu);

  // Create actions for the new view menu
  //QAction *openFiles = new QAction("Open Files", "&Open Files", NULL, this);
  //connect(openFiles, SIGNAL(activated()), qApp, SLOT(openView
  
  
  newViewMenu = new QPopupMenu(this);
  menuBar()->insertItem("&Create", newViewMenu);
  connect(newViewMenu, SIGNAL(activated(int)), this, SLOT(openView(int)));
  connect(newViewMenu, SIGNAL(aboutToShow()), this, SLOT(newViewAboutToShow()));

  //Create the Window Menu
  windowMenu = new QPopupMenu( this );
  windowMenu->setCheckable( TRUE );
  connect( windowMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowMenuAboutToShow() ) );
  menuBar()->insertItem( "&Windows", windowMenu );

  //Create the Options Menu
  optionsMenu = new QPopupMenu( this );
  menuBar()->insertItem( "&Options", optionsMenu);
  //optionsMenu->insertItem("Apply &Equal loudness curve", this, SLOT( toggleOption(int) ), 0, 0);
  //optionsMenu->setItemChecked(0, gdata->equalLoudness);
  //optionsMenu->insertItem("Apply &Partial Masking", this, SLOT( toggleOption(int) ), 0, 1);
  //optionsMenu->setItemChecked(1, gdata->useMasking);
  //optionsMenu->insertItem("&Write a Ridge File", this, SLOT( toggleOption(int) ), 0, 2);
  //optionsMenu->setItemChecked(2, gdata->useRidgeFile);
  //optionsMenu->insertItem("Process &Stereo", this, SLOT( toggleOption(int) ), 0, 3);
  //optionsMenu->setItemChecked(3, (gdata->process_channels==2));
  //optionsMenu->insertSeparator();
  optionsMenu->insertItem("&Preferences", this, SLOT(menuPreferences()));

  helpMenu = new QPopupMenu(this);
  menuBar()->insertItem( "&Help", helpMenu);
  helpMenu->insertItem("About Tartini", this, SLOT(aboutTartini()), 0, 0);
  helpMenu->insertItem("About Qt", this, SLOT(aboutQt()), 0, 1);
  
  //Create the other toolbar, which contains some option stuff
  QToolBar *analysisToolBar = new QToolBar("Analysis Toobar", this, QMainWindow::Top, FALSE);
/*
  QCheckBox *autoFollow = new QCheckBox("AutoFollow Active Channel", analysisToolBar);
  //autoFollow->setChecked(gdata->settings.getBool("View", "autoFollow"));
  autoFollow->setChecked(gdata->view->autoFollow());
  connect(autoFollow, SIGNAL(toggled(bool)), gdata->view, SLOT(setAutoFollow(bool)));
*/
  QIconSet *autoFollowIconSet = new QIconSet();
  autoFollowIconSet->setPixmap(QPixmap(autofollow32x32_xpm), QIconSet::Small, QIconSet::Normal);
  QToolButton *autoFollowButton = new QToolButton(*autoFollowIconSet, "Auto Follow", "Moves the view up and down automaticlly with the active channel when playing or recording", NULL, NULL, analysisToolBar, "autoFollow");
  autoFollowButton->setToggleButton(true);
  autoFollowButton->setAutoRaise(true);
  autoFollowButton->setOn(gdata->view->autoFollow());
  connect(autoFollowButton, SIGNAL(toggled(bool)), gdata->view, SLOT(setAutoFollow(bool)));
  
  QIconSet *backgroundShadingIconSet = new QIconSet();
  backgroundShadingIconSet->setPixmap(QPixmap(shadingon32x32_xpm), QIconSet::Small, QIconSet::Normal, QIconSet::On);
  backgroundShadingIconSet->setPixmap(QPixmap(shadingoff32x32_xpm), QIconSet::Small, QIconSet::Normal, QIconSet::Off);
  QToolButton *backgroundShadingButton = new QToolButton(*backgroundShadingIconSet, "Background Shading", "Toggle background shading on/off", NULL, NULL, analysisToolBar, "backgroundShading");
  backgroundShadingButton->setToggleButton(true);
  backgroundShadingButton->setAutoRaise(true);
  backgroundShadingButton->setOn(gdata->view->backgroundShading());
  connect(backgroundShadingButton, SIGNAL(toggled(bool)), gdata->view, SLOT(setBackgroundShading(bool)));
  
    
  View *view = gdata->view;
  QToolBar *timeBarDock = new QToolBar("Time-axis Slider", this, QMainWindow::Bottom);
  timeBarDock->setHorizontallyStretchable(true);

/*
  timeScrollBar = new MyScrollBar(gdata->leftTime(), gdata->rightTime(), 0.1, view->viewWidth(), view->currentTime(), 10000, Qt::Horizontal, timeBarDock);
  timeScrollBar->setMinimumSize(300, 16);
  connect(timeScrollBar, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  connect(timeScrollBar, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(gdata, SIGNAL(timeRangeChanged(double, double)), timeScrollBar, SLOT(setRange(double, double)));
  connect(view, SIGNAL(currentTimeChanged(double)), timeScrollBar, SLOT(setValue(double)));
  connect(view, SIGNAL(viewWidthChanged(double)), timeScrollBar, SLOT(setPageStep(double)));
*/
  
  timeSlider = new QwtSlider(timeBarDock, "time slider", Qt::Horizontal, QwtSlider::None, QwtSlider::BgBoth);
  timeSlider->setRange(gdata->leftTime(), gdata->rightTime(), 1.0/10000.0, 1000);
  timeSlider->setValue(view->currentTime());
  timeSlider->setTracking(true);
  timeSlider->setThumbWidth(20);
  timeSlider->setThumbLength(60);
  timeSlider->setBorderWidth(4);
  timeSlider->setMargins(2, 2);
  //timeSlider->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, 10, 0));
  timeSlider->setMinimumWidth(200);
  //timeSlider->setScaleMaxMinor(4);
  //timeSlider->setScaleMaxMajor(8);
  //timeSlider->scaleDraw()->setMargin(0, 0);
  //connect(timeSlider, SIGNAL(valueChanged(double)), view, SLOT(setCurrentTime(double)));
  connect(timeSlider, SIGNAL(sliderMoved(double)), gdata, SLOT(updateActiveChunkTime(double)));
  connect(timeSlider, SIGNAL(sliderMoved(double)), view, SLOT(doSlowUpdate()));
  //connect(view, SIGNAL(currentTimeChanged(double)), timeSlider, SLOT(setValue(double)));
  connect(view, SIGNAL(onSlowUpdate(double)), timeSlider, SLOT(setValue(double)));
  connect(gdata, SIGNAL(timeRangeChanged(double, double)), this, SLOT(setTimeRange(double, double)));
  //connect(view, SIGNAL(viewWidthChanged(double)), this, SLOT(setPageStep(double)));
  //connect(view, SIGNAL(onSlowUpdate()), timeSlider, SLOT(repaint()));

  
  QToolBar *volumeMeterToolBar = new QToolBar("Volume Meter", this, QMainWindow::Top, FALSE);
  (new VolumeMeterView(VIEW_VOLUME_METER, volumeMeterToolBar, "volumemeterbar"));//->show();

  QFont font("Courier", 12, QFont::Bold);
  
  noteLabel = new MyLabel("Note: 9999", statusBar(), "notelabel");
/*
  noteLabel = new QLabel("", statusBar(), "notelabel");
  noteLabel->setTextFormat(Qt::PlainText);
  noteLabel->setFont(font);
*/
  statusBar()->addWidget(noteLabel, 0, true);
  setNoteLabel();
  QToolTip::add(noteLabel, "The current note number in the active file");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setNoteLabel()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), this, SLOT(setNoteLabel()));

  
  chunkLabel = new MyLabel("Chunk: 999999", statusBar(), "chunklabel");
/*
  chunkLabel = new QLabel("", statusBar(), "chunklabel");
  chunkLabel->setTextFormat(Qt::PlainText);
  chunkLabel->setFont(font);
*/
  statusBar()->addWidget(chunkLabel, 0, true);
  setChunkLabel();
  QToolTip::add(chunkLabel, "The current chunk number in the active file");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setChunkLabel()));
  connect(gdata->view, SIGNAL(onSlowUpdate()), this, SLOT(setChunkLabel()));

  timeLabel = new MyLabel("Time: -00:00.000", statusBar(), "timelabel");
/*
  timeLabel = new QLabel("", statusBar(), "timelabel");
  //timeLabel->setTextFormat(Qt::RichText);
  timeLabel->setTextFormat(Qt::PlainText);
  //timeLabel->setScaledContents(false);
  timeLabel->setFont(font);
*/
  statusBar()->addWidget(timeLabel, 0, true);
  setTimeLabel(0);
  QToolTip::add(timeLabel, "The current time positon for all files (mins:sec)");
  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), this, SLOT(setTimeLabel(double)));
  connect(gdata->view, SIGNAL(onSlowUpdate(double)), this, SLOT(setTimeLabel(double)));

  //connect(gdata->view, SIGNAL(currentTimeChanged(double)), gdata, SLOT(updateActiveChunkTime(double)));

  //open views in reverse for some reason to get the right order ???
  //openView(VIEW_PITCH_COMPASS);
  //openView(VIEW_VOLUME_METER);
  //openView(VIEW_PIANO);
  openView(VIEW_CORRELATION);
  openView(VIEW_WAVE);
  openView(VIEW_FREQ);
  //openView(VIEW_HBLOCK);
  openView(VIEW_TUNER);
  openView(VIEW_OPEN_FILES);
  

  setCentralWidget( theWorkspace );
  statusBar()->message( "Ready", 2000 );

  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(setTitle(Channel*)));
}

MainWindow::~MainWindow()
{
  delete rewindTimer;
  delete fastforwardTimer;
  fprintf(stderr, "Has pending events = %s\n", (qApp->hasPendingEvents()) ? "Yes" : "No");
}

bool MainWindow::close(bool alsoDelete)
{
  if(!gdata->closeAllFiles()) return false;
  return QMainWindow::close(alsoDelete);
}

//void MainWindow::customEvent( QCustomEvent * e )
bool MainWindow::event( QEvent * e )
{
  if(e->type() == UPDATE_FAST || e->type() == UPDATE_SLOW) {
    SoundFile *soundFile = gdata->audioThread.soundFile();
    if(soundFile) {
      soundFile->lock();
      gdata->view->setCurrentTime(soundFile->timeAtCurrentChunk());
      soundFile->unlock();
    }
    gdata->updateViewLeftRightTimes();
/*
    if(e->type() == UPDATE_SLOW) {
      gdata->view->doAutoFollowing();
      gdata->view->doSlowUpdate();
    }
*/
    gdata->need_update = false;
	  return true;
  } else if(e->type() == SOUND_STARTED) {
    playStopButton->setIconSet(*stopIconSet);
    recordButton->setIconSet(*stopIconSet);
  } else if(e->type() == SOUND_STOPPED) {
    playStopButton->setIconSet(*playIconSet);
    recordButton->setIconSet(*recordIconSet);
  } else if(e->type() == SETTINGS_CHANGED) {
    gdata->updateQuickRefSettings();
  }
	return QMainWindow::event(e);
}

void MainWindow::keyPressEvent ( QKeyEvent * e )
{
  View *view = gdata->view;
  
  double speed = 1;
  if(e->state() & Qt::ShiftButton) speed *= 4;
  if(e->state() & Qt::ControlButton) speed *= 10;
  double newTime;
  
  switch(e->key()) {
#ifdef MYDEBUG
  case Qt::Key_Escape:
    gdata->audioThread.stopAndWait();
    qApp->closeAllWindows();
    break;
#endif
  case Qt::Key_Space:
      gdata->pauseSound();
      break;
  case Qt::Key_Left:
    if(gdata->running == STREAM_FORWARD) {
      gdata->rewind();
    } else {
      if(gdata->getActiveChannel())
        newTime = view->currentTime() - gdata->getActiveChannel()->timePerChunk()*speed;
      else
        newTime = view->currentTime() - 0.10*speed;  //move 1/5th of a second back
      gdata->updateActiveChunkTime(newTime);
      //view->setCurrentTime(newTime);
    }
    break;
  case Qt::Key_Right:
    if(gdata->running == STREAM_FORWARD) {
      gdata->fastforward();
    } else {
      if(gdata->getActiveChannel())
        newTime = view->currentTime() + gdata->getActiveChannel()->timePerChunk()*speed;
      else
        newTime = view->currentTime() + 0.10*speed; //move 1/5th of a second forward
      gdata->updateActiveChunkTime(newTime);
    }
    break;
  case Qt::Key_Up:
      view->setViewBottom(view->viewBottom() + 0.2*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_Down:
      view->setViewBottom(view->viewBottom() - 0.2*speed); //move 1/5 of a semi-tone
      gdata->view->doSlowUpdate();
    break;
  case Qt::Key_Home:
      gdata->updateActiveChunkTime(gdata->leftTime());
    break;
  case Qt::Key_End:
      gdata->updateActiveChunkTime(gdata->rightTime());
    break;
  case Qt::Key_Comma:
      view->setViewOffset(view->viewOffset() - view->viewWidth()/20.0);
      view->doSlowUpdate();
      break;
  case Qt::Key_Period:
      view->setViewOffset(view->viewOffset() + view->viewWidth()/20.0);
      view->doSlowUpdate();
      break;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
      emit zoomInPressed();
      //view->viewZoomInX();
      //view->viewZoomInY();
      break;
  case Qt::Key_Minus:
      emit zoomOutPressed();
      //view->viewZoomOutX();
      //view->viewZoomOutY();
      break;
  default:
    e->ignore();
  }
}

void MainWindow::openFile()
{
/*
  OpenDialog d(this);
  if(d.exec() != QDialog::Accepted) return;
  QString s = QString(d.selectedFile());
*/
  QString s = OpenDialog::getOpenWavFileName(this);
  if(s.isEmpty()) return;
  s = QDir::convertSeparators(s);
  openFile(s.latin1());
}

void MainWindow::openFile(const char *filename)
{
  SoundFile *newSoundFile = new SoundFile();
  if(!newSoundFile->openRead(filename)) {
    fprintf(stderr, "Error opening %s\n", filename);
    delete newSoundFile;
    return;
  }
  
  newSoundFile->preProcess();
  gdata->updateViewLeftRightTimes();
  
  gdata->addFileToList(newSoundFile);
  gdata->setActiveChannel(newSoundFile->channels(0));
  //gdata->view->setCurrentTime(0.0);
  QApplication::postEvent(mainWindow, new QCustomEvent(UPDATE_SLOW));
  gdata->view->doFastUpdate();
  gdata->view->doUpdate();
  //playSound();
}

void MainWindow::openRecord()
{
  if(gdata->running) {
    gdata->stop();
    return;
  }
	
  int rate = gdata->settings.getInt("Sound", "sampleRate");
  QString numberOfChannels = gdata->settings.getString("Sound", "numberOfChannels");
  int channels;
  if(numberOfChannels.lower() == "mono") channels = 1;
  else channels = 2;
  int bits = gdata->settings.getInt("Sound", "bitsPerSample");
  int windowSize = gdata->getAnalysisBufferSize(rate);
  int stepSize = gdata->getAnalysisStepSize(rate);
  
  QString tempFileFolder = gdata->settings.getString("General", "tempFilesFolder");
  QDir dir = QDir(tempFileFolder);
  QFileInfo fileInfo; //(dir, fileName);
  QString fileName;
  bool fileExists;
  do {
    fileExists = false;
    fileName = gdata->getFilenameString();
    fileInfo.setFile(dir, fileName);
    if(fileInfo.exists()) {
      fileExists = true;
      int fileGeneratingNumber = gdata->settings.getInt("General", "fileGeneratingNumber");
      gdata->settings.setInt("General", "fileGeneratingNumber", fileGeneratingNumber+1);
    }
      /*
      if(QMessageBox::question(this, tr("Overwrite File?"),
                                //tr("A file called %1 already exists.\n Do you want to overwrite it?").arg( filename ),
                                tr("A file called %1 already exists.\n Do you want to overwrite it?").arg( fileInfo.fileName() ),
                                tr("&Yes"), tr("&No"), QString::null, 0, 1 )) {
          return;
      }
      */
  } while(fileExists);
  
  SoundFile *newSoundFile = new SoundFile();
  QString newFilename(fileInfo.absFilePath().latin1());
//#ifdef WINDOWS
//  newFilename.replace(QChar('/'), QChar('\\'));
//#endif
  newFilename = QDir::convertSeparators(newFilename);
  if(!newSoundFile->openWrite(newFilename, rate, channels, bits, windowSize, stepSize)) {
    fprintf(stderr, "Error opening %s for writing\n", fileInfo.absFilePath().latin1());
    delete newSoundFile;
    return;
  }
  
  if(!gdata->openRecord(newSoundFile)) {
    gdata->closeFile(newSoundFile, false);
    QMessageBox::warning(mainWindow, "Error", QString("Error opening sound device for recording"), QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  gdata->addFileToList(newSoundFile);
  gdata->view->setCurrentTime(0.0);
  gdata->setActiveChannel(newSoundFile->channels(0));

  int fileGeneratingNumber = gdata->settings.getInt("General", "fileGeneratingNumber");
  gdata->settings.setInt("General", "fileGeneratingNumber", fileGeneratingNumber+1);
}

void MainWindow::windowMenuAboutToShow()
{
  windowMenu->clear();

  QWidgetList windows = theWorkspace->windowList();
  for ( int i = 0; i < int(windows.count()); ++i ) {
  	int id = windowMenu->insertItem(windows.at(i)->caption(),
				 this, SLOT( windowMenuActivated( int ) ) );
  	windowMenu->setItemParameter( id, i );
  	windowMenu->setItemChecked( id, theWorkspace->activeWindow() == windows.at(i) );
  }
  
  windowMenu->insertSeparator();
  int cascade = windowMenu->insertItem( "&Cascade", theWorkspace, SLOT( cascade() ), 0 );
  //int tile = windowMenu->insertItem( "&Tile", theWorkspace, SLOT( tile() ), 0 );
  int close = windowMenu->insertItem( "Close &All", this, SLOT( closeAllWidgets() ), 0 );

  if (windows.isEmpty()) {
    windowMenu->setItemEnabled( cascade, FALSE );
  	//windowMenu->setItemEnabled( tile, FALSE );
  	windowMenu->setItemEnabled( close, FALSE );
  }
}

void MainWindow::windowMenuActivated( int id )
{
    QWidget* w = theWorkspace->windowList().at( id );
    if ( w ) {
    	w->showNormal();
      w->setFocus();
    }
}

void MainWindow::message(QString s, int msec)
{
  QStatusBar *sb = statusBar();
  if(sb) sb->message(s, msec);
}

/*
void MainWindow::toggleOption(int id)
{
  if(optionsMenu->isItemChecked(id)) {
    optionsMenu->setItemChecked(id, false);
  } else {
	  optionsMenu->setItemChecked(id, true);
  }
    if(id == 0) gdata->equalLoudness = !(gdata->equalLoudness);
    else if(id == 1) gdata->useMasking = !(gdata->useMasking);
    //else if(id == 2) gdata->useRidgeFile = !(gdata->useRidgeFile);
    else if(id == 3) {
	if(optionsMenu->isItemChecked(id))
	  if(gdata->in_channels < 2) {
	    optionsMenu->setItemChecked(id, false);
		  gdata->process_channels = 1;
	  } else
		  gdata->process_channels = 2;
	else
	  gdata->process_channels = 1;
  }
}
*/

void MainWindow::closeAllWidgets()
{
  QWidgetList opened = theWorkspace->windowList();
  QWidgetListIt it(opened);

  QWidget *widget;
  while ((widget = it.current())) {
    widget->close();
    ++it;
  }
  delete widget;
}

void MainWindow::menuPreferences()
{
	SettingsDialog *settings = new SettingsDialog(this, "Settings", false, Qt::WDestructiveClose);
	settings->show();
}


/**
 * Opens a view based on a viewId (which should be #defined).
 * All widgets should be created anonymously - ie no saving pointers!
 *
 * @param viewID the view to open's ID.
 */
void MainWindow::openView(int viewID)
{
  switch(viewID) {
    case VIEW_OPEN_FILES:
      (new OpenFiles(viewID, theWorkspace))->show();
    break;
    case VIEW_FREQ:
	  {
        FreqView *freqView = new FreqView(viewID, theWorkspace, "freqwidget");
        freqView->show();
        connect(this, SIGNAL(zoomInPressed()), freqView, SLOT(zoomIn()));
        connect(this, SIGNAL(zoomOutPressed()), freqView, SLOT(zoomOut()));
	  }
	  break;
    case VIEW_SUMMARY:
      (new SummaryView( viewID, theWorkspace, "summaryview"))->show();
      break;
   case VIEW_PITCH_COMPASS:
      (new PitchCompassView(viewID, theWorkspace, "pitchcompasswidget"))->show();
      break;
   case VIEW_VOLUME_METER:
      (new VolumeMeterView(viewID, theWorkspace, "volumemeterwidget"))->show();
      break;
   case VIEW_TUNER:
      (new TunerView(viewID, theWorkspace, "tunerwidget"))->show();
      break;
   case VIEW_HBLOCK:
      (new HBlockView(viewID, theWorkspace, "harmonicblockwidget"))->show();
      break;
//   case VIEW_HSTACK:
//      (new HStackView(viewID, theWorkspace, "harmonicstackwidget"))->show();
//      break;
   case VIEW_WAVE:
      (new WaveView(viewID, theWorkspace, "wavewidget"))->show();
      break;
   case VIEW_PIANO:
      (new PianoView(viewID, theWorkspace, "pianowidget"))->show();
      break;
   case VIEW_HTRACK:
      (new HTrackView(viewID, theWorkspace, "htrackwidget"))->show();
      break;
   case VIEW_CORRELATION:
      (new CorrelationView(viewID, theWorkspace, "correlationwidget"))->show();
      break;
   case VIEW_FFT:
      (new FFTView(viewID, theWorkspace, "fftwidget"))->show();
      break;
   case VIEW_CEPSTRUM:
      (new CepstrumView(viewID, theWorkspace, "cepstrumwidget"))->show();
      break;

   default:
    	return;
  }
}



void MainWindow::newViewAboutToShow()
{
  newViewMenu->clear();

  for(int j=0; j < NUM_VIEWS; j++) {
    int id = newViewMenu->insertItem(viewData[j].menuName, j);
    myassert(id == j);
  }

  QWidgetList opened = theWorkspace->windowList();
  QWidgetListIt it(opened);

  QWidget *widget;
  while ((widget = it.current())) {
    for (int i = 0; i < NUM_VIEWS; i++) {
      if (widget->isA(viewData[i].className))
        newViewMenu->setItemEnabled( i, FALSE );
    }
    ++it;
  }
  //delete widget;
}

void MainWindow::setTimeLabel(double t)
{
  char temp[128];
  if(timeLabel) {
    int sign = (t < 0) ? -1 : 1;
    t = fabs(t);
    int mins = int(floor(t)) / 60;
    //double secs = t - double(mins * 60);
    int secs = int(floor(t - double(mins * 60)));
    int milliSecs = int(floor((t - double(mins * 60) - double(secs))*1000.0));
    //sprintf(temp, "<qt><nobr><b><big>Time: %s%02d:%02d</big>.%03d</b></nobr></qt>", (sign == -1) ? "-" : " ", mins, secs, milliSecs);
    sprintf(temp, "Time: %s%02d:%02d.%03d", (sign == -1) ? "-" : " ", mins, secs, milliSecs);
    timeLabel->setText(QString(temp));
  }
}

void MainWindow::setChunkLabel()
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active) {
    sprintf(temp, "Chunk: %d", active->currentChunk());
  } else {
    sprintf(temp, "Chunk: 0");
  }
  chunkLabel->setText(QString(temp));
}

void MainWindow::setNoteLabel()
{
  char temp[128];
  Channel *active = gdata->getActiveChannel();
  if(active) {
    active->lock();
    AnalysisData *data = active->dataAtCurrentChunk();
    if(data && active->isVisibleNote(data->noteIndex) && active->isLabelNote(data->noteIndex)) sprintf(temp, "Note: %d", data->noteIndex);
    else sprintf(temp, "Note:    ");
    active->unlock();
  } else {
    sprintf(temp, "Note:    ");
  }
  noteLabel->setText(QString(temp));
}

void MainWindow::setTimeRange(double min_, double max_)
{
  //if(timeSlider) timeSlider->setRange(min_, max_, timeSlider->step(), timeSlider->pageSize());
  if(timeSlider) timeSlider->setRange(min_, max_, timeSlider->step(), 1000);
}

/*
void MainWindow::setPageStep(double step_)
{
  if(timeSlider) {
    //timeSlider->setRange(timeSlider->minValue(), timeSlider->maxValue(), timeSlider->step(), toInt(step_ / timeSlider->step() / 4.0));
    //timeSlider->setRange(timeSlider->minValue(), timeSlider->maxValue(), timeSlider->step(), 100);
  }
}
*/

void MainWindow::rewindPressed()
{
  gdata->rewind();
  rewindTimer->start(gdata->fastUpdateSpeed()); //every 0.2 seconds
}

void MainWindow::rewindReleased()
{
  rewindTimer->stop(); //every 0.2 seconds
}

void MainWindow::playStopClicked()
{
  if(gdata->running) {
    gdata->stop();
    //playStopButton->setIconSet(*playIconSet);
  } else {
    if(gdata->play()) {
      playStopButton->setIconSet(*stopIconSet);
    }
  }  
}

void MainWindow::fastforwardPressed()
{
  gdata->fastforward();
  fastforwardTimer->start(gdata->fastUpdateSpeed()); //every 0.2 seconds
}

void MainWindow::fastforwardReleased()
{
  fastforwardTimer->stop(); //every 0.2 seconds
}

void MainWindow::setTitle(Channel *ch)
{
  if(ch) setCaption(QString("Tartini - ") + ch->getUniqueFilename());
  else setCaption(QString("Tartini"));
}

void MainWindow::aboutTartini()
{
  TartiniDialog *tartiniDialog = new TartiniDialog(this);
  tartiniDialog->exec();
  delete tartiniDialog;
}

void MainWindow::aboutQt()
{
  QMessageBox::aboutQt(this, "About Qt");
}

TartiniDialog::TartiniDialog(QWidget *parent) : QDialog(parent)
{
  setCaption("About Tartini - Version " TARTINI_VERSION_STR);
  setBackgroundColor(Qt::black);
  QBoxLayout *layout = new QVBoxLayout(this, 6, 6);

  QBoxLayout *topLayout = new QHBoxLayout(layout);
  QLabel *tartiniPicture = new QLabel(this, "TartiniPicture");
  QPixmap *tartiniPixmap = new QPixmap();
  tartiniPixmap->load("pics/tartinilogo.jpg");
  tartiniPicture->setPixmap(*tartiniPixmap);
  delete tartiniPixmap;
  //tartiniPicture->setFrameStyle(QFrame::NoFrame);
  tartiniPicture->setBackgroundColor(Qt::black);
  topLayout->addStretch(10);
  topLayout->addWidget(tartiniPicture);
  topLayout->addStretch(10);
        
  QTextEdit *tartiniTextEdit = new QTextEdit(this, "TartiniTextEdit");
  tartiniTextEdit->setPaper(Qt::black);
  tartiniTextEdit->setReadOnly(true);
  tartiniTextEdit->setTextFormat(Qt::RichText);
  //tartiniTextEdit->setColor(Qt::white);
  //tartiniTextEdit->setPointSize(6);
  tartiniTextEdit->setText(
    "<qt text=\"white\"><font size=-1>Tartini is a graphical tool for analysing the music of solo instruments.<br>"
    "This program was created by Philip McLeod as part of PhD work at the University of Otago, New Zealand<br>"
    "You can find the latest info about Tartini at <b><u>http://www.tartini.net</u></b><br><br>"
    "Copyright 2002-2005 Philip McLeod (pmcleod@cs.otago.ac.nz).<br><br>"
    "Thanks to:-<br>"
    "Prof. Geoff Wyvill - PhD Supervisor (Computer Science)<br>"
    "Dr. Donald Warrington - PhD Supervisor (Physics)<br>"
    "Stuart Miller - Programming<br>"
    "Mr Kevin Lefohn - Support (Violin)<br>"
    "Miss Judy Bellingham - Support (Voice)<br>"
    "Jean-philippe Grenier - Logo design<br>"
    "People at my lab, especially Alexis Angelidis, Sui-Ling Ming-Wong, Brendan McCane<br><br>"
    "Tartini version " TARTINI_VERSION_STR " is released under the GPL license <b><u>http://www.gnu.org/licenses/gpl.html</u></b>.<br>"
    "Libraries used by tartini include:<br>"
    "Qt - <b><u>http://www.trolltech.com/qt</u></b><br>"
    "FFTW - <b><u>http://www.fftw.org</u></b><br>"
    "Qwt - <b><u>http://qwt.sourceforge.net</u></b><br>"
    "<br>"
    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.<br><br>"
    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.<br><br>"
    "You should have received a copy of the GNU General Public License "
    "along with this program; if not, write to the Free Software "
    "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.<br><br>"
    "Please read LICENSE.txt for details.<br></qt>");

    
  layout->addWidget(tartiniTextEdit);
  
  QBoxLayout *bottomLayout = new QHBoxLayout(layout);
  QPushButton *okButton = new QPushButton("&Ok", this);
  okButton->setBackgroundColor(Qt::black);
  bottomLayout->addStretch(10);
  bottomLayout->addWidget(okButton);
  bottomLayout->addStretch(10);
  setFocusProxy(okButton);
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}
