#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#if HAVE_FTS_H
#include <fts.h>
extern "C"
{
#include <errno.h>
#include <dirent.h>
}
#else
#include "dirent.h"
#endif

namespace libprojectM {
namespace Renderer {

typedef std::function<void(std::string &path, std::string &name)> ScanCallback;

class FileScanner
{
public:
	FileScanner();
	FileScanner(const std::vector<std::string> &rootDirs, std::vector<std::string> &extensions);

	void scan(ScanCallback cb);
	std::string extensionMatches(std::string &filename);

private:
	std::vector<std::string> _rootDirs;
	std::vector<std::string> _extensions;

	void scanGeneric(ScanCallback cb, const char *dir);
	void scanPosix(ScanCallback cb);
	void handleDirectoryError(std::string dir);
	bool isValidFilename(std::string &filename);
};

} // namespace Renderer
} // namespace libprojectM
