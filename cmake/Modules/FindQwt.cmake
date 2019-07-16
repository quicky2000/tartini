#
#  Copyright (C) 2019  Julien Thevenon ( julien_thevenon at yahoo.fr )
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
#.rst:
# FindQwt
# -------------
#
# Module to locate Qwt library
#
# This module define the following variables
#    Qwt_FOUND
#    Qwt_VERSION
#    Qwt_LIBRARIES
#    Qwt_INCLUDE_DIRS
#
find_package(PkgConfig)
pkg_check_modules(PC_QWT QUIET libqwt)
if("${PC_QWT}" STREQUAL "")
    set(QWT_HINTS $ENV{QWT_INSTALL})

    set(QWT_PATHS
        /usr
        /usr/local/
        /usr/local/share/
        /usr/share/
       )
    if(NOT ${QWT_HINTS} STREQUAL "")
        set(QWT_PATHS ${QWT_HINTS})
    endif(NOT ${QWT_HINTS} STREQUAL "")

    find_path(Qwt_INCLUDE_DIR
              NAMES qwt.h
              PATHS ${QWT_PATHS}
              HINTS ${QWT_HINTS}
              PATH_SUFFIXES include qwt-qt4 qwt-qt5
              DOC "Variable storing the location of Qwt header"
             )

    set(ARCH_SUFFIX "lib")
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
      set(ARCH_SUFFIX "")
    endif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")

    find_library(Qwt_LIBRARY
                 NAMES qwt qwt-qt4 qwt-qt5
                 PATHS ${QWT_PATHS}
                 HINTS ${QWT_HINTS}
                 PATH_SUFFIXES ${ARCH_SUFFIX}
                 DOC "Variable storing the location of Qwt library"
                 NO_DEFAULT_PATH
                 NO_CMAKE_PATH
                )

    set(Qwt_VERSION ${Qwt_FIND_VERSION})
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Qwt
                                      FOUND_VAR Qwt_FOUND
                                      REQUIRED_VARS
                                      Qwt_LIBRARY
                                      Qwt_INCLUDE_DIR
                                      VERSION_VAR Qwt_VERSION
                                      )

    if(Qwt_FOUND)
        set(Qwt_LIBRARIES ${Qwt_LIBRARY})
        set(Qwt_INCLUDE_DIRS ${Qwt_INCLUDE_DIR})
        #set(Qwt_DEFINITIONS ${PC_Qwt_CFLAGS_OTHER})
    endif()
    if(Qwt_FOUND AND NOT TARGET Qwt::Qwt)
        add_library(Qwt::Qwt UNKNOWN IMPORTED)
        set_target_properties(Qwt::Qwt PROPERTIES
        IMPORTED_LOCATION "${Qwt_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS ""
        INTERFACE_INCLUDE_DIRECTORIES "${Qwt_INCLUDE_DIR}")
    endif()
endif("${PC_QWT}" STREQUAL "")
