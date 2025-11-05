// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.doc;

using namespace pragma;

doc::Group doc::Group::Create() { return {}; }
const std::string &doc::Group::GetName() const { return m_name; }
