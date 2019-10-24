/***************************************************************************
                          savedialog.cpp  -  description
                             -------------------
    begin                : Feb 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

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
SaveDialog::SaveDialog(QWidget * p_parent):
  Q3FileDialog(QDir::convertSeparators(g_data->getSettingsValue("Dialogs/saveFilesFolder", QDir::currentDirPath())),
               tr("Wave files (*.wav)"), p_parent, NULL, true)
{
  setCaption(tr("Choose a filename to save under"));
  setMode(Q3FileDialog::AnyFile);
  
  QWidget *l_base_widget = new QWidget(this);
  addWidgets(NULL, l_base_widget, NULL);
  Q3BoxLayout *l_base_layout = new Q3VBoxLayout(l_base_widget);
    m_append_wav_check_box =      new QCheckBox(tr("Append .wav extension if needed"), l_base_widget);
    m_remember_folder_check_box = new QCheckBox(tr("Remember current folder"), l_base_widget);
  m_append_wav_check_box->setChecked(g_data->getSettingsValue("Dialogs/appendWav", true));
  m_remember_folder_check_box->setChecked(g_data->getSettingsValue("Dialogs/rememberSaveFolder", true));
  l_base_layout->addSpacing(10);
  l_base_layout->addWidget(m_append_wav_check_box);
  l_base_layout->addWidget(m_remember_folder_check_box);
}

//------------------------------------------------------------------------------
SaveDialog::~SaveDialog(void)
{
}

//------------------------------------------------------------------------------
void SaveDialog::accept(void)
{
  bool l_remember = m_remember_folder_check_box->isChecked();
  g_data->setSettingsValue("Dialogs/rememberSaveFolder", l_remember);
  if(l_remember == true) {
    const QDir *l_cur_dir = dir();
    g_data->setSettingsValue("Dialogs/saveFilesFolder", l_cur_dir->absPath());
    delete l_cur_dir;
  }
  bool l_append_wav = m_append_wav_check_box->isChecked();
  g_data->setSettingsValue("Dialogs/appendWav", l_append_wav);
  if(l_append_wav == true) {
    QString l_string = selectedFile();
    if(!l_string.lower().endsWith(".wav")) { l_string += ".wav"; }
    setSelection(l_string);
  }
  Q3FileDialog::accept();
}

//------------------------------------------------------------------------------
QString SaveDialog::getSaveWavFileName(QWidget *p_parent)
{
  SaveDialog l_dialog(p_parent);
  if(l_dialog.exec() != QDialog::Accepted) return QString();
  return l_dialog.selectedFile();
}

// EOF
