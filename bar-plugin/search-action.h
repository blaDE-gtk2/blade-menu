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

#ifndef BLADEMENU_SEARCH_ACTION_H
#define BLADEMENU_SEARCH_ACTION_H

#include "element.h"

#include <string>

namespace BladeMenu
{

class SearchAction : public Element
{
public:
	SearchAction();
	SearchAction(const gchar* name, const gchar* pattern, const gchar* command, bool is_regex, bool show_description);
	~SearchAction();

	enum
	{
		Type = 3
	};
	int get_type() const
	{
		return Type;
	}

	const gchar* get_name() const
	{
		return m_name.c_str();
	}

	const gchar* get_pattern() const
	{
		return m_pattern.c_str();
	}

	const gchar* get_command() const
	{
		return m_command.c_str();
	}

	bool get_is_regex() const
	{
		return m_is_regex;
	}

	void run(GdkScreen* screen) const;
	guint search(const Query& query);

	void set_name(const gchar* name);
	void set_pattern(const gchar* pattern);
	void set_command(const gchar* command);
	void set_is_regex(bool is_regex);

private:
	guint match_prefix(const gchar* haystack);
	guint match_regex(const gchar* haystack);
	void update_text();

private:
	std::string m_name;
	std::string m_pattern;
	std::string m_command;
	bool m_is_regex;
	bool m_show_description;

	std::string m_expanded_command;
	GRegex* m_regex;
};

}

#endif // BLADEMENU_SEARCH_ACTION_H
