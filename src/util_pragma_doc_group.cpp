/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

doc::Group doc::Group::Create() {return {};}
doc::Group doc::Group::Read(std::shared_ptr<VFilePtrInternal> &f)
{
	Group group {};
	group.m_name = f->ReadString();
	return group;
}
void doc::Group::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
}
const std::string &doc::Group::GetName() const {return m_name;}
