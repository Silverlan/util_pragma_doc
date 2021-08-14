/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
bool doc::Variant::operator==(const Variant &other) const
{
	return name == other.name && flags == other.flags && typeParameters == other.typeParameters;
}
bool doc::Variant::IsOptional() const
{
	return umath::is_flag_set(flags,Flags::Optional);
}
std::string doc::Variant::GetFormattedType(ParameterFormatType formatType,const std::function<void(const Variant&,std::string&)> &typeTranslator) const
{
	std::string s;

	auto tmpName = name;
	if(typeTranslator)
		typeTranslator(*this,tmpName);
	s += tmpName;

	auto first = true;
	for(auto &variant : typeParameters)
	{
		auto optional = variant.IsOptional();
		if(optional)
			s += "[";
		if(first)
			first = false;
		else
			s += "|";
		auto multiple = (variant.typeParameters.size() > 1 && !optional);
		if(multiple)
			s += "(";
		s += variant.GetFormattedType(formatType,typeTranslator);
		if(multiple)
			s += ")";
		if(optional)
			s += "]";
	}
	return s;
}
std::string doc::Variant::GetFullType() const
{
	auto fullType = name;

	if(!typeParameters.empty())
	{
		fullType += "<";
		auto first = true;
		for(auto &variant : typeParameters)
		{
			if(first)
				first = false;
			else
				fullType += ",";
			fullType += variant.GetFullType();
		}
		fullType += ">";
	}
	return fullType;
}
doc::Parameter doc::Parameter::Create(const std::string &name)
{
	Parameter param {};
	param.SetName(name);
	return param;
}
std::string doc::Parameter::GetFormattedParameterString(const std::vector<Parameter> &params,ParameterFormatType formatType,bool includeParameterNames,const std::function<void(const Variant&,std::string&)> &typeTranslator)
{
	std::string s = "";
	auto first = true;
	auto curOptional = false;
	for(auto &param : params)
	{
		auto &type = param.GetType();
		if(type.IsOptional())
		{
			curOptional = true;
			s += "[";
		}
		else if(curOptional)
		{
			curOptional = false;
			s += "]";
		}
		if(first)
			first = false;
		else
			s += ",";

		auto &name = param.GetName();
		if(includeParameterNames && !name.empty() && formatType == ParameterFormatType::ZeroBrane)
			s += name +": ";

		s += type.GetFormattedType(formatType,typeTranslator);

		if(includeParameterNames && !name.empty() && formatType == ParameterFormatType::Generic)
			s += " " +name;
	}
	if(curOptional)
		s += "]";
	return s;
}
bool doc::Parameter::operator==(const Parameter &other) const
{
	return m_name == other.m_name &&
		m_type == other.m_type;
}
const std::string &doc::Parameter::GetName() const {return m_name;}
const doc::Variant &doc::Parameter::GetType() const {return const_cast<Parameter*>(this)->GetType();}
doc::Variant &doc::Parameter::GetType() {return m_type;}
std::string doc::Parameter::GetFullType() const
{
	return ""; // TODO m_type.GetFullType();
}
const std::optional<std::string> &doc::Parameter::GetDefault() const {return m_default;}
doc::Parameter::Flags doc::Parameter::GetFlags() const {return m_flags;}
doc::GameStateFlags doc::Parameter::GetGameStateFlags() const {return m_gameStateFlags;}
void doc::Parameter::SetName(const std::string &name) {m_name = name;}
void doc::Parameter::SetType(const Variant &type) {m_type = type;}
void doc::Parameter::SetType(Variant &&type) {m_type = std::move(type);}
void doc::Parameter::SetDefault(const std::string &def) {m_default = def;}
void doc::Parameter::ClearDefault() {m_default = {};}
void doc::Parameter::SetFlags(Flags flags) {m_flags = flags;}
void doc::Parameter::SetGameStateFlags(GameStateFlags flags) {m_gameStateFlags = flags;}
std::string doc::Parameter::GetFormattedType() const
{
	std::string s;
	return s;
}
#pragma optimize("",on)
