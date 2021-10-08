/*
 * Copyright (C) 2013, 2015 Graeme Gott <graeme@gottcode.org>
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

#ifndef BLADEMENU_APPLICATIONS_PAGE_H
#define BLADEMENU_APPLICATIONS_PAGE_H

#include "page.h"

#include <map>
#include <string>
#include <vector>

#include <pojk/pojk.h>

namespace BladeMenu
{

class Category;
class SectionButton;

class ApplicationsPage : public Page
{

public:
	explicit ApplicationsPage(Window* window);
	~ApplicationsPage();

	GtkTreeModel* create_launcher_model(std::vector<std::string>& desktop_ids) const;
	Launcher* get_application(const std::string& desktop_id) const;

	void invalidate_applications();
	void load_applications();
	void reload_category_icon_size();

private:
	void apply_filter(GtkToggleButton* togglebutton);
	void clear_applications();
	void load_pojk_menu();
	void load_contents();
	void load_menu(PojkMenu* menu, Category* parent_category);
	void load_menu_item(PojkMenuItem* menu_item, Category* category);

private:
	PojkMenu* m_pojk_menu;
	PojkMenu* m_pojk_settings_menu;
	std::vector<Category*> m_categories;
	std::map<std::string, Launcher*> m_items;
	int m_load_status;
};

}

#endif // BLADEMENU_APPLICATIONS_PAGE_H
