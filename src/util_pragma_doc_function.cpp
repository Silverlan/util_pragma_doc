/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>
#include <sharedutils/util_string.h>

using namespace pragma;

#pragma optimize("",off)
doc::Function::Function(const Collection &collection)
	: BaseCollectionObject{&collection}
{}
doc::Function doc::Function::Create(const Collection &collection,const std::string &name)
{
	Function fc {collection};
	fc.m_name = name;
	return fc;
}
doc::Function doc::Function::Read(const Collection &collection,std::shared_ptr<VFilePtrInternal> &f)
{
	Function function {collection};
	function.m_name = f->ReadString();
	function.m_description = f->ReadString();
	function.m_type = f->Read<Type>();
	function.m_flags = f->Read<Flags>();
	function.m_gameStateFlags = f->Read<GameStateFlags>();

	auto numOverloads = f->Read<uint32_t>();
	function.m_overloads.reserve(numOverloads);
	for(auto i=decltype(numOverloads){0u};i<numOverloads;++i)
		function.m_overloads.push_back(Overload::Read(f));

	auto bHasExampleCode = f->Read<bool>();
	if(bHasExampleCode == true)
	{
		ExampleCode exampleCode {};
		exampleCode.code = f->ReadString();
		exampleCode.description = f->ReadString();
		function.m_exampleCode = exampleCode;
	}
	function.m_url = f->ReadString();
	
	auto numRelated = f->Read<uint32_t>();
	function.m_related.reserve(numRelated);
	for(auto i=decltype(numRelated){0u};i<numRelated;++i)
		function.m_related.push_back(f->ReadString());

	auto numGroups = f->Read<uint32_t>();
	function.m_groups.reserve(numGroups);
	for(auto i=decltype(numGroups){0u};i<numGroups;++i)
		function.m_groups.push_back(Group::Read(f));
	return function;
}
void doc::Function::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
	f->WriteString(m_description);
	f->Write<Type>(m_type);
	f->Write<Flags>(m_flags);
	f->Write<GameStateFlags>(m_gameStateFlags);

	f->Write<uint32_t>(m_overloads.size());
	for(auto &overload : m_overloads)
		overload.Write(f);

	f->Write<bool>(m_exampleCode.has_value());
	if(m_exampleCode.has_value())
	{
		f->WriteString(m_exampleCode->code);
		f->WriteString(m_exampleCode->description);
	}

	f->WriteString(m_url);

	f->Write<uint32_t>(m_related.size());
	for(auto &related : m_related)
		f->WriteString(related);
	
	f->Write<uint32_t>(m_groups.size());
	for(auto &group : m_groups)
		group.Write(f);
}
const std::string &doc::Function::GetName() const {return m_name;}
std::string doc::Function::GetFullName() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	switch(m_type)
	{
		case Type::Hook:
		case Type::Method:
			name += ':';
			break;
		default:
			name += '.';
			break;
	}
	return name +GetName();
}
const std::string &doc::Function::GetDescription() const {return m_description;}
doc::Function::Type doc::Function::GetType() const {return m_type;}
doc::Function::Flags doc::Function::GetFlags() const {return m_flags;}
doc::GameStateFlags doc::Function::GetGameStateFlags() const {return m_gameStateFlags;}
const std::vector<doc::Overload> &doc::Function::GetOverloads() const {return m_overloads;}
const std::optional<doc::Function::ExampleCode> &doc::Function::GetExampleCode() const {return m_exampleCode;}
const std::string &doc::Function::GetURL() const {return m_url;}
const std::vector<std::string> &doc::Function::GetRelated() const {return m_related;}
std::vector<doc::Group> &doc::Function::GetGroups() {return m_groups;}
void doc::Function::SetName(const std::string &name) {m_name = name;}
void doc::Function::SetDescription(const std::string &desc) {m_description = desc;}
void doc::Function::SetType(Type type) {m_type = type;}
void doc::Function::SetFlags(Flags flags) {m_flags = flags;}
void doc::Function::SetGameStateFlags(GameStateFlags flags) {m_gameStateFlags = flags;}
void doc::Function::AddOverload(const Overload &overload)
{
	if(m_overloads.size() == m_overloads.capacity())
		m_overloads.reserve(m_overloads.size() *1.5f +5);
	m_overloads.push_back(overload);
}
void doc::Function::SetExampleCode(const ExampleCode &code) {m_exampleCode = code;}
void doc::Function::ClearExampleCode() {m_exampleCode = {};}
void doc::Function::SetURL(const std::string &url) {m_url = url;}
void doc::Function::AddRelated(const std::string &related) {m_related.push_back(related);}
void doc::Function::AddGroup(const Group &group) {m_groups.push_back(group);}
std::string doc::Function::GetWikiURL() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return "";
	auto url = pCollection->GetWikiURL();
	return url +"_fc_" +ustring::name_to_identifier(GetName());
}
#pragma optimize("",on)
