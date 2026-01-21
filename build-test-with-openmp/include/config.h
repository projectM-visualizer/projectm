#pragma once

/* Define USE_GLES */
/* #undef USE_GLES */

/* Define PROJECTM_USE_THREADS */
#define PROJECTM_USE_THREADS 0

/* Version number of package */
#define VERSION "4.1.0"

/* Boost or std filesystem API support */
/* #undef PROJECTM_FILESYSTEM_USE_BOOST */
#define PROJECTM_FILESYSTEM_NAMESPACE std
#define PROJECTM_FILESYSTEM_INCLUDE <filesystem>
