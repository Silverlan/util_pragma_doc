// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.doc;

import pragma.string;

static std::string normalize_text(const std::string &text)
{
	auto normalizedText = text;
	pragma::string::replace(normalizedText, "[[", "\\[\\[");
	pragma::string::replace(normalizedText, "]]", "\\]\\]");
	return normalizedText;
}

static void write_zerobrane_function(std::stringstream &ss, const pragma::doc::Function &function, const std::string &t)
{
	std::string type {};
	switch(function.GetType()) {
	case pragma::doc::Function::Type::Method:
	case pragma::doc::Function::Type::Hook:
		type = "method";
		break;
	default:
		type = "function";
		break;
	}
	ss << t << "[\"" << function.GetName() << "\"] = {\n";
	ss << t << "\ttype = \"" << type << "\",\n";
	ss << t << "\tdescription = [[" << normalize_text(function.GetDescription()) << "]],\n";

	auto &overloads = function.GetOverloads();
	if(overloads.empty() == false) {
		auto &overload = overloads.front();
		ss << t << "\targs = \"(";
		auto bFirst = true;
		auto numOptional = 0u;
		auto strParams = pragma::doc::Parameter::GetFormattedParameterString(overload.GetParameters(), pragma::doc::ParameterFormatType::ZeroBrane);
		ss << strParams;
		for(auto i = decltype(numOptional) {0u}; i < numOptional; ++i)
			ss << "]";
		ss << ")\",\n";

		ss << t << "\treturns = \"(";
		bFirst = true;
		auto &returnValues = overload.GetReturnValues();
		auto strRet = pragma::doc::Parameter::GetFormattedParameterString(overload.GetReturnValues(), pragma::doc::ParameterFormatType::ZeroBrane, false);
		ss << strRet;
		ss << ")\"";
		if(returnValues.empty() == false)
			ss << ",\n" << t << "\tvaluetype = \"" + returnValues.front().GetType().GetFullType() + "\"\n";
		else
			ss << "\n";
	}
	else {
		ss << t << "\targs = \"()\",\n";
		ss << t << "\treturns = \"(void)\"\n";
	}
	ss << t << "}";
}

static void write_zerobrane_member(std::stringstream &ss, const pragma::doc::Member &member, const std::string &t)
{
	ss << t << "[\"" << member.GetName() << "\"] = {\n";
	ss << t << "\ttype = \"value\",\n";
	ss << t << "\tdescription = [[" << normalize_text(member.GetDescription()) << "]],\n";
	ss << t << "\tvaluetype = \"" << member.GetType().GetFullType() << "\"\n";
	ss << t << "}";
}

static void write_zerobrane_enum(std::stringstream &ss, const pragma::doc::EnumSet &enumSet, const pragma::doc::Enum &e, const std::string &t)
{
	ss << t << "[\"" << e.GetName() << "\"] = {\n";
	ss << t << "\ttype = \"value\",\n";
	ss << t << "\tdescription = [[" << normalize_text(e.GetDescription()) << "]],\n";
	ss << t << "\tvaluetype = \"" << enumSet.GetUnderlyingType() << "\"\n";
	ss << t << "}";
}

static void generate_zerobrane_autocomplete(std::stringstream &ss, const std::vector<pragma::doc::PCollection> &collections, const std::string &t = "\t")
{
	auto bFirst = true;
	for(auto &collection : collections) {
		if(bFirst == false)
			ss << ",\n";
		else
			bFirst = true;
		ss << t << "[\"" << collection->GetName() << "\"] = {\n";
		ss << t << "\ttype = \"";
		auto flags = collection->GetFlags();
		if((flags & pragma::doc::Collection::Flags::Library) != pragma::doc::Collection::Flags::None)
			ss << "lib";
		else if((flags & pragma::doc::Collection::Flags::Class) != pragma::doc::Collection::Flags::None)
			ss << "class";
		ss << "\",\n";

		std::string inherits {};
		auto bFirst = true;
		for(auto &df : collection->GetDerivedFrom()) {
			if(bFirst == false)
				inherits += ' ';
			else
				bFirst = false;
			inherits += df->GetName();
		}
		if(inherits.empty() == false)
			ss << t << "\tinherits = \"" << inherits << "\",\n";
		ss << t << "\tdescription = [[" << normalize_text(collection->GetDescription()) << "]],\n";

		ss << t << "\tchilds = {\n";
		bFirst = true;
		for(auto &fc : collection->GetFunctions()) {
			if(bFirst == false)
				ss << ",\n";
			else
				bFirst = false;
			write_zerobrane_function(ss, fc, t + "\t\t");
		}
		for(auto &enumSet : collection->GetEnumSets()) {
			for(auto &e : enumSet->GetEnums()) {
				if(bFirst == false)
					ss << ",\n";
				else
					bFirst = false;
				write_zerobrane_enum(ss, *enumSet, e, t + "\t\t");
			}
		}
		for(auto &member : collection->GetMembers()) {
			if(bFirst == false)
				ss << ",\n";
			else
				bFirst = false;
			write_zerobrane_member(ss, member, t + "\t\t");
		}
		auto &children = collection->GetChildren();
		if(children.empty() == false) {
			if(bFirst == false)
				ss << ",\n";
			else
				bFirst = false;
			generate_zerobrane_autocomplete(ss, children, t + "\t\t");
		}
		ss << "\n" << t << "\t}\n";
		ss << t << "},\n";
	}
}

std::string pragma::doc::zerobrane::generate_autocomplete_script(const std::vector<PCollection> &collections)
{
	std::stringstream ss;
	ss << "return {\n";
	generate_zerobrane_autocomplete(ss, collections);
	ss << "}";
	return ss.str();
}

static void resolve_keywords(std::string &val)
{
	static std::unordered_set<std::string> keywords {"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "not", "or", "repeat", "return", "then", "true", "until", "while"};
	auto it = keywords.find(val);
	if(it != keywords.end())
		val = val + "_";
}

static void normalize_argument(std::string &val)
{
	resolve_keywords(val);
	if(pragma::string::is_integer(val))
		val = "arg" + val;
}

static std::string get_ldoc_type_name(const pragma::doc::Variant &var)
{
	static std::unordered_map<std::string, std::string> typeTranslations {{"uint8", "int"}, {"int8", "int"}, {"uint16", "int"}, {"int16", "int"}, {"uint32", "int"}, {"int32", "int"}, {"uint64", "int"}, {"int64", "int"}, {"float", "number"}, {"double", "number"}};
	auto type = var.name; //.GetFullType();
	auto it = typeTranslations.find(type);
	if(it != typeTranslations.end())
		type = it->second;
	resolve_keywords(type);
	return type;
}

static std::string get_argument_list(const pragma::doc::Overload &overload, bool includeTypes = false)
{
	std::string argList;
	auto first = true;
	for(auto &param : overload.GetParameters()) {
		if(first)
			first = false;
		else
			argList += ", ";
		auto paramName = param.GetName();
		normalize_argument(paramName);
		argList += paramName;
		if(includeTypes) {
			argList += ": ";
			argList += get_ldoc_type_name(param.GetType());
		}
	}
	return argList;
}

std::stringstream &pragma::doc::luals::Doc::GetStream(const std::string &name)
{
	auto it = streams.find(name);
	if(it == streams.end()) {
		it = streams.insert(std::make_pair(name, std::stringstream {})).first;
		it->second << "--- @meta\n";
	}
	return it->second;
}

static void generate_ldoc_autocomplete(pragma::doc::luals::Doc &docFileManager, std::stringstream &ssParent, const std::vector<pragma::doc::PCollection> &collections, const std::string &classPrefix, const std::string &t = "\t")
{
	std::string prefix = "--- ";
	auto bFirst = true;
	for(auto &collection : collections) {
		std::stringstream *libStream = nullptr;
		if(pragma::math::is_flag_set(collection->GetFlags(), pragma::doc::Collection::Flags::Library)) {
			libStream = &docFileManager.GetStream(collection->GetName());
		}
		auto &ss = libStream ? *libStream : ssParent;
		ss << prefix << collection->GetDescription() << "\n";
		ss << prefix << "@class " << classPrefix << collection->GetName();
		auto &derivedFrom = collection->GetDerivedFrom();
		if(!derivedFrom.empty()) {
			ss << ": ";
			auto first = true;
			for(auto &df : derivedFrom) {
				if(first)
					first = false;
				else
					ss << ", ";
				ss << df->GetName();
			}
		}
		ss << "\n";

		for(auto &member : collection->GetMembers()) {
			auto &type = member.GetType();
			ss << prefix << "@field " << member.GetName() << " " << get_ldoc_type_name(type) << " " << member.GetDescription() << "\n";
		}

		for(auto &fc : collection->GetFunctions()) {
			auto &name = fc.GetName();
			if(name != "__init")
				continue;
			for(auto &overload : fc.GetOverloads())
				ss << prefix << "@overload fun(" + get_argument_list(overload, true) + "):" << classPrefix << collection->GetName() << "\n";
		}

		ss << classPrefix << collection->GetName() << " = {}\n";
		ss << "\n";

		auto subName = classPrefix + collection->GetName();
		auto subClass = subName + '.';
		for(auto &fc : collection->GetFunctions()) {
			auto &name = fc.GetName();
			if(name == "__init")
				continue;
			auto &overloads = fc.GetOverloads();
			if(overloads.empty())
				continue;
			auto &mainOverload = overloads.front();
			ss << prefix << fc.GetDescription() << "\n";

			for(auto &param : mainOverload.GetParameters()) {
				ss << prefix << "@param " << param.GetName() << " " << get_ldoc_type_name(param.GetType()) << "\n";
			}

			auto &returnValues = mainOverload.GetReturnValues();
			if(!returnValues.empty()) {
				for(auto &val : returnValues) {
					auto returnType = get_ldoc_type_name(val.GetType());
					if(returnType == "nil")
						continue;
					ss << prefix << "@return " << returnType << " " << val.GetName() << "\n";
				}
			}

			for(auto i = decltype(overloads.size()) {1u}; i < overloads.size(); ++i) {
				auto &overload = overloads[i];
				ss << prefix << "@overload fun(" << get_argument_list(overload, true) << "): ";
				auto first = true;
				for(auto &val : returnValues) {
					auto returnType = get_ldoc_type_name(val.GetType());
					if(returnType == "nil")
						continue;
					if(first)
						first = false;
					else
						ss << ", ";
					ss << returnType;
				}
				ss << "\n";
			}

			ss << "function " << subName;
			switch(fc.GetType()) {
			case pragma::doc::Function::Type::Method:
				ss << ":";
				break;
			default:
				ss << ".";
				break;
			}
			ss << fc.GetName() << "(" << get_argument_list(mainOverload) << ") end\n";
			ss << "\n";
		}
		ss << "\n";

		for(auto &es : collection->GetEnumSets()) {
			ss << prefix << "@enum " << es->GetName() << "\n";
			//ss << subClass << es->GetName() << " = {\n";
			ss << subName << " = {\n";
			for(auto &e : es->GetEnums()) {
				ss << "\t" << e.GetName() << " = " << e.GetValue() << ",\n";
			}
			ss << "}\n";
			ss << "\n";
		}

		auto &children = collection->GetChildren();
		if(children.empty() == false) {
			generate_ldoc_autocomplete(docFileManager, ss, children, subClass, t + "\t\t");
		}
	}
}

pragma::doc::luals::Doc pragma::doc::luals::generate_doc(const std::vector<PCollection> &collections)
{
	// See https://github.com/LuaLS/lua-language-server/wiki/Annotations
	Doc docFileManager {};
	auto &ss = docFileManager.GetStream("root");
	generate_ldoc_autocomplete(docFileManager, ss, collections, "");
	return docFileManager;
}
