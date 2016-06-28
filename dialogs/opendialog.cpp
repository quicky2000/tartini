/***************************************************************************
                          opendialog.cpp  -  description
                             -------------------
    begin                : Feb 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include "opendialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <QVBoxLayout>
#include "gdata.h"

//------------------------------------------------------------------------------
OpenDialog::OpenDialog(QWidget * parent):
  QFileDialog(parent,tr("Open File"),QDir::convertSeparators(gdata->getSettingsValue("Dialogs/openFilesFolder",QDir::currentDirPath())),"Wave files (*.wav)")
{
  setCaption("Choose a file to open");
  setMode(QFileDialog::ExistingFile);
}

//------------------------------------------------------------------------------
OpenDialog::~OpenDialog(void)
{
}

//------------------------------------------------------------------------------
void OpenDialog::accept(void)
{
  QFileDialog::accept();
}

//------------------------------------------------------------------------------
QString OpenDialog::getOpenWavFileName(QWidget *parent)
{
  OpenDialog d(parent);
  if(d.exec() != QDialog::Accepted)
    {
      return QString();
    }
  return d.selectedFile();
}

// EOF
