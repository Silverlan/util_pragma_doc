// SPDX-FileCopyrightText: (c) 2024 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifdef UTIL_PRAGMA_DOC_EXE
#include "util_pragma_doc.hpp"

int main(int argc, char *argv[])
{
	auto f = FileManager::OpenFile("pragma.wdd", "rb");
	if(f != nullptr) {
		std::vector<pragma::doc::PCollection> collections {};
		pragma::doc::load_collections(f, collections);
		f = nullptr;

		auto fZb = FileManager::OpenFile<fs::VFilePtrReal>("pragma.lua", "w");
		fZb->WriteString(pragma::doc::zerobrane::generate_autocomplete_script(collections));
		fZb = nullptr;
	}
	std::cout << "Complete!" << std::endl;
	char c;
	std::cin >> c;
	return EXIT_SUCCESS;
}
#endif
