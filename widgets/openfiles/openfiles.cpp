/***************************************************************************
                          openfiles.cpp  -  description
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



#include <qapplication.h>
#include <q3listview.h>
#include <q3header.h>

#include <qpixmap.h>
#include <QResizeEvent>

#include "openfiles.h"
#include "gdata.h"
#include "channel.h"
#include "mainwindow.h"
#include "soundfile.h"

OpenFiles::OpenFiles(int id, QWidget *parent) : ViewWidget(id, parent)
{
  //Create the list of channels down the left hand side
  theListView = new Q3ListView(this);
  theListView->addColumn("Filename (Channel)", 178);
  theListView->addColumn("A", 20);

  theListView->setWhatsThis("A list of all open channels in all open sounds. "
    "The current active channel is marked with an 'A' beside it. "
    "The tick specifies if a channel should be visible or not in the multi-channel views");

  theListView->setSelectionMode(Q3ListView::Extended);
  theListView->setSelectionMode(Q3ListView::Single);
  theListView->setSorting(-1);
  theListView->setFocusPolicy(Qt::NoFocus);

  connect(gdata, SIGNAL(channelsChanged()), this, SLOT(refreshChannelList()));
  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(slotActiveChannelChanged(Channel *)));
  connect(theListView, SIGNAL(pressed(Q3ListViewItem*)), this, SLOT(listViewChanged(Q3ListViewItem*)));
  connect(theListView, SIGNAL(selectionChanged(Q3ListViewItem*)), this, SLOT(slotCurrentChanged(Q3ListViewItem*)));

  refreshChannelList();
}

OpenFiles::~OpenFiles()
{
}
  
void OpenFiles::refreshChannelList()
{
  //put in any channel items that already exist
  theListView->clear();
  
  QString s;
  int j=0;
  unsigned int l_index = 0;
  for(l_index = 0 ; l_index < gdata->getChannelsSize() ; ++l_index)
    {
      s = gdata->getChannelAt(l_index)->getUniqueFilename();

      Q3CheckListItem *newElement = new Q3CheckListItem(theListView, theListView->lastItem(), s, Q3CheckListItem::CheckBox);
    
      if(gdata->getChannelAt(l_index)->isVisible())
	{
	  newElement->setOn(true);
	}
      if(gdata->getChannelAt(l_index) == gdata->getActiveChannel())
	{
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
	for (index = 0; index < int(gdata->getChannelsSize()); index++) {
		if (gdata->getChannelAt(index) == active) {
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
		Q3ListViewItem *item = theListView->firstChild();
		while (item != NULL) {
			if (pos == index) {
        theListView->setSelected(item, true);
			}
			item = item->nextSibling();
			pos++;
		}
	}
}

/**
 * Toggles a channel on or off for a specified item.
 *
 * @param item the channel to toggle.
 **/
void OpenFiles::listViewChanged(Q3ListViewItem* item)
{
  if(item == NULL) return;
  int pos = 0;
  Q3ListViewItem *myChild = theListView->firstChild();
  while(myChild) {
    if(myChild == item) break;
    myChild = myChild->nextSibling();
    pos++;
  }
  myassert(pos < int(gdata->getChannelsSize()));
  bool state = ((Q3CheckListItem *)item)->isOn();
  if(gdata->getChannelAt(pos)->isVisible() != state) gdata->getChannelAt(pos)->setVisible(state);
  gdata->getView().doUpdate();
}

/**
 * Changes the active channel to the item.
 *
 * @param item the channel to toggle.
 **/
void OpenFiles::slotCurrentChanged(Q3ListViewItem* item)
{
  if(item == NULL) return;
  int pos = 0;
  // Go through the channels before the active one, and reset the markers
  Q3ListViewItem *myChild = theListView->firstChild();
  while(myChild) {
    if(myChild == item) break;
    myChild->setText(1, "");
    myChild = myChild->nextSibling();
    pos++;
  }
  myassert(pos < int(gdata->getChannelsSize()));
  myChild->setText(1, "A");
  gdata->setActiveChannel(gdata->getChannelAt(pos));

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
}

