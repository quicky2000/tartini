/***************************************************************************
                          openfiles.cpp  -  description
                             -------------------
    begin                : Thu Dec 16 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/



#include <qapplication.h>
#include <qlistview.h>
#include <qheader.h>

#include <qpixmap.h>

#include "openfiles.h"
#include "gdata.h"
#include "channel.h"
#include "mainwindow.h"
#include "soundfile.h"

OpenFiles::OpenFiles(int id, QWidget *parent) : ViewWidget(id, parent)
{
  //setCaption("Open Files");

  //Create the list of channels down the left hand side
  theListView = new QListView(this);
  theListView->addColumn("Filename (Channel)", 178);
  theListView->addColumn("A", 20);

  // Make it so the Active column magically appears if needed
  //theListView->setColumnWidthMode(0, QListView::Manual);
  //theListView->setColumnWidth(1, 0);
  //theListView->setColumnWidthMode(0, QListView::Maximum);
  //theListView->setColumnWidth(0, 178);
  //theListView->setColumnAlignment(1, Qt::AlignRight);
  //theListView->header()->setStretchEnabled(true);
  //theListView->setResizeMode(QListView::LastColumn);
  
  //theListView->header()->hide();
  //theListView->setAllColumnsShowFocus(true);
  theListView->setSelectionMode(QListView::Extended);
  theListView->setSelectionMode(QListView::Single);
  theListView->setSorting(-1);
  theListView->setFocusPolicy(QWidget::NoFocus);

  connect(gdata, SIGNAL(channelsChanged()), this, SLOT(refreshChannelList()));
  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(slotActiveChannelChanged(Channel *)));
  //connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(refreshChannelList()));
  connect(theListView, SIGNAL(pressed(QListViewItem*)), this, SLOT(listViewChanged(QListViewItem*)));
  //connect(theListView, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotCurrentChanged(QListViewItem*)));
  connect(theListView, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotCurrentChanged(QListViewItem*)));
  //connect(theListView, SIGNAL(pressed(QListViewItem*)), gdata->view, SLOT(doSlowUpdate()));

  refreshChannelList();
}

OpenFiles::~OpenFiles()
{
}
  
void OpenFiles::refreshChannelList()
{
  //put in any channel items that already exist
  //char s[2048];
  theListView->clear();
  
  QString s;
  int j=0;
  for(std::vector<Channel*>::iterator it = gdata->channels.begin(); it != gdata->channels.end(); it++) {
    //sprintf(s, "%s(%d)", getFilenamePart((*it)->getParent()->filename), j+1);
    s = (*it)->getUniqueFilename();
    //theListView->insertItem(new QListViewItem(theListView, theListView->lastItem(), " "));

    QCheckListItem *newElement = new QCheckListItem(theListView, theListView->lastItem(), s, QCheckListItem::CheckBox);
    
    if((*it)->isVisible()) {
      newElement->setOn(true);
    }
    if((*it) == gdata->getActiveChannel()) {
      newElement->setText(1, "A");
      theListView->setSelected(newElement, true);
      theListView->setCurrentItem(newElement);
    }
    j++;
  }
}

//TODO: Tidy this method up
void OpenFiles::slotActiveChannelChanged(Channel *active)
{
  int index = 0;
	bool found = false;

	// Find the index of the active channel
	for (index = 0; index < int(gdata->channels.size()); index++) {
		if (gdata->channels.at(index) == active) {
			found = true;
			break;
		}
	}

	// Set the active marker for each item on or off, depending on what it should be.
	// This depends on them being in the same order as the channels list.
	if (found) {
		int pos = 0;
		// Go through all the elements in the list view and turn the active channel 
		// markers off, or on if we find the right index
		QListViewItem *item = theListView->firstChild();
		while (item != NULL) {
			if (pos == index) {
        theListView->setSelected(item, true);
			//  item->setText(1, "A");
			//} else {
			//	item->setText(1, "");
			}
			item = item->nextSibling();
			pos++;
		}
	}
  //gdata->view->doUpdate();
}

/**
 * Toggles a channel on or off for a specified item.
 *
 * @param item the channel to toggle.
 **/
void OpenFiles::listViewChanged(QListViewItem* item)
{
  if(item == NULL) return;
  int pos = 0;
  QListViewItem *myChild = theListView->firstChild();
  while(myChild) {
    if(myChild == item) break;
    myChild = myChild->nextSibling();
    pos++;
  }
  myassert(pos < int(gdata->channels.size()));
  bool state = ((QCheckListItem *)item)->isOn();
  if(gdata->channels.at(pos)->isVisible() != state) gdata->channels.at(pos)->setVisible(state);
  //gdata->view->doSlowUpdate();
  //gdata->view->doFastUpdate();
  gdata->view->doUpdate();
}

/**
 * Changes the active channel to the item.
 *
 * @param item the channel to toggle.
 **/
void OpenFiles::slotCurrentChanged(QListViewItem* item)
{
  if(item == NULL) return;
  int pos = 0;
  // Go through the channels before the active one, and reset the markers
  QListViewItem *myChild = theListView->firstChild();
  while(myChild) {
    if(myChild == item) break;
    myChild->setText(1, "");
    myChild = myChild->nextSibling();
    pos++;
  }
  myassert(pos < int(gdata->channels.size()));
  myChild->setText(1, "A");
  gdata->setActiveChannel(gdata->channels.at(pos));

  // Go through the rest of the items and reset their active channel markers
  myChild = myChild->nextSibling();
  while(myChild) {
    myChild->setText(1, "");
    myChild = myChild->nextSibling();
  }
}

void OpenFiles::resizeEvent(QResizeEvent *)
{
  theListView->resize(size());
  //theListView->setColumnWidth(0, theListView->width()-40);
}

/*
void OpenFiles::slotAddFilename(QString s)
{

	QCheckListItem *newElement = new QCheckListItem(theListView, theListView->lastItem(), s, QCheckListItem::CheckBox);
  newElement->setOn(true);
  theListView->setSelected(newElement, true);
  theListView->setCurrentItem(newElement);
}
*/
