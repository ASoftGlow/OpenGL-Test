#pragma once

#include <filesystem>

using namespace std::filesystem;

namespace platform_paths {
	path getProgramFiles();
	path getAppData();
	path getStaticAppFolder();
	path getDynamicAppFolder();
	path getSavesFolder();
	path getSaveFolder(int id);
}