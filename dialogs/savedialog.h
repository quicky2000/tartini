/***************************************************************************
                          savedialog.h  -  description
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

#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include <QFileDialog>

class QCheckBox;

extern const char *windowSizeBoxStr[];

class SaveDialog : public QFileDialog
{
  Q_OBJECT
  public:
    SaveDialog(QWidget * p_parent = 0);
    ~SaveDialog();
    void accept();

    static QString getSaveWavFileName(QWidget * p_parent = 0);

  private:
    QCheckBox * m_remember_folder_check_box;
    QCheckBox * m_append_wav_check_box;
};

#endif // SAVE_DIALOG_H
// EOF
