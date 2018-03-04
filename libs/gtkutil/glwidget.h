/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

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

#include <uilib/uilib.h>

#if !defined( INCLUDED_GTKUTIL_GLWIDGET_H )
#define INCLUDED_GTKUTIL_GLWIDGET_H

struct OpenGLBinding;

extern void (*GLWidget_sharedContextCreated)(OpenGLBinding &GL);

extern void (*GLWidget_sharedContextDestroyed)(OpenGLBinding &GL);

ui::GLArea glwidget_new(OpenGLBinding &GL, bool zbuffer);

void glwidget_create_context(ui::GLArea self);

void glwidget_destroy_context(ui::GLArea self);

bool glwidget_make_current(ui::GLArea self);

void glwidget_swap_buffers(ui::GLArea self);

#endif
