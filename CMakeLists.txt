cmake_minimum_required(VERSION 3.20)

project(Markly
    VERSION 1.0.0
    DESCRIPTION "A pleasant note-taking platform (Qt 6 rewrite of VNote)"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Generate compile_commands.json for tooling.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Network Sql Qml Quick QuickWidgets QuickControls2 WebEngineQuick)

qt_standard_project_setup()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

# Packaging (#21 / #21b). include(CPack) must come after all install() rules (src/).
set(CPACK_PACKAGE_NAME "Markly")
set(CPACK_PACKAGE_VENDOR "Markly")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "markly@example.com")

# Multi-platform generators (#21b): pick by platform. On Linux, TGZ+STGZ are always
# producible here; DEB/RPM are added only when their tooling is present (guarded), so
# the config never fails on a box that lacks dpkg-deb/rpmbuild.
if(WIN32)
  set(CPACK_GENERATOR "NSIS;ZIP")
  set(CPACK_NSIS_PACKAGE_NAME "Markly")
  set(CPACK_NSIS_DISPLAY_NAME "Markly")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_PACKAGE_FILE_NAME "Markly-${PROJECT_VERSION}-Windows")
elseif(APPLE)
  set(CPACK_GENERATOR "DragNDrop;TGZ")
  set(CPACK_PACKAGE_FILE_NAME "Markly-${PROJECT_VERSION}-macOS")
else()
  set(CPACK_GENERATOR "TGZ;STGZ")
  find_program(DPKG_DEB_EXECUTABLE dpkg-deb)
  find_program(RPMBUILD_EXECUTABLE rpmbuild)
  if(DPKG_DEB_EXECUTABLE)
    list(APPEND CPACK_GENERATOR "DEB")
  endif()
  if(RPMBUILD_EXECUTABLE)
    list(APPEND CPACK_GENERATOR "RPM")
  endif()
  set(CPACK_PACKAGE_FILE_NAME "Markly-${PROJECT_VERSION}-Linux")
endif()

# DEB/RPM metadata (only consumed when the matching generator is active).
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Markly <markly@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6qml6")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")

include(CPack)
