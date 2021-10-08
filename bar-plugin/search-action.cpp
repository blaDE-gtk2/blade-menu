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

#include "search-action.h"

#include "query.h"
#include "settings.h"

#include <libbladeui/libbladeui.h>

using namespace BladeMenu;

//-----------------------------------------------------------------------------

SearchAction::SearchAction() :
	m_is_regex(false),
	m_show_description(true),
	m_regex(NULL)
{
	set_icon("folder-saved-search");
	update_text();
}

//-----------------------------------------------------------------------------

SearchAction::SearchAction(const gchar* name, const gchar* pattern, const gchar* command, bool is_regex, bool show_description) :
	m_name(name ? name : ""),
	m_pattern(pattern ? pattern : ""),
	m_command(command ? command : ""),
	m_is_regex(is_regex),
	m_show_description(show_description),
	m_regex(NULL)
{
	set_icon("folder-saved-search");
	update_text();
}

//-----------------------------------------------------------------------------

SearchAction::~SearchAction()
{
	if (m_regex)
	{
		g_regex_unref(m_regex);
	}
}

//-----------------------------------------------------------------------------

guint SearchAction::search(const Query& query)
{
	if (m_pattern.empty() || m_command.empty())
	{
		return false;
	}

	m_expanded_command.clear();

	const gchar* haystack = query.raw_query().c_str();
	guint found = !m_is_regex ? match_prefix(haystack) : match_regex(haystack);

	if ((found != G_MAXUINT) && (m_show_description != wm_settings->launcher_show_description))
	{
		m_show_description = wm_settings->launcher_show_description;
		update_text();
	}

	return found;
}

//-----------------------------------------------------------------------------

guint SearchAction::match_prefix(const gchar* haystack)
{
	if (!g_str_has_prefix(haystack, m_pattern.c_str()))
	{
		return G_MAXUINT;
	}

	gchar* trimmed = g_strdup(haystack + m_pattern.length());
	trimmed = g_strstrip(trimmed);

	gchar* uri = NULL;

	m_expanded_command = m_command;
	std::string::size_type pos = 0, lastpos = m_expanded_command.length() - 1;
	while ((pos = m_expanded_command.find('%', pos)) != std::string::npos)
	{
		if (pos == lastpos)
		{
			break;
		}

		switch (m_expanded_command[pos + 1])
		{
		case 's':
			m_expanded_command.replace(pos, 2, trimmed);
			pos += strlen(trimmed) + 1;
			break;

		case 'S':
			m_expanded_command.replace(pos, 2, haystack);
			pos += strlen(haystack) + 1;
			break;

		case 'u':
			if (!uri)
			{
				uri = g_uri_escape_string(trimmed, NULL, true);
			}
			m_expanded_command.replace(pos, 2, uri);
			pos += strlen(uri) + 1;
			break;

		case '%':
			m_expanded_command.erase(pos, 1);
			pos += 1;
			break;

		default:
			m_expanded_command.erase(pos, 2);
			break;
		}
	}

	g_free(trimmed);
	g_free(uri);

	return m_pattern.length();
}

//-----------------------------------------------------------------------------

guint SearchAction::match_regex(const gchar* haystack)
{
	guint found = G_MAXUINT;

	if (!m_regex)
	{
		m_regex = g_regex_new(m_pattern.c_str(), G_REGEX_OPTIMIZE, GRegexMatchFlags(0), NULL);
		if (!m_regex)
		{
			return found;
		}
	}
	GMatchInfo* match = NULL;
	if (g_regex_match(m_regex, haystack, GRegexMatchFlags(0), &match))
	{
		gchar* expanded = g_match_info_expand_references(match, m_command.c_str(), NULL);
		if (expanded)
		{
			m_expanded_command = expanded;
			g_free(expanded);
			found = m_pattern.length();
		}
	}
	if (match != NULL)
	{
		g_match_info_free(match);
	}

	return found;
}

//-----------------------------------------------------------------------------

void SearchAction::run(GdkScreen* screen) const
{
	GError* error = NULL;
	gboolean result = xfce_spawn_command_line_on_screen(screen, m_expanded_command.c_str(), FALSE, FALSE, &error);

	if (G_UNLIKELY(!result))
	{
		xfce_dialog_show_error(NULL, error, _("Failed to execute command \"%s\"."), m_expanded_command.c_str());
		g_error_free(error);
	}
}

//-----------------------------------------------------------------------------

void SearchAction::set_name(const gchar* name)
{
	if (!name || (m_name == name))
	{
		return;
	}

	m_name = name;
	wm_settings->set_modified();

	m_show_description = wm_settings->launcher_show_description;
	update_text();
}

//-----------------------------------------------------------------------------

void SearchAction::set_pattern(const gchar* pattern)
{
	if (!pattern || (m_pattern == pattern))
	{
		return;
	}

	m_pattern = pattern;
	wm_settings->set_modified();

	if (m_regex)
	{
		g_regex_unref(m_regex);
		m_regex = NULL;
	}
}

//-----------------------------------------------------------------------------

void SearchAction::set_command(const gchar* command)
{
	if (!command || (m_command == command))
	{
		return;
	}

	m_command = command;
	wm_settings->set_modified();
}

//-----------------------------------------------------------------------------

void SearchAction::set_is_regex(bool is_regex)
{
	if (m_is_regex == is_regex)
	{
		return;
	}

	m_is_regex = is_regex;
	wm_settings->set_modified();
}

//-----------------------------------------------------------------------------

void SearchAction::update_text()
{
	const gchar* direction = (gtk_widget_get_default_direction() != GTK_TEXT_DIR_RTL) ? "\342\200\216" : "\342\200\217";
	const gchar* description = _("Search Action");
	if (m_show_description)
	{
		set_text(g_markup_printf_escaped("%s<b>%s</b>\n%s%s", direction, m_name.c_str(), direction, description));
	}
	else
	{
		set_text(g_markup_printf_escaped("%s%s", direction, m_name.c_str()));
	}
	set_tooltip(description);
}

//-----------------------------------------------------------------------------
