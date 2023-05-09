#include "platform_paths.h"
#include "config.h"

#ifdef _WIN32
#include <shlobj_core.h>

path platform_paths::getProgramFiles()
{
	wchar_t wpf[MAX_PATH];
	SHGetSpecialFolderPath(
		0,
		wpf,
		CSIDL_PROGRAM_FILES,
		false);
	return path(wpf);
}

path platform_paths::getAppData()
{
	wchar_t wpf[MAX_PATH];
	SHGetSpecialFolderPath(
		0,
		wpf,
		CSIDL_APPDATA,
		false);
	return path(wpf);
}
#elif __APPLE__
//https://stackoverflow.com/a/74991379
#include <sysdir.h>  // for sysdir_start_search_path_enumeration
#include <glob.h>    // for glob needed to expand ~ to user dir

std::string expandTilde(const char* str) {
	if (!str) return {};

	glob_t globbuf;
	if (glob(str, GLOB_TILDE, nullptr, &globbuf) == 0) {
		std::string result(globbuf.gl_pathv[0]);
		globfree(&globbuf);
		return result;
	}
	else {
		throw std::exception("Unable to expand tilde");
	}
}

path platform_paths::getProgramFiles()
{
	char result_path[PATH_MAX];
	auto state = sysdir_start_search_path_enumeration(SYSDIR_DIRECTORY_APPLICATION,
		SYSDIR_DOMAIN_MASK_USER);
	if ((state = sysdir_get_next_search_path_enumeration(state, result_path))) {
		return path(expandTilde(result_path));
	}
	else {
		throw std::exception("Failed to get settings folder");
	}
}

path platform_paths::getAppData()
{
	char result_path[PATH_MAX];
	auto state = sysdir_start_search_path_enumeration(SYSDIR_DIRECTORY_APPLICATION_SUPPORT,
		SYSDIR_DOMAIN_MASK_USER);
	if ((state = sysdir_get_next_search_path_enumeration(state, result_path))) {
		return path(expandTilde(result_path));
	}
	else {
		throw std::exception("Failed to get settings folder");
	}
}
#endif

path platform_paths::getDynamicAppFolder()
{
#ifdef _DEBUG
	return current_path();
#else
	return path(getAppData() / GAME::COMPANY / GAME::NAME);
#endif
}

path platform_paths::getStaticAppFolder()
{
#ifdef _DEBUG
	return current_path();
#else
	return path(getProgramFiles() / GAME::COMPANY / GAME::NAME);
#endif
}

path platform_paths::getSavesFolder()
{
	return path(getDynamicAppFolder() / "saves");
}

path platform_paths::getSaveFolder(int id)
{
	return getSavesFolder() / std::to_string(id);
}