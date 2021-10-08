/*
 * Copyright (C) 2013, 2015, 2016 Graeme Gott <graeme@gottcode.org>
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

#ifndef BLADEMENU_LAUNCHER_H
#define BLADEMENU_LAUNCHER_H

#include "element.h"

#include <string>
#include <vector>

#include <pojk/pojk.h>

namespace BladeMenu
{

class DesktopAction
{
#ifdef POJK_TYPE_MENU_ITEM_ACTION

public:
	DesktopAction(PojkMenuItemAction* action) :
		m_action(action)
	{
	}

	const gchar* get_name() const
	{
		return pojk_menu_item_action_get_name(m_action);
	}

	const gchar* get_icon() const
	{
		return pojk_menu_item_action_get_icon_name(m_action);
	}

	const gchar* get_command() const
	{
		return pojk_menu_item_action_get_command(m_action);
	}

private:
	PojkMenuItemAction* m_action;

#else

public:
	const gchar* get_name() const
	{
		return NULL;
	}

	const gchar* get_icon() const
	{
		return NULL;
	}

	const gchar* get_command() const
	{
		return NULL;
	}

#endif
};

class Launcher : public Element
{
public:
	explicit Launcher(PojkMenuItem* item);
	~Launcher();

	enum
	{
		Type = 2
	};
	int get_type() const
	{
		return Type;
	}

	std::vector<DesktopAction*> get_actions() const
	{
		return m_actions;
	}

	const gchar* get_display_name() const
	{
		return m_display_name;
	}

	const gchar* get_desktop_id() const
	{
		return pojk_menu_item_get_desktop_id(m_item);
	}

	GFile* get_file() const
	{
		return pojk_menu_item_get_file(m_item);
	}

	gchar* get_uri() const
	{
		return pojk_menu_item_get_uri(m_item);
	}

	void run(GdkScreen* screen) const;

	void run(GdkScreen* screen, DesktopAction* action) const;

	guint search(const Query& query);

	enum SearchFlag
	{
		RecentFlag = 0x1,
		FavoriteFlag = 0x2
	};
	void set_flag(SearchFlag flag, bool enabled);

private:
	PojkMenuItem* m_item;
	const gchar* m_display_name;
	std::string m_search_name;
	std::string m_search_generic_name;
	std::string m_search_comment;
	std::string m_search_command;
	guint m_search_flags;
	std::vector<DesktopAction*> m_actions;
};

}

#endif // BLADEMENU_LAUNCHER_H
