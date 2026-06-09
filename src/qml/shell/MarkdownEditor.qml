// Markdown source editor: TextEdit + line-number gutter + current-line highlight +
// C++ MarkdownHighlighter on its textDocument. Content syncs to the buffer via Views.
import QtQuick
import Markly.Editor

Flickable {
    id: root
    property double bufferId: 0
    property string content: ""

    // Set the editor text only when the active buffer changes (avoids feedback loop).
    // Read content fresh from C++ to dodge QML binding-evaluation-order staleness.
    onBufferIdChanged: {
        edit.text = (bufferId > 0 && typeof Views !== "undefined") ? Views.textForBuffer(bufferId) : "";
        updateStats();
    }
    Component.onCompleted: {
        if (bufferId > 0 && typeof Views !== "undefined") { edit.text = Views.textForBuffer(bufferId); updateStats(); }
    }

    readonly property int fontSize: (typeof EditorCfg !== "undefined") ? EditorCfg.fontSize : 14
    readonly property bool showLineNumbers: (typeof EditorCfg !== "undefined") ? EditorCfg.lineNumber : true
    readonly property bool highlightLine: (typeof EditorCfg !== "undefined") ? EditorCfg.highlightCurrentLine : true
    readonly property int tabWidth: (typeof EditorCfg !== "undefined") ? EditorCfg.tabWidth : 4

    contentWidth: width
    contentHeight: Math.max(height, editRow.height + 32)
    clip: true

    function gutterText() {
        var n = edit.lineCount;
        var s = "";
        for (var i = 1; i <= n; ++i) s += i + (i < n ? "\n" : "");
        return s;
    }
    function updateStats() {
        if (typeof Views === "undefined") return;
        var pos = edit.cursorPosition;
        var before = edit.text.substring(0, pos);
        var line = before.split("\n").length;
        var col = pos - before.lastIndexOf("\n");
        Views.setStats(line, col, edit.lineCount, edit.text.length);
    }

    Row {
        id: editRow
        x: 0; y: 16
        width: root.width
        spacing: 0

        // Line-number gutter (one multi-line Text, same font as the editor -> aligns).
        Text {
            id: gutter
            visible: root.showLineNumbers
            width: 46
            horizontalAlignment: Text.AlignRight
            rightPadding: 14
            text: root.gutterText()
            color: Theme.gutter
            font.family: Theme.fontMono
            font.pixelSize: root.fontSize
            lineHeight: edit.lineHeight2
        }

        Item {
            width: root.width - (root.showLineNumbers ? 46 : 0)
            height: edit.implicitHeight

            // Current-line highlight.
            Rectangle {
                visible: root.highlightLine && edit.activeFocus
                x: 0; width: parent.width
                y: edit.cursorRectangle.y
                height: edit.cursorRectangle.height
                color: Theme.hover
            }

            TextEdit {
                id: edit
                readonly property real lineHeight2: 1.0
                width: parent.width
                wrapMode: TextEdit.NoWrap
                textFormat: TextEdit.PlainText
                color: Theme.text
                selectionColor: Theme.selection
                selectedTextColor: Theme.text
                selectByMouse: true
                persistentSelection: true
                font.family: Theme.fontMono
                font.pixelSize: root.fontSize
                tabStopDistance: root.tabWidth * fontMetrics.advanceWidth(" ")
                leftPadding: 10; rightPadding: 24

                FontMetrics { id: fontMetrics; font: edit.font }

                MarkdownHighlighter { document: edit.textDocument }

                onTextChanged: {
                    // Buffer.setContent no-ops on identical text, so no loop guard needed.
                    if (root.bufferId > 0 && typeof Views !== "undefined")
                        Views.updateText(root.bufferId, text);
                    root.updateStats();
                }
                onCursorPositionChanged: root.updateStats()

                Keys.onPressed: function(event) {
                    if (event.key === Qt.Key_Tab) {
                        var spaces = "";
                        for (var i = 0; i < root.tabWidth; ++i) spaces += " ";
                        edit.insert(edit.cursorPosition, (typeof EditorCfg !== "undefined" && !EditorCfg.expandTab) ? "\t" : spaces);
                        event.accepted = true;
                    } else if (event.key === Qt.Key_S && (event.modifiers & Qt.ControlModifier)) {
                        if (typeof Views !== "undefined") Views.saveTab(root.bufferId);
                        event.accepted = true;
                    }
                }
            }
        }
    }
}
