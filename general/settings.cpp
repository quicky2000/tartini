/***************************************************************************
                          settings.cpp  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#include "settings.h"

#include "myassert.h"

#include <QSettings>
#include <QDir>

//-----------------------------------------------------------------------------
Settings::Settings()
{
}

//-----------------------------------------------------------------------------
Settings::Settings( QString p_domain
                  , QString p_product
                  )
{
    init(p_domain, p_product);
}

//-----------------------------------------------------------------------------
void Settings::init( QString p_domain
                   , QString p_product
                   )
{
    m_domain = p_domain;
    m_product = p_product;
}

//-----------------------------------------------------------------------------
QString Settings::getString( QString p_group
                           , QString p_key
                           )
{
    // Preconditions
    myassert( !p_group.isNull() && !p_key.isNull() );

    // First try to load it from the map
    std::map<QString, std::map<QString, QString> >::const_iterator l_iter = m_settings.find(p_group);
    if (l_iter != m_settings.end())
    {
        std::map<QString, QString>::const_iterator l_iter_value = (l_iter->second).find(p_key);
        if (l_iter_value != (l_iter->second).end())
        {
            return l_iter_value->second;
        }
        else
        {
            fprintf(stderr, "No defined key[%s] in group[%s]. (%s, %s)\n", p_key.ascii(), p_group.ascii(), m_domain.ascii(), m_product.ascii());
            myassert(false); //The defaults haven't been defined for this key!
            return QString("");
        }
    }
    else
    {
        fprintf(stderr, "No defined group[%s], can't get key[%s]. (%s, %s)\n", p_group.ascii(), p_key.ascii(), m_domain.ascii(), m_product.ascii());
        myassert(false); //The defaults haven't been defined for this key!
        return QString("");
    }

/*
  // If we're here, then the value was not found. Load it in from the defaults.
  std::map<QString, std::map<QString, QString> >::const_iterator defIter = defaults.find(group);
  if (defIter != settings.end()) {
    std::map<QString, QString>::const_iterator defIterValue = (defIter->second).find(key);
    if (defIterValue != (defIter->second).end()) {
      QString value = defIterValue->second;
      (settings[group])[key] = defIterValue->second;
    } else {
      fprintf(stderr, "Default setting for group[%s], key[%s] does not exist!\n", group.ascii(), key.ascii());
      myassert(false); //The defaults haven't been defined for this key!
      return QString("");
    }
  }
  return settings[group][key];
*/
	
}

//-----------------------------------------------------------------------------
int Settings::getInt( QString p_group
                    , QString p_key
                    )
{
    // Preconditions
    myassert( !p_group.isNull() && !p_key.isNull() );

    bool l_ok = false;
    int l_to_return = getString(p_group, p_key).toInt(&l_ok);

    myassert(l_ok);
    return l_to_return;
}

//-----------------------------------------------------------------------------
double Settings::getDouble( QString p_group
                          , QString p_key
                          )
{
    // Preconditions
    myassert( !p_group.isNull() && !p_key.isNull() );

    bool l_ok = false;
    double l_to_return = getString(p_group, p_key).toDouble(&l_ok);

    myassert(l_ok);
    return l_to_return;
}

//-----------------------------------------------------------------------------
bool Settings::getBool( QString p_group
                      , QString p_key
                      )
{
    // Preconditions
    myassert(!p_group.isNull() && !p_key.isNull());

    if(getString(p_group, p_key).at(0).lower() == QChar('t'))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
void Settings::setString( QString p_group
                        , QString p_key
                        , QString p_value
                        )
{
    // Preconditions
    myassert(!p_group.isNull() && !p_key.isNull() && !p_value.isNull());

    m_settings[p_group][p_key] = p_value;

    // Postconditions
    myassert((m_settings[p_group])[p_key] == p_value);
}

//-----------------------------------------------------------------------------
void Settings::setInt( QString p_group
                     , QString p_key
                     , int p_value
                     )
{
    QString l_string;
    setString(p_group, p_key, l_string.setNum(p_value));
}

//-----------------------------------------------------------------------------
void Settings::setDouble( QString p_group
                        , QString p_key
                        , double p_value
                        )
{
    QString l_string;
    setString(p_group, p_key, l_string.setNum(p_value));
}

//-----------------------------------------------------------------------------
void Settings::setBool( QString p_group
                      , QString p_key
                      , bool p_value
                      )
{
    setString(p_group, p_key, (p_value) ? "true" : "false");
}

//-----------------------------------------------------------------------------
void Settings::load()
{
    myassert(m_domain != "" && m_product != "");
  
//#ifdef MACX
//  QSettings l_on_disk_settings(QSettings::Ini);
//#else
//  QSettings l_on_disk_settings(QSettings::Native);
//#endif
    QSettings l_on_disk_settings(m_domain, m_product);

    //l_on_disk_settings.setPath(m_domain, m_product, QSettings::UserScope);
    l_on_disk_settings.beginGroup(QString("/") + m_product);
    //l_on_disk_settings.beginGroup(QString("/General"));
  
    /* QT is the stupidest thing in the world. Instead of being able to use the
     * subkeylist function, which is meant to do _exactly_ what I want, we can't
     * use it in anything but Windows. So, we have to iterate through our
     * defaults map and use the first key as a category.
     */
    QStringList l_subkeys;
    std::map<QString, std::map<QString, QString> >::const_iterator iter = m_settings.begin();
    while (!(iter == m_settings.end()))
    {
      l_subkeys += iter->first;
      iter++;
    }
  
    // Get all the entries in the folders, and stick them in the map
    QStringList l_entries;
    QString l_key;
    for(int i = 0; i < l_subkeys.size(); i++)
    {
        l_entries = l_on_disk_settings.entryList("/" + *(l_subkeys.at(i)));
        //printf("There are %d entries in category %s.\n", l_entries.size(), (*l_subkeys.at(i)).data());
        for(int l_n = 0; l_n < l_entries.size(); l_n++)
        {
            //key = QString("/") + *(l_subkeys.at(i)) + "/" + *(l_entries.at(l_n));
            l_key = "/" + l_subkeys.at(i) + "/" + l_entries.at(l_n);
            //(settings[*(l_subkeys.at(i))])[*(l_entries.at(l_n))] = l_on_disk_settings.readEntry(l_key, "");
            (m_settings[l_subkeys.at(i)])[l_entries.at(l_n)] = l_on_disk_settings.readEntry(l_key, "");
        }
    }

    l_on_disk_settings.endGroup();
}

//-----------------------------------------------------------------------------
void Settings::save()
{
    myassert(m_domain != "" && m_product != "");
    // Save settings to www.cs.otago.ac.nz/Pitch/Pitch/_GroupName_/_Key_/
    // Needs to be like this so it works properly in Unix.

    //#ifdef MACX
    //  QSettings l_on_disk_settings(QSettings::Ini);
    //#else
    //  QSettings l_on_disk_settings(QSettings::Native);
    //#endif

    QSettings l_on_disk_settings(m_domain, m_product);

    //l_on_disk_settings.setPath(m_domain, m_product, QSettings::UserScope);
    l_on_disk_settings.beginGroup(QString("/") + m_product);

    std::map<QString, std::map<QString, QString> >::const_iterator l_iter = m_settings.begin();
    while (l_iter != m_settings.end())
    {
        l_on_disk_settings.beginGroup(l_iter->first);
        //qDebug(l_on_disk_settings.group());
        std::map<QString, QString>::const_iterator l_iter_value = l_iter->second.begin();
        while (l_iter_value != l_iter->second.end())
        {
            l_on_disk_settings.writeEntry("/" + l_iter_value->first, l_iter_value->second);
            //qDebug("Wrote /" + l_iter_value->first + "= " + l_iter_value->second);
            l_iter_value++;
        }
        l_on_disk_settings.endGroup();
        l_iter++;
    }
    l_on_disk_settings.endGroup();
}

//-----------------------------------------------------------------------------
void Settings::print()
{
    std::map<QString, std::map<QString, QString> >::const_iterator l_iter = m_settings.begin();
    for(; l_iter != m_settings.end(); l_iter++)
    {
        std::map<QString, QString>::const_iterator l_iter_value = (l_iter->second).begin();
        for(; l_iter_value != (l_iter->second).end(); l_iter_value++)
        {
            printf("%s/%s=%s\n", l_iter->first.latin1(), l_iter_value->first.latin1(), l_iter_value->second.latin1());
        }
    }
    fflush(stdout);
}
// EOF