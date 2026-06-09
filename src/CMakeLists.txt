# Core library (logic layer, no UI dependency beyond what Qt Widgets needs for now).
# Kept as a static lib so tests can link against it without rebuilding sources.
set(MARKLY_CORE_SOURCES
    core/configmgr.cpp
    core/configmgr.h
    core/coreconfig.cpp
    core/coreconfig.h
    core/editorconfig.cpp
    core/editorconfig.h
    core/exception.h
    core/global.h
    core/iconfig.h
    core/logger.cpp
    core/logger.h
    core/mainconfig.cpp
    core/mainconfig.h
    core/marklyapp.cpp
    core/marklyapp.h
    core/noncopyable.h
    core/sessionconfig.cpp
    core/sessionconfig.h
    core/singleinstanceguard.cpp
    core/singleinstanceguard.h
    core/thememgr.cpp
    core/thememgr.h
    core/widgetconfig.cpp
    core/widgetconfig.h
    utils/fileutils.cpp
    utils/fileutils.h
    utils/pathutils.cpp
    utils/pathutils.h
    application/commandlineoptions.cpp
    application/commandlineoptions.h
)

qt_add_library(markly_core STATIC ${MARKLY_CORE_SOURCES})

target_include_directories(markly_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(markly_core PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Default config and other packaged resources.
# No BASE: the alias defaults to the path relative to this dir, i.e.
# "data/core/markly.json" -> resource ":/data/core/markly.json".
qt_add_resources(markly_core "markly_data"
    PREFIX "/"
    FILES "data/core/markly.json"
)

# Main executable.
qt_add_executable(markly
    main.cpp
    application/application.cpp
    application/application.h
    widgets/mainwindow.cpp
    widgets/mainwindow.h
)

target_link_libraries(markly PRIVATE markly_core)

install(TARGETS markly
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
