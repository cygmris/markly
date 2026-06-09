// Markdown source editor: TextEdit + line-number gutter + current-line highlight +
// C++ MarkdownHighlighter on its textDocument. Content syncs to the buffer via Views.
import QtQuick
import Markly.Editor
import "components" as C

Flickable {
    id: root
    property double bufferId: 0
    property string content: ""

    readonly property bool autoIndent: (typeof EditorCfg !== "undefined") ? EditorCfg.autoIndent : true
    readonly property bool continueList: (typeof EditorCfg !== "undefined") ? EditorCfg.continueList : true
    readonly property bool autoPair: (typeof EditorCfg !== "undefined") ? EditorCfg.autoPair : true

    // Move the cursor to line n (1-based), focus, and scroll it into view.
    function gotoLine(n) {
        var t = edit.text, pos = 0, line = 1;
        while (line < n) {
            var nl = t.indexOf("\n", pos);
            if (nl === -1) { pos = t.length; break; }
            pos = nl + 1; line++;
        }
        edit.cursorPosition = pos;
        edit.forceActiveFocus();
        var y = edit.positionToRectangle(pos).y;
        root.contentY = Math.max(0, Math.min(y - root.height / 2, root.contentHeight - root.height));
        root.updateStats();
    }

    // Indent unit string per config.
    function tabUnit() {
        if (typeof EditorCfg !== "undefined" && !EditorCfg.expandTab) return "\t";
        var s = ""; for (var i = 0; i < root.tabWidth; ++i) s += " "; return s;
    }
    // Text of the line containing position p.
    function lineAt(p) {
        var t = edit.text;
        var start = t.lastIndexOf("\n", p - 1) + 1;
        var end = t.indexOf("\n", p);
        if (end === -1) end = t.length;
        return { start: start, end: end, text: t.substring(start, end) };
    }

    // Set the editor text only when the active buffer changes (avoids feedback loop).
    // Read content fresh from C++ to dodge QML binding-evaluation-order staleness.
    onBufferIdChanged: {
        edit.text = (bufferId > 0 && typeof Views !== "undefined") ? Views.textForBuffer(bufferId) : "";
        updateStats();
        // Consume a pending search jump-to-line, if any.
        if (bufferId > 0 && typeof Views !== "undefined") {
            var ln = Views.takePendingGotoLine();
            if (ln >= 1) Qt.callLater(function() { root.gotoLine(ln); });
        }
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
                    var ctrl = (event.modifiers & Qt.ControlModifier);
                    var hasSel = edit.selectionEnd > edit.selectionStart;
                    var multiSel = hasSel && edit.text.substring(edit.selectionStart, edit.selectionEnd).indexOf("\n") !== -1;

                    // Save.
                    if (event.key === Qt.Key_S && ctrl) {
                        if (typeof Views !== "undefined") Views.saveTab(root.bufferId);
                        event.accepted = true; return;
                    }
                    // Find / replace.
                    if (event.key === Qt.Key_F && ctrl) { findBar.open(false); event.accepted = true; return; }
                    if (event.key === Qt.Key_H && ctrl) { findBar.open(true); event.accepted = true; return; }
                    if (event.key === Qt.Key_Escape && findBar.visible) { findBar.close(); event.accepted = true; return; }

                    // Enter: auto-indent / list continuation.
                    if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) &&
                        !(event.modifiers & (Qt.ShiftModifier | Qt.ControlModifier)) &&
                        typeof EditInput !== "undefined") {
                        var ln = root.lineAt(edit.cursorPosition);
                        var r = EditInput.continueOnEnter(ln.text, root.autoIndent, root.continueList);
                        if (r.clearMarker) {
                            edit.remove(ln.start, ln.end);
                            edit.insert(ln.start, r.lineIndent + "\n");
                            edit.cursorPosition = ln.start + r.lineIndent.length + 1;
                        } else {
                            var at = edit.cursorPosition;
                            edit.insert(at, r.insert);
                            edit.cursorPosition = at + r.insert.length;
                        }
                        event.accepted = true; return;
                    }

                    // Multi-line indent / outdent.
                    if (event.key === Qt.Key_Tab && multiSel) {
                        root.reindentSelection(false); event.accepted = true; return;
                    }
                    if (event.key === Qt.Key_Backtab) {
                        root.reindentSelection(true); event.accepted = true; return;
                    }
                    // Single Tab insert (no selection).
                    if (event.key === Qt.Key_Tab) {
                        edit.insert(edit.cursorPosition, root.tabUnit());
                        event.accepted = true; return;
                    }

                    // Auto-pair on opener.
                    if (root.autoPair && event.text.length === 1 && typeof EditInput !== "undefined") {
                        var close = EditInput.pairFor(event.text);
                        if (close.length === 1) {
                            if (hasSel) {
                                var s = edit.selectionStart, e = edit.selectionEnd;
                                var sel = edit.text.substring(s, e);
                                edit.remove(s, e);
                                edit.insert(s, event.text + sel + close);
                                edit.select(s + 1, s + 1 + sel.length);
                            } else {
                                var p = edit.cursorPosition;
                                edit.insert(p, event.text + close);
                                edit.cursorPosition = p + 1;
                            }
                            event.accepted = true; return;
                        }
                        // Skip over an existing close char.
                        if ((event.text === ")" || event.text === "]" || event.text === "}" ||
                             event.text === "\"" || event.text === "`") &&
                            edit.text.charAt(edit.cursorPosition) === event.text) {
                            edit.cursorPosition = edit.cursorPosition + 1;
                            event.accepted = true; return;
                        }
                    }

                    // Backspace between an empty pair: delete both.
                    if (event.key === Qt.Key_Backspace && !hasSel && root.autoPair) {
                        var cp = edit.cursorPosition;
                        var left = edit.text.charAt(cp - 1), right = edit.text.charAt(cp);
                        if (typeof EditInput !== "undefined" && left.length === 1 &&
                            EditInput.pairFor(left) === right && right.length === 1) {
                            edit.remove(cp - 1, cp + 1);
                            event.accepted = true; return;
                        }
                    }
                }
            }
        }
    }

    // Apply indent/outdent to the lines spanned by the current selection (or current line).
    function reindentSelection(outdent) {
        var t = edit.text;
        var selStart = edit.selectionStart, selEnd = edit.selectionEnd;
        var bs = t.lastIndexOf("\n", selStart - 1) + 1;
        var be = t.indexOf("\n", selEnd > selStart ? selEnd - 1 : selEnd);
        if (be === -1) be = t.length;
        var block = t.substring(bs, be);
        var out = outdent ? EditInput.outdentLines(block, root.tabUnit())
                          : EditInput.indentLines(block, root.tabUnit());
        edit.remove(bs, be);
        edit.insert(bs, out);
        edit.select(bs, bs + out.length);
    }

    // Find / replace bar pinned to the visible top of the editor.
    C.FindReplaceBar {
        id: findBar
        target: edit
        width: root.width
        y: root.contentY
        z: 10
    }
}
