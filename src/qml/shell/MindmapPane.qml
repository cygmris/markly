// Mindmap view (#18b read-only / #18c editable): renders the current note's heading
// outline as a MindElixir mindmap in a WebEngineView (mindmap.html). When `editable`,
// the map is interactive and Ctrl+S serializes it back to the note buffer (round-trip).
import QtQuick
import QtWebEngine

WebEngineView {
    id: web
    property string text: ""
    property bool editable: false      // #18c: interactive edit + save-back
    property int bufferId: -1          // target buffer for save-back (Views.updateText)
    property bool _ready: false

    url: "qrc:/data/web/mindmap.html"
    backgroundColor: (typeof Theme !== "undefined") ? Theme.canvas : "white"

    function _render() {
        if (_ready)
            web.runJavaScript("renderMindmap(" + JSON.stringify(text) + ", " +
                              (editable ? "true" : "false") + ")");
    }

    // #18c: read the current mindmap tree as Markdown and write it back to the buffer.
    // Visible-guarded so only the active (mindmap-mode) pane responds to the command.
    function saveToNote() {
        if (!_ready || !web.visible) return;
        web.runJavaScript("mindmapMarkdown()", function(md) {
            if (md && md.length > 0 && bufferId >= 0 && typeof Views !== "undefined")
                Views.updateText(bufferId, md);
        });
    }

    // Command entry "保存思维导图到笔记" routes through Views.requestSaveMindmap().
    Connections {
        target: (typeof Views !== "undefined") ? Views : null
        function onSaveMindmapRequested() { web.saveToNote() }
    }

    onLoadingChanged: function(info) {
        if (info.status === WebEngineView.LoadSucceededStatus) { _ready = true; _render(); }
    }
    onTextChanged: _render()

    // Ctrl+S save-back only in editable mode.
    Shortcut {
        sequence: "Ctrl+S"
        enabled: web.editable && web.activeFocus
        onActivated: web.saveToNote()
    }
}
