/*
 * Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
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

#ifndef BLADEMENU_FAVORITES_PAGE_H
#define BLADEMENU_FAVORITES_PAGE_H

#include "page.h"

#include <vector>

namespace BladeMenu
{

class FavoritesPage : public Page
{
public:
	FavoritesPage(Window* window);
	~FavoritesPage();

	bool contains(Launcher* launcher) const;

	void add(Launcher* launcher);
	void remove(Launcher* launcher);
	void set_menu_items();
	void unset_menu_items();

private:
	void extend_context_menu(GtkWidget* menu);
	bool remember_launcher(Launcher* launcher);
	void on_row_changed(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter);
	void on_row_inserted(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter);
	void on_row_deleted(GtkTreeModel*, GtkTreePath* path);
	void sort(std::vector<Launcher*>& items) const;
	void sort_ascending();
	void sort_descending();
};

}

#endif // BLADEMENU_FAVORITES_PAGE_H
