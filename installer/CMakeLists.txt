#
# Naming and versioning.
#

# Product version.
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_BUILD "0")
set(VERSION_REVISION "1")

#
# CPack.
#

list(APPEND CPACK_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/nsis")

# CPack configuration
set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "wooniezie-robot")
set(CPACK_PACKAGE_VENDOR "Avantize")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Test application")
set(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}.${VERSION_REVISION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "wooniezie-robot")
set(CPACK_NSIS_MODIFY_PATH OFF)
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/deploy/")
if (CMAKE_CL_64)
	set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
else()
	set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
endif()

# Disables the component-based installation mechanism.
set(CPACK_MONOLITHIC_INSTALL ON)

# Ask about uninstalling previous versions first
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Set the programs displayed under the Start menu shortcut
# We do not use CPACK_PACKAGE_EXECUTABLES because it expects the binaries to be in a
# directory named "bin" (CMake bug tracker ID: 0007829).

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "	
	# Includes custom plugin directory.
	!addplugindir '${CMAKE_SOURCE_DIR}/cmake/nsis/Plugins'
	ExecShell 'open' '$INSTDIR\\\\App.exe'
")

include(CPack)
