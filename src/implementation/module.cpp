// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <fsys/filesystem.h>

module pragma.doc;

using namespace pragma;

const std::string &doc::Module::GetName() const { return m_name; }
const std::string &doc::Module::GetTarget() const { return m_target; }
