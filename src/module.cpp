/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fsys/filesystem.h>

module util_pragma_doc;

using namespace pragma;

const std::string &doc::Module::GetName() const { return m_name; }
const std::string &doc::Module::GetTarget() const { return m_target; }
