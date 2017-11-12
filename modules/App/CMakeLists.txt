cmake_minimum_required(VERSION 3.5.0)

project(App)

#
# 3rd-party libraries.
#

# Qt5.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Positioning REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)
find_package(Qt5WebChannel REQUIRED)
find_package(Qt5WebEngineCore REQUIRED)
find_package(Qt5WebEngineWidgets REQUIRED)
find_package(Qt5Widgets REQUIRED)

#
# Source files.
#

set(SOURCES
	src/Controller.cpp
	src/Controller.h
	src/Main.cpp
	src/MainWindow.cpp
	src/MainWindow.h)

set(UI
	src/MainWindow.ui)

set(MISC
	src/pch.h
	src/targetver.h)

source_group("Implementation Files" FILES ${SOURCES})
source_group("UI Files" FILES ${UI})
source_group("" FILES ${MISC})

#
# Target.
#

add_executable(${PROJECT_NAME} WIN32
	${MISC}
	${SOURCES}
	${UI})

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		Qt5::WebEngineWidgets)

# Copy Qt5 dependencies to the target directory.
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different 
		$<TARGET_FILE:Qt5::Core>
		$<TARGET_FILE:Qt5::Gui>
		$<TARGET_FILE:Qt5::Network>
		$<TARGET_FILE:Qt5::Positioning>
		$<TARGET_FILE:Qt5::PrintSupport>
		$<TARGET_FILE:Qt5::Qml>
		$<TARGET_FILE:Qt5::Quick>
		$<TARGET_FILE:Qt5::QuickWidgets>
		$<TARGET_FILE:Qt5::WebChannel>
		$<TARGET_FILE:Qt5::WebEngineCore>
		$<TARGET_FILE:Qt5::WebEngineWidgets>
		$<TARGET_FILE:Qt5::Widgets>
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
if (MSVC)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
			$<$<CONFIG:debug>:${QT_ROOT}/bin/libEGLd.dll>$<$<CONFIG:release>:${QT_ROOT}/bin/libEGL.dll>
			$<$<CONFIG:debug>:${QT_ROOT}/bin/libGLESv2d.dll>$<$<CONFIG:release>:${QT_ROOT}/bin/libGLESv2.dll>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

# Precompiled headers.
set_target_properties(${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "src/pch.h")
cotire(${PROJECT_NAME})

#
# Installation.
#

install(TARGETS ${PROJECT_NAME}
	DESTINATION . COMPONENT Application)

install(FILES
		$<TARGET_FILE:Qt5::Core>
		$<TARGET_FILE:Qt5::Gui>
		$<TARGET_FILE:Qt5::Network>
		$<TARGET_FILE:Qt5::Positioning>
		$<TARGET_FILE:Qt5::PrintSupport>
		$<TARGET_FILE:Qt5::Qml>
		$<TARGET_FILE:Qt5::Quick>
		$<TARGET_FILE:Qt5::QuickWidgets>
		$<TARGET_FILE:Qt5::WebChannel>
		$<TARGET_FILE:Qt5::WebEngineCore>
		$<TARGET_FILE:Qt5::WebEngineWidgets>
		$<TARGET_FILE:Qt5::Widgets>
	DESTINATION . COMPONENT Application)
if (MSVC)
	install(FILES
			$<$<CONFIG:debug>:${QT_ROOT}/bin/libEGLd.dll>$<$<CONFIG:release>:${QT_ROOT}/bin/libEGL.dll>
			$<$<CONFIG:debug>:${QT_ROOT}/bin/libGLESv2d.dll>$<$<CONFIG:release>:${QT_ROOT}/bin/libGLESv2.dll>
			${QT_ROOT}/bin/QtWebEngineProcess.exe
			${QT_ROOT}/resources/icudtl.dat
			${QT_ROOT}/resources/qtwebengine_resources.pak
			${QT_ROOT}/resources/qtwebengine_resources_100p.pak
			${QT_ROOT}/resources/qtwebengine_resources_200p.pak
		DESTINATION . COMPONENT Application)
	install(FILES
			${QT_ROOT}/plugins/platforms/qwindows.dll
		DESTINATION ./platforms COMPONENT Application)
	install(DIRECTORY
			${QT_ROOT}/translations/qtwebengine_locales
		DESTINATION . COMPONENT Application)
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")

include(InstallRequiredSystemLibraries)
