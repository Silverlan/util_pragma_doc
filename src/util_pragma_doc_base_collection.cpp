/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"

using namespace pragma;

doc::BaseCollectionObject::BaseCollectionObject(const Collection *collection)
	: m_collection{(collection != nullptr) ? collection->shared_from_this() : nullptr}
{}
const doc::Collection *doc::BaseCollectionObject::GetCollection() const {return m_collection.lock().get();}
void doc::BaseCollectionObject::SetCollection(Collection &col) {m_collection = col.shared_from_this();}
