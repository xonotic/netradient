/*
   Copyright (C) 1999-2006 Id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if !defined( INCLUDED_CONSOLE_H )
#define INCLUDED_CONSOLE_H

#include <cstddef>
#include <uilib/uilib.h>

#define SYS_VRB 0 ///< verbose support (on/off)
#define SYS_STD 1 ///< standard print level - this is the default
#define SYS_WRN 2 ///< warnings
#define SYS_ERR 3 ///< error
#define SYS_NOCON 4 ///< no console, only print to the file (useful whenever Sys_Printf and output IS the problem)

class TextOutputStream;
TextOutputStream& getSysPrintOutputStream();
TextOutputStream& getSysPrintErrorStream();

ui::Widget Console_constructWindow( ui::Window toplevel );

std::size_t Sys_Print( int level, const char* buf, std::size_t length );

// will open/close the log file based on the parameter
void Sys_EnableLogFile( bool enable );
extern bool g_Console_enableLogging;


#endif
