// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.doc;

using namespace pragma;

doc::Enum doc::Enum::Create(const EnumSet &es) { return Enum {es}; }
doc::Enum::Enum(const EnumSet &enumSet) : BaseCollectionObject(*enumSet.GetCollection()), m_enumSet {enumSet.shared_from_this()} {}
bool doc::Enum::operator==(const Enum &other) const { return m_name == other.m_name && m_value == other.m_value && m_type == other.m_type; }
const std::string &doc::Enum::GetName() const { return m_name; }
std::string doc::Enum::GetFullName() const
{
	if(m_enumSet.expired())
		return GetName();
	auto *pCollection = m_enumSet.lock()->GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	return name + '.' + GetName();
}
const std::string &doc::Enum::GetValue() const { return m_value; }
const std::string &doc::Enum::GetDescription() const { return m_description; }
doc::Enum::Type doc::Enum::GetType() const { return m_type; }
doc::GameStateFlags doc::Enum::GetGameStateFlags() const { return m_gameStateFlags; }
std::string doc::Enum::GetWikiURL() const { return (m_enumSet.expired() == false) ? detail::get_wiki_url(GetFullName(), false) : ""; }
const doc::EnumSet *doc::Enum::GetEnumSet() const { return m_enumSet.lock().get(); }
void doc::Enum::SetEnumSet(EnumSet &es) { m_enumSet = es.shared_from_this(); }
