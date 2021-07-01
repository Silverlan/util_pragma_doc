/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
doc::Overload doc::Overload::Read(std::shared_ptr<VFilePtrInternal> &f)
{
	Overload overload {};

	auto numParameters = f->Read<uint32_t>();
	overload.m_parameters.reserve(numParameters);
	for(auto i=decltype(numParameters){0u};i<numParameters;++i)
		overload.m_parameters.push_back(Parameter::Read(f));

	auto numReturnValues = f->Read<uint32_t>();
	overload.m_returnValues.reserve(numReturnValues);
	for(auto i=decltype(numReturnValues){0u};i<numReturnValues;++i)
		overload.m_returnValues.push_back(Parameter::Read(f));
	return overload;
}
void doc::Overload::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->Write<uint32_t>(m_parameters.size());
	for(auto &param : m_parameters)
		param.Write(f);
	f->Write<uint32_t>(m_returnValues.size());
	for(auto &returnValue : m_returnValues)
		returnValue.Write(f);
}
doc::Overload doc::Overload::Create() {return Overload{};}
std::vector<doc::Parameter> &doc::Overload::GetParameters() {return m_parameters;}
std::vector<doc::Parameter> &doc::Overload::GetReturnValues() {return m_returnValues;}

void doc::Overload::AddParameter(const Parameter &parameter) {m_parameters.push_back(parameter);}
void doc::Overload::AddReturnValue(const Parameter &parameter) {m_returnValues.push_back(parameter);}
#pragma optimize("",on)
