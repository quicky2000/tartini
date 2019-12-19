/***************************************************************************
                          mainwindow.cpp  -  description
                             -------------------
    begin                : May 2002
    copyright            : (C) 2002-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "mainwindow.h"
#include "tartinidialog.h"
#include "gdata.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>

//------------------------------------------------------------------------------
TartiniDialog::TartiniDialog(QWidget *p_parent)
: QDialog(p_parent)
{
    setModal(true);
    setWindowTitle("About Tartini - Version " TARTINI_VERSION_STR);

    QVBoxLayout * l_layout = new QVBoxLayout(this);
    l_layout->setContentsMargins(6,6,6,6);
    l_layout->setSpacing(6);
    setBackgroundRole(QPalette::Shadow);

    QHBoxLayout * l_top_layout = new QHBoxLayout();
    l_layout->addLayout(l_top_layout);
    QLabel * l_tartini_picture = new QLabel(this);
    l_tartini_picture->setObjectName("TartiniPicture");
    QPixmap l_tartini_pixmap(":/res/tartinilogo.jpg", "JPG");
    l_tartini_picture->setPixmap(l_tartini_pixmap);
    l_tartini_picture->setBackgroundRole(QPalette::Shadow);
    l_top_layout->addStretch(10);
    l_top_layout->addWidget(l_tartini_picture);
    l_top_layout->addStretch(10);

    QTextBrowser * l_tartini_text_edit = new QTextBrowser(this);
    l_tartini_text_edit->setObjectName("TartiniTextEdit");
    l_tartini_text_edit->setOpenExternalLinks(true);

    QColor l_link_color(Qt::cyan);
    QString l_sheet = QString::fromLatin1("a { text-decoration: underline; color: %1 }").arg(l_link_color.name());
    l_tartini_text_edit->document()->setDefaultStyleSheet(l_sheet);

    l_tartini_text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    l_tartini_text_edit->setHtml(
            "Tartini is a graphical tool for analysing the music of solo instruments.<br>"
            "This program was created by Philip McLeod as part of PhD work at the University of Otago, New Zealand<br>"
            "You can find the latest info about Tartini at <a href=\"http://www.tartini.net\">http://www.tartini.net</a><br><br>"
            "Copyright 2002-2007 Philip McLeod (pmcleod@cs.otago.ac.nz).<br><br>"

            "New features in version 1.2 include:<br>"
            "- The ability to set an offset frequency. i.e. not fixed to A=440 Hz<br>"
            "- Vertical reference lines<br>"
            "- Fixed a bug in note detection which caused a number of pitch detection problems<br><br>"

            "New features in version 1.1 include:<br>"
            "- A vibrato analysis tool<br>"
            "- Full duplex sound, allowing play and record at the same time<br>"
            "- Choice of musical scales<br>"
            "- Some basic printing abilities (pitch view only)<br>"
            "- Some basic exporting abilities (plain text or matlab code)<br>"
            "- A refined pitch algorithm<br>"
            "- A musical score generator (still in very early stages of development)<br><br>"

            "<i>Thanks to:-</i><br>"
            "Prof. Geoff Wyvill - PhD Supervisor (Computer Science)<br>"
            "Dr. Donald Warrington - PhD Supervisor (Physics)<br>"
            "Rob Ebbers - Programming<br>"
            "Maarten van Sambeek - Programming<br>"
            "Stuart Miller - Programming<br>"
            "Mr Kevin Lefohn - Support (Violin)<br>"
            "Miss Judy Bellingham - Support (Voice)<br>"
            "Jean-Philippe Grenier - Logo design<br>"
            "People at my lab, especially Alexis Angelidis, Sui-Ling Ming-Wong, Brendan McCane and Damon Simpson<br><br>"
            "Tartini version " TARTINI_VERSION_STR " is released under the GPL license <a href=\"http://www.gnu.org/licenses/gpl.html\">http://www.gnu.org/licenses/gpl.html</a>.<br><br>"

                                                   "<i>Libraries used by tartini include:</i><br>"
                                                   "Qt - <a href=\"http://www.trolltech.com/qt\">http://www.trolltech.com/qt</a><br>"
                                                   "FFTW - <a href=\"http://www.fftw.org\">http://www.fftw.org</a><br>"
                                                   "Qwt - <a href=\"http://qwt.sourceforge.net\">http://qwt.sourceforge.net</a><br>"
                                                   "RtAudio - <a href=\"http://www.music.mcgill.ca/~gary/rtaudio/index.html\">http://www.music.mcgill.ca/~gary/rtaudio/index.html</a><br>"
                                                   "<br>"
                                                   "This program is free software; you can redistribute it and/or modify "
                                                   "it under the terms of the GNU General Public License as published by "
                                                   "the Free Software Foundation; either version 2 of the License, or "
                                                   "(at your option) any later version.<br><br>"
                                                   "This program is distributed in the hope that it will be useful, "
                                                   "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                                   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                                   "GNU General Public License for more details.<br><br>"
                                                   "You should have received a copy of the GNU General Public License "
                                                   "along with this program; if not, write to the Free Software "
                                                   "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.<br><br>"
                                                   "Please click 'Read License' or see LICENSE.txt for details.<br>"
    );

    QPalette l_palette;
    l_palette = palette();
    l_palette.setColor(QPalette::Base, Qt::black);
    l_palette.setColor(QPalette::Text, Qt::white);
    l_tartini_text_edit->setPalette(l_palette);
    l_layout->addWidget(l_tartini_text_edit);

    QHBoxLayout * l_bottom_layout = new QHBoxLayout();
    l_layout->addLayout(l_bottom_layout);
    QPushButton * l_ok_button = new QPushButton("&Ok", this);
    QPushButton * l_GPL_button = new QPushButton("&Read License", this);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_ok_button);
    l_bottom_layout->addStretch(10);
    l_bottom_layout->addWidget(l_GPL_button);
    l_bottom_layout->addStretch(10);
    setFocusProxy(l_ok_button);
    connect(l_ok_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(l_GPL_button, SIGNAL(clicked()), g_main_window, SLOT(aboutGPL()));
}

//------------------------------------------------------------------------------
QSize TartiniDialog::sizeHint() const
{
    return QSize(600, 600);
}


// EOF