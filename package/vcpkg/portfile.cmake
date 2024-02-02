find_program(GIT git)

# GitHub repository
set(GIT_URL "https://github.com/projectM-visualizer/projectm.git")

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/GIT_REF)
  file(READ ${CMAKE_CURRENT_LIST_DIR}/GIT_REF GIT_REF)
  string(STRIP ${GIT_REF} GIT_REF)
else()
  # set(GIT_REF "v${VERSION}")
  set(GIT_REF "master")
endif()

# Set variables for the port
set(PORT_NAME "libprojectM")
set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src/${GIT_REF}.clean)

# Create the source directory, if it does not exist
if(NOT EXISTS "${SOURCE_PATH}")
    file(MAKE_DIRECTORY "${SOURCE_PATH}")
endif()

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/GIT_REF)
  set(GIT_CLONE_COMMAND ${GIT} clone --recurse-submodules ${GIT_URL} ${SOURCE_PATH})
else()
  set(GIT_CLONE_COMMAND ${GIT} clone --depth 1 --branch master --recurse-submodules ${GIT_URL} ${SOURCE_PATH})
endif()

# Pull source from GitHub with submodules
if(EXISTS "${SOURCE_PATH}" AND NOT EXISTS "${SOURCE_PATH}/.git")

	  message(STATUS "Fetching source from ${GIT_URL} with ref ${GIT_REF}")
    vcpkg_execute_required_process(
      COMMAND ${GIT_CLONE_COMMAND}
      WORKING_DIRECTORY ${SOURCE_PATH}
      LOGNAME "clone-dbg"
	  )

    vcpkg_execute_required_process(
      COMMAND ${GIT} checkout ${GIT_REF}
      WORKING_DIRECTORY ${SOURCE_PATH}
      LOGNAME "checkout-dbg"
    )

    vcpkg_execute_required_process(
      COMMAND ${GIT} submodule update --init --recursive
      WORKING_DIRECTORY ${SOURCE_PATH}
      LOGNAME "submodule-dbg"
    )

else()

    message(STATUS "Source already exists, skipping git clone")

endif()

# Configure CMake for the project
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

# Install the project using CMake
vcpkg_cmake_install(
    ADD_BIN_TO_PATH
)

# Create directories
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/share/${PORT_NAME}")
file(MAKE_DIRECTORY "${CURRENT_PACKAGES_DIR}/debug/share/${PORT_NAME}")

# Perform any necessary fixups for the installed package
vcpkg_cmake_config_fixup(PACKAGE_NAME "${PORT_NAME}")

# Remove unnecessary files in the debug include directory
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# Install LICENSE and configure_file (usage file)
file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT_NAME}" RENAME copyright)
configure_file("${CMAKE_CURRENT_LIST_DIR}/usage" "${CURRENT_PACKAGES_DIR}/share/${PORT_NAME}/usage" COPYONLY)