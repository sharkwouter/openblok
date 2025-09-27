cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(openblok LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(ProjectVersion)

# Default to release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()


# Portable install
set(INSTALL_PORTABLE_DEFAULT OFF)
if(WIN32 OR CYGWIN)
    set(INSTALL_PORTABLE_DEFAULT ON)
endif()
option(INSTALL_PORTABLE "The installation step should put the data directory next to the runtime" ${INSTALL_PORTABLE_DEFAULT})

option(USE_BUNDLED_SDL2 "Download and build SDL2 libraries" OFF)
option(USE_BUNDLED_SDL2PP "Download and build libSDL2pp" ON)
option(USE_BUNDLED_UNITTESTPP "Download and build UnitTest++" ON)

option(ENABLE_MP3 "Enable MP3 music support" ON)
option(ENABLE_MOD "Enable MOD music support" ON)
option(ENABLE_FLAC "Enable FLAC music support" ON)

# Currently unit tests only work only in Debug
set(BUILD_TESTS_DEFAULT OFF)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(BUILD_TESTS_DEFAULT ON)
endif()
option(BUILD_TESTS "Build the unit tests" ${BUILD_TESTS_DEFAULT})
option(BUILD_TEST_COVERAGE "Build the test coverage report" OFF)

# Intallation locations
if(INSTALL_PORTABLE)
    set(EXEDIR "." CACHE STRING "Install location of the runtime executable")
    set(DATADIR "./data" CACHE STRING "Install location of the data files")
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
else()
    set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share" CACHE STRING "Base directory of installed data files")
    set(EXEDIR "${CMAKE_INSTALL_PREFIX}/games" CACHE STRING "Install location of the runtime executable")
    set(DATADIR "${SHAREDIR}/openblok" CACHE STRING "Install location of the data files")
    if(UNIX AND NOT APPLE AND NOT CYGWIN)
        option(INSTALL_DESKTOPICON "Install desktop shortcut and icon" ON)
        option(INSTALL_APPSTREAM "Install AppStream metainfo" ON)
        set(ICONDIR "${SHAREDIR}/pixmaps" CACHE STRING "Install location of the icon file")
        set(DESKTOPDIR "${SHAREDIR}/applications" CACHE STRING "Install location of the desktop shortcut")
        set(APPSTREAMDIR "${SHAREDIR}/metainfo" CACHE STRING "Install location of the AppStream metainfo")
    endif()
endif()

# Localization
option(ENABLE_LOCALES "Enable localization support" ON)
if(ENABLE_LOCALES)
    add_subdirectory(locale)
endif()


# Global compiler settings

# Build coverage report if requested
if(BUILD_TEST_COVERAGE)
    include(CodeCoverage)
    setup_target_for_coverage(openblok_coverage openblok_test coverage)
endif()


include(BundledDependencies)


# The main game source
include_directories(src)
add_subdirectory(src)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


# Install
install(DIRECTORY data/ DESTINATION ${DATADIR} PATTERN "*.txt" EXCLUDE)
if(INSTALL_DESKTOPICON)
    install(FILES etc/linux/hu.mmatyas.openblok.desktop DESTINATION ${DESKTOPDIR})
    install(FILES data/icon.png DESTINATION ${ICONDIR} RENAME openblok.png)
endif()
if(INSTALL_APPSTREAM)
    install(FILES etc/linux/hu.mmatyas.openblok.metainfo.xml DESTINATION ${APPSTREAMDIR})
endif()


# Package
include(CPackConfig)
include(CPack)


# Display settings
set(MSG_BUILDTYPE ${CMAKE_BUILD_TYPE})
set(MSG_TESTS "do not build")
if(BUILD_TESTS)
    set(MSG_TESTS "build, tests only")
    if(BUILD_TEST_COVERAGE)
        set(MSG_TESTS "build, with coverage")
    endif()
endif()
set(MSG_INSTALL "install to ${CMAKE_INSTALL_PREFIX}")
if(INSTALL_PORTABLE)
    set(MSG_INSTALL "portable, default ${MSG_INSTALL}")
endif()
message(STATUS "|")
message(STATUS "|  Build type:       ${MSG_BUILDTYPE}")
message(STATUS "|  Tests:            ${MSG_TESTS}")
message(STATUS "|  Install:          ${MSG_INSTALL}")
message(STATUS "|  - runtime dir:    ${EXEDIR}")
message(STATUS "|  - data dir:       ${DATADIR}")
if(INSTALL_DESKTOPICON)
    message(STATUS "|  - shortcut, icon: yes")
endif()
if(INSTALL_APPSTREAM)
    message(STATUS "|  - appstream:      yes")
endif()
if(ENABLE_LOCALES)
    message(STATUS "|  - localization:   yes")
endif()
message(STATUS "|")
