/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <sharedutils/util_string.h>
#include <sstream>

static std::string normalize_text(const std::string &text)
{
	auto normalizedText = text;
	ustring::replace(normalizedText,"[[","\\[\\[");
	ustring::replace(normalizedText,"]]","\\]\\]");
	return normalizedText;
}

static void write_zerobrane_function(std::stringstream &ss,const pragma::doc::Function &function,const std::string &t)
{
	std::string type {};
	switch(function.GetType())
	{
		case pragma::doc::Function::Type::Method:
		case pragma::doc::Function::Type::Hook:
			type = "method";
			break;
		default:
			type = "function";
			break;
	}
	ss<<t<<"[\""<<function.GetName()<<"\"] = {\n";
	ss<<t<<"\ttype = \""<<type<<"\",\n";
	ss<<t<<"\tdescription = [["<<normalize_text(function.GetDescription())<<"]],\n";

	auto &overloads = function.GetOverloads();
	if(overloads.empty() == false)
	{
		auto &overload = overloads.front();
		ss<<t<<"\targs = \"(";
		auto bFirst = true;
		auto numOptional = 0u;
		for(auto &param : overload.GetParameters())
		{
			auto bOptional = param.GetDefault().has_value();
			if(bOptional == true)
			{
				if(bFirst == false)
					ss<<" ";
				ss<<"[";
				++numOptional;
			}
			if(bFirst == false)
				ss<<", ";
			else
				bFirst = false;
			
			ss<<param.GetName()<<": "<<param.GetType();
		}
		for(auto i=decltype(numOptional){0u};i<numOptional;++i)
			ss<<"]";
		ss<<")\",\n";

		ss<<t<<"\treturns = \"(";
		bFirst = true;
		auto &returnValues = overload.GetReturnValues();
		for(auto &returnValue : returnValues)
		{
			if(bFirst == false)
				ss<<", ";
			else
				bFirst = false;
			ss<<returnValue.GetName()<<": "<<returnValue.GetType();
		}
		ss<<")\"";
		if(returnValues.empty() == false)
			ss<<",\n"<<t<<"\tvaluetype = \"" +returnValues.front().GetType() +"\"\n";
		else
			ss<<"\n";
	}
	else
	{
		ss<<t<<"\targs = \"()\",\n";
		ss<<t<<"\treturns = \"(void)\"\n";
	}
	ss<<t<<"}";
}

static void write_zerobrane_member(std::stringstream &ss,const pragma::doc::Member &member,const std::string &t)
{
	ss<<t<<"[\""<<member.GetName()<<"\"] = {\n";
	ss<<t<<"\ttype = \"value\",\n";
	ss<<t<<"\tdescription = [["<<normalize_text(member.GetDescription())<<"]],\n";
	ss<<t<<"\tvaluetype = \""<<member.GetType()<<"\"\n";
	ss<<t<<"}";
}

static void write_zerobrane_enum(std::stringstream &ss,const pragma::doc::EnumSet &enumSet,const pragma::doc::Enum &e,const std::string &t)
{
	ss<<t<<"[\""<<e.GetName()<<"\"] = {\n";
	ss<<t<<"\ttype = \"value\",\n";
	ss<<t<<"\tdescription = [["<<normalize_text(e.GetDescription())<<"]],\n";
	ss<<t<<"\tvaluetype = \""<<enumSet.GetUnderlyingType()<<"\"\n";
	ss<<t<<"}";
}

static void generate_zerobrane_autocomplete(std::stringstream &ss,const std::vector<pragma::doc::PCollection> &collections,const std::string &t="\t")
{
	auto bFirst = true;
	for(auto &collection : collections)
	{
		if(bFirst == false)
			ss<<",\n";
		else
			bFirst = true;
		ss<<t<<"[\""<<collection->GetName()<<"\"] = {\n";
		ss<<t<<"\ttype = \"";
		auto flags = collection->GetFlags();
		if((flags &pragma::doc::Collection::Flags::Library) != pragma::doc::Collection::Flags::None)
			ss<<"lib";
		else if((flags &pragma::doc::Collection::Flags::Class) != pragma::doc::Collection::Flags::None)
			ss<<"class";
		ss<<"\",\n";

		std::string inherits {};
		auto bFirst = true;
		for(auto &df : collection->GetDerivedFrom())
		{
			if(bFirst == false)
				inherits += ' ';
			else
				bFirst = false;
			inherits += df->GetName();
		}
		if(inherits.empty() == false)
			ss<<t<<"\tinherits = \""<<inherits<<"\",\n";
		ss<<t<<"\tdescription = [["<<normalize_text(collection->GetDescription())<<"]],\n";

		ss<<t<<"\tchilds = {\n";
		bFirst = true;
		for(auto &fc : collection->GetFunctions())
		{
			if(bFirst == false)
				ss<<",\n";
			else
				bFirst = false;
			write_zerobrane_function(ss,fc,t +"\t\t");
		}
		for(auto &enumSet : collection->GetEnumSets())
		{
			for(auto &e : enumSet->GetEnums())
			{
				if(bFirst == false)
					ss<<",\n";
				else
					bFirst = false;
				write_zerobrane_enum(ss,*enumSet,e,t +"\t\t");
			}
		}
		for(auto &member : collection->GetMembers())
		{
			if(bFirst == false)
				ss<<",\n";
			else
				bFirst = false;
			write_zerobrane_member(ss,member,t +"\t\t");
		}
		auto &children = collection->GetChildren();
		if(children.empty() == false)
		{
			if(bFirst == false)
				ss<<",\n";
			else
				bFirst = false;
			generate_zerobrane_autocomplete(ss,children,t +"\t\t");
		}
		ss<<"\n"<<t<<"\t}\n";
		ss<<t<<"},\n";
	}
}

std::string pragma::doc::zerobrane::generate_autocomplete_script(const std::vector<pragma::doc::PCollection> &collections)
{
	std::stringstream ss;
	ss<<"return {\n";
	generate_zerobrane_autocomplete(ss,collections);
	ss<<"}";
	return ss.str();
}
