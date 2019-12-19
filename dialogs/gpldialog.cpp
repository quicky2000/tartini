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

#include "gpldialog.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

//------------------------------------------------------------------------------
GPLDialog::GPLDialog(QWidget *p_parent)
        : QDialog(p_parent)
{
    setModal(true);
    setWindowTitle("GPL Licence");

    QVBoxLayout * l_layout = new QVBoxLayout(this);
    l_layout->setContentsMargins(6,6,6,6);
    l_layout->setSpacing(6);
    QTextEdit * l_GPL_text_edit = new QTextEdit(this);
    l_GPL_text_edit->setObjectName("GPLTextEdit");
    l_GPL_text_edit->setReadOnly(true);
    l_GPL_text_edit->setAcceptRichText(false);
    QString l_the_text;
    {
        QFile l_the_file(":res/LICENSE.txt");
        l_the_file.open(QIODevice::ReadOnly);
        QTextStream l_text_stream(&l_the_file);
        l_the_text = l_text_stream.readAll();
        l_the_file.close();
    }
    l_GPL_text_edit->setText(l_the_text);

    l_layout->addWidget(l_GPL_text_edit);

    QHBoxLayout * l_bottom_layout = new QHBoxLayout();
    l_layout->addLayout(l_bottom_layout);
    QPushButton * l_ok_button = new QPushButton("&Ok", this);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_ok_button);
    l_bottom_layout->addStretch(10);
    setFocusProxy(l_ok_button);
    connect(l_ok_button, SIGNAL(clicked()), this, SLOT(accept()));
}

//------------------------------------------------------------------------------
QSize GPLDialog::sizeHint() const
{
    return QSize(600, 480);
}

// EOF
