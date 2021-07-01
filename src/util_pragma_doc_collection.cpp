/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "util_pragma_doc.hpp"
#include "util_pragma_doc_impl.hpp"
#include <sharedutils/util.h>
#include <sharedutils/util_string.h>
#include <sharedutils/util_path.hpp>
#include <fsys/filesystem.h>

using namespace pragma;

#pragma optimize("",off)
doc::Collection::Collection(Collection *parent)
	: BaseCollectionObject(),m_parent{(parent != nullptr) ? parent->shared_from_this() : nullptr}
{}
void doc::Collection::SetName(const std::string &name) {m_name = name;}
std::vector<doc::Function> &doc::Collection::GetFunctions() {return m_functions;}
std::vector<doc::Member> &doc::Collection::GetMembers() {return m_members;}
std::vector<doc::PEnumSet> &doc::Collection::GetEnumSets() {return m_enumSets;}
std::vector<std::string> &doc::Collection::GetRelated() {return m_related;}
std::vector<doc::PCollection> &doc::Collection::GetChildren() {return m_children;}
std::vector<std::shared_ptr<doc::DerivedFrom>> &doc::Collection::GetDerivedFrom() {return m_derivedFrom;}
const std::string &doc::Collection::GetName() const {return m_name;}
std::string doc::Collection::GetFullName() const
{
	auto name = GetName();
	if(m_parent.expired())
		return name;
	auto parent = m_parent.lock();
	if(parent && parent->m_parent.expired() && parent->m_name == "_G")
		return name;
	return parent->GetFullName() +'.' +name;
}
const std::string &doc::Collection::GetDescription() const {return m_description;}
const std::string &doc::Collection::GetURL() const {return m_url;}
doc::Collection::Flags doc::Collection::GetFlags() const {return m_flags;}
const doc::Collection *doc::Collection::GetParent() const {return m_parent.lock().get();}
doc::Collection *doc::Collection::FindChildCollection(const std::string_view &name)
{
	std::string spath {name};
	std::replace(spath.begin(),spath.end(),'.','/');
	auto path = util::Path {spath};
	if(path.IsEmpty())
		return this;
	auto front = path.GetFront();
	auto it = std::find_if(m_children.begin(),m_children.end(),[&front](const PCollection &child) {
		return child->GetName() == front;
	});
	if(it == m_children.end())
		return nullptr;
	path.PopFront();
	return (*it)->FindChildCollection(path.GetString());
}
template<typename T>
	void strip_duplicates(std::vector<T> &v0,const std::vector<T> &v1)
{
	for(auto it=v0.begin();it!=v0.end();)
	{
		auto &f = *it;
		auto itp = std::find_if(v1.begin(),v1.end(),[&f](const T &fOther) {
			if constexpr(util::is_specialization<T,std::shared_ptr>::value)
				return f->GetName() == fOther->GetName();
			else
				return f.GetName() == fOther.GetName();
		});
		if(itp == v1.end())
		{
			++it;
			continue;
		}
		it = v0.erase(it);
	}
}
void doc::Collection::StripBaseDefinitionsFromDerivedCollections(Collection &root)
{
	for(auto &child : m_children)
		child->StripBaseDefinitionsFromDerivedCollections(root);
	std::function<void(const std::vector<std::shared_ptr<DerivedFrom>>&)> fStripBaseDefinitions = nullptr;
	fStripBaseDefinitions = [this,&fStripBaseDefinitions,&root](const std::vector<std::shared_ptr<DerivedFrom>> &derivedFrom) {
		for(auto &df : derivedFrom)
		{
			auto *derived = root.FindChildCollection(df->GetName());
			if(!derived)
				continue;
			derived->StripBaseDefinitionsFromDerivedCollections();
			fStripBaseDefinitions(derived->m_derivedFrom);

			strip_duplicates(m_functions,derived->m_functions);
			strip_duplicates(m_members,derived->m_members);
			strip_duplicates(m_enumSets,derived->m_enumSets);
			for(auto it=m_derivedFrom.begin();it!=m_derivedFrom.end();)
			{
				auto &df = *it;
				auto itOther = std::find(derived->m_derivedFrom.begin(),derived->m_derivedFrom.end(),df);
				if(itOther != derived->m_derivedFrom.end())
					it = m_derivedFrom.erase(it);
				else
					++it;
			}
		}
	};
	fStripBaseDefinitions(m_derivedFrom);
}
void doc::Collection::StripBaseDefinitionsFromDerivedCollections() {StripBaseDefinitionsFromDerivedCollections(*this);}
std::string doc::Collection::GetWikiURL() const
{
	std::string url {};
	auto *parent = GetParent();
	if(parent != nullptr)
		url = parent->GetWikiURL() +"_";
	else
		url = doc::GetWikiURL() +"?title=";
	auto flags = GetFlags();
	if((flags &Flags::Base) != Flags::None)
		url += "base_";
	else if((flags &Flags::Class) != Flags::None)
		url += "class_";
	else if((flags &Flags::Library) != Flags::None)
		url += "lib_";
	return url +ustring::name_to_identifier(GetName());
}
void doc::Collection::AddFunction(const Function &function)
{
	if(m_functions.size() == m_functions.capacity())
		m_functions.reserve(m_functions.size() *1.5 +50);
	m_functions.push_back(function);
	m_functions.back().m_collection = shared_from_this();
}
void doc::Collection::AddMember(const Member &member)
{
	if(m_members.size() == m_members.capacity())
		m_members.reserve(m_members.size() *1.5 +5);
	m_members.push_back(member);
	m_members.back().m_collection = shared_from_this();
}
void doc::Collection::AddEnumSet(const PEnumSet &enumSet)
{
	if(m_enumSets.size() == m_enumSets.capacity())
		m_enumSets.reserve(m_enumSets.size() *1.5 +5);
	m_enumSets.push_back(enumSet);
}
void doc::Collection::AddRelated(const std::string &related)
{
	if(m_related.size() == m_related.capacity())
		m_related.reserve(m_related.size() *1.5 +5);
	m_related.push_back(related);
}
void doc::Collection::AddChild(const PCollection &collection)
{
	if(m_children.size() == m_children.capacity())
		m_children.reserve(m_children.size() *1.5 +5);
	m_children.push_back(collection);
	collection->m_parent = shared_from_this();
}
void doc::Collection::AddDerivedFrom(DerivedFrom &derivedFrom)
{
	if(m_derivedFrom.size() == m_derivedFrom.capacity())
		m_derivedFrom.reserve(m_derivedFrom.size() *1.5 +5);
	m_derivedFrom.push_back(derivedFrom.shared_from_this());
}
void doc::Collection::SetDescription(const std::string &desc) {m_description = desc;}
void doc::Collection::SetURL(const std::string &url) {m_url = url;}
void doc::Collection::SetFlags(Flags flags) {m_flags = flags;}
void doc::Collection::SetParent(Collection *optParent)
{
	m_parent = optParent ? optParent->shared_from_this() : std::weak_ptr<Collection>{};
}
pragma::doc::PCollection pragma::doc::Collection::Create() {return std::shared_ptr<Collection>{new Collection{}};}

pragma::doc::PCollection pragma::doc::Collection::Load(const udm::AssetData &data,std::string &outErr)
{
	auto col = Create();
	if(col->LoadFromAssetData(data,outErr) == false)
		return nullptr;
	return col;
}
#pragma optimize("",on)
