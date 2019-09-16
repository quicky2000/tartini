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

//------------------------------------------------------------------------------
OpenFiles::OpenFiles(int p_id, QWidget *p_parent):
  ViewWidget(p_id, p_parent)
{
  //Create the list of channels down the left hand side
  m_the_list_view = new Q3ListView(this);
  m_the_list_view->addColumn("Filename (Channel)", 178);
  m_the_list_view->addColumn("A", 20);

  m_the_list_view->setWhatsThis("A list of all open channels in all open sounds. "
    "The current active channel is marked with an 'A' beside it. "
    "The tick specifies if a channel should be visible or not in the multi-channel views");

  m_the_list_view->setSelectionMode(Q3ListView::Extended);
  m_the_list_view->setSelectionMode(Q3ListView::Single);
  m_the_list_view->setSorting(-1);
  m_the_list_view->setFocusPolicy(Qt::NoFocus);

  connect(gdata, SIGNAL(channelsChanged()), this, SLOT(refreshChannelList()));
  connect(gdata, SIGNAL(activeChannelChanged(Channel*)), this, SLOT(slotActiveChannelChanged(Channel *)));
  connect(m_the_list_view, SIGNAL(pressed(Q3ListViewItem*)), this, SLOT(listViewChanged(Q3ListViewItem*)));
  connect(m_the_list_view, SIGNAL(selectionChanged(Q3ListViewItem*)), this, SLOT(slotCurrentChanged(Q3ListViewItem*)));

  refreshChannelList();
}

//------------------------------------------------------------------------------
OpenFiles::~OpenFiles(void)
{
}
  
//------------------------------------------------------------------------------
void OpenFiles::refreshChannelList(void)
{
  //put in any channel items that already exist
  m_the_list_view->clear();
  
  QString l_string;
  int l_j = 0;
  unsigned int l_index = 0;
  for(l_index = 0 ; l_index < gdata->getChannelsSize() ; ++l_index)
    {
      l_string = gdata->getChannelAt(l_index)->getUniqueFilename();

      Q3CheckListItem *l_new_element = new Q3CheckListItem(m_the_list_view, m_the_list_view->lastItem(), l_string, Q3CheckListItem::CheckBox);
    
      if(gdata->getChannelAt(l_index)->isVisible())
	{
	  l_new_element->setOn(true);
	}
      if(gdata->getChannelAt(l_index) == gdata->getActiveChannel())
	{
	  l_new_element->setText(1, "A");
	  m_the_list_view->setSelected(l_new_element, true);
	  m_the_list_view->setCurrentItem(l_new_element);
	}
      l_j++;
    }
}

//TODO: Tidy this method up
//------------------------------------------------------------------------------
void OpenFiles::slotActiveChannelChanged(Channel * p_active)
{
  int l_index = 0;
  bool l_found = false;

  // Find the index of the active channel
  for (l_index = 0; l_index < int(gdata->getChannelsSize()); l_index++)
    {
      if (gdata->getChannelAt(l_index) == p_active)
	{
	  l_found = true;
	  break;
	}
    }

  // Set the active marker for each item on or off, depending on what it should be.
  // This depends on them being in the same order as the channels list.
  if (l_found)
    {
      int l_pos = 0;
      // Go through all the elements in the list view and turn the active channel 
      // markers off, or on if we find the right index
      Q3ListViewItem *l_item = m_the_list_view->firstChild();
      while (l_item != NULL)
	{
	  if (l_pos == l_index)
	    {
	      m_the_list_view->setSelected(l_item, true);
	    }
	  l_item = l_item->nextSibling();
	  l_pos++;
	}
    }
}

//------------------------------------------------------------------------------
void OpenFiles::listViewChanged(Q3ListViewItem* p_item)
{
  if(p_item == NULL)
    {
      return;
    }
  int l_pos = 0;
  Q3ListViewItem *l_my_child = m_the_list_view->firstChild();
  while(l_my_child)
    {
      if(l_my_child == p_item)
	{
	  break;
	}
      l_my_child = l_my_child->nextSibling();
      l_pos++;
    }
  myassert(l_pos < int(gdata->getChannelsSize()));
  bool l_state = ((Q3CheckListItem *)p_item)->isOn();
  if(gdata->getChannelAt(l_pos)->isVisible() != l_state)
    {
      gdata->getChannelAt(l_pos)->setVisible(l_state);
    }
  gdata->getView().doUpdate();
}

//------------------------------------------------------------------------------
void OpenFiles::slotCurrentChanged(Q3ListViewItem* p_item)
{
  if(p_item == NULL)
    {
      return;
    }
  int l_pos = 0;
  // Go through the channels before the active one, and reset the markers
  Q3ListViewItem *l_my_child = m_the_list_view->firstChild();
  while(l_my_child)
    {
      if(l_my_child == p_item)
	{
	  break;
	}
      l_my_child->setText(1, "");
      l_my_child = l_my_child->nextSibling();
      l_pos++;
    }
  myassert(l_pos < int(gdata->getChannelsSize()));
  l_my_child->setText(1, "A");
  gdata->setActiveChannel(gdata->getChannelAt(l_pos));

  // Go through the rest of the items and reset their active channel markers
  l_my_child = l_my_child->nextSibling();
  while(l_my_child)
    {
      l_my_child->setText(1, "");
      l_my_child = l_my_child->nextSibling();
    }
}

//------------------------------------------------------------------------------
void OpenFiles::resizeEvent(QResizeEvent *)
{
  m_the_list_view->resize(size());
}

//------------------------------------------------------------------------------
QSize OpenFiles::sizeHint(void) const
{
  return QSize(200, 300);
}

// EOF
