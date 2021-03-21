#include "uilib.h"

#include <tuple>

#include <gtk/gtk.h>

#include "gtkutil/dialog.h"
#include "gtkutil/filechooser.h"
#include "gtkutil/messagebox.h"
#include "gtkutil/window.h"

namespace ui {

    bool init(int *argc, char **argv[], char const *parameter_string, char const **error)
    {
        gtk_disable_setlocale();
        static GOptionEntry entries[] = {{}};
        char const *translation_domain = NULL;
        GError *gerror = NULL;
        bool ret = gtk_init_with_args(argc, argv, parameter_string, entries, translation_domain, &gerror) != 0;
        if (!ret) {
            *error = gerror->message;
        }
        return ret;
    }

    void main()
    {
        gtk_main();
    }

    void process()
    {
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
    }

#define IMPL(T, F) template<> _IMPL(T, F)
#define _IMPL(T, F) struct verify<T *> { using self = T; static self test(self it) { return self::from(F(it)); } }

    template<class T>
    struct verify;

    template<class T> _IMPL(T,);

    template<class T>
    using pointer_remove_const = std::add_pointer<
            typename std::remove_const<
                    typename std::remove_pointer<T>::type
            >::type
    >;

#define this (verify<self>::test(*static_cast<self>(const_cast<pointer_remove_const<decltype(this)>::type>(this))))

    IMPL(Editable, GTK_EDITABLE);

    void IEditable::editable(bool value)
    {
        gtk_editable_set_editable(this, value);
    }

    IMPL(TreeModel, GTK_TREE_MODEL);

    IMPL(Widget, GTK_WIDGET);

    Widget::Widget(ui::New_t) : Widget(nullptr)
    {}

    Window IWidget::window()
    {
        return Window::from(gtk_widget_get_toplevel(this));
    }

    const char *
    IWidget::file_dialog(bool open, const char *title, const char *path, const char *pattern, bool want_load,
                         bool want_import, bool want_save)
    {
        return ::file_dialog(this.window(), open, title, path, pattern, want_load, want_import, want_save);
    }

    bool IWidget::visible()
    {
        return gtk_widget_get_visible(this) != 0;
    }

    void IWidget::visible(bool shown)
    {
        if (shown) {

#ifdef WORKAROUND_WINDOWS_GTK2_GLWIDGET
            /* workaround for gtk 2.24 issue: not displayed glwidget after toggle */
            GtkWidget* glwidget = GTK_WIDGET( g_object_get_data( G_OBJECT( this ), "glwidget" ) );
            if ( glwidget ){
                gtk_widget_hide( glwidget );
                gtk_widget_show( glwidget );
            }
#endif // WORKAROUND_WINDOWS_GTK2_GLWIDGET

            this.show();
        } else {
            this.hide();
        }
    }

    void IWidget::show()
    {
        gtk_widget_show(this);
    }

    void IWidget::hide()
    {
        gtk_widget_hide(this);
    }

    Dimensions IWidget::dimensions()
    {
        GtkAllocation allocation;
        gtk_widget_get_allocation(this, &allocation);
        return Dimensions{allocation.width, allocation.height};
    }

    void IWidget::dimensions(int width, int height)
    {
        gtk_widget_set_size_request(this, width, height);
    }

    void IWidget::destroy()
    {
        gtk_widget_destroy(this);
    }

    IMPL(Container, GTK_CONTAINER);

    void IContainer::add(Widget widget)
    {
        gtk_container_add(this, widget);
    }

    void IContainer::remove(Widget widget)
    {
        gtk_container_remove(this, widget);
    }

    IMPL(Bin, GTK_BIN);

    IMPL(Window, GTK_WINDOW);

    Window::Window(window_type type) : Window(GTK_WINDOW(gtk_window_new(
            type == window_type::TOP ? GTK_WINDOW_TOPLEVEL :
            type == window_type::POPUP ? GTK_WINDOW_POPUP :
            GTK_WINDOW_TOPLEVEL
    )))
    {}

    Window IWindow::create_dialog_window(const char *title, void func(), void *data, int default_w, int default_h)
    {
        return Window(::create_dialog_window(this, title, func, data, default_w, default_h));
    }

    Window IWindow::create_modal_dialog_window(const char *title, ModalDialog &dialog, int default_w, int default_h)
    {
        return Window(::create_modal_dialog_window(this, title, dialog, default_w, default_h));
    }

    Window IWindow::create_floating_window(const char *title)
    {
        return Window(::create_floating_window(title, this));
    }

    std::uint64_t IWindow::on_key_press(bool (*f)(Widget widget, _GdkEventKey *event, void *extra), void *extra)
    {
        using f_t = decltype(f);
        struct user_data {
            f_t f;
            void *extra;
        } *pass = new user_data{f, extra};
        auto dtor = [](user_data *data, GClosure *) {
            delete data;
        };
        auto func = [](_GtkWidget *widget, GdkEventKey *event, user_data *args) -> bool {
            return args->f(Widget::from(widget), event, args->extra);
        };
        auto clos = g_cclosure_new(G_CALLBACK(+func), pass, reinterpret_cast<GClosureNotify>(+dtor));
        return g_signal_connect_closure(G_OBJECT(this), "key-press-event", clos, false);
    }

    void IWindow::add_accel_group(AccelGroup group)
    {
        gtk_window_add_accel_group(this, group);
    }

    IMPL(Alignment, GTK_ALIGNMENT);

    Alignment::Alignment(float xalign, float yalign, float xscale, float yscale)
            : Alignment(GTK_ALIGNMENT(gtk_alignment_new(xalign, yalign, xscale, yscale)))
    {}

    IMPL(Frame, GTK_FRAME);

    Frame::Frame(const char *label) : Frame(GTK_FRAME(gtk_frame_new(label)))
    {}

    IMPL(Button, GTK_BUTTON);

    Button::Button(ui::New_t) : Button(GTK_BUTTON(gtk_button_new()))
    {}

    Button::Button(const char *label) : Button(GTK_BUTTON(gtk_button_new_with_label(label)))
    {}

    IMPL(ToggleButton, GTK_TOGGLE_BUTTON);

    bool IToggleButton::active() const
    {
        return gtk_toggle_button_get_active(this) != 0;
    }

    void IToggleButton::active(bool value)
    {
        gtk_toggle_button_set_active(this, value);
    }

    IMPL(CheckButton, GTK_CHECK_BUTTON);

    CheckButton::CheckButton(ui::New_t) : CheckButton(GTK_CHECK_BUTTON(gtk_check_button_new()))
    {}

    CheckButton::CheckButton(const char *label) : CheckButton(GTK_CHECK_BUTTON(gtk_check_button_new_with_label(label)))
    {}

    IMPL(MenuItem, GTK_MENU_ITEM);

    MenuItem::MenuItem(ui::New_t) : MenuItem(GTK_MENU_ITEM(gtk_menu_item_new()))
    {}

    MenuItem::MenuItem(const char *label, bool mnemonic) : MenuItem(
            GTK_MENU_ITEM((mnemonic ? gtk_menu_item_new_with_mnemonic : gtk_menu_item_new_with_label)(label)))
    {}

    IMPL(TearoffMenuItem, GTK_TEAROFF_MENU_ITEM);

    TearoffMenuItem::TearoffMenuItem(ui::New_t) : TearoffMenuItem(GTK_TEAROFF_MENU_ITEM(gtk_tearoff_menu_item_new()))
    {}

    IMPL(ComboBoxText, GTK_COMBO_BOX_TEXT);

    ComboBoxText::ComboBoxText(ui::New_t) : ComboBoxText(GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new()))
    {}

    IMPL(ScrolledWindow, GTK_SCROLLED_WINDOW);

    ScrolledWindow::ScrolledWindow(ui::New_t) : ScrolledWindow(GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr)))
    {}

    void IScrolledWindow::overflow(Policy x, Policy y)
    {
        gtk_scrolled_window_set_policy(this, static_cast<GtkPolicyType>(x), static_cast<GtkPolicyType>(y));
    }

    IMPL(Box, GTK_BOX);

    void IBox::pack_start(ui::Widget child, bool expand, bool fill, unsigned int padding)
    {
        gtk_box_pack_start(this, child, expand, fill, padding);
    }

    void IBox::pack_end(ui::Widget child, bool expand, bool fill, unsigned int padding)
    {
        gtk_box_pack_end(this, child, expand, fill, padding);
    }

    void IBox::set_child_packing(ui::Widget child, bool expand, bool fill, unsigned int padding, ui::Packing packing)
    {
        gtk_box_set_child_packing(this, child, expand, fill, padding, (GtkPackType) packing);
    }

    IMPL(VBox, GTK_VBOX);

    VBox::VBox(bool homogenous, int spacing) : VBox(GTK_VBOX(gtk_vbox_new(homogenous, spacing)))
    {}

    IMPL(HBox, GTK_HBOX);

    HBox::HBox(bool homogenous, int spacing) : HBox(GTK_HBOX(gtk_hbox_new(homogenous, spacing)))
    {}

    IMPL(HPaned, GTK_HPANED);

    HPaned::HPaned(ui::New_t) : HPaned(GTK_HPANED(gtk_hpaned_new()))
    {}

    IMPL(VPaned, GTK_VPANED);

    VPaned::VPaned(ui::New_t) : VPaned(GTK_VPANED(gtk_vpaned_new()))
    {}

    IMPL(Menu, GTK_MENU);

    Menu::Menu(ui::New_t) : Menu(GTK_MENU(gtk_menu_new()))
    {}

    IMPL(Table, GTK_TABLE);

    Table::Table(std::size_t rows, std::size_t columns, bool homogenous) : Table(
            GTK_TABLE(gtk_table_new(rows, columns, homogenous))
    )
    {}

    void ITable::attach(Widget child, TableAttach attach, TableAttachOptions options, TablePadding padding) {
        gtk_table_attach(this, child,
                         attach.left, attach.right, attach.top, attach.bottom,
                         static_cast<GtkAttachOptions>(options.x), static_cast<GtkAttachOptions>(options.y),
                         padding.x, padding.y
        );
    }

    IMPL(TextView, GTK_TEXT_VIEW);

    TextView::TextView(ui::New_t) : TextView(GTK_TEXT_VIEW(gtk_text_view_new()))
    {}

    void ITextView::text(char const *str)
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(this);
        gtk_text_buffer_set_text(buffer, str, -1);
    }

    IMPL(TreeView, GTK_TREE_VIEW);

    TreeView::TreeView(ui::New_t) : TreeView(GTK_TREE_VIEW(gtk_tree_view_new()))
    {}

    TreeView::TreeView(TreeModel model) : TreeView(GTK_TREE_VIEW(gtk_tree_view_new_with_model(model)))
    {}

    IMPL(Label, GTK_LABEL);

    Label::Label(const char *label) : Label(GTK_LABEL(gtk_label_new(label)))
    {}

    void ILabel::text(char const *str)
    {
        gtk_label_set_text(this, str);
    }

    IMPL(Image, GTK_IMAGE);

    Image::Image(ui::New_t) : Image(GTK_IMAGE(gtk_image_new()))
    {}

    IMPL(Entry, GTK_ENTRY);

    Entry::Entry(ui::New_t) : Entry(GTK_ENTRY(gtk_entry_new()))
    {}

    Entry::Entry(std::size_t max_length) : Entry(ui::New)
    {
        gtk_entry_set_max_length(this, static_cast<gint>(max_length));
    }

    char const *IEntry::text()
    {
        return gtk_entry_get_text(this);
    }

    void IEntry::text(char const *str)
    {
        return gtk_entry_set_text(this, str);
    }

    IMPL(SpinButton, GTK_SPIN_BUTTON);

    SpinButton::SpinButton(Adjustment adjustment, double climb_rate, std::size_t digits) : SpinButton(
            GTK_SPIN_BUTTON(gtk_spin_button_new(adjustment, climb_rate, digits)))
    {}

    IMPL(HScale, GTK_HSCALE);

    HScale::HScale(Adjustment adjustment) : HScale(GTK_HSCALE(gtk_hscale_new(adjustment)))
    {}

    HScale::HScale(double min, double max, double step) : HScale(GTK_HSCALE(gtk_hscale_new_with_range(min, max, step)))
    {}

    IMPL(Adjustment, GTK_ADJUSTMENT);

    Adjustment::Adjustment(double value,
                           double lower, double upper,
                           double step_increment, double page_increment,
                           double page_size)
            : Adjustment(
            GTK_ADJUSTMENT(gtk_adjustment_new(value, lower, upper, step_increment, page_increment, page_size)))
    {}

    IMPL(CellRendererText, GTK_CELL_RENDERER_TEXT);

    CellRendererText::CellRendererText(ui::New_t) : CellRendererText(GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new()))
    {}

    IMPL(TreeViewColumn, GTK_TREE_VIEW_COLUMN);

    TreeViewColumn::TreeViewColumn(const char *title, CellRenderer renderer,
                                   std::initializer_list<TreeViewColumnAttribute> attributes)
            : TreeViewColumn(gtk_tree_view_column_new_with_attributes(title, renderer, nullptr))
    {
        for (auto &it : attributes) {
            gtk_tree_view_column_add_attribute(this, renderer, it.attribute, it.column);
        }
    }

    IMPL(AccelGroup, GTK_ACCEL_GROUP);

    AccelGroup::AccelGroup(ui::New_t) : AccelGroup(GTK_ACCEL_GROUP(gtk_accel_group_new()))
    {}

    IMPL(ListStore, GTK_LIST_STORE);

    void IListStore::clear()
    {
        gtk_list_store_clear(this);
    }

    void IListStore::append()
    {
        gtk_list_store_append(this, nullptr);
    }

    IMPL(TreeStore, GTK_TREE_STORE);

    // IMPL(TreePath, GTK_TREE_PATH);

    TreePath::TreePath(ui::New_t) : TreePath(gtk_tree_path_new())
    {}

    TreePath::TreePath(const char *path) : TreePath(gtk_tree_path_new_from_string(path))
    {}

    // Custom

#if GTK_TARGET == 3

    IMPL(GLArea, (void *));

#elif GTK_TARGET == 2

    IMPL(GLArea, GTK_DRAWING_AREA);

#endif

    guint IGLArea::on_render(GCallback pFunction, void *data)
    {
#if GTK_TARGET == 3
        return this.connect("render", pFunction, data);
#endif
#if GTK_TARGET == 2
        return this.connect("expose_event", pFunction, data);
#endif
    }

    // global

    Window root{ui::null};

    alert_response alert(Window parent, std::string text, std::string title, alert_type type, alert_icon icon)
    {
        auto ret = gtk_MessageBox(parent, text.c_str(),
                                  title.c_str(),
                                  type == alert_type::OK ? eMB_OK :
                                  type == alert_type::OKCANCEL ? eMB_OKCANCEL :
                                  type == alert_type::YESNO ? eMB_YESNO :
                                  type == alert_type::YESNOCANCEL ? eMB_YESNOCANCEL :
                                  type == alert_type::NOYES ? eMB_NOYES :
                                  eMB_OK,
                                  icon == alert_icon::Default ? eMB_ICONDEFAULT :
                                  icon == alert_icon::Error ? eMB_ICONERROR :
                                  icon == alert_icon::Warning ? eMB_ICONWARNING :
                                  icon == alert_icon::Question ? eMB_ICONQUESTION :
                                  icon == alert_icon::Asterisk ? eMB_ICONASTERISK :
                                  eMB_ICONDEFAULT
        );
        return
                ret == eIDOK ? alert_response::OK :
                ret == eIDCANCEL ? alert_response::CANCEL :
                ret == eIDYES ? alert_response::YES :
                ret == eIDNO ? alert_response::NO :
                alert_response::OK;
    }

}
