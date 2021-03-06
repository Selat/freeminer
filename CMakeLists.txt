
cmake_minimum_required(VERSION 2.6)
if(${CMAKE_VERSION} STREQUAL "2.8.2")
  # bug http://vtk.org/Bug/view.php?id=11020
  message( WARNING "CMake/CPack version 2.8.2 will not create working .deb packages!")
endif(${CMAKE_VERSION} STREQUAL "2.8.2")

# This can be read from ${PROJECT_NAME} after project() is called
project(freeminer)

set(VERSION_EXTRA "" CACHE STRING "Stuff to append to version string")

# Also remember to set PROTOCOL_VERSION in clientserver.h when releasing
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 11)
set(VERSION_TWEAK 5)
set(VERSION_PATCH_ORIG ${VERSION_PATCH})

if(VERSION_EXTRA)
	set(VERSION_TWEAK ${VERSION_TWEAK}-${VERSION_EXTRA})
else()
	# Comment the following line during release
	set(VERSION_TWEAK ${VERSION_TWEAK}-dev)
endif()
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}")

MESSAGE(STATUS "*** Will build version ${VERSION_STRING} ***")

# Configuration options

set(RUN_IN_PLACE 1 CACHE BOOL "Run directly in source directory structure")

# RUN_IN_PLACE is exported as a #define value, ensure it's 1/0 instead of ON/OFF
if(RUN_IN_PLACE)
	set(RUN_IN_PLACE 1)
else()
	set(RUN_IN_PLACE 0)
endif()

set(BUILD_CLIENT 1 CACHE BOOL "Build client")
if(WIN32)
	# broken! http://forum.freeminer.org/threads/building-errors.182/#post-1852
	#if( ${CMAKE_SYSTEM_VERSION} VERSION_LESS 6.0 )
	#	MESSAGE(FATAL_ERROR "Building is not supported for Windows ${CMAKE_SYSTEM_VERSION}")
	#endif()
	set(BUILD_SERVER 0 CACHE BOOL "Build server")
else()
	set(BUILD_SERVER 1 CACHE BOOL "Build server")
endif()

set(WARN_ALL 1 CACHE BOOL "Enable -Wall for Release build")

set(DEBUG 0 CACHE BOOL "Build with debug symbols and without optimization")
if(DEBUG)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type: Debug or Release" FORCE)
else()
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()
MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Included stuff
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(${CMAKE_SOURCE_DIR}/cmake/Modules/misc.cmake)

# This is done here so that relative search paths are more reasnable
find_package(Irrlicht)

#
# Installation
#

if(WIN32)
	set(SHAREDIR ".")
	set(BINDIR "bin")
	set(DOCDIR "doc")
	set(EXAMPLE_CONF_DIR ".")
	set(LOCALEDIR "locale")
elseif(APPLE)
	set(SHAREDIR ".")
	set(BINDIR ".")
	set(DOCDIR "./doc/${PROJECT_NAME}")
	set(EXAMPLE_CONF_DIR ${DOCDIR})
	set(LOCALEDIR "locale")
elseif(UNIX) # Linux, BSD etc
	if(RUN_IN_PLACE)
		set(SHAREDIR ".")
		set(BINDIR "bin")
		set(DOCDIR "doc")
		set(EXAMPLE_CONF_DIR ".")
		set(MANDIR "unix/man")
		set(XDG_APPS_DIR "unix/applications")
		set(APPDATADIR "unix/appdata")
		set(ICONDIR "unix/icons")
		set(LOCALEDIR "locale")
	else()
		set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")
		set(BINDIR "${CMAKE_INSTALL_PREFIX}/bin")
		set(DOCDIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME}")
		set(MANDIR "${CMAKE_INSTALL_PREFIX}/share/man")
		set(EXAMPLE_CONF_DIR ${DOCDIR})
		set(XDG_APPS_DIR "${CMAKE_INSTALL_PREFIX}/share/applications")
		set(APPDATADIR "${CMAKE_INSTALL_PREFIX}/share/appdata")
		set(ICONDIR "${CMAKE_INSTALL_PREFIX}/share/icons")
		set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/locale")
	endif()
endif()

set(CUSTOM_SHAREDIR "" CACHE STRING "Directory to install data files into")
if(NOT CUSTOM_SHAREDIR STREQUAL "")
	set(SHAREDIR "${CUSTOM_SHAREDIR}")
	message(STATUS "Using SHAREDIR=${SHAREDIR}")
endif()
set(CUSTOM_BINDIR "" CACHE STRING "Directory to install binaries into")
if(NOT CUSTOM_BINDIR STREQUAL "")
	set(BINDIR "${CUSTOM_BINDIR}")
	message(STATUS "Using BINDIR=${BINDIR}")
endif()
set(CUSTOM_DOCDIR "" CACHE STRING "Directory to install documentation into")
if(NOT CUSTOM_DOCDIR STREQUAL "")
	set(DOCDIR "${CUSTOM_DOCDIR}")
	message(STATUS "Using DOCDIR=${DOCDIR}")
endif()
set(CUSTOM_MANDIR "" CACHE STRING "Directory to install manpages into")
if(NOT CUSTOM_MANDIR STREQUAL "")
	set(MANDIR "${CUSTOM_MANDIR}")
	message(STATUS "Using MANDIR=${MANDIR}")
endif()
set(CUSTOM_EXAMPLE_CONF_DIR "" CACHE STRING "Directory to install example config file into")
if(NOT CUSTOM_EXAMPLE_CONF_DIR STREQUAL "")
	set(EXAMPLE_CONF_DIR "${CUSTOM_EXAMPLE_CONF_DIR}")
	message(STATUS "Using EXAMPLE_CONF_DIR=${EXAMPLE_CONF_DIR}")
endif()
set(CUSTOM_XDG_APPS_DIR "" CACHE STRING "Directory to install .desktop files into")
if(NOT CUSTOM_XDG_APPS_DIR STREQUAL "")
	set(XDG_APPS_DIR "${CUSTOM_XDG_APPS_DIR}")
	message(STATUS "Using XDG_APPS_DIR=${XDG_APPS_DIR}")
endif()
set(CUSTOM_ICONDIR "" CACHE STRING "Directory to install icons into")
if(NOT CUSTOM_ICONDIR STREQUAL "")
	set(ICONDIR "${CUSTOM_ICONDIR}")
	message(STATUS "Using ICONDIR=${ICONDIR}")
endif()
set(CUSTOM_LOCALEDIR "" CACHE STRING "Directory to install l10n files into")
if(NOT CUSTOM_LOCALEDIR STREQUAL "")
	set(LOCALEDIR "${CUSTOM_LOCALEDIR}")
	message(STATUS "Using LOCALEDIR=${LOCALEDIR}")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/builtin" DESTINATION "${SHAREDIR}")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/client" DESTINATION "${SHAREDIR}")
set(DEFAULT_GAME_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/games/default")
if(EXISTS ${DEFAULT_GAME_SOURCE} AND IS_DIRECTORY ${DEFAULT_GAME_SOURCE} AND EXISTS ${DEFAULT_GAME_SOURCE}/game.conf)
	install(FILES ${DEFAULT_GAME_SOURCE}/game.conf DESTINATION "${SHAREDIR}/games/default/")
	install(FILES ${DEFAULT_GAME_SOURCE}/README.txt DESTINATION "${SHAREDIR}/games/default/")
	install(DIRECTORY ${DEFAULT_GAME_SOURCE}/mods DESTINATION "${SHAREDIR}/games/default")
	install(DIRECTORY ${DEFAULT_GAME_SOURCE}/menu DESTINATION "${SHAREDIR}/games/default")
endif()
if(BUILD_CLIENT)
	#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sounds/base/pack" DESTINATION "${SHAREDIR}/sounds/base")
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/textures/base/pack" DESTINATION "${SHAREDIR}/textures/base")
endif()
if(RUN_IN_PLACE)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/mods/mods_here.txt" DESTINATION "${SHAREDIR}/mods")
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/textures/texture_packs_here.txt" DESTINATION "${SHAREDIR}/textures")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fonts" DESTINATION "${SHAREDIR}")

install(FILES "README.md" DESTINATION "${DOCDIR}")
install(FILES "doc/lua_api.txt" DESTINATION "${DOCDIR}")
install(FILES "doc/menu_lua_api.txt" DESTINATION "${DOCDIR}")
install(FILES "doc/mapformat.txt" DESTINATION "${DOCDIR}")
install(FILES "freeminer.conf.example" DESTINATION "${EXAMPLE_CONF_DIR}")

if(UNIX AND NOT APPLE)
	install(FILES "doc/freeminer.6" "doc/freeminerserver.6" DESTINATION "${MANDIR}/man6")
	install(FILES "misc/freeminer.desktop" DESTINATION "${XDG_APPS_DIR}")
	install(FILES "misc/freeminer.appdata.xml" DESTINATION "${APPDATADIR}")
	install(FILES "misc/freeminer.svg" DESTINATION "${ICONDIR}/hicolor/scalable/apps")
endif()

#
# Subdirectories
# Be sure to add all relevant definitions above this
#

add_subdirectory(src)

# CPack

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An InfiniMiner/Minecraft inspired game")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${VERSION_TWEAK})
set(CPACK_PACKAGE_VENDOR "celeron55")
set(CPACK_PACKAGE_CONTACT "Perttu Ahola <celeron55@gmail.com>")

if(WIN32)
	# For some reason these aren't copied otherwise
	# NOTE: For some reason now it seems to work without these
	#if(BUILD_CLIENT)
	#	install(FILES bin/freeminer.exe DESTINATION bin)
	#endif()
	#if(BUILD_SERVER)
	#	install(FILES bin/freeminerserver.exe DESTINATION bin)
	#endif()

	execute_process(COMMAND git describe --always --tag --dirty
		WORKING_DIRECTORY "${GENERATE_VERSION_SOURCE_DIR}"
		OUTPUT_VARIABLE VERSION_GITHASH OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET)

	if(VERSION_GITHASH)
		set(PROJECT_VERSION ${VERSION_GITHASH})
	else()
		set(PROJECT_VERSION ${VERSION_STRING})
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win64")
	else(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win32")
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

	set(CPACK_GENERATOR ZIP)

	# This might be needed for some installer
	#set(CPACK_PACKAGE_EXECUTABLES bin/freeminer.exe "Freeminer" bin/freeminerserver.exe "Freeminer Server")
elseif(APPLE)
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION_STRING}-osx")
	set(CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/misc/freeminer-icon.icns)
	set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
	set(CPACK_BUNDLE_ICON ${CPACK_PACKAGE_ICON})
	set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/misc/Info.plist)
	set(CPACK_GENERATOR "Bundle")
else()
	set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${VERSION_STRING}-linux")
	set(CPACK_GENERATOR TGZ)
	set(CPACK_SOURCE_GENERATOR TGZ)
endif()

include(CPack)

# Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)

