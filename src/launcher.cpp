// Copyright (C) 2013 Graeme Gott <graeme@gottcode.org>
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library.  If not, see <http://www.gnu.org/licenses/>.


#include "launcher.hpp"

extern "C"
{
#include <exo/exo.h>
#include <libxfce4ui/libxfce4ui.h>
}

using namespace WhiskerMenu;

//-----------------------------------------------------------------------------

static bool f_show_name = true;
static bool f_show_description = true;

//-----------------------------------------------------------------------------

static void replace_with_quoted_string(std::string& command, size_t& index, const gchar* unquoted)
{
	if (!exo_str_is_empty(unquoted))
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
	if (!exo_str_is_empty(unquoted))
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

static void replace_with_quoted_string(std::string& command, size_t& index, gchar* unquoted)
{
	replace_with_quoted_string(command, index, unquoted);
	g_free(unquoted);
}

//-----------------------------------------------------------------------------

Launcher::Launcher(GarconMenuItem* item) :
	m_item(item),
	m_icon(nullptr),
	m_text(nullptr)
{
	garcon_menu_item_ref(m_item);

	// Fetch icon
	const gchar* icon = garcon_menu_item_get_icon_name(m_item);
	if (G_LIKELY(icon))
	{
		if (!g_path_is_absolute(icon))
		{
			gchar* pos = g_strrstr(icon, ".");
			m_icon = !pos ? g_strdup(icon) : g_strndup(icon, pos - icon);
		}
		else
		{
			m_icon = g_strdup(icon);
		}
	}

	// Fetch text
	const gchar* name = garcon_menu_item_get_name(m_item);
	if (G_UNLIKELY(!name))
	{
		name = "";
	}

	const gchar* generic_name = garcon_menu_item_get_generic_name(m_item);
	if (G_UNLIKELY(!generic_name))
	{
		generic_name = "";
	}

	// Create display text
	const gchar* display_name = (f_show_name || exo_str_is_empty(generic_name)) ? name : generic_name;
	if (f_show_description)
	{
		const gchar* details = garcon_menu_item_get_comment(m_item);
		if (!details)
		{
			details = generic_name;
		}
		m_text = g_markup_printf_escaped("<b>%s</b>\n%s", display_name, details);
	}
	else
	{
		m_text = g_markup_printf_escaped("%s", display_name);
	}
}

//-----------------------------------------------------------------------------

Launcher::~Launcher()
{
	garcon_menu_item_unref(m_item);
	g_free(m_icon);
	g_free(m_text);
}

//-----------------------------------------------------------------------------

const gchar* Launcher::get_search_text()
{
	if (!m_search_text.empty())
	{
		return m_search_text.c_str();
	}

	// Combine name, comment, and generic name into single casefolded string
	const gchar* name = garcon_menu_item_get_name(m_item);
	if (name)
	{
		gchar* normalized = g_utf8_normalize(name, -1, G_NORMALIZE_DEFAULT);
		gchar* utf8 = g_utf8_casefold(normalized, -1);
		m_search_text += utf8;
		g_free(utf8);
		g_free(normalized);
		m_search_text += '\n';
	}

	const gchar* generic_name = garcon_menu_item_get_generic_name(m_item);
	if (generic_name)
	{
		gchar* normalized = g_utf8_normalize(generic_name, -1, G_NORMALIZE_DEFAULT);
		gchar* utf8 = g_utf8_casefold(normalized, -1);
		m_search_text += utf8;
		g_free(utf8);
		g_free(normalized);
		m_search_text += '\n';
	}

	const gchar* comment = garcon_menu_item_get_comment(m_item);
	if (comment)
	{
		gchar* normalized = g_utf8_normalize(comment, -1, G_NORMALIZE_DEFAULT);
		gchar* utf8 = g_utf8_casefold(normalized, -1);
		m_search_text += utf8;
		g_free(utf8);
		g_free(normalized);
	}

	return m_search_text.c_str();
}

//-----------------------------------------------------------------------------

void Launcher::run(GdkScreen* screen) const
{
	const gchar* string = garcon_menu_item_get_command(m_item);
	if (exo_str_is_empty(string))
	{
		return;
	}
	std::string command(string);

	if (garcon_menu_item_requires_terminal(m_item))
	{
		command.insert(0, "exo-open --launch TerminalEmulator ");
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
				replace_with_quoted_string(command, i, "--icon ", garcon_menu_item_get_icon_name(m_item));
				length = command.length() - 1;
				break;

			case 'c':
				replace_with_quoted_string(command, i, garcon_menu_item_get_name(m_item));
				length = command.length() - 1;
				break;

			case 'k':
				replace_with_quoted_string(command, i, garcon_menu_item_get_uri(m_item));
				length = command.length() - 1;
				break;

			case '%':
				command.erase(i, 1);
				length = command.length() - 1;
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
				length = command.length() - 1;
				break;
			}
		}
	}

	// Parse and spawn command
	gchar** argv;
	gboolean result = false;
	GError* error = nullptr;
	if (g_shell_parse_argv(command.c_str(), nullptr, &argv, &error))
	{
		result = xfce_spawn_on_screen(screen,
				garcon_menu_item_get_path(m_item),
				argv, nullptr, G_SPAWN_SEARCH_PATH,
				garcon_menu_item_supports_startup_notification(m_item),
				gtk_get_current_event_time(),
				garcon_menu_item_get_icon_name(m_item),
				&error);
		g_strfreev(argv);
	}

	if (G_UNLIKELY(!result))
	{
		xfce_dialog_show_error(nullptr, error, _("Failed to execute command \"%s\"."), string);
		g_error_free(error);
	}
}

//-----------------------------------------------------------------------------

bool Launcher::get_show_name()
{
	return f_show_name;
}

//-----------------------------------------------------------------------------

bool Launcher::get_show_description()
{
	return f_show_description;
}

//-----------------------------------------------------------------------------

void Launcher::set_show_name(bool show)
{
	f_show_name = show;
}

//-----------------------------------------------------------------------------

void Launcher::set_show_description(bool show)
{
	f_show_description = show;
}

//-----------------------------------------------------------------------------
