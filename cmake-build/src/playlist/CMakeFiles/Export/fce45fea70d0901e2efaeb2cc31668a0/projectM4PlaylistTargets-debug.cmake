#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libprojectM::playlist" for configuration "Debug"
set_property(TARGET libprojectM::playlist APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libprojectM::playlist PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libprojectM-4-playlistd.so.4.1.0"
  IMPORTED_SONAME_DEBUG "libprojectM-4-playlistd.so.4"
  )

list(APPEND _cmake_import_check_targets libprojectM::playlist )
list(APPEND _cmake_import_check_files_for_libprojectM::playlist "${_IMPORT_PREFIX}/lib/libprojectM-4-playlistd.so.4.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
