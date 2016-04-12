/***************************************************************************
                          openfiles.h  -  description
                             -------------------
    begin                : Thu Dec 16 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef OPENFILES_H
#define OPENFILES_H

#include "viewwidget.h"
#include <QResizeEvent>
#include <QItemSelection>
#include <QTableWidgetItem>

class Channel;

class OpenFiles : public ViewWidget
{

  Q_OBJECT

  public:
    OpenFiles( int p_id
             , QWidget *p_parent
             );
    virtual ~OpenFiles(void);

    //private: // Although it should be private, really
    QTableWidget * m_table;

    void resizeEvent(QResizeEvent *);

    QSize sizeHint(void) const;

  private slots:
    void refreshChannelList();

    /**
     * Toggles a channel on or off for a specified item.
     *
     * @param p_item the channel to toggle.
     */
    void listViewChanged();

    /**
     * Changes the active channel to the item.
     */
    void slotCurrentChanged(int p_row, int p_column);
    void slotActiveChannelChanged(Channel * p_active);

};
#endif // OPENFILES_H
// EOF

