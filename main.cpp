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
#endif // MACX

#include "gdata.h"
#include "myassert.h"

int main( int p_argc
        , char ** p_argv
        )
{

#ifdef WINDOWS
    freopen("stdout.txt", "w", stdout);
    freopen("stderr.txt", "w", stderr);
#endif // WINDOWS
	
#ifdef MACX
    CFURLRef pluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFStringRef macPath = CFURLCopyFileSystemPath(pluginRef, kCFURLPOSIXPathStyle);
    const char *pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
    macxPathString = QString(pathPtr);
    if(!macxPathString.endsWith("/"))
    {
        macxPathString.append('/');
    }
    CFRelease(pluginRef);
    CFRelease(macPath);
#endif // MACX
	
    application l_app(p_argc, p_argv);
    QString l_locale = QLocale::system().name();

    QTranslator l_translator;
    l_translator.load(QString("tartini_") + l_locale);
    l_app.installTranslator(&l_translator);

    Q_INIT_RESOURCE(pitch);

    std::cerr << "QT_VERSION_STR=\"" << QT_VERSION_STR << "\"" << std::endl;
    std::cerr << "QT_VERSION=" << std::hex << QT_VERSION << std::dec << std::endl;
    std::cerr << "QWT_VERSION_STR=\"" << QWT_VERSION_STR << "\"" << std::endl;
    std::cerr << "QWT_VERSION=" << std::hex << QWT_VERSION << std::dec << std::endl;

    //Create one instance only of the global data
   GData::createUniqueInstance();

    g_main_window = new MainWindow();

    //call init after we know the windows size
    GData::getUniqueInstance().getView().init();

    g_main_window->showMaximized();
    
    g_main_window->show();

    l_app.connect(&l_app, SIGNAL(lastWindowClosed()), &l_app, SLOT(quit()));

    if(!g_main_window->loadViewGeometry())
    {
        g_main_window->aboutTartini(); //open the tartini dialog on first time open
    }

    //open any files on the command line
    if(p_argc >= 2)
    {
        for(int l_index = 1; l_index < p_argc; l_index++)
        {
            g_main_window->openFile(p_argv[l_index]);
        }
    }
  
    int l_ret_value = l_app.exec();

    GData::deleteUniqueInstance();
  
    return l_ret_value;
}
//EOF
