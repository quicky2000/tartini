/*    This file is part of Tartini
      Copyright (C) 2016  Julien Thevenon ( julien_thevenon at yahoo.fr )
      Copyright (C) 2004 by Philip McLeod ( pmcleod@cs.otago.ac.nz )

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//------------------------------------------------------------------------------
bool GData::settingsContains(const QString & p_key)const
{
  return qsettings->contains(p_key);

}

//------------------------------------------------------------------------------
int GData::getSettingsValue(const QString & p_key,const int & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toInt();

}

//------------------------------------------------------------------------------
bool GData::getSettingsValue(const QString & p_key,const bool & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toBool();

}

//------------------------------------------------------------------------------
QString GData::getSettingsValue(const QString & p_key,const QString & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toString();

}

//------------------------------------------------------------------------------
QPoint GData::getSettingsValue(const QString & p_key,const QPoint & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toPoint();

}

//------------------------------------------------------------------------------
QSize GData::getSettingsValue(const QString & p_key,const QSize & p_default_value)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key, p_default_value).toSize();

}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QString & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const int & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QPoint & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::setSettingsValue(const QString & p_key,const QSize & p_value)
{
  qsettings->setValue(p_key,p_value);
}

//------------------------------------------------------------------------------
void GData::clearSettings(void)
{
  qsettings->clear();
}

//------------------------------------------------------------------------------
void GData::syncSettings(void)
{
  qsettings->sync();
}

//------------------------------------------------------------------------------
int GData::getSettingsIntValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toInt();

}

//------------------------------------------------------------------------------
bool GData::getSettingsBoolValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toBool();

}

//------------------------------------------------------------------------------
QString GData::getSettingsStringValue(const QString & p_key)const
{
  assert(qsettings->contains(p_key));
  return qsettings->value(p_key).toString();

}

//EOF
