// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;


module pragma.doc;

import pragma.string;

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
