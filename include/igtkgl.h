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

#if !defined( INCLUDED_IGTKGL_H )
#define INCLUDED_IGTKGL_H

#include <uilib/uilib.h>
#include "generic/constant.h"

struct OpenGLBinding;

template<class T>
using func = T *;

struct _QERGtkGLTable {
    STRING_CONSTANT(Name, "gtkgl");
    INTEGER_CONSTANT(Version, 1);

    func<ui::GLArea(OpenGLBinding &GL, bool zbufffer)> glwidget_new;
    func<void(ui::GLArea self)> glwidget_swap_buffers;
    func<bool(ui::GLArea self)> glwidget_make_current;
    func<void(ui::GLArea self)> glwidget_destroy_context;
    func<void(ui::GLArea self)> glwidget_create_context;
};

#endif
