//
//  FileScanner.cpp
//  libprojectM
//
//

#include "FileScanner.hpp"

/** Per-platform path separators and includes */
#ifdef _WIN32
char constexpr pathSeparator{'\\'};
#else
char constexpr pathSeparator{'/'};
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <algorithm>

FileScanner::FileScanner()
{
}

FileScanner::FileScanner(const std::vector<std::string>& rootDirs, std::vector<std::string>& extensions)
    : _rootDirs(rootDirs)
    , _extensions(extensions)
{
}

void FileScanner::scan(ScanCallback cb)
{
#if HAVE_FTS_H
    scanPosix(cb);
#else
    for (const auto& dir : _rootDirs)
        scanGeneric(cb, dir.c_str());
#endif
}

void FileScanner::handleDirectoryError(std::string dir)
{
#ifndef HAVE_FTS_H
    std::cerr << "[PresetLoader] warning: errno unsupported on win32, etc platforms. fix me" << std::endl;
#else

    std::cerr << dir << " scan error: ";

    switch (errno)
    {
        case ENOENT:
            std::cerr << "ENOENT error. The path \"" << dir << "\" probably does not exist. \"man open\" for more info." << std::endl;
            break;
        case ENOMEM:
            std::cerr << "out of memory!" << std::endl;
            abort();
        case ENOTDIR:
            std::cerr << "directory specified is not a directory! Trying to continue..." << std::endl;
            break;
        case ENFILE:
            std::cerr << "Your system has reached its open file limit. Trying to continue..." << std::endl;
            break;
        case EMFILE:
            std::cerr << "too many files in use by projectM! Bailing!" << std::endl;
            break;
        case EACCES:
            std::cerr << "permissions issue reading the specified preset directory." << std::endl;
            break;
        default:
            break;
    }
#endif
}

std::string FileScanner::extensionMatches(std::string& filename)
{
    // returns file name without extension
    // TODO: optimize me

    std::string lowerCaseFileName(filename);
    std::transform(lowerCaseFileName.begin(), lowerCaseFileName.end(), lowerCaseFileName.begin(), tolower);

    // Remove extension
    for (auto ext : _extensions)
    {
        size_t found = lowerCaseFileName.find(ext);
        if (found != std::string::npos)
        {
            std::string name = filename;
            name.replace(int(found), ext.size(), "");
            return name;
        }
    }

    return {};
}

bool FileScanner::isValidFilename(std::string& filename)
{
    if (filename.find("__MACOSX") != std::string::npos)
        return false;
    return true;
}

// generic implementation using dirent
void FileScanner::scanGeneric(ScanCallback cb, const char* currentDir)
{
    DIR* m_dir;

    // Allocate a new a stream given the current directory name
    if ((m_dir = opendir(currentDir)) == NULL)
    {
        return; // no files found in here
    }

    struct dirent* dir_entry;

    while ((dir_entry = readdir(m_dir)) != NULL)
    {
        // Convert char * to friendly string
        std::string filename(dir_entry->d_name);

        // Some sanity checks
        if (!isValidFilename(filename))
            continue;
        if (filename.length() == 0 || filename[0] == '.')
            continue;

        std::string fullPath = std::string(currentDir) + pathSeparator + filename;

#ifndef _WIN32
        // filesystems are free to return DT_UNKNOWN
        if (dir_entry->d_type == DT_UNKNOWN)
        {
            struct stat stat_path;
            if (stat(fullPath.c_str(), &stat_path) == 0)
            {
                /**/ if (S_ISDIR(stat_path.st_mode))
                    dir_entry->d_type = DT_DIR;
                else if (S_ISLNK(stat_path.st_mode))
                    dir_entry->d_type = DT_LNK;
                else if (S_ISREG(stat_path.st_mode))
                    dir_entry->d_type = DT_REG;
            }
        }
#endif

        if (dir_entry->d_type == DT_DIR)
        {
            // recurse into dir
            scanGeneric(cb, fullPath.c_str());
            continue;
        }
        else if (dir_entry->d_type != DT_REG && dir_entry->d_type != DT_LNK)
        {
            // not regular file/link
            continue;
        }

        auto nameMatched = extensionMatches(filename);
        if (!nameMatched.empty())
            cb(fullPath, nameMatched);
    }

    if (m_dir)
    {
        closedir(m_dir);
        m_dir = 0;
    }
}

#if HAVE_FTS_H
// more optimized posix "fts" directory traversal
int fts_compare(const FTSENT** one, const FTSENT** two)
{
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}
#endif

void FileScanner::scanPosix(ScanCallback cb)
{
#if HAVE_FTS_H

    // efficient directory traversal
    FTS* fileSystem = NULL;
    FTSENT* node = NULL;

    if (_rootDirs.empty())
    {
        return;
    }

    // list of directories to scan
    auto rootDirCount = _rootDirs.size();

    char** dirList = (char**) malloc(sizeof(char*) * (rootDirCount + 1));
    for (unsigned long i = 0; i < rootDirCount; i++)
    {
        dirList[i] = (char*) _rootDirs[i].c_str();
    }
    dirList[rootDirCount] = NULL;

    // initialize file hierarchy traversal
    fileSystem = fts_open(dirList, FTS_LOGICAL | FTS_NOCHDIR | FTS_NOSTAT, &fts_compare);
    if (fileSystem == NULL)
    {
        std::string s;
        for (std::size_t i = 0; i < _rootDirs.size(); i++)
            s += _rootDirs[i] + ' ';
        handleDirectoryError(s);

        free(dirList);
        return;
    }

    std::string path, name, nameMatched;

    // traverse dirList
    while ((node = fts_read(fileSystem)) != NULL)
    {
        switch (node->fts_info)
        {
            case FTS_F:
            case FTS_SL:
            case FTS_NSOK:
                // found a file
                path = std::string(node->fts_path);
                name = std::string(node->fts_name);

                if (!isValidFilename(path) || !isValidFilename(name))
                {
                    break;
                }

                // check extension
                nameMatched = extensionMatches(name);
                if (!nameMatched.empty())
                {
                    cb(path, nameMatched);
                }
                break;
            default:
                break;
        }
    }
    fts_close(fileSystem);
    free(dirList);

#endif
}
