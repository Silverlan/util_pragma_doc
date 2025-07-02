// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <fsys/filesystem.h>

module pragma.doc;

using namespace pragma;

std::shared_ptr<doc::DerivedFrom> doc::DerivedFrom::Create(const std::string &name, DerivedFrom *optParent)
{
	auto derivedFrom = std::shared_ptr<DerivedFrom> {new DerivedFrom {}};
	derivedFrom->m_name = name;
	if(optParent)
		derivedFrom->m_parent = optParent->shared_from_this();
	return derivedFrom;
}
const std::string &doc::DerivedFrom::GetName() const { return m_name; }
const std::shared_ptr<doc::DerivedFrom> &doc::DerivedFrom::GetParent() const { return m_parent; }
