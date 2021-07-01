/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>

using namespace pragma;

doc::Module doc::Module::Read(std::shared_ptr<VFilePtrInternal> &f)
{
	Module mod {};
	mod.m_name = f->ReadString();
	mod.m_target = f->ReadString();
	return mod;
}
void doc::Module::Write(std::shared_ptr<VFilePtrInternalReal> &f) const
{
	f->WriteString(m_name);
	f->WriteString(m_target);
}
const std::string &doc::Module::GetName() const {return m_name;}
const std::string &doc::Module::GetTarget() const {return m_target;}
