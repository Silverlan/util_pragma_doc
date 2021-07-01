/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
doc::Parameter doc::Parameter::Create(const std::string &name)
{
	Parameter param {};
	param.SetName(name);
	return param;
}
doc::Parameter doc::Parameter::Read(std::shared_ptr<VFilePtrInternal> &f)
{
	Parameter parameter {};
	parameter.m_name = f->ReadString();
	parameter.m_type = f->ReadString();
	auto bHasDefault = f->Read<bool>();
	if(bHasDefault)
		parameter.m_default = f->ReadString();
	auto bHasSubType = f->Read<bool>();
	if(bHasSubType)
		parameter.m_subType = f->ReadString();
	auto bHasSubSubType = f->Read<bool>();
	if(bHasSubSubType)
		parameter.m_subSubType = f->ReadString();
	parameter.m_flags = f->Read<Flags>();
	return parameter;
}
void doc::Parameter::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
	f->WriteString(m_type);

	f->Write<bool>(m_default.has_value());
	if(m_default.has_value())
		f->WriteString(*m_default);
	
	f->Write<bool>(m_subType.has_value());
	if(m_subType.has_value())
		f->WriteString(*m_subType);

	f->Write<bool>(m_subSubType.has_value());
	if(m_subSubType.has_value())
		f->WriteString(*m_subSubType);

	f->Write<Flags>(m_flags);
}
const std::string &doc::Parameter::GetName() const {return m_name;}
const std::string &doc::Parameter::GetType() const {return m_type;}
std::string doc::Parameter::GetFullType() const
{
	auto type = GetType();

	auto &subType = GetSubType();
	if(subType.has_value() == false)
		return type;
	type += ':' +*subType;

	auto &subSubType = GetSubSubType();
	if(subSubType.has_value() == false)
		return type;
	return type +':' +*subSubType;
}
const std::optional<std::string> &doc::Parameter::GetDefault() const {return m_default;}
const std::optional<std::string> &doc::Parameter::GetSubType() const {return m_subType;}
const std::optional<std::string> &doc::Parameter::GetSubSubType() const {return m_subSubType;}
doc::Parameter::Flags doc::Parameter::GetFlags() const {return m_flags;}
doc::GameStateFlags doc::Parameter::GetGameStateFlags() const {return m_gameStateFlags;}
void doc::Parameter::SetName(const std::string &name) {m_name = name;}
void doc::Parameter::SetType(const std::string &type) {m_type = type;}
void doc::Parameter::SetDefault(const std::string &def) {m_default = def;}
void doc::Parameter::ClearDefault() {m_default = {};}
void doc::Parameter::SetSubType(const std::string &subType) {m_subType = subType;}
void doc::Parameter::ClearSubType() {m_subType = {};}
void doc::Parameter::SetSubSubType(const std::string &subSubType) {m_subSubType = subSubType;}
void doc::Parameter::ClearSubSubType() {m_subSubType = {};}
void doc::Parameter::SetFlags(Flags flags) {m_flags = flags;}
void doc::Parameter::SetGameStateFlags(GameStateFlags flags) {m_gameStateFlags = flags;}
#pragma optimize("",on)
