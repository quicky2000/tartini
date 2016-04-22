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

#include <iostream>

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
            fprintf(stderr, "No defined key[%s] in group[%s]. (%s, %s)\n", p_key.toLocal8Bit().data(), p_group.toLocal8Bit().data(), m_domain.toLocal8Bit().data(), m_product.toLocal8Bit().data());
            myassert(false); //The defaults haven't been defined for this key!
            return QString("");
        }
    }
    else
    {
        fprintf(stderr, "No defined group[%s], can't get key[%s]. (%s, %s)\n", p_group.toLocal8Bit().data(), p_key.toLocal8Bit().data(), m_domain.toLocal8Bit().data(), m_product.toLocal8Bit().data());
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
      fprintf(stderr, "Default setting for group[%s], key[%s] does not exist!\n", group.toLocal8Bit().data(), key.toLocal8Bit().data());
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

    if(getString(p_group, p_key).at(0).toLower() == QChar('t'))
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
  
    QStringList l_groups = l_on_disk_settings.childGroups();
    for(int l_group_index = 0 ; l_group_index < l_groups.size() ; ++l_group_index)
    {
        l_on_disk_settings.beginGroup(l_groups[l_group_index]);
        QStringList l_keys = l_on_disk_settings.allKeys();
        for(int l_key_index = 0 ; l_key_index < l_keys.size() ; ++l_key_index)
        {
            m_settings[l_groups[l_group_index]][l_keys[l_key_index]] = l_on_disk_settings.value(l_keys[l_key_index]).toString();
        }
        l_on_disk_settings.endGroup();
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
            l_on_disk_settings.setValue("/" + l_iter_value->first, l_iter_value->second);
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
            printf("%s/%s=%s\n", l_iter->first.toLocal8Bit().data(), l_iter_value->first.toLocal8Bit().data(), l_iter_value->second.toLocal8Bit().data());
        }
    }
    fflush(stdout);
}
// EOF