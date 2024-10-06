/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <fsys/filesystem.h>

module pragma.doc;

using namespace pragma;

doc::Overload doc::Overload::Create() { return Overload {}; }
std::vector<doc::Parameter> &doc::Overload::GetParameters() { return m_parameters; }
std::vector<doc::Parameter> &doc::Overload::GetReturnValues() { return m_returnValues; }

bool doc::Overload::operator==(const Overload &other) const
{
	if(m_parameters.size() != other.m_parameters.size() || m_returnValues.size() != other.m_returnValues.size())
		return false;
	for(auto i = decltype(m_parameters.size()) {0u}; i < m_parameters.size(); ++i) {
		if(m_parameters[i] != other.m_parameters[i])
			return false;
	}
	for(auto i = decltype(m_returnValues.size()) {0u}; i < m_returnValues.size(); ++i) {
		if(m_returnValues[i] != other.m_returnValues[i])
			return false;
	}
	return true;
}

void doc::Overload::AddParameter(const Parameter &parameter) { m_parameters.push_back(parameter); }
void doc::Overload::AddReturnValue(const Parameter &parameter) { m_returnValues.push_back(parameter); }
