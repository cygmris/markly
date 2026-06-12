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
    core/historymgr.cpp
    core/historymgr.h
    core/snippet/snippetmgr.cpp
    core/snippet/snippetmgr.h
    core/task/taskmgr.cpp
    core/task/taskmgr.h
    core/spell/spellchecker.cpp
    core/spell/spellchecker.h
    widgets/spell/spellbridge.cpp
    widgets/spell/spellbridge.h
    widgets/task/taskbridge.cpp
    widgets/task/taskbridge.h
    widgets/locale/localebridge.cpp
    widgets/locale/localebridge.h
    widgets/quick/quickbridge.cpp
    widgets/quick/quickbridge.h
    widgets/snippet/snippetbridge.cpp
    widgets/snippet/snippetbridge.h
    widgets/export/exporthelper.cpp
    widgets/export/exporthelper.h
    core/export/pandocexporter.cpp
    core/export/pandocexporter.h
    widgets/export/pandocbridge.cpp
    widgets/export/pandocbridge.h
    core/export/notemerge.cpp
    core/export/notemerge.h
    widgets/export/mergebridge.cpp
    widgets/export/mergebridge.h
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
    widgets/tags/tagbridge.cpp
    widgets/tags/tagbridge.h
    widgets/images/imagehelper.cpp
    widgets/images/imagehelper.h
    core/imagehost/githubimagehost.cpp
    core/imagehost/githubimagehost.h
    core/imagehost/giteeimagehost.cpp
    core/imagehost/giteeimagehost.h
    core/imagehost/repoimagehost.cpp
    core/imagehost/repoimagehost.h
    core/update/updatechecker.cpp
    core/update/updatechecker.h
    core/desktop/fileassoc.cpp
    core/desktop/fileassoc.h
    core/hotkey/hotkeyparser.cpp
    core/hotkey/hotkeyparser.h
    widgets/hotkey/globalhotkey.cpp
    widgets/hotkey/globalhotkey.h
    widgets/update/updatebridge.cpp
    widgets/update/updatebridge.h
    widgets/update/traycfg.cpp
    widgets/update/traycfg.h
    widgets/images/imagehostbridge.cpp
    widgets/images/imagehostbridge.h
    widgets/editors/markdownhighlighter.cpp
    widgets/editors/markdownhighlighter.h
    widgets/editors/editorcfgqml.cpp
    widgets/editors/editorcfgqml.h
    widgets/editors/editinputqml.cpp
    widgets/editors/editinputqml.h
    core/editor/editinputhelper.cpp
    core/editor/editinputhelper.h
    core/editor/viengine.cpp
    core/editor/viengine.h
    widgets/editors/vibridge.cpp
    widgets/editors/vibridge.h
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

# Hunspell spell-checking (#8c).
find_package(PkgConfig REQUIRED)
pkg_check_modules(HUNSPELL REQUIRED hunspell)

# X11 global hotkey (#17b): optional; GlobalHotkey no-ops without it.
pkg_check_modules(XCB xcb xcb-keysyms)

target_link_libraries(markly_core PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Sql
    Qt6::Qml
    Qt6::Quick
    ${HUNSPELL_LIBRARIES}
)
target_include_directories(markly_core PRIVATE ${HUNSPELL_INCLUDE_DIRS})

if(XCB_FOUND)
    target_link_libraries(markly_core PUBLIC ${XCB_LIBRARIES})
    target_include_directories(markly_core PRIVATE ${XCB_INCLUDE_DIRS})
endif()

# Default config and other packaged resources.
# No BASE: the alias defaults to the path relative to this dir, i.e.
# "data/core/markly.json" -> resource ":/data/core/markly.json".
qt_add_resources(markly_core "markly_data"
    PREFIX "/"
    FILES "data/core/markly.json"
)

# Translations (#20). In markly_core so tests can access :/i18n/*.qm.
qt_add_resources(markly_core "markly_i18n"
    PREFIX "/"
    FILES "i18n/markly_en_US.qm"
)

# Main executable.
qt_add_executable(markly
    main.cpp
    application/application.cpp
    application/application.h
    widgets/mainwindow.cpp
    widgets/mainwindow.h
    widgets/test/testbridge.cpp
    widgets/test/testbridge.h
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
        "data/web/markdown-it-footnote.min.js"
        "data/web/markdown-it-sub.min.js"
        "data/web/markdown-it-sup.min.js"
        "data/web/markdown-it-mark.min.js"
        "data/web/markdown-it-emoji.min.js"
        "data/web/markdown-it-task-lists.js"
        "data/web/markdownItAnchor.umd.js"
        "data/web/markdownItTocDoneRight.umd.js"
        "data/web/prism.min.js"
        "data/web/mermaid.min.js"
        "data/web/raphael.min.js"
        "data/web/flowchart.min.js"
        "data/web/viz.js"
        "data/web/viz.lite.render.js"
        "data/web/wavedrom.theme-default.js"
        "data/web/wavedrom.min.js"
        "data/web/plantuml.synchro2.js"
        "data/web/plantuml.zopfli.raw.min.js"
        "data/web/mindmap.html"
        "data/web/mindmap.mindelixir.js"
        "data/web/katex/katex.min.js"
        "data/web/katex/katex.min.css"
        "data/web/markdown-it-texmath.js"
        "data/web/katex/fonts/KaTeX_AMS-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Caligraphic-Bold.woff2"
        "data/web/katex/fonts/KaTeX_Caligraphic-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Fraktur-Bold.woff2"
        "data/web/katex/fonts/KaTeX_Fraktur-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Main-BoldItalic.woff2"
        "data/web/katex/fonts/KaTeX_Main-Bold.woff2"
        "data/web/katex/fonts/KaTeX_Main-Italic.woff2"
        "data/web/katex/fonts/KaTeX_Main-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Math-BoldItalic.woff2"
        "data/web/katex/fonts/KaTeX_Math-Italic.woff2"
        "data/web/katex/fonts/KaTeX_SansSerif-Bold.woff2"
        "data/web/katex/fonts/KaTeX_SansSerif-Italic.woff2"
        "data/web/katex/fonts/KaTeX_SansSerif-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Script-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Size1-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Size2-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Size3-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Size4-Regular.woff2"
        "data/web/katex/fonts/KaTeX_Typewriter-Regular.woff2"
)

# QML files (loaded by QQuickWidget via qrc:/qml/...).
# Tray icon (#20b): alias packaging/markly.png to :/markly.png.
set_source_files_properties("${CMAKE_SOURCE_DIR}/packaging/markly.png"
    PROPERTIES QT_RESOURCE_ALIAS "markly.png")
qt_add_resources(markly "markly_icon"
    PREFIX "/"
    FILES "${CMAKE_SOURCE_DIR}/packaging/markly.png")

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
        "qml/shell/ViewerPane.qml"
        "qml/shell/MindmapPane.qml"
        "qml/shell/SearchPanel.qml"
        "qml/shell/TagsPanel.qml"
        "qml/shell/QuickAccessPanel.qml"
        "qml/shell/SnippetPanel.qml"
        "qml/shell/ExportView.qml"
        "qml/shell/SettingsDialog.qml"
        "qml/shell/UnitedEntry.qml"
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
        "qml/shell/components/MklModal.qml"
        "qml/shell/components/MklMenu.qml"
        "qml/shell/components/MklMenuItem.qml"
        "qml/shell/components/MklMenuSep.qml"
        "qml/shell/components/MklMenuLabel.qml"
        "qml/shell/components/MklTextField.qml"
        "qml/shell/components/MklButton.qml"
        "qml/shell/components/Toast.qml"
        "qml/shell/components/MklField.qml"
        "qml/shell/components/MklTextArea.qml"
        "qml/shell/components/MklSegmented.qml"
        "qml/shell/components/MklToggle.qml"
        "qml/shell/components/MklCheckbox.qml"
        "qml/shell/components/MklStepper.qml"
        "qml/shell/components/MklDropzone.qml"
        "qml/shell/AppDialogs.qml"
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

# Desktop integration (#21).
install(FILES "${CMAKE_SOURCE_DIR}/packaging/markly.desktop"
    DESTINATION share/applications)
install(FILES "${CMAKE_SOURCE_DIR}/packaging/markly.png"
    DESTINATION share/icons/hicolor/128x128/apps)
