/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef UTIL_PRAGMA_DOC_EXE
#include "util_pragma_doc.hpp"
#include <fsys/filesystem.h>
#include <iostream>
#include <array>
#include <sstream>
#include <sharedutils/util_string.h>

int main(int argc,char *argv[])
{
	auto f = FileManager::OpenFile("pragma.wdd","rb");
	if(f != nullptr)
	{
		std::vector<pragma::doc::PCollection> collections {};
		pragma::doc::load_collections(f,collections);
		f = nullptr;

		auto fZb = FileManager::OpenFile<VFilePtrReal>("pragma.lua","w");
		fZb->WriteString(pragma::doc::zerobrane::generate_autocomplete_script(collections));
		fZb = nullptr;
	}
	std::cout<<"Complete!"<<std::endl;
	char c;
	std::cin>>c;
	return EXIT_SUCCESS;
}
#endif
