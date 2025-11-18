// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.doc;

using namespace pragma;

doc::Function::Function(const Collection &collection) : BaseCollectionObject {&collection} {}
doc::Function doc::Function::Create(const Collection &collection, const std::string &name)
{
	Function fc {collection};
	fc.m_name = name;
	return fc;
}
bool doc::Function::operator==(const Function &other) const { return m_name == other.m_name && m_type == other.m_type; }
const std::string &doc::Function::GetName() const { return m_name; }
std::string doc::Function::GetFullName() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	switch(m_type) {
	case Type::Hook:
	case Type::Method:
		name += ':';
		break;
	default:
		name += '.';
		break;
	}
	return name + GetName();
}
const std::string &doc::Function::GetDescription() const { return m_description; }
doc::Function::Type doc::Function::GetType() const { return m_type; }
doc::Function::Flags doc::Function::GetFlags() const { return m_flags; }
doc::GameStateFlags doc::Function::GetGameStateFlags() const { return m_gameStateFlags; }
std::vector<doc::Overload> &doc::Function::GetOverloads() { return m_overloads; }
const std::optional<doc::Function::ExampleCode> &doc::Function::GetExampleCode() const { return m_exampleCode; }
const std::string &doc::Function::GetURL() const { return m_url; }
const std::vector<std::string> &doc::Function::GetRelated() const { return m_related; }
std::vector<doc::Group> &doc::Function::GetGroups() { return m_groups; }
void doc::Function::SetName(const std::string &name) { m_name = name; }
void doc::Function::SetDescription(const std::string &desc) { m_description = desc; }
void doc::Function::SetType(Type type) { m_type = type; }
void doc::Function::SetFlags(Flags flags) { m_flags = flags; }
void doc::Function::SetGameStateFlags(GameStateFlags flags) { m_gameStateFlags = flags; }
void doc::Function::AddOverload(const Overload &overload)
{
	if(m_overloads.size() == m_overloads.capacity())
		m_overloads.reserve(m_overloads.size() * 1.5f + 5);
	m_overloads.push_back(overload);
}
void doc::Function::SetExampleCode(const ExampleCode &code) { m_exampleCode = code; }
void doc::Function::ClearExampleCode() { m_exampleCode = {}; }
void doc::Function::SetURL(const std::string &url) { m_url = url; }
void doc::Function::AddRelated(const std::string &related) { m_related.push_back(related); }
void doc::Function::AddGroup(const Group &group) { m_groups.push_back(group); }
std::string doc::Function::GetWikiURL() const { return detail::get_wiki_url(GetFullName(), true); }
