/*
 * Copyright (C) 2013, 2017 Graeme Gott <graeme@gottcode.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BLADEMENU_PAGE_H
#define BLADEMENU_PAGE_H

#include <gtk/gtk.h>

namespace BladeMenu
{

class DesktopAction;
class Launcher;
class LauncherView;
class Window;

class Page
{
public:
	explicit Page(BladeMenu::Window *window);
	virtual ~Page();

	GtkWidget* get_widget() const
	{
		return m_widget;
	}

	LauncherView* get_view() const
	{
		return m_view;
	}

	void reset_selection();

protected:
	Window* get_window() const
	{
		return m_window;
	}

private:
	virtual bool remember_launcher(Launcher* launcher);
	void item_activated(GtkTreeView* view, GtkTreePath* path, GtkTreeViewColumn*);
	void item_action_activated(GtkMenuItem* menuitem, DesktopAction* action);
	gboolean view_button_press_event(GtkWidget* view, GdkEvent* event);
	gboolean view_popup_menu_event(GtkWidget* view);
	void on_unmap();
	void destroy_context_menu(GtkMenuShell* menu);
	void add_selected_to_desktop();
	void add_selected_to_bar();
	void add_selected_to_favorites();
	void edit_selected();
	void remove_selected_from_favorites();
	Launcher* get_selected_launcher() const;
	void create_context_menu(GtkTreeIter* iter, GdkEvent* event);
	virtual void extend_context_menu(GtkWidget* menu);
	static void position_context_menu(GtkMenu*, gint* x, gint* y, gboolean* push_in, Page* page);

private:
	Window* m_window;
	GtkWidget* m_widget;
	LauncherView* m_view;
	GtkTreePath* m_selected_path;
};

}

#endif // BLADEMENU_PAGE_H
