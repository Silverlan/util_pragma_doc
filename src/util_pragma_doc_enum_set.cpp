/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <sharedutils/util_string.h>
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
std::shared_ptr<doc::EnumSet> doc::EnumSet::Create(const std::string &name,const Collection *collection)
{
	auto es = std::shared_ptr<EnumSet>{new EnumSet{collection}};
	es->m_name = name;
	return es;
}
doc::EnumSet::EnumSet(const Collection *collection)
	: BaseCollectionObject(collection)
{}
void doc::EnumSet::ReserveEnums(uint32_t n) {m_enums.reserve(n);}
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
void doc::EnumSet::Read(EnumSet &outEnumSet,const Collection &collection,std::shared_ptr<VFilePtrInternal> &f)
{
	outEnumSet = {&collection};

	outEnumSet.m_name = f->ReadString();
	outEnumSet.m_underlyingType = f->ReadString();
	auto numEnums = f->Read<uint32_t>();
	outEnumSet.m_enums.reserve(numEnums);
	for(auto i=decltype(numEnums){0u};i<numEnums;++i)
		outEnumSet.m_enums.push_back(Enum::Read(outEnumSet,f));
}
void doc::EnumSet::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
	f->WriteString(m_underlyingType);
	f->Write<uint32_t>(m_enums.size());
	for(auto &e : m_enums)
		e.Write(f);
}
const std::string &doc::EnumSet::GetName() const {return m_name;}
std::string doc::EnumSet::GetFullName() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	return name +'.' +GetName();
}
std::vector<doc::Enum> &doc::EnumSet::GetEnums() {return m_enums;}
const std::string &doc::EnumSet::GetUnderlyingType() const {return m_underlyingType;}
std::string doc::EnumSet::GetWikiURL() const
{
	auto *pCollection = GetCollection();
	if(pCollection == nullptr)
		return "";
	auto url = pCollection->GetWikiURL();
	return url +"_en_" +ustring::name_to_identifier(GetName());
}
#pragma optimize("",on)
