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
    core/theme/tokens.h
    core/theme/thememodel.cpp
    core/theme/thememodel.h
    core/theme/appearanceconfig.cpp
    core/theme/appearanceconfig.h
    core/theme/appearance.cpp
    core/theme/appearance.h
    core/theme/stylesheetgenerator.cpp
    core/theme/stylesheetgenerator.h
    core/notebookmgr.cpp
    core/notebookmgr.h
    core/buffer/buffer.cpp
    core/buffer/buffer.h
    core/buffer/buffermgr.cpp
    core/buffer/buffermgr.h
    widgets/explorer/notebookexplorer.cpp
    widgets/explorer/notebookexplorer.h
    widgets/explorer/dialoghelper.cpp
    widgets/explorer/dialoghelper.h
    widgets/viewarea/viewarea.cpp
    widgets/viewarea/viewarea.h
    core/search/searchdata.h
    core/search/searcher.cpp
    core/search/searcher.h
    widgets/search/searchbridge.cpp
    widgets/search/searchbridge.h
    widgets/editors/markdownhighlighter.cpp
    widgets/editors/markdownhighlighter.h
    widgets/editors/editorcfgqml.cpp
    widgets/editors/editorcfgqml.h
    widgets/editors/editinputqml.cpp
    widgets/editors/editinputqml.h
    core/editor/editinputhelper.cpp
    core/editor/editinputhelper.h
    core/notebook/nodevisual.h
    core/notebook/node.cpp
    core/notebook/node.h
    core/notebook/notebookdatabaseaccess.cpp
    core/notebook/notebookdatabaseaccess.h
    core/notebook/notebooktagmgr.cpp
    core/notebook/notebooktagmgr.h
    core/notebook/notebook.cpp
    core/notebook/notebook.h
    core/versioncontroller/iversioncontroller.h
    core/notebookbackend/inotebookbackend.h
    core/notebookbackend/localnotebookbackend.cpp
    core/notebookbackend/localnotebookbackend.h
    core/notebookconfigmgr/vxnodeconfig.cpp
    core/notebookconfigmgr/vxnodeconfig.h
    core/notebookconfigmgr/notebookconfig.cpp
    core/notebookconfigmgr/notebookconfig.h
    utils/colorutils.cpp
    utils/colorutils.h
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
    Qt6::Sql
    Qt6::Qml
    Qt6::Quick
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
    widgets/framelessmainwindow/framelessmainwindow.cpp
    widgets/framelessmainwindow/framelessmainwindow.h
)

target_link_libraries(markly PRIVATE
    markly_core
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::QuickControls2
    Qt6::WebEngineQuick
)

# Markdown preview web assets.
qt_add_resources(markly "markly_web"
    PREFIX "/"
    FILES
        "data/web/preview.html"
        "data/web/preview.css"
        "data/web/markdown-it.min.js"
)

# QML files (loaded by QQuickWidget via qrc:/qml/...).
qt_add_resources(markly "markly_qml"
    PREFIX "/"
    FILES
        "qml/MarklyShell.qml"
        "qml/shell/NodeContextMenu.qml"
        "qml/shell/NotebookSelector.qml"
        "qml/shell/EmptyState.qml"
        "qml/shell/EditorArea.qml"
        "qml/shell/MarkdownEditor.qml"
        "qml/shell/PreviewPane.qml"
        "qml/shell/SearchPanel.qml"
        "qml/shell/components/FindReplaceBar.qml"
        "qml/theme/ThemePreview.qml"
        "qml/theme/AppearancePanel.qml"
        "qml/icons/Icon.qml"
        "qml/icons/IconPaths.js"
        "qml/shell/DemoData.js"
        "qml/shell/ShellRefined.qml"
        "qml/shell/ShellFocus.qml"
        "qml/shell/ShellWorkbench.qml"
        "qml/shell/components/WinControls.qml"
        "qml/shell/components/RailButton.qml"
        "qml/shell/components/StatusBar.qml"
        "qml/shell/components/Chip.qml"
        "qml/shell/components/EditorTab.qml"
        "qml/shell/components/EditorToolbar.qml"
        "qml/shell/components/TreeRow.qml"
        "qml/shell/components/OutlineRow.qml"
        "qml/shell/components/NoteCard.qml"
        "qml/shell/components/Omnibar.qml"
)

install(TARGETS markly
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
