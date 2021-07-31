/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

std::shared_ptr<doc::DerivedFrom> doc::DerivedFrom::Create(const std::string &name,DerivedFrom *optParent)
{
	auto derivedFrom = std::shared_ptr<DerivedFrom>{new DerivedFrom{}};
	derivedFrom->m_name = name;
	if(optParent)
		derivedFrom->m_parent = optParent->shared_from_this();
	return derivedFrom;
}
const std::string &doc::DerivedFrom::GetName() const {return m_name;}
const std::shared_ptr<doc::DerivedFrom> &doc::DerivedFrom::GetParent() const {return m_parent;}
