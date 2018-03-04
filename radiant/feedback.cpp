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

//-----------------------------------------------------------------------------
//
// DESCRIPTION:
// classes used for describing geometry information from q3map feedback
//

#include "feedback.h"

#include <gtk/gtk.h>

#include "debugging/debugging.h"

#include "igl.h"
#include "iselection.h"

#include "map.h"
#include "dialog.h"
#include "mainframe.h"


CDbgDlg g_DbgDlg;

void Feedback_draw2D(OpenGLBinding &GL, VIEWTYPE viewType)
{
    g_DbgDlg.draw2D(GL, viewType);
}

void CSelectMsg::saxStartElement(message_info_t *ctx, const xmlChar *name, const xmlChar **attrs)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "select")) {
        // read the message
        ESelectState = SELECT_MESSAGE;
    } else {
        // read the brush
        ASSERT_MESSAGE(string_equal(reinterpret_cast<const char *>( name ), "brush"), "FEEDBACK PARSE ERROR");
        ASSERT_MESSAGE(ESelectState == SELECT_MESSAGE, "FEEDBACK PARSE ERROR");
        ESelectState = SELECT_BRUSH;
        globalOutputStream() << message.c_str() << '\n';
    }
}

void CSelectMsg::saxEndElement(message_info_t *ctx, const xmlChar *name)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "select")) {
    }
}

void CSelectMsg::saxCharacters(message_info_t *ctx, const xmlChar *ch, int len)
{
    if (ESelectState == SELECT_MESSAGE) {
        message.write(reinterpret_cast<const char *>( ch ), len);
    } else {
        brush.write(reinterpret_cast<const char *>( ch ), len);
    }
}

IGL2DWindow *CSelectMsg::Highlight()
{
    GlobalSelectionSystem().setSelectedAll(false);
    int entitynum, brushnum;
    if (sscanf(reinterpret_cast<const char *>( brush.c_str()), "%i %i", &entitynum, &brushnum) == 2) {
        SelectBrush(entitynum, brushnum);
    }
    return 0;
}

void CPointMsg::saxStartElement(message_info_t *ctx, const xmlChar *name, const xmlChar **attrs)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "pointmsg")) {
        // read the message
        EPointState = POINT_MESSAGE;
    } else {
        // read the brush
        ASSERT_MESSAGE(string_equal(reinterpret_cast<const char *>( name ), "point"), "FEEDBACK PARSE ERROR");
        ASSERT_MESSAGE(EPointState == POINT_MESSAGE, "FEEDBACK PARSE ERROR");
        EPointState = POINT_POINT;
        globalOutputStream() << message.c_str() << '\n';
    }
}

void CPointMsg::saxEndElement(message_info_t *ctx, const xmlChar *name)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "pointmsg")) {
    } else if (string_equal(reinterpret_cast<const char *>( name ), "point")) {
        sscanf(point.c_str(), "%g %g %g", &(pt[0]), &(pt[1]), &(pt[2]));
        point.clear();
    }
}

void CPointMsg::saxCharacters(message_info_t *ctx, const xmlChar *ch, int len)
{
    if (EPointState == POINT_MESSAGE) {
        message.write(reinterpret_cast<const char *>( ch ), len);
    } else {
        ASSERT_MESSAGE(EPointState == POINT_POINT, "FEEDBACK PARSE ERROR");
        point.write(reinterpret_cast<const char *>( ch ), len);
    }
}

IGL2DWindow *CPointMsg::Highlight()
{
    return this;
}

void CPointMsg::DropHighlight()
{
}

void CPointMsg::Draw2D(OpenGLBinding &GL, VIEWTYPE vt)
{
    int nDim1 = (vt == YZ) ? 1 : 0;
    int nDim2 = (vt == XY) ? 1 : 2;
    glPointSize(4);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(pt[nDim1], pt[nDim2]);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(pt[nDim1] - 8, pt[nDim2] - 8);
    glVertex2f(pt[nDim1] + 8, pt[nDim2] - 8);
    glVertex2f(pt[nDim1] + 8, pt[nDim2] + 8);
    glVertex2f(pt[nDim1] - 8, pt[nDim2] + 8);
    glEnd();
}

void CWindingMsg::saxStartElement(message_info_t *ctx, const xmlChar *name, const xmlChar **attrs)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "windingmsg")) {
        // read the message
        EPointState = WINDING_MESSAGE;
    } else {
        // read the brush
        ASSERT_MESSAGE(string_equal(reinterpret_cast<const char *>( name ), "winding"), "FEEDBACK PARSE ERROR");
        ASSERT_MESSAGE(EPointState == WINDING_MESSAGE, "FEEDBACK PARSE ERROR");
        EPointState = WINDING_WINDING;
        globalOutputStream() << message.c_str() << '\n';
    }
}

void CWindingMsg::saxEndElement(message_info_t *ctx, const xmlChar *name)
{
    if (string_equal(reinterpret_cast<const char *>( name ), "windingmsg")) {
    } else if (string_equal(reinterpret_cast<const char *>( name ), "winding")) {
        const char *c = winding.c_str();
        sscanf(c, "%i ", &numpoints);

        int i = 0;
        for (; i < numpoints; i++) {
            c = strchr(c + 1, '(');
            if (c) { // even if we are given the number of points when the cycle begins .. don't trust it too much
                sscanf(c, "(%g %g %g)", &wt[i][0], &wt[i][1], &wt[i][2]);
            } else {
                break;
            }
        }
        numpoints = i;
    }
}

void CWindingMsg::saxCharacters(message_info_t *ctx, const xmlChar *ch, int len)
{
    if (EPointState == WINDING_MESSAGE) {
        message.write(reinterpret_cast<const char *>( ch ), len);
    } else {
        ASSERT_MESSAGE(EPointState == WINDING_WINDING, "FEEDBACK PARSE ERROR");
        winding.write(reinterpret_cast<const char *>( ch ), len);
    }
}

IGL2DWindow *CWindingMsg::Highlight()
{
    return this;
}

void CWindingMsg::DropHighlight()
{
}

void CWindingMsg::Draw2D(OpenGLBinding &GL, VIEWTYPE vt)
{
    int i;

    int nDim1 = (vt == YZ) ? 1 : 0;
    int nDim2 = (vt == XY) ? 1 : 2;
    glColor3f(1.0f, 0.f, 0.0f);

    glPointSize(4);
    glBegin(GL_POINTS);
    for (i = 0; i < numpoints; i++)
        glVertex2f(wt[i][nDim1], wt[i][nDim2]);
    glEnd();
    glPointSize(1);

    glEnable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.133f, 0.4f, 1.0f, 0.5f);
    glBegin(GL_POLYGON);
    for (i = 0; i < numpoints; i++)
        glVertex2f(wt[i][nDim1], wt[i][nDim2]);
    glEnd();
    glDisable(GL_BLEND);
}

// triggered when the user selects an entry in the feedback box
static void feedback_selection_changed(ui::TreeSelection selection, gpointer data)
{
    g_DbgDlg.DropHighlight();

    GtkTreeModel *model;
    GtkTreeIter selected;
    if (gtk_tree_selection_get_selected(selection, &model, &selected)) {
        auto path = gtk_tree_model_get_path(model, &selected);
        g_DbgDlg.SetHighlight(gtk_tree_path_get_indices(path)[0]);
        gtk_tree_path_free(path);
    }
}

void CDbgDlg::DropHighlight()
{
    if (m_pHighlight != 0) {
        m_pHighlight->DropHighlight();
        m_pHighlight = 0;
        m_pDraw2D = 0;
    }
}

void CDbgDlg::SetHighlight(gint row)
{
    ISAXHandler *h = GetElement(row);
    if (h != NULL) {
        m_pDraw2D = h->Highlight();
        m_pHighlight = h;
    }
}

ISAXHandler *CDbgDlg::GetElement(std::size_t row)
{
    return static_cast<ISAXHandler *>(g_ptr_array_index(m_pFeedbackElements, gint( row )) );
}

void CDbgDlg::Init()
{
    DropHighlight();

    // free all the ISAXHandler*, clean it
    while (m_pFeedbackElements->len) {
        static_cast<ISAXHandler *>(g_ptr_array_index(m_pFeedbackElements, 0) )->Release();
        g_ptr_array_remove_index(m_pFeedbackElements, 0);
    }

    if (m_clist) {
        m_clist.clear();
    }
}

void CDbgDlg::Push(ISAXHandler *pHandler)
{
    // push in the list
    g_ptr_array_add(m_pFeedbackElements, (void *) pHandler);

    if (!GetWidget()) {
        Create();
    }

    // put stuff in the list
    m_clist.clear();
    for (std::size_t i = 0; i < static_cast<std::size_t>( m_pFeedbackElements->len ); ++i) {
        m_clist.append(0, GetElement(i)->getName());
    }

    ShowDlg();
}

ui::Window CDbgDlg::BuildDialog()
{
    auto window = MainFrame_getWindow().create_floating_window("Q3Map debug window");

    auto scr = ui::ScrolledWindow(ui::New);
    scr.show();
    window.add(scr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scr), GTK_SHADOW_IN);

    {
        ui::ListStore store = ui::ListStore::from(gtk_list_store_new(1, G_TYPE_STRING));

        auto view = ui::TreeView(ui::TreeModel::from(store._handle));
        gtk_tree_view_set_headers_visible(view, FALSE);

        {
            auto renderer = ui::CellRendererText(ui::New);
            auto column = ui::TreeViewColumn("", renderer, {{"text", 0}});
            gtk_tree_view_append_column(view, column);
        }

        {
            auto selection = ui::TreeSelection::from(gtk_tree_view_get_selection(view));
            gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
            selection.connect("changed", G_CALLBACK(feedback_selection_changed), NULL);
        }

        view.show();

        scr.add(view);

        store.unref();

        m_clist = store;
    }

    return window;
}
