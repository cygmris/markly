// Find / replace bar overlaid on top of the editor (#8). Operates on `target` (a
// TextEdit): find next/prev with wrap, replace current, replace all, case toggle.
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property var target          // the TextEdit to search
    property bool replaceMode: false
    property int matchCount: 0
    property int matchIndex: 0
    property bool caseSensitive: false

    visible: false
    height: visible ? (replaceMode ? 76 : 42) : 0
    color: Theme.bar
    border.color: Theme.border
    border.width: 1

    function open(replace) {
        replaceMode = replace;
        visible = true;
        if (target && target.selectedText.length > 0)
            findField.text = target.selectedText;
        recount();
        findField.forceActiveFocus();
        findField.selectAll();
    }
    function close() {
        visible = false;
        if (target) target.forceActiveFocus();
    }

    function _cmp(hay) { return caseSensitive ? hay : hay.toLowerCase(); }

    function recount() {
        if (!target || findField.text.length === 0) { matchCount = 0; matchIndex = 0; return; }
        var text = _cmp(target.text), kw = _cmp(findField.text);
        var n = 0, from = 0, i;
        while ((i = text.indexOf(kw, from)) !== -1) { n++; from = i + kw.length; }
        matchCount = n;
    }

    // Find the next match at or after `start`; wraps. Returns index or -1.
    function _find(start, backward) {
        if (!target || findField.text.length === 0) return -1;
        var text = _cmp(target.text), kw = _cmp(findField.text);
        if (backward) {
            var idx = text.lastIndexOf(kw, Math.max(0, start - 1));
            if (idx === -1) idx = text.lastIndexOf(kw); // wrap
            return idx;
        } else {
            var i = text.indexOf(kw, start);
            if (i === -1) i = text.indexOf(kw); // wrap
            return i;
        }
    }

    function findNext(backward) {
        if (!target) return;
        var start = backward ? target.selectionStart : target.selectionEnd;
        var i = _find(start, backward);
        if (i === -1) { matchIndex = 0; return; }
        target.select(i, i + findField.text.length);
        // Compute 1-based index among matches.
        var text = _cmp(target.text), kw = _cmp(findField.text), from = 0, n = 0, j;
        while ((j = text.indexOf(kw, from)) !== -1 && j <= i) { n++; from = j + kw.length; }
        matchIndex = n;
    }

    function replaceCurrent() {
        if (!target || findField.text.length === 0) return;
        // If the current selection equals the find term, replace it.
        if (_cmp(target.selectedText) === _cmp(findField.text) && target.selectedText.length > 0) {
            var at = target.selectionStart;
            target.remove(target.selectionStart, target.selectionEnd);
            target.insert(at, replaceField.text);
            target.cursorPosition = at + replaceField.text.length;
        }
        recount();
        findNext(false);
    }

    function replaceAll() {
        if (!target || findField.text.length === 0) return;
        var src = target.text, text = _cmp(src), kw = _cmp(findField.text);
        var out = "", from = 0, i, n = 0;
        while ((i = text.indexOf(kw, from)) !== -1) {
            out += src.substring(from, i) + replaceField.text;
            from = i + kw.length; n++;
        }
        out += src.substring(from);
        if (n > 0) target.text = out;  // triggers onTextChanged -> buffer sync
        recount();
        matchIndex = 0;
    }

    Keys.onEscapePressed: root.close()

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Row {
            spacing: 6
            height: 28
            Rectangle {
                width: 200; height: 28; radius: 7; color: Theme.card
                border.color: findField.activeFocus ? Theme.accent : Theme.border; border.width: 1
                TextField {
                    id: findField
                    anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8
                    verticalAlignment: TextInput.AlignVCenter
                    placeholderText: "查找"; color: Theme.text; placeholderTextColor: Theme.faint
                    font.pixelSize: 13; font.family: Theme.fontUi; background: Item {}
                    onTextChanged: root.recount()
                    onAccepted: root.findNext(false)
                    Keys.onEscapePressed: root.close()
                }
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: findField.text.length === 0 ? "" :
                      (root.matchCount === 0 ? "无结果" : (root.matchIndex + "/" + root.matchCount))
                color: root.matchCount === 0 && findField.text.length > 0 ? Theme.pink : Theme.dim
                font.pixelSize: 12; font.family: Theme.fontMono
                width: 64
            }
            Repeater {
                model: [{ t: "Aa", act: "case" }, { t: "‹", act: "prev" }, { t: "›", act: "next" }]
                Rectangle {
                    width: 28; height: 28; radius: 7
                    property bool sel: modelData.act === "case" && root.caseSensitive
                    color: sel ? Theme.accent : Theme.card
                    border.color: sel ? Theme.accent : Theme.border; border.width: 1
                    Text { anchors.centerIn: parent; text: modelData.t
                           color: parent.sel ? Theme.accentText : Theme.dim
                           font.pixelSize: 13; font.family: Theme.fontUi }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (modelData.act === "case") { root.caseSensitive = !root.caseSensitive; root.recount(); }
                            else root.findNext(modelData.act === "prev");
                        }
                    }
                }
            }
            Rectangle {
                width: 28; height: 28; radius: 7; color: Theme.card; border.color: Theme.border; border.width: 1
                Text { anchors.centerIn: parent; text: "✕"; color: Theme.dim; font.pixelSize: 12 }
                MouseArea { anchors.fill: parent; onClicked: root.close() }
            }
        }

        // Replace row.
        Row {
            visible: root.replaceMode
            spacing: 6
            height: 28
            Rectangle {
                width: 200; height: 28; radius: 7; color: Theme.card
                border.color: replaceField.activeFocus ? Theme.accent : Theme.border; border.width: 1
                TextField {
                    id: replaceField
                    anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8
                    verticalAlignment: TextInput.AlignVCenter
                    placeholderText: "替换为"; color: Theme.text; placeholderTextColor: Theme.faint
                    font.pixelSize: 13; font.family: Theme.fontUi; background: Item {}
                    onAccepted: root.replaceCurrent()
                    Keys.onEscapePressed: root.close()
                }
            }
            Repeater {
                model: [{ t: "替换", act: "one" }, { t: "全部", act: "all" }]
                Rectangle {
                    width: 48; height: 28; radius: 7; color: Theme.card; border.color: Theme.border; border.width: 1
                    Text { anchors.centerIn: parent; text: modelData.t; color: Theme.dim
                           font.pixelSize: 12; font.family: Theme.fontUi }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: modelData.act === "one" ? root.replaceCurrent() : root.replaceAll()
                    }
                }
            }
        }
    }
}
