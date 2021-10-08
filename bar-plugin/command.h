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

#ifndef BLADEMENU_COMMAND_H
#define BLADEMENU_COMMAND_H

#include <gtk/gtk.h>

namespace BladeMenu
{

class Command
{
public:
	Command(const gchar* icon, const gchar* text, const gchar* command, const gchar* error_text);
	~Command();

	GtkWidget* get_button();
	GtkWidget* get_menuitem();

	const gchar* get() const
	{
		return m_command;
	}

	bool get_shown() const
	{
		return m_shown;
	}

	const gchar* get_text() const
	{
		return m_text;
	}

	void set(const gchar* command);

	void set_shown(bool shown);

	void check();

	void activate();

private:
	GtkWidget* m_button;
	GtkWidget* m_menuitem;
	gchar* m_icon;
	gchar* m_text;
	gchar* m_command;
	gchar* m_error_text;
	int m_status;
	bool m_shown;
};

}

#endif // BLADEMENU_COMMAND_H
