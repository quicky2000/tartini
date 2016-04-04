#QMake file for Tartini
#
#Change the values of MY_LIB_PATH and MY_INCLUDE_PATH for your platform
#to the location of Qt, Qwt and fftw on your system.
#
#Don't forget the -L before each library path name

unix{
  macx{ #MACOSX
    MY_LIB_PATH = -L/Users/student/stuart/usr/local/lib
    MY_INCLUDE_PATH = /Users/student/stuart/usr/local/include
  }
  else{ #LINUX
    MY_LIB_PATH += -L/home/research/pitch/lib
    MY_INCLUDE_PATH += /home/research/pitch/include /home/research/pitch/qwt-4.2.0/include
  }
}
win32{ #WINDOWS
  MY_INCLUDE_PATH += "./"
}




TEMPLATE = app
CONFIG += qt thread opengl
#CONFIG += debug
CONFIG += release
CONFIG += warn_on
#CONFIG += precompile_header
DEFINES += SHARED_DRAWING_BUFFER
LANGUAGE = C++
TARGET = tartini

FORMS += dialogs/settingsdialog.ui 
DISTFILES += INSTALL.txt \
             LICENSE.txt \
             README.txt \
             pics/open.xpm \
             pics/close32x32.xpm \
             pics/beginning32x32.xpm \
             pics/rewind32x32.xpm \
             pics/stop32x32.xpm \
             pics/play32x32.xpm \
             pics/fastforward32x32.xpm \
             pics/end32x32.xpm \
             pics/autofollow32x32.xpm \
             pics/shadingon32x32.xpm \
             pics/shadingoff32x32.xpm \
             pics/chain_open.xpm \
             pics/chain_closed.xpm \
             pics/record32x32.xpm \
             pics/save32x32.xpm \
             pics/background.xpm \
             pics/zoomx.xpm \
             pics/zoomy.xpm \
             pics/zoomxout.xpm \
             pics/zoomyout.xpm \
             pics/tartinilogo.jpg \
             gomacx \
             gowindows.bat

HEADERS += main.h \
           sound/sound_stream.h \
           sound/wave_stream.h \
           sound/sound_file_stream.h \
           sound/audio_thread.h \
           general/myio.h \
           general/useful.h \
           general/mystring.h \
           general/settings.h \
           general/bspline.h \
           include/freqpair.h \
           widgets/ledindicator.h \
           widgets/drawwidget.h \
           widgets/viewwidget.h \
           widgets/myscrollbar.h \
           widgets/mainwindow/mainwindow.h \
           widgets/openfiles/openfiles.h \
           widgets/freq/freqdrawwidget.h \
           widgets/freq/freqview.h \
           widgets/pitchcompass/pitchcompassview.h \
           widgets/pitchcompass/pitchcompassdrawwidget.h \
           widgets/summary/summarydrawwidget.h \
           widgets/summary/summaryview.h \
           widgets/tuner/tunerview.h \
           widgets/tuner/tunerwidget.h \
           widgets/hblock/hblockview.h \
           widgets/hblock/hblockwidget.h \
           widgets/hstack/hstackview.h \
           widgets/hstack/hstackwidget.h \
           widgets/wave/waveview.h \
           widgets/wave/wavewidget.h \
           global/gdata.h \
           sound/filter.h \
           include/array1d.h \
           include/array2d.h \
           sound/channel.h \
           sound/soundfile.h \
           global/view.h \
           include/myqmutex.h \
           general/mytransforms.h \
           widgets/timeaxis.h \
           sound/analysisdata.h \
           widgets/volumemeter/volumemeterview.h \
           widgets/volumemeter/volumemeterwidget.h \
           include/myassert.h \
           sound/zoomlookup.h \
           dialogs/savedialog.h \
           dialogs/opendialog.h \
           general/equalloudness.h \
           widgets/piano/pianowidget.h \
           widgets/freq/amplitudewidget.h \
           widgets/piano/pianoview.h \
           widgets/htrack/htrackview.h \
           widgets/htrack/htrackwidget.h \
           widgets/htrack/piano3d.h \
           widgets/correlation/correlationview.h \
           widgets/correlation/correlationwidget.h \
           widgets/fft/fftwidget.h \
           widgets/fft/fftview.h \
           widgets/cepstrum/cepstrumwidget.h \
           widgets/cepstrum/cepstrumview.h \
           sound/notedata.h \
           general/large_vector.h \
           widgets/mylabel.h \
           dialogs/settingsdialog.ui.h
SOURCES += main.cpp \
           sound/wave_stream.cpp \
           sound/audio_thread.cpp \
           general/myio.cpp \
           sound/filter.cpp \
           sound/channel.cpp \
           sound/soundfile.cpp \
           general/useful.cpp \
           general/mystring.cpp \
           general/settings.cpp \
           general/bspline.cpp \
           widgets/ledindicator.cpp \
           widgets/drawwidget.cpp \
           widgets/myscrollbar.cpp \
           widgets/viewwidget.cpp \
           widgets/mainwindow/mainwindow.cpp \
           widgets/openfiles/openfiles.cpp \
           widgets/freq/freqdrawwidget.cpp \
           widgets/freq/freqview.cpp \
           widgets/pitchcompass/pitchcompassview.cpp \
           widgets/pitchcompass/pitchcompassdrawwidget.cpp \
           widgets/summary/summarydrawwidget.cpp \
           widgets/summary/summaryview.cpp \
           widgets/tuner/tunerview.cpp \
           widgets/tuner/tunerwidget.cpp \
           widgets/hblock/hblockview.cpp \
           widgets/hblock/hblockwidget.cpp \
           widgets/hstack/hstackview.cpp \
           widgets/hstack/hstackwidget.cpp \
           widgets/wave/waveview.cpp \
           widgets/wave/wavewidget.cpp \
           global/gdata.cpp \
           cleanup.c \
           global/view.cpp \
           sound/sound_stream.cpp \
           general/mytransforms.cpp \
           widgets/timeaxis.cpp \
           sound/analysisdata.cpp \
           widgets/volumemeter/volumemeterview.cpp \
           widgets/volumemeter/volumemeterwidget.cpp \
           sound/zoomlookup.cpp \
           dialogs/savedialog.cpp \
           dialogs/opendialog.cpp \
           general/equalloudness.cpp \
           widgets/piano/pianowidget.cpp \
           widgets/freq/amplitudewidget.cpp \
           widgets/piano/pianoview.cpp \
           widgets/htrack/htrackview.cpp \
           widgets/htrack/htrackwidget.cpp \
           widgets/htrack/piano3d.cpp \
           widgets/correlation/correlationview.cpp \
           widgets/correlation/correlationwidget.cpp \
           widgets/fft/fftwidget.cpp \
           widgets/fft/fftview.cpp \
           widgets/cepstrum/cepstrumwidget.cpp \
           widgets/cepstrum/cepstrumview.cpp \
           sound/notedata.cpp \
           widgets/mylabel.cpp 

MYPATHS = include/ general/ sound/ widgets/ global/ dialogs/
MYPATHS += widgets/mainwindow widgets/freq widgets/summary widgets/pitchcompass widgets/openfiles widgets/volumemeter widgets/tuner widgets/hblock widgets/hstack widgets/wave widgets/piano widgets/htrack widgets/correlation widgets/fft widgets/cepstrum

INCLUDEPATH += $$MYPATHS
DEPENDPATH += $$MYPATHS

debug{
  DEFINES += MYDEBUG
}
win32{
  HEADERS += windows/audio_stream.h
  SOURCES += windows/audio_stream.cpp
  INCLUDEPATH += $$MY_INCLUDE_PATH
  INCLUDEPATH += "./" windows/
  DEPENDPATH += windows/
  DEFINES += WINDOWS
  REQUIRES += thread
  LIBS += qwt.lib
  DEFINES += QT_DLL QT_THREAD_SUPPORT
}
unix{
  macx{
    INCLUDEPATH += macx/
    DEPENDPATH += macx/
    QMAKE_LFLAGS_SONAME = -W1,-install_name,@executable_path/..Frameworks/
    INCLUDEPATH += $$MY_INCLUDE_PATH
    HEADERS += macx/audio_stream.h
    SOURCES += macx/audio_stream.cpp
    LIBS += -framework CoreAudio $$MY_LIB_PATH -lfftw3f -lqwt
    RC_FILE = icons/pitch.icns
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
    DEFINES += MACX
    CONFIG += static
  }
  else{
    INCLUDEPATH += unix/
    DEPENDPATH += unix/
    HEADERS += unix/audio_stream.h
    SOURCES += unix/audio_stream.cpp
    DEFINES += LINUX
    INCLUDEPATH += $$MY_INCLUDE_PATH
    LIBS += $$MY_LIB_PATH -lfftw3f -lqwt
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
    precompile_header{
      QMAKE_CXXFLAGS += -include static.h
    }
  }
}
