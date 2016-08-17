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
application::application(int & p_argc, char ** p_argv):
  QApplication(p_argc, p_argv)
{
}

//------------------------------------------------------------------------------
bool application::notify(QObject * p_receiver, QEvent * p_event)
{
  try
    {
      return QApplication::notify(p_receiver,p_event);
    }
  catch(std::exception & p_exception)
    {
      std::cout << std::endl << "!!! " << tr("FATAL EXCEPTION").toStdString() << " !!!" << std::endl ;
      std::cout << std::string(80,'-') << std::endl ;
      std::cout << p_exception.what() << std::endl ;
      std::cout << std::string(80,'-') << std::endl ;
      std::cout << tr("The application encountered an unexpected exception please copy the message above and send it to julien_thevenon@yahoo.fr").toStdString() << std::endl;
      emit lastWindowClosed();
    }
  return false;
}

// EOF
