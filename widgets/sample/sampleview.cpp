/***************************************************************************
                          sampleview.cpp  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

/** To create a new tartini widget
    1. Copy the sample folder (and files)
    2. Rename the sample* files to myclass* files, e.g. mv sampleview.cpp myclassview.cpp
    3. Change the class names to MyClassView & MyClassWidget (where MyClass is the name of your new class)
    4. In MainWindow.h increment NUM_VIEWS and add to the enum VIEW_MYCLASS
    5. Add a ViewData item to viewData in MainWindow.cpp
    6. Add #include "myclassview.h" near top of MainWindow.cpp
    7. In MainWindow::openView(int viewID) add a case to create your class
    8. Add your files names and folder names into the pitch.pro (HEADERS, SOURCES and MYPATHS respectivly)
    9. Done. Just rebuild the project (regenerating the Makefile)
*/

#include "sampleview.h"
#include "samplewidget.h"
#include "gdata.h"

//------------------------------------------------------------------------------
SampleView::SampleView( int p_view_id
                  , QWidget * p_parent
                  )
: ViewWidget( p_view_id, p_parent)
, m_sample_widget(new SampleWidget(this))
{
    m_sample_widget->show();

    //make any connections
    connect(&(GData::getUniqueInstance().getView()), SIGNAL(onFastUpdate(double)), m_sample_widget, SLOT(update()));
}

//------------------------------------------------------------------------------
SampleView::~SampleView()
{
    delete m_sample_widget;
}

//------------------------------------------------------------------------------
void SampleView::resizeEvent(QResizeEvent *)
{
    m_sample_widget->resize(size());
}

//------------------------------------------------------------------------------
QSize SampleView::sizeHint() const
{
    return QSize(300, 200);
}

// EOF
