/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <udm.hpp>
#include <sharedutils/magic_enum.hpp>

using namespace pragma;
#pragma optimize("",off)
static std::string generate_identifier(const pragma::doc::Function &f)
{
	auto name = f.GetFullName();
	ustring::to_lower(name);
	std::replace(name.begin(),name.end(),'.','-');
	name = "f-" +name;
	return name;
}

static std::string generate_identifier(const pragma::doc::Collection &c)
{
	auto name = c.GetFullName();
	ustring::to_lower(name);
	std::replace(name.begin(),name.end(),'.','-');
	if(umath::is_flag_set(c.GetFlags(),pragma::doc::Collection::Flags::Class))
		name = "c-" +name;
	else if(umath::is_flag_set(c.GetFlags(),pragma::doc::Collection::Flags::Library))
		name = "l-" +name;
	else
		name = "g-" +name;
	return name;
}

static void save_variant(udm::LinkedPropertyWrapper &udmVariant,const pragma::doc::Variant &variant)
{
	udmVariant["name"] = variant.name;
	udmVariant["flags"] = udm::flags_to_string(variant.flags);

	if(!variant.typeParameters.empty())
	{
		auto udmTypeParams = udmVariant.AddArray("typeParameters",variant.typeParameters.size());
		uint32_t idx = 0;
		for(auto &typeParam : variant.typeParameters)
		{
			auto udmTypeParam = udmTypeParams[idx++];
			save_variant(udmTypeParam,typeParam);
		}
	}
}

static void save_parameter(udm::LinkedPropertyWrapper &udmParam,const pragma::doc::Parameter &param)
{
	udmParam["name"] = param.GetName();
	udmParam["flags"] = udm::flags_to_string(param.GetFlags());
	udmParam["gameStateFlags"] = udm::flags_to_string(param.GetGameStateFlags());

	auto udmType = udmParam["type"];
	save_variant(udmType,param.GetType());

	auto &def = param.GetDefault();
	if(def.has_value())
		udmParam["default"] = *def;
}

static void save_collection(udm::LinkedPropertyWrapper udmCollection,const pragma::doc::Collection &collection)
{
	udmCollection["desc"] = collection.GetDescription();
	if(!collection.GetURL().empty())
		udmCollection["url"] = collection.GetURL();
	udmCollection["flags"] = udm::flags_to_string(collection.GetFlags());
	udmCollection["identifier"] = generate_identifier(collection);

	auto udmFunctions = udmCollection["functions"];
	for(auto &f : collection.GetFunctions())
	{
		auto udmFunction = udmFunctions[f.GetName()];
		udmFunction["desc"] = f.GetDescription();
		if(!f.GetURL().empty())
			udmFunction["url"] = f.GetURL();
		udmFunction["type"] = f.GetType();
		udmFunction["flags"] = udm::flags_to_string(f.GetFlags());
		udmFunction["gameStateFlags"] = udm::flags_to_string(f.GetGameStateFlags());
		udmFunction["related"] = f.GetRelated();
		udmFunction["identifier"] = generate_identifier(f);

		auto exampleCode = f.GetExampleCode();
		if(exampleCode.has_value())
		{
			auto udmExampleCode = udmFunction["exampleCode"];
			udmExampleCode["code"] = exampleCode->code;
			udmExampleCode["desc"] = exampleCode->description;
		}

		auto &overloads = f.GetOverloads();
		if(!overloads.empty())
		{
			auto udmOverloads = udmFunction.AddArray("overloads",overloads.size());
			uint32_t idx = 0;
			for(auto &overload : overloads)
			{
				auto udmOverload = udmOverloads[idx++];

				auto &params = overload.GetParameters();
				auto udmParams = udmOverload.AddArray("params",params.size());
				uint32_t idx = 0;
				for(auto &param : params)
				{
					auto udmParam = udmParams[idx++];
					save_parameter(udmParam,param);
				}

				auto &returnValues = overload.GetReturnValues();
				auto udmReturnValues = udmOverload.AddArray("returnValues",returnValues.size());
				idx = 0;
				for(auto &param : returnValues)
				{
					auto udmReturnValue = udmReturnValues[idx++];
					save_parameter(udmReturnValue,param);
				}

				auto &source = overload.GetSource();
				if(source.has_value())
				{
					auto udmSource = udmOverload["source"];
					udmSource["moduleName"] = source->moduleName;
					udmSource["fileName"] = source->fileName;
					udmSource["line"] = source->line;
				}

				auto &url = overload.GetURL();
				if(!url.empty())
					udmOverload["url"] = url;
			}
		}

		auto &groups = f.GetGroups();
		if(!groups.empty())
		{
			auto udmGroups = udmFunction.AddArray("groups",groups.size());
			uint32_t idx = 0;
			for(auto &group : groups)
			{
				auto udmGroup = udmGroups[idx++];
				udmGroup["name"] = group.GetName();
			}
		}
	}
		
	auto udmMembers = udmCollection["members"];
	for(auto &m : collection.GetMembers())
	{
		auto udmMember = udmMembers[m.GetName()];
		auto udmType = udmMember["type"];
		save_variant(udmType,m.GetType());
		udmMember["desc"] = m.GetDescription();
		udmMember["gameStateFlags"] = udm::flags_to_string(m.GetGameStateFlags());
		auto &def = m.GetDefault();
		if(def.has_value())
			udmMember["default"] = *def;
		udmMember["mode"] = m.GetMode();
	}
		
	auto udmEnumSets = udmCollection["enumSets"];
	for(auto &es : collection.GetEnumSets())
	{
		auto udmEnumSet = udmEnumSets[es->GetName()];
		udmEnumSet["underlyingType"] = es->GetUnderlyingType();

		auto udmEnums = udmEnumSet["enums"];
		for(auto &e : es->GetEnums())
		{
			auto udmEnum = udmEnums[e.GetName()];
			udmEnum["value"] = e.GetValue();
			udmEnum["desc"] = e.GetDescription();
			udmEnum["type"] = e.GetType();
			udmEnum["gameStateFlags"] = udm::flags_to_string(e.GetGameStateFlags());
		}
	}

	std::vector<std::string> derivedFromNames;
	auto &derivedFrom = collection.GetDerivedFrom();
	derivedFromNames.reserve(derivedFrom.size());
	for(auto &df : derivedFrom)
		derivedFromNames.push_back(df->GetName());

	udmCollection["derivedFrom"] = derivedFromNames;

	auto &children = collection.GetChildren();
	auto udmChildren = udmCollection["children"];
	for(auto &child : children)
		save_collection(udmChildren[child->GetName()],*child);
}

bool doc::Collection::Save(udm::AssetDataArg outData,std::string &outErr)
{
	outData.SetAssetType(PDOC_IDENTIFIER);
	outData.SetAssetVersion(PDOC_VERSION);
	auto udmCollections = outData.GetData()["collections"];
	for(auto &child : m_children)
		save_collection(udmCollections[child->GetName()],*child);
	return true;
}

static void load_variant(udm::LinkedPropertyWrapper &udmVariant,pragma::doc::Variant &variant)
{
	udmVariant["name"](variant.name);
	udm::to_flags(udmVariant["flags"],variant.flags);

	auto udmTypeParams = udmVariant["typeParameters"];
	auto n = udmTypeParams.GetSize();
	variant.typeParameters.reserve(n);
	for(auto &udmTypeParam : udmTypeParams)
	{
		variant.typeParameters.push_back({});
		load_variant(udmTypeParam,variant.typeParameters.back());
	}
}

void doc::Collection::Load(udm::LinkedPropertyWrapper &udmCollection)
{
	udmCollection["desc"](m_description);
	udmCollection["url"](m_url);
	udm::to_flags<Flags>(udmCollection["flags"],m_flags);
	udmCollection["identifier"](m_identifier);

	auto fLoadParam = [](udm::LinkedPropertyWrapper &udmParam,pragma::doc::Parameter &param) {
		udmParam["name"](param.m_name);
		udm::to_flags(udmParam["flags"],param.m_flags);
		udm::to_flags(udmParam["gameStateFlags"],param.m_gameStateFlags);
		
		auto udmType = udmParam["type"];
		Variant type;
		load_variant(udmType,type);
		param.SetType(std::move(type));

		{
			auto udmDefault = udmParam["default"];
			if(udmDefault)
			{
				param.m_default = std::string{};
				udmDefault(*param.m_default);
			}
		}
	};

	auto udmFunctions = udmCollection["functions"];
	m_functions.reserve(udmFunctions.GetChildCount());
	for(auto &pair : udmFunctions.ElIt())
	{
		auto f = Function::Create(*this,std::string{pair.key});
		auto &udmFunction = pair.property;
		udmFunction["desc"](f.m_description);
		if(!f.m_url.empty())
			udmFunction["url"](f.m_url);
		udmFunction["type"](f.m_type);
		udm::to_flags(udmFunction["flags"],f.m_flags);
		udm::to_flags(udmFunction["gameStateFlags"],f.m_gameStateFlags);
		udmFunction["related"](f.m_related);
		udmFunction["identifier"](m_identifier);
		
		auto udmExampleCode = udmFunction["exampleCode"];
		if(udmExampleCode)
		{
			f.m_exampleCode = Function::ExampleCode {};
			udmExampleCode["code"](f.m_exampleCode->code);
			udmExampleCode["desc"](f.m_exampleCode->description);
		}

		auto udmOverloads = udmFunction["overloads"];
		auto &overloads = f.m_overloads;
		overloads.reserve(udmOverloads.GetSize());
		for(auto &udmOverload : udmOverloads)
		{
			overloads.push_back(Overload::Create());
			auto &overload = overloads.back();

			auto udmParams = udmOverload["params"];
			auto &params = overload.GetParameters();
			params.reserve(udmParams.GetSize());
			for(auto &udmParam : udmParams)
			{
				params.push_back({});
				fLoadParam(pair.property,params.back());
			}

			auto udmReturnValues = udmOverload["returnValues"];
			auto &returnValues = overload.GetReturnValues();
			returnValues.reserve(udmReturnValues.GetSize());
			for(auto &udmReturnValue : udmReturnValues)
			{
				returnValues.push_back({});
				fLoadParam(pair.property,returnValues.back());
			}

			auto udmSource = udmOverload["source"];
			if(udmSource)
			{
				Source source {};
				udmSource["moduleName"](source.moduleName);
				udmSource["fileName"](source.fileName);
				udmSource["line"](source.line);
			}

			if(udmOverload["url"])
			{
				std::string url;
				udmOverload["url"](url);
				overload.SetURL(url);
			}
		}

		auto udmGroups = udmFunction["groups"];
		auto &groups = f.GetGroups();
		groups.reserve(udmGroups.GetSize());
		for(auto udmGroup : udmGroups)
		{
			groups.push_back({});
			auto &group = groups.back();
			udmGroup["name"](group.m_name);
		}
		m_functions.push_back(std::move(f));
	}
	
	auto udmMembers = udmCollection["members"];
	m_members.reserve(udmMembers.GetChildCount());
	for(auto &pair : udmMembers.ElIt())
	{
		auto member = Member::Create(*this,std::string{pair.key});
		auto &udmMember = pair.property;
		auto udmType = udmMember["type"];
		load_variant(udmType,member.m_type);
		udmMember["desc"](member.m_description);
		udm::to_flags(udmMember["gameStateFlags"],member.m_gameStateFlags);

		auto udmDefault = udmMember["default"];
		if(udmDefault)
		{
			member.m_default = std::string{};
			udmDefault(*member.m_default);
		}
		udmMember["mode"](member.m_mode);
		m_members.push_back(std::move(member));
	}
		
	auto udmEnumSets = udmCollection["enumSets"];
	m_enumSets.reserve(udmEnumSets.GetChildCount());
	for(auto &pair : udmEnumSets.ElIt())
	{
		auto es = EnumSet::Create(std::string{pair.key},this);
		auto &udmEnumSet = pair.property;
		udmEnumSet["underlyingType"](es->m_underlyingType);

		auto udmEnums = udmEnumSet["enums"];
		auto &enums = es->GetEnums();
		enums.reserve(udmEnums.GetSize());
		for(auto &pair : udmEnums.ElIt())
		{
			enums.push_back(Enum::Create(*es));
			auto &e = enums.back();
			e.SetName(std::string{pair.key});
			auto &udmEnum = pair.property;
			udmEnum["value"](e.m_value);
			udmEnum["desc"](e.m_description);
			udmEnum["type"](e.m_type);
			udm::to_flags(udmEnum["gameStateFlags"],e.m_gameStateFlags);
		}
		m_enumSets.push_back(std::move(es));
	}

	std::vector<std::string> derivedFromNames;
	udmCollection["derivedFrom"](derivedFromNames);
	m_derivedFrom.reserve(derivedFromNames.size());
	for(auto &name : derivedFromNames)
		m_derivedFrom.push_back(DerivedFrom::Create(name));

	auto udmChildren = udmCollection["children"];
	auto &children = m_children;
	children.reserve(udmChildren.GetSize());
	for(auto &pair : udmChildren.ElIt())
	{
		auto child = Collection::Create();
		child->m_parent = shared_from_this();
		child->m_name = std::string{pair.key};
		child->Load(pair.property);
		children.push_back(child);
	}
}

bool doc::Collection::LoadFromAssetData(const udm::AssetData &data,std::string &outErr)
{
	/*if(data.GetAssetType() != PDOC_IDENTIFIER)
	{
		outErr = "Incorrect format!";
		return false;
	}

	auto version = data.GetAssetVersion();
	if(version < 1)
	{
		outErr = "Invalid version!";
		return false;
	}*/
	auto udm = *data;
	// if(version > PDOC_VERSION)
	// 	return false;
	m_name = "_G";
	auto udmCollections = udm["collections"];
	m_children.reserve(udmCollections.GetChildCount());
	for(auto &pair : udmCollections.ElIt())
	{
		auto child = Collection::Create();
		child->m_name = std::string{pair.key};
		child->Load(pair.property);
		m_children.push_back(child);
	}
	return true;
}
#pragma optimize("",on)
