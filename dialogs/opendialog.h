/***************************************************************************
                          opendialog.h  -  description
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

#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QFileDialog>

class QCheckBox;

class OpenDialog : public QFileDialog
{
  Q_OBJECT

  public:

    OpenDialog(QWidget * p_parent);
    ~OpenDialog(void);
    void accept(void);
  
    static QString getOpenWavFileName(QWidget * p_parent = 0);
  
    QCheckBox * m_remember_folder_check_box;
};

#endif // OPENDIALOG_H
// EOF
