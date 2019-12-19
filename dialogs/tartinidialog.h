/***************************************************************************
                          mainwindow.h  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2019 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef TARTINI_TARTINIDIALOG_H
#define TARTINI_TARTINIDIALOG_H

#include <QDialog>

class TartiniDialog :public QDialog
{
    Q_OBJECT

  public:
    TartiniDialog(QWidget *p_parent = NULL);
    QSize sizeHint() const;
};


#endif //TARTINI_TARTINIDIALOG_H
// EOF