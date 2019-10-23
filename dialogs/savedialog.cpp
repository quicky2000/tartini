/***************************************************************************
                          savedialog.cpp  -  description
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

#include "savedialog.h"
#include <qcheckbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <Q3VBoxLayout>
#include "gdata.h"

//------------------------------------------------------------------------------
SaveDialog::SaveDialog(QWidget * parent):
  Q3FileDialog(QDir::convertSeparators(g_data->getSettingsValue("Dialogs/saveFilesFolder", QDir::currentDirPath())),
               tr("Wave files (*.wav)"), parent, NULL, true)
{
  setCaption(tr("Choose a filename to save under"));
  setMode(Q3FileDialog::AnyFile);
  
  QWidget *baseWidget = new QWidget(this);
  addWidgets(NULL, baseWidget, NULL);
  Q3BoxLayout *baseLayout = new Q3VBoxLayout(baseWidget);
  appendWavCheckBox =      new QCheckBox(tr("Append .wav extension if needed"), baseWidget);
  rememberFolderCheckBox = new QCheckBox(tr("Remember current folder"), baseWidget);
  appendWavCheckBox->setChecked(g_data->getSettingsValue("Dialogs/appendWav", true));
  rememberFolderCheckBox->setChecked(g_data->getSettingsValue("Dialogs/rememberSaveFolder", true));
  baseLayout->addSpacing(10);
  baseLayout->addWidget(appendWavCheckBox);
  baseLayout->addWidget(rememberFolderCheckBox);
}

//------------------------------------------------------------------------------
SaveDialog::~SaveDialog(void)
{
}

//------------------------------------------------------------------------------
void SaveDialog::accept(void)
{
  bool remember = rememberFolderCheckBox->isChecked();
  g_data->setSettingsValue("Dialogs/rememberSaveFolder", remember);
  if(remember == true) {
    const QDir *curDir = dir();
    g_data->setSettingsValue("Dialogs/saveFilesFolder", curDir->absPath());
    delete curDir;
  }
  bool appendWav = appendWavCheckBox->isChecked();
  g_data->setSettingsValue("Dialogs/appendWav", appendWav);
  if(appendWav == true) {
    QString s = selectedFile();
    if(!s.lower().endsWith(".wav")) { s += ".wav"; }
    setSelection(s);
  }
  Q3FileDialog::accept();
}

//------------------------------------------------------------------------------
QString SaveDialog::getSaveWavFileName(QWidget *parent)
{
  SaveDialog d(parent);
  if(d.exec() != QDialog::Accepted) return QString();
  return d.selectedFile();
}

// EOF
