/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

module;

#include <fsys/filesystem.h>

module pragma.doc;

using namespace pragma;

doc::Group doc::Group::Create() { return {}; }
const std::string &doc::Group::GetName() const { return m_name; }
