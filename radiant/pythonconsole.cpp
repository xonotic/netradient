/*
   Copyright (C) 2018, Sebastian Schmidt
   All Rights Reserved.

   This file is part of NetRadiant.

   NetRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   NetRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NetRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "pythonconsole.h"

#include <uilib/uilib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "gtkutil/accelerator.h"
#include "gtkutil/messagebox.h"
#include "gtkutil/container.h"
#include "gtkutil/menu.h"
#include "gtkutil/nonmodal.h"

#include "pybindconnector.h"


ui::TextView g_pythonconsole_output{ui::null};

static gint Input_keypress(ui::Entry widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_Return) {
        try
        {
//             auto result = PYBIND_callfunction("os", std::string(widget.text()));
            PYBIND_exec(std::string(widget.text()));
            std::string result = PYBIND_scope_as_string();
            g_pythonconsole_output.text(result.c_str());
        }
        catch (const std::exception& e)
        {
            g_pythonconsole_output.text(e.what());
        }
        widget.text("");
        return TRUE;
    }
    if (event->keyval == GDK_KEY_Escape) {
        gtk_window_set_focus(widget.window(), NULL);
        return TRUE;
    }

    return FALSE;
}

ui::Widget PythonConsole_constructWindow(ui::Window toplevel)
{
    auto vBox = ui::VBox(FALSE, 2);
    vBox.show();
    gtk_container_set_border_width(GTK_CONTAINER(vBox), 2);
    auto scr = ui::ScrolledWindow(ui::New);
    scr.overflow(ui::Policy::AUTOMATIC, ui::Policy::AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scr), GTK_SHADOW_IN);
    scr.show();

    {
        auto text = ui::TextView(ui::New);
        text.dimensions(0, -1); // allow shrinking
        gtk_text_view_set_wrap_mode(text, GTK_WRAP_WORD);
        gtk_text_view_set_editable(text, FALSE);
        scr.add(text);
        text.show();
        g_pythonconsole_output = text;

    }

    auto input = ui::Entry(ui::New);
    input.show();
    gtk_widget_set_events(input, GDK_KEY_PRESS_MASK);
    input.connect("key_press_event", G_CALLBACK(Input_keypress), 0);

    vBox.pack_start(scr, TRUE, TRUE, 0);
    vBox.pack_end(input, FALSE, FALSE, 0);

    gtk_container_set_focus_chain(GTK_CONTAINER(vBox), NULL);

    return vBox;
}
