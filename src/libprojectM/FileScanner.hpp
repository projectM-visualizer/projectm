//
//  FileScanner.hpp
//  libprojectM
//
//  Cross-platform directory traversal with filtering by extension

#ifndef FileScanner_hpp
#define FileScanner_hpp

#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include "Common.hpp"

#ifdef WIN32
#include "dirent.h"
#else
#include <fts.h>
#endif

#ifdef __unix__
extern "C"
{
#include <errno.h>
#include <dirent.h>
}
#endif

#ifdef __APPLE__
extern "C"
{
#include <errno.h>
#include <dirent.h>
}
#endif


typedef std::function<void (std::string &path, std::string &name)> ScanCallback;

class FileScanner {
public:
    FileScanner();
    FileScanner(std::vector<std::string> &rootDirs, std::vector<std::string> &extensions);
    
    void scan(ScanCallback cb);
    
private:
    std::vector<std::string> _rootDirs;
    std::vector<std::string> _extensions;

    void scanGeneric(ScanCallback cb, const char *dir);
    void scanPosix(ScanCallback cb);
    void handleDirectoryError(std::string dir);
};


#endif /* FileScanner_hpp */
