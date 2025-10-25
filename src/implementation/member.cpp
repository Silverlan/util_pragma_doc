// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <string>
#include <optional>

module pragma.doc;

using namespace pragma;

doc::Member::Member(const Collection &collection) : BaseCollectionObject(collection) {}
doc::Member doc::Member::Create(const Collection &collection, const std::string &name)
{
	Member member {collection};
	member.SetName(name);
	return member;
}
bool doc::Member::operator==(const Member &other) const { return m_name == other.m_name && m_type == other.m_type; }
const std::string &doc::Member::GetName() const { return m_name; }
std::string doc::Member::GetFullName() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	return name + '.' + GetName();
}
const doc::Variant &doc::Member::GetType() const { return m_type; }
const std::string &doc::Member::GetDescription() const { return m_description; }
doc::GameStateFlags doc::Member::GetGameStateFlags() const { return m_gameStateFlags; }
const std::optional<std::string> &doc::Member::GetDefault() const { return m_default; }
doc::Member::Mode doc::Member::GetMode() const { return m_mode; }
std::string doc::Member::GetWikiURL() const { return detail::get_wiki_url(GetFullName(), true); }
void doc::Member::SetName(const std::string &name) { m_name = name; }
void doc::Member::SetType(const Variant &type) { m_type = type; }
void doc::Member::SetType(Variant &&type) { m_type = std::move(type); }
void doc::Member::SetDescription(const std::string &desc) { m_description = desc; }
void doc::Member::SetGameStateFlags(GameStateFlags flags) { m_gameStateFlags = flags; }
void doc::Member::SetDefault(const std::string &def) { m_default = def; }
void doc::Member::ClearDefault() { m_default = {}; }
void doc::Member::SetMode(Mode mode) { m_mode = mode; }
