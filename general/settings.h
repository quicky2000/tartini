/***************************************************************************
                          settings.h  -  description
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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
#include <QString>

class Settings
{

  public:

    /**
     * Load the settings from disk into the map.
     */
    Settings();

    Settings( QString p_domain
            , QString p_product
            );

    virtual ~Settings() {};

    void init( QString p_domain
             , QString p_product
             );



    /**
     * Return the value indexed in the map, or load it in from the defaults if need be
     */
    QString getString( QString p_group
                     , QString p_key
                     );

    /**
     * Use getString, convert to an int
     */
    int getInt( QString p_group
	          , QString p_key
	          );

    /**
     * Use getString, convert to a double
     */
	double getDouble( QString p_group
	                , QString p_key
	                );

    /**
     * Use getString, convert to a bool
     */
    bool getBool( QString p_group
	            , QString p_key
	            );
  
	void setString( QString p_group
	              , QString p_key
	              , QString p_value
	              );

	void setInt( QString p_group
	           , QString p_key
	           , int p_value
	           );

	void setDouble( QString p_group
	              , QString p_key
	              , double p_value
	              );

	void setBool( QString p_group
	            , QString p_key
	            , bool p_value
	            );

	//void loadDefaults();

	void load(); //loads settings in from disk
	void save(); //saves settings out to disk

	void print();

  private:

    std::map<QString, std::map<QString, QString> > m_settings; /*< A memory version of the settings on disk. Only the Settings form can change these values. */
	//std::map<QString, std::map<QString, QString> > defaults; /*< Default settings */
  
	QString m_domain;
	QString m_product;
};

//extern Settings *settings;

#endif // SETTINGS_H
// EOF