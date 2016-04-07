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
#include <QVBoxLayout>
#include "gdata.h"

//------------------------------------------------------------------------------
SaveDialog::SaveDialog(QWidget * p_parent)
: QFileDialog( p_parent
             , tr("Save file")
             , QDir::convertSeparators( g_data->getSettingsValue( "Dialogs/saveFilesFolder"
                                                                , QDir::currentDirPath()
                                                                )
                                      )
             ,tr("Wave files (*.wav)")
)
{
    setCaption(tr("Choose a filename to save under"));
    setAcceptMode(QFileDialog::AcceptSave);
    setFileMode(QFileDialog::AnyFile);
    setModal(true);
    setOption(QFileDialog::DontUseNativeDialog,true);
    QLayout * l_layout = this->layout();

    QVBoxLayout * l_base_layout = new QVBoxLayout();
    l_layout->addItem(l_base_layout);

    m_append_wav_check_box = new QCheckBox(tr("Append .wav extension if needed"), this);
    m_append_wav_check_box->setChecked(g_data->getSettingsValue("Dialogs/appendWav", true));

    m_remember_folder_check_box = new QCheckBox(tr("Remember current folder"), this);
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
    if(l_remember == true)
    {
        QDir l_current_dir = directory();
        g_data->setSettingsValue("Dialogs/saveFilesFolder", l_current_dir.absPath());
    }
    bool l_append_wav = m_append_wav_check_box->isChecked();
    g_data->setSettingsValue("Dialogs/appendWav", l_append_wav);
    if(l_append_wav == true)
    {
        QString l_selected_file = selectedFile();
        if(!l_selected_file.lower().endsWith(".wav"))
        {
            l_selected_file += ".wav";
        }
        selectFile(l_selected_file);
    }
    QFileDialog::accept();
}

//------------------------------------------------------------------------------
QString SaveDialog::getSaveWavFileName(QWidget * p_parent)
{
    SaveDialog l_dialog(p_parent);
    if(l_dialog.exec() != QDialog::Accepted)
    {
        return QString();
    }
    return l_dialog.selectedFile();
}

// EOF
