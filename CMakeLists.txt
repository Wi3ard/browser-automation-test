cmake_minimum_required(VERSION 3.5.0)

project(WooniezieRobot)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

set(CMAKE_CXX_STANDARD 11)

include(cotire)

#
# Project subdirectories.
#

set(CMAKE_PREFIX_PATH "${QT_ROOT}")

add_subdirectory(modules/App)
add_subdirectory(installer)
