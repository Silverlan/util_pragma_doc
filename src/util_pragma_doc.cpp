/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <array>
#include <sharedutils/util_string.h>
#include <fsys/filesystem.h>

using namespace pragma;

std::string pragma::detail::get_wiki_url(const std::string &fullPath, bool isFunOrMember)
{
	auto url = fullPath;
	ustring::replace(url, ".", "/");

	if(isFunOrMember) {
		auto p = url.rfind('/');
		if(p != std::string::npos)
			url[p] = '#';
	}
	url = "https://wiki.pragma-engine.com/api/docs/" + url;
	return url;
}
