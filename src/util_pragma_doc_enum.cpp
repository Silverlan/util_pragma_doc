/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
doc::Enum doc::Enum::Create(const EnumSet &es) {return Enum{es};}
doc::Enum::Enum(const EnumSet &enumSet)
	: BaseCollectionObject(*enumSet.GetCollection()),m_enumSet{enumSet.shared_from_this()}
{}
doc::Enum doc::Enum::Read(const EnumSet &enumSet,std::shared_ptr<VFilePtrInternal> &f)
{
	Enum e {enumSet};
	e.m_name = f->ReadString();
	e.m_value = f->ReadString();
	e.m_type = f->Read<Type>();
	e.m_gameStateFlags = f->Read<GameStateFlags>();
	e.m_description = f->ReadString();
	return e;
}
void doc::Enum::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
	f->WriteString(m_value);
	f->Write<Type>(m_type);
	f->Write<GameStateFlags>(m_gameStateFlags);
	f->WriteString(m_description);
}
const std::string &doc::Enum::GetName() const {return m_name;}
std::string doc::Enum::GetFullName() const
{
	if(m_enumSet.expired())
		return GetName();
	auto *pCollection = m_enumSet.lock()->GetCollection();
	if(pCollection == nullptr)
		return GetName();
	auto name = pCollection->GetFullName();
	return name +'.' +GetName();
}
const std::string &doc::Enum::GetValue() const {return m_value;}
const std::string &doc::Enum::GetDescription() const {return m_description;}
doc::Enum::Type doc::Enum::GetType() const {return m_type;}
doc::GameStateFlags doc::Enum::GetGameStateFlags() const {return m_gameStateFlags;}
std::string doc::Enum::GetWikiURL() const {return (m_enumSet.expired() == false) ? m_enumSet.lock()->GetWikiURL() : "";}
const doc::EnumSet *doc::Enum::GetEnumSet() const {return m_enumSet.lock().get();}
void doc::Enum::SetEnumSet(EnumSet &es) {m_enumSet = es.shared_from_this();}
#pragma optimize("",on)
