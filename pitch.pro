#Tartini 1.1 QMakefile
#Usage:
#[]$ qmake pitch.pro
#[]$ make

#Set the paths to Qt4, fftw and qwt libraries and includes here.
#for you platform. Note multiple paths can be listed.
#Note the -L before the each library paths, but not include paths.

unix{
  macx{ #MacOSX
    QMAKE_INFO_PLIST = mac/Info.plist
    ICON = mac/pitch.icns

    MY_INCLUDE_PATH += /usr/local/Cellar/qwt/6.1.4/lib/qwt.framework/Versions/6/Headers/
    MY_INCLUDE_PATH += /usr/local/include
    LIBS+= -F/usr/local/Cellar/qwt/6.1.4/lib
    LIBS+= -L/usr/local/lib
  }else{ #Linux
    MY_TARTINI_PATH += .

    # QWT 5
    #QWT_LIB_NAME= qwt-qt4
    #MY_QWT_LIB_PATH =
    #MY_QWT_INCLUDE_PATH =  /usr/include/qwt-qt4/
    #end of Qwt5
    # QWT 6
    QWT_LIB_NAME = qwt-qt5
    MY_QWT_INCLUDE_PATH = /usr/include/qwt
    # end of Qwt6

    MY_LIB_PATH += -L$$MY_TARTINI_PATH/lib -L/usr/lib -L/usr/lib/x86_64-linux-gnu/
    MY_INCLUDE_PATH += /usr/include/GL $$MY_TARTINI_PATH/include $$MY_QWT_INCLUDE_PATH
  }
}
win32{ #Windows
  MY_LIB_PATH += -L"C:/home/Qwt/lib" -L"C:/home/fftw"
  MY_INCLUDE_PATH += "./" "C:/home/Qwt/include" "C:/home/fftw"
  #Note just stick the *.dll files in the executables directory to run
}


TEMPLATE = app
CONFIG += qt thread opengl
CONFIG += debug
CONFIG -= release
CONFIG += warn_on
CONFIG += precompile_header
#CONFIG += profile
#DEFINES += SHARED_DRAWING_BUFFER
DEFINES += SINGLE_DRAWING_BUFFER
LANGUAGE = C++
TARGET = tartini
VERSION = 1.2.1

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
  pics/playrecord32x32.xpm \
  pics/save32x32.xpm \
  pics/background.xpm \
  pics/zoomx.xpm \
  pics/zoomy.xpm \
  pics/zoomxout.xpm \
  pics/zoomyout.xpm \
  pics/tartinilogo.jpg \
  gomacx \
  gowindows.bat \
  include/static.h

HEADERS += \
  dialogs/gpldialog.h \
  dialogs/opendialog.h \
  dialogs/savedialog.h \
  dialogs/tartinidialog.h \
  dialogs/tartinisettingsdialog.h \
  general/bspline.h \
  general/equalloudness.h \
  general/fast_smooth.h \
  general/fast_smooth.hpp \
  general/large_vector.h \
  general/large_vector.hpp \
  general/myalgo.h \
  general/mygl.h \
  general/mygl.hpp \
  general/myio.h \
  general/mymatrix.h \
  general/mymatrix.hpp \
  general/myqt.h \
  general/mystring.h \
  general/mytransforms.h \
  general/mytransforms.hpp \
  general/prony.h \
  general/settings.h \
  general/useful.h \
  general/useful.hpp \
  global/conversions.h \
  global/gdata.h \
  global/gdata.hpp \
  global/view.h \
  global/view.hpp \
  include/array1d.h \
  include/array1d.hpp \
  include/array2d.h \
  include/myassert.h \
  include/RingBuffer.h \
  music/music_note.h \
  music/music_note.hpp \
  music/music_scale.h \
  music/music_scale.hpp \
  music/music_temperament.h \
  music/music_temperament.hpp \
  sound/filters/FastSmoothedAveragingFilter.h \
  sound/filters/Filter.h \
  sound/filters/FixedAveragingFilter.h \
  sound/filters/GrowingAveragingFilter.h \
  sound/filters/IIR_Filter.h \
  sound/analysisdata.h \
  sound/analysisdata.hpp \
  sound/audio_stream.h \
  sound/audio_thread.h \
  sound/audio_thread.hpp \
  sound/channel.h \
  sound/channel.hpp \
  sound/notedata.h \
  sound/notedata.hpp \
  sound/sound_file_stream.h \
  sound/sound_file_stream.hpp \
  sound/sound_stream.h \
  sound/sound_stream.hpp \
  sound/soundfile.h \
  sound/soundfile.h \
  sound/soundfile.hpp \
  sound/wave_stream.h \
  sound/zoomlookup.h \
  sound/zoomlookup.hpp \
  widgets/cepstrum/cepstrumview.h \
  widgets/cepstrum/cepstrumwidget.h \
  widgets/correlation/correlationview.h \
  widgets/correlation/correlationwidget.h \
  widgets/debugview/debugview.h \
  widgets/debugview/debugwidget.h \
  widgets/fft/fftview.h \
  widgets/fft/fftwidget.h \
  widgets/freq/amplitudewidget.h \
  widgets/freq/amplitudewidget.hpp \
  widgets/freq/freqview.h \
  widgets/freq/freqwidgetGL.h \
  widgets/freq/freqwidgetGL.hpp \
  widgets/hblock/hblockview.h \
  widgets/hblock/hblockwidget.h \
  widgets/hbubble/hbubbleview.h \
  widgets/hbubble/hbubblewidget.h \
  widgets/hcircle/hcircleview.h \
  widgets/hcircle/hcirclewidget.h \
  widgets/hstack/hstackview.h \
  widgets/hstack/hstackwidget.h \
  widgets/htrack/htrackview.h \
  widgets/htrack/htrackwidget.h \
  widgets/htrack/htrackwidget.hpp \
  widgets/htrack/piano3d.h \
  widgets/htrack/piano3d.hpp \
  widgets/ledindicator.h \
  widgets/mainwindow/mainwindow.h \
  widgets/mainwindow/viewdata.h \
  widgets/openfiles/openfiles.h \
  widgets/piano/pianoview.h \
  widgets/piano/pianowidget.h \
  widgets/piano/pianowidget.hpp \
  widgets/pitchcompass/pitchcompassdrawwidget.h \
  widgets/pitchcompass/pitchcompassview.h \
  widgets/sample/sampleview.h \
  widgets/sample/samplewidget.h \
  widgets/score/scoresegmentiterator.h \
  widgets/score/scoresegmentiterator.hpp \
  widgets/score/scoreview.h \
  widgets/score/scorewidget.h \
  widgets/score/scorewidget.hpp \
  widgets/summary/summarydrawwidget.h \
  widgets/summary/summaryview.h \
  widgets/tuner/tunerview.h \
  widgets/vibrato/vibratocirclewidget.h \
  widgets/vibrato/vibratoperiodwidget.h \
  widgets/vibrato/vibratospeedwidget.h \
  widgets/vibrato/vibratotimeaxis.h \
  widgets/vibrato/vibratotunerwidget.h \
  widgets/vibrato/vibratoview.h \
  widgets/vibrato/vibratowidget.h \
  widgets/volumemeter/volumemeterview.h \
  widgets/volumemeter/volumemeterwidget.h \
  widgets/wave/waveview.h \
  widgets/wave/wavewidget.h \
  widgets/application.h \
  widgets/application.hpp \
  widgets/drawwidget.h \
  widgets/mylabel.h \
  widgets/mylabel.hpp \
  widgets/myscrollbar.h \
  widgets/myscrollbar.hpp \
  widgets/timeaxis.h \
  widgets/timeaxis.hpp \
  widgets/viewwidget.h \
  widgets/widget_utils.h \
  main.h

SOURCES += \
   dialogs/gpldialog.cpp \
   dialogs/opendialog.cpp \
   dialogs/savedialog.cpp \
   dialogs/tartinidialog.cpp \
   dialogs/tartinisettingsdialog.cpp \
   general/bspline.cpp \
   general/equalloudness.cpp \
   general/fast_smooth.cpp \
   general/myalgo.cpp \
   general/myio.cpp \
   general/mymatrix.cpp \
   general/myqt.cpp \
   general/mystring.cpp \
   general/mytransforms.cpp \
   general/prony.cpp \
   general/settings.cpp \
   general/useful.cpp \
   global/conversions.cpp \
   global/gdata.cpp \
   global/view.cpp \
   music/music_note.cpp \
   music/music_scale.cpp \
   music/music_temperament.cpp \
   sound/filters/FastSmoothedAveragingFilter.cpp \
   sound/filters/FixedAveragingFilter.cpp \
   sound/filters/GrowingAveragingFilter.cpp \
   sound/filters/IIR_Filter.cpp \
   sound/analysisdata.cpp \
   sound/audio_stream.cpp \
   sound/audio_thread.cpp \
   sound/channel.cpp \
   sound/notedata.cpp \
   sound/sound_stream.cpp \
   sound/soundfile.cpp \
   sound/wave_stream.cpp \
   sound/zoomlookup.cpp \
   widgets/cepstrum/cepstrumview.cpp \
   widgets/cepstrum/cepstrumwidget.cpp \
   widgets/correlation/correlationview.cpp \
   widgets/correlation/correlationwidget.cpp \
   widgets/debugview/debugview.cpp \
   widgets/debugview/debugwidget.cpp \
   widgets/fft/fftview.cpp \
   widgets/fft/fftwidget.cpp \
   widgets/freq/amplitudewidget.cpp \
   widgets/freq/freqview.cpp \
   widgets/freq/freqwidgetGL.cpp \
   widgets/hblock/hblockview.cpp \
   widgets/hblock/hblockwidget.cpp \
   widgets/hbubble/hbubbleview.cpp \
   widgets/hbubble/hbubblewidget.cpp \
   widgets/hcircle/hcircleview.cpp \
   widgets/hcircle/hcirclewidget.cpp \
   widgets/hstack/hstackview.cpp \
   widgets/hstack/hstackwidget.cpp \
   widgets/htrack/htrackview.cpp \
   widgets/htrack/htrackwidget.cpp \
   widgets/htrack/piano3d.cpp \
   widgets/mainwindow/mainwindow.cpp \
   widgets/mainwindow/viewdata.cpp \
   widgets/openfiles/openfiles.cpp \
   widgets/piano/pianoview.cpp \
   widgets/piano/pianowidget.cpp \
   widgets/pitchcompass/pitchcompassdrawwidget.cpp \
   widgets/pitchcompass/pitchcompassview.cpp \
   widgets/sample/sampleview.cpp \
   widgets/sample/samplewidget.cpp \
   widgets/score/scoresegmentiterator.cpp \
   widgets/score/scoreview.cpp \
   widgets/score/scorewidget.cpp \
   widgets/summary/summarydrawwidget.cpp \
   widgets/summary/summaryview.cpp \
   widgets/tuner/tunerview.cpp \
   widgets/vibrato/vibratocirclewidget.cpp \
   widgets/vibrato/vibratoperiodwidget.cpp \
   widgets/vibrato/vibratospeedwidget.cpp \
   widgets/vibrato/vibratotimeaxis.cpp \
   widgets/vibrato/vibratotunerwidget.cpp \
   widgets/vibrato/vibratoview.cpp \
   widgets/vibrato/vibratowidget.cpp \
   widgets/volumemeter/volumemeterview.cpp \
   widgets/volumemeter/volumemeterwidget.cpp \
   widgets/wave/waveview.cpp \
   widgets/wave/wavewidget.cpp \
   widgets/drawwidget.cpp \
   widgets/ledindicator.cpp \
   widgets/mylabel.cpp \
   widgets/myscrollbar.cpp \
   widgets/timeaxis.cpp \
   widgets/viewwidget.cpp \
   cleanup.c \
   main.cpp

 
RESOURCES += pitch.qrc
PRECOMPILED_HEADER = include/static.h
  
TRANSLATIONS += tartini_de.ts \
                tartini_fr.ts

MYPATHS = include/ general/ sound/ widgets/ global/ dialogs/ music/
MYPATHS += widgets/mainwindow widgets/freq widgets/summary widgets/pitchcompass widgets/openfiles widgets/volumemeter widgets/tuner widgets/hblock widgets/hstack widgets/wave widgets/piano widgets/htrack widgets/correlation widgets/fft widgets/cepstrum widgets/hbubble widgets/hcircle widgets/debugview widgets/score widgets/vibrato sound/filters

INCLUDEPATH += $$MYPATHS
DEPENDPATH += $$MYPATHS

unix{
  debug {
    OBJECTS_DIR=.debug_obj
    MOC_DIR=.debug_moc
  }
  release {
    OBJECTS_DIR=.obj
    MOC_DIR=.moc
  }
  macx{

    INCLUDEPATH += rtAudio/
    DEPENDPATH += rtAudio/
    HEADERS += rtAudio/RtAudio.h
    SOURCES += rtAudio/rtAudio.cpp

    QMAKE_LFLAGS_SONAME = -W1,-install_name,@executable_path/..Frameworks/
    INCLUDEPATH += $$MY_INCLUDE_PATH
    LIBS += -framework CoreAudio $$MY_LIB_PATH -lfftw3f -framework qwt -framework Cocoa
    # -framework CoreFoundation -framework ApplicationServices -framework CoreData -framework System -framework Carbon -framework CoreServices -framework AppKit -framework AGL -framework Quartz -framework Foundation
    DEFINES += MACX
    DEFINES += __MACOSX_CORE__

    # Suppress warnings about Open GL. Apple deprecated OpenGL entirely in macOS 10.14, but they still support OpenGL 4.1 (at least through macOS 10.15).
    DEFINES += GL_SILENCE_DEPRECATION

    #CONFIG += link_prl
    #CONFIG += static
    #REQUIRES  = full-config
    #QMAKE_CXXFLAGS += -fast -mcpu=7450
    #QMAKE_CXXFLAGS_RELEASE -= -Os
    QMAKE_CXXFLAGS_RELEASE += -O2
    #CONFIG += ppc
    QMAKE_CXXFLAGS += -Wdocumentation
  }
  else{ #linux

    #RtAudio sound library routines
    INCLUDEPATH += rtAudio/
    DEPENDPATH += rtAudio/
    HEADERS += rtAudio/RtAudio.h
    SOURCES += rtAudio/rtAudio.cpp
    #DEFINES += __LINUX_OSS__
    DEFINES += __LINUX_ALSA__
    #DEFINES += __LINUX_JACK__  #Uncomment to use Jack. Note untested.

    DEFINES += LINUX
    INCLUDEPATH += $$MY_INCLUDE_PATH
    LIBS += $$MY_LIB_PATH -lfftw3f -l$$QWT_LIB_NAME -lasound -lGLU
#   CONFIG += warn_off
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS -= -g
    profile {
      QMAKE_CXXFLAGS += -pg
      QMAKE_LFLAGS += -pg
    }
    tartini.files=tartini
    tartini.path=/usr/bin
    INSTALLS+=tartini
  }
}
win32{
  INCLUDEPATH += $$MY_INCLUDE_PATH

  #to use windows MM sound libries uncomment the following block
  INCLUDEPATH += "./" windows/
  DEPENDPATH += windows/
  HEADERS += windows/audio_stream.h
  SOURCES += windows/audio_stream.cpp
  LIBS += -lwinmm

  #to use rtAudio uncomment the following block
  #INCLUDEPATH += rtAudio/
  #DEPENDPATH += rtAudio/
  #HEADERS += rtAudio/audio_stream.h rtAudio/RtAudio.h
  #SOURCES += rtAudio/audio_stream.cpp
  ##DEFINES += __WINDOWS_DS__   #untested under MinGW
  #DEFINES += __WINDOWS_ASIO__  #tested successfully. Note requires ASIO sound drivers
  #SOURCES += asio/asio.cpp asio/asiodrivers.cpp asio/asiolist.cpp asio/iasiothiscallresolver.cpp

  DEFINES += WINDOWS
  REQUIRES += thread
  LIBS += $$MY_LIB_PATH -lqwt5 -lfftw3f-3 -lole32
  #DEFINES += QT_DLL QT_THREAD_SUPPORT
  DEFINES -= UNICODE       #I think some things broke without this?
}
debug{
  DEFINES += MYDEBUG
}

#QT +=  opengl qt3support
QT +=  opengl
QT +=  printsupport gui widgets
CONFIG += uic

UI_DIR = dialogs
