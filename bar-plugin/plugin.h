/*
 * Copyright (C) 2013, 2014, 2015 Graeme Gott <graeme@gottcode.org>
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

#ifndef BLADEMENU_PLUGIN_H
#define BLADEMENU_PLUGIN_H

#define PLUGIN_WEBSITE "http://goodies.xfce.org/projects/bar-plugins/blade-menu-plugin"

#include <string>

#include <gtk/gtk.h>
#include <libbladebar/libbladebar.h>

namespace BladeMenu
{

class Window;

class Plugin
{
public:
	explicit Plugin(BladeBarPlugin* plugin);
	~Plugin();

	GtkWidget* get_button() const
	{
		return m_button;
	}

	enum ButtonStyle
	{
		ShowIcon = 0x1,
		ShowText = 0x2,
		ShowIconAndText = ShowIcon | ShowText
	};

	ButtonStyle get_button_style() const;
	std::string get_button_title() const;
	static std::string get_button_title_default();
	std::string get_button_icon_name() const;

	void reload();
	void set_button_style(ButtonStyle style);
	void set_button_title(const std::string& title);
	void set_button_icon_name(const std::string& icon);
	void set_configure_enabled(bool enabled);

private:
	void button_toggled(GtkToggleButton* button);
	void menu_hidden();
	void configure();
#if (LIBBLADEBAR_CHECK_VERSION(4,9,0))
	void mode_changed(BladeBarPlugin*, BladeBarPluginMode mode);
#else
	void orientation_changed(BladeBarPlugin*, GtkOrientation orientation);
#endif
	gboolean remote_event(BladeBarPlugin*, gchar* name, GValue* value);
	void save();
	void show_about();
	gboolean size_changed(BladeBarPlugin*, gint size);
	void update_size();
	void show_menu(GtkWidget* parent, bool horizontal);

private:
	BladeBarPlugin* m_plugin;
	Window* m_window;

	GtkWidget* m_button;
	GtkBox* m_button_box;
	GtkLabel* m_button_label;
	GtkImage* m_button_icon;

	int m_opacity;
};

}

#endif // BLADEMENU_PLUGIN_H
