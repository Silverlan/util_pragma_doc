// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

module pragma.doc;

using namespace pragma;

doc::BaseCollectionObject::BaseCollectionObject(const BaseCollectionObject &other) : m_collection {other.m_collection} {}
doc::BaseCollectionObject &doc::BaseCollectionObject::operator=(const BaseCollectionObject &other)
{
	m_collection = other.m_collection;
	return *this;
}
doc::BaseCollectionObject::BaseCollectionObject(const Collection *collection) : m_collection {(collection != nullptr) ? collection->shared_from_this() : nullptr} {}
doc::Collection *doc::BaseCollectionObject::GetCollection() { return const_cast<Collection *>(m_collection.lock().get()); }
void doc::BaseCollectionObject::SetCollection(Collection &col) { m_collection = col.shared_from_this(); }
