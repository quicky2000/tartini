/***************************************************************************
                          main.cpp  -  description
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

#include "main.h"
#include "application.h"

#ifdef MACX
#include <CoreFoundation/CoreFoundation.h>
QString macxPathString;
#endif

#include "gdata.h"
#include "myassert.h"
#include "myglfonts.h"

int main( int argc, char **argv )
{

#ifdef WINDOWS
  freopen("stdout.txt", "w", stdout);
  freopen("stderr.txt", "w", stderr);
#endif
	
#ifdef MACX  
  CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, kCFURLPOSIXPathStyle);
  const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  macxPathString = QString(pathPtr);
  if(!macxPathString.endsWith("/")) macxPathString.append('/');
  CFRelease(pluginRef);
  CFRelease(macPath);
#endif
	
  application a( argc, argv );
  QString locale = QLocale::system().name();

  QTranslator translator;
  translator.load(QString("tartini_") + locale);
  a.installTranslator(&translator);

  Q_INIT_RESOURCE(pitch);

  fprintf(stderr, "QT_VERSION_STR=%s\n", QT_VERSION_STR);
  fprintf(stderr, "QT_VERSION=%x\n", QT_VERSION);

  //Create one instance only of the global data
  gdata = new GData();

  /*
    The view needs to be created here, not in GData's constructor because of the View's
    autofollow method and the signals in the View class. It can try to reference the GData 
    object before the constructor finishes, which causes an access violation in 
    Visual Studio 6.
  */
  gdata->setView(*(new View()));

  mainWindow = new MainWindow();

  //call init after we know the windows size
  gdata->getView().init();

  mainWindow->showMaximized();
    
  a.setMainWidget(mainWindow);
  mainWindow->show();

  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );

  if(!mainWindow->loadViewGeometry())
    {
      mainWindow->aboutTartini(); //open the tartini dialog on first time open
    }

  //open any files on the command line
  if(argc >= 2)
    {
      for(int j=1; j<argc; j++)
	{
	  mainWindow->openFile(argv[j]);
	}
    }
  
  int ret = a.exec();

  delete & (gdata->getView());
  delete gdata;
  
  return ret;
}
//EOF
