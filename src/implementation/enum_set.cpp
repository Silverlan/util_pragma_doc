/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <sharedutils/util_string.h>
#include <fsys/filesystem.h>

module pragma.doc;

using namespace pragma;

std::shared_ptr<doc::EnumSet> doc::EnumSet::Create(const std::string &name, const Collection *collection)
{
	auto es = std::shared_ptr<EnumSet> {new EnumSet {collection}};
	es->m_name = name;
	return es;
}
doc::EnumSet::EnumSet(const Collection *collection) : BaseCollectionObject(collection) {}
bool doc::EnumSet::operator==(const EnumSet &other) const { return m_name == other.m_name && m_underlyingType == other.m_underlyingType; }
void doc::EnumSet::ReserveEnums(uint32_t n) { m_enums.reserve(n); }
void doc::EnumSet::AddEnum(const Enum &e)
{
	m_enums.push_back(e);
	m_enums.back().m_enumSet = shared_from_this();
}
void doc::EnumSet::AddEnum(Enum &&e)
{
	m_enums.push_back(std::move(e));
	m_enums.back().m_enumSet = shared_from_this();
}
const std::string &doc::EnumSet::GetName() const { return m_name; }
std::string doc::EnumSet::GetFullName() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	return name + '.' + GetName();
}
std::vector<doc::Enum> &doc::EnumSet::GetEnums() { return m_enums; }
const std::string &doc::EnumSet::GetUnderlyingType() const { return m_underlyingType; }
std::string doc::EnumSet::GetWikiURL() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return "";
	return detail::get_wiki_url(GetFullName(), false);
}
