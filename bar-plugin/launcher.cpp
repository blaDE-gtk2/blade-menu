/*
 * Copyright (C) 2013, 2014, 2015, 2016 Graeme Gott <graeme@gottcode.org>
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

#include "launcher.h"

#include "query.h"
#include "settings.h"

#include <blxo/blxo.h>
#include <libbladeui/libbladeui.h>

using namespace BladeMenu;

//-----------------------------------------------------------------------------

static std::string normalize(const gchar* string)
{
	std::string result;

	gchar* normalized = g_utf8_normalize(string, -1, G_NORMALIZE_DEFAULT);
	if (G_UNLIKELY(!normalized))
	{
		return result;
	}

	gchar* utf8 = g_utf8_casefold(normalized, -1);
	if (G_UNLIKELY(!utf8))
	{
		g_free(normalized);
		return result;
	}

	result = utf8;

	g_free(utf8);
	g_free(normalized);

	return result;
}

//-----------------------------------------------------------------------------

static void replace_with_quoted_string(std::string& command, size_t& index, const gchar* unquoted)
{
	if (!blxo_str_is_empty(unquoted))
	{
		gchar* quoted = g_shell_quote(unquoted);
		command.replace(index, 2, quoted);
		index += strlen(quoted);
		g_free(quoted);
	}
	else
	{
		command.erase(index, 2);
	}
}

//-----------------------------------------------------------------------------

static void replace_with_quoted_string(std::string& command, size_t& index, const char* prefix, const gchar* unquoted)
{
	if (!blxo_str_is_empty(unquoted))
	{
		command.replace(index, 2, prefix);
		index += strlen(prefix);

		gchar* quoted = g_shell_quote(unquoted);
		command.insert(index, quoted);
		index += strlen(quoted);
		g_free(quoted);
	}
	else
	{
		command.erase(index, 2);
	}
}

//-----------------------------------------------------------------------------

static void replace_and_free_with_quoted_string(std::string& command, size_t& index, gchar* unquoted)
{
	replace_with_quoted_string(command, index, unquoted);
	g_free(unquoted);
}

//-----------------------------------------------------------------------------

Launcher::Launcher(PojkMenuItem* item) :
	m_item(item),
	m_search_flags(0)
{
	// Fetch icon
	const gchar* icon = pojk_menu_item_get_icon_name(m_item);
	if (G_LIKELY(icon))
	{
		if (!g_path_is_absolute(icon))
		{
			gchar* pos = g_strrstr(icon, ".");
			if (!pos)
			{
				set_icon(icon);
			}
			else
			{
				gchar* suffix = g_utf8_casefold(pos, -1);
				if ((strcmp(suffix, ".png") == 0)
						|| (strcmp(suffix, ".xpm") == 0)
						|| (strcmp(suffix, ".svg") == 0)
						|| (strcmp(suffix, ".svgz") == 0))
				{
					set_icon(g_strndup(icon, pos - icon));
				}
				else
				{
					set_icon(icon);
				}
				g_free(suffix);
			}
		}
		else
		{
			set_icon(icon);
		}
	}

	// Fetch text
	const gchar* name = pojk_menu_item_get_name(m_item);
	if (G_UNLIKELY(!name) || !g_utf8_validate(name, -1, NULL))
	{
		name = "";
	}

	const gchar* generic_name = pojk_menu_item_get_generic_name(m_item);
	if (G_UNLIKELY(!generic_name) || !g_utf8_validate(generic_name, -1, NULL))
	{
		generic_name = "";
	}

	if (!wm_settings->launcher_show_name && !blxo_str_is_empty(generic_name))
	{
		std::swap(name, generic_name);
	}
	m_display_name = name;

	const gchar* details = pojk_menu_item_get_comment(m_item);
	if (!details || !g_utf8_validate(details, -1, NULL))
	{
		details = generic_name;
	}

	// Create display text
	const gchar* direction = (gtk_widget_get_default_direction() != GTK_TEXT_DIR_RTL) ? "\342\200\216" : "\342\200\217";
	if (wm_settings->launcher_show_description)
	{
		set_text(g_markup_printf_escaped("%s<b>%s</b>\n%s%s", direction, m_display_name, direction, details));
	}
	else
	{
		set_text(g_markup_printf_escaped("%s%s", direction, m_display_name));
	}
	set_tooltip(details);

	// Create search text for display name
	m_search_name = normalize(m_display_name);
	m_search_generic_name = normalize(generic_name);
	m_search_comment = normalize(details);

	// Create search text for command
	const gchar* command = pojk_menu_item_get_command(m_item);
	if (!blxo_str_is_empty(command) && g_utf8_validate(command, -1, NULL))
	{
		m_search_command = normalize(command);
	}

	// Fetch desktop actions
#ifdef POJK_TYPE_MENU_ITEM_ACTION
	GList* actions = pojk_menu_item_get_actions(m_item);
	for (GList* i = actions; i != NULL; i = i->next)
	{
		PojkMenuItemAction* action = pojk_menu_item_get_action(m_item, reinterpret_cast<gchar*>(i->data));
		if (action)
		{
			m_actions.push_back(new DesktopAction(action));
		}
	}
	g_list_free(actions);
#endif
}

//-----------------------------------------------------------------------------

Launcher::~Launcher()
{
	for (std::vector<DesktopAction*>::size_type i = 0, end = m_actions.size(); i < end; ++i)
	{
		delete m_actions[i];
	}
}

//-----------------------------------------------------------------------------

void Launcher::run(GdkScreen* screen) const
{
	const gchar* string = pojk_menu_item_get_command(m_item);
	if (blxo_str_is_empty(string))
	{
		return;
	}
	std::string command(string);

	if (pojk_menu_item_requires_terminal(m_item))
	{
		command.insert(0, "blxo-open --launch TerminalEmulator ");
	}

	// Expand the field codes
	size_t length = command.length() - 1;
	for (size_t i = 0; i < length; ++i)
	{
		if (G_UNLIKELY(command[i] == '%'))
		{
			switch (command[i + 1])
			{
			case 'i':
				replace_with_quoted_string(command, i, "--icon ", pojk_menu_item_get_icon_name(m_item));
				break;

			case 'c':
				replace_with_quoted_string(command, i, pojk_menu_item_get_name(m_item));
				break;

			case 'k':
				replace_and_free_with_quoted_string(command, i, pojk_menu_item_get_uri(m_item));
				break;

			case '%':
				command.erase(i, 1);
				break;

			case 'f':
				// unsupported, pass in a single file dropped on launcher
			case 'F':
				// unsupported, pass in a list of files dropped on launcher
			case 'u':
				// unsupported, pass in a single URL dropped on launcher
			case 'U':
				// unsupported, pass in a list of URLs dropped on launcher
			default:
				command.erase(i, 2);
				break;
			}
			length = command.length() - 1;
		}
	}

	// Parse and spawn command
	gchar** argv;
	gboolean result = false;
	GError* error = NULL;
	if (g_shell_parse_argv(command.c_str(), NULL, &argv, &error))
	{
		result = xfce_spawn_on_screen(screen,
				pojk_menu_item_get_path(m_item),
				argv, NULL, G_SPAWN_SEARCH_PATH,
				pojk_menu_item_supports_startup_notification(m_item),
				gtk_get_current_event_time(),
				pojk_menu_item_get_icon_name(m_item),
				&error);
		g_strfreev(argv);
	}

	if (G_UNLIKELY(!result))
	{
		xfce_dialog_show_error(NULL, error, _("Failed to execute command \"%s\"."), string);
		g_error_free(error);
	}
}

//-----------------------------------------------------------------------------

void Launcher::run(GdkScreen* screen, DesktopAction* action) const
{
	const gchar* string = action->get_command();
	if (blxo_str_is_empty(string))
	{
		return;
	}
	std::string command(string);

	// Expand the field codes
	size_t length = command.length() - 1;
	for (size_t i = 0; i < length; ++i)
	{
		if (G_UNLIKELY(command[i] == '%'))
		{
			switch (command[i + 1])
			{
			case 'i':
				replace_with_quoted_string(command, i, "--icon ", action->get_icon());
				break;

			case 'c':
				replace_with_quoted_string(command, i, action->get_name());
				break;

			case 'k':
				replace_and_free_with_quoted_string(command, i, pojk_menu_item_get_uri(m_item));
				break;

			case '%':
				command.erase(i, 1);
				break;

			case 'f':
				// unsupported, pass in a single file dropped on launcher
			case 'F':
				// unsupported, pass in a list of files dropped on launcher
			case 'u':
				// unsupported, pass in a single URL dropped on launcher
			case 'U':
				// unsupported, pass in a list of URLs dropped on launcher
			default:
				command.erase(i, 2);
				break;
			}
			length = command.length() - 1;
		}
	}

	// Parse and spawn command
	gchar** argv;
	gboolean result = false;
	GError* error = NULL;
	if (g_shell_parse_argv(command.c_str(), NULL, &argv, &error))
	{
		result = xfce_spawn_on_screen(screen,
				pojk_menu_item_get_path(m_item),
				argv, NULL, G_SPAWN_SEARCH_PATH,
				pojk_menu_item_supports_startup_notification(m_item),
				gtk_get_current_event_time(),
				action->get_icon(),
				&error);
		g_strfreev(argv);
	}

	if (G_UNLIKELY(!result))
	{
		xfce_dialog_show_error(NULL, error, _("Failed to execute command \"%s\"."), string);
		g_error_free(error);
	}
}

//-----------------------------------------------------------------------------

guint Launcher::search(const Query& query)
{
	// Prioritize matches in favorites and recent, then favories, and then recent
	const guint flags = 3 - m_search_flags;

	// Sort matches in names first
	guint match = query.match(m_search_name);
	if (match != G_MAXUINT)
	{
		return match | flags | 0x400;
	}

	match = query.match(m_search_generic_name);
	if (match != G_MAXUINT)
	{
		return match | flags | 0x800;
	}

	// Sort matches in comments next
	match = query.match(m_search_comment);
	if (match != G_MAXUINT)
	{
		return match | flags | 0x1000;
	}

	// Sort matches in executables last
	match = query.match(m_search_command);
	if (match != G_MAXUINT)
	{
		return match | flags | 0x2000;
	}

	return G_MAXUINT;
}

//-----------------------------------------------------------------------------

void Launcher::set_flag(SearchFlag flag, bool enabled)
{
	if (enabled)
	{
		m_search_flags |= flag;
	}
	else
	{
		m_search_flags &= ~flag;
	}
}

//-----------------------------------------------------------------------------
