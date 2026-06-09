// United entry command palette (#17). Ctrl+P overlay: type to run commands or jump
// to notes (name search via #12 Search). Pure QML reusing existing bridges.
import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent
    visible: false

    // Actions injected by the shell: { openSettings, setPage(p), doExport(fmt), newNote }.
    property var actions: ({})
    property int currentIndex: 0

    function show() { visible = true; field.text = ""; root.currentIndex = 0; field.forceActiveFocus(); }
    function hide() { visible = false; if (typeof Search !== "undefined") Search.clear(); }

    // Static command list. Each has a label + run() closure.
    readonly property var commands: [
        { label: "新建笔记", run: function(){ if (root.actions.newNote) root.actions.newNote() } },
        { label: "快速记录（闪念）", run: function(){ if (typeof Quick !== "undefined") Quick.openFlash() } },
        { label: "打开设置", run: function(){ if (root.actions.openSettings) root.actions.openSettings() } },
        { label: "切换 深色/浅色", run: function(){ if (typeof Appearance !== "undefined") Appearance.theme = Appearance.resolvedDark ? 0 : 1 } },
        { label: "聚焦：搜索", run: function(){ if (root.actions.setPage) root.actions.setPage("search") } },
        { label: "聚焦：标签", run: function(){ if (root.actions.setPage) root.actions.setPage("tags") } },
        { label: "聚焦：快速访问", run: function(){ if (root.actions.setPage) root.actions.setPage("quick") } },
        { label: "聚焦：片段", run: function(){ if (root.actions.setPage) root.actions.setPage("snippet") } },
        { label: "导出 PDF", run: function(){ root.doExport("pdf") } },
        { label: "导出 HTML", run: function(){ root.doExport("html") } },
        { label: "导出 Markdown", run: function(){ root.doExport("md") } }
    ]

    // Self-contained export (same as EditorToolbar.doExport, reuses ExportView/#15).
    ExportView { id: exportView }
    function doExport(fmt) {
        if (typeof Views === "undefined" || Views.currentFileName.length === 0) return;
        var base = Views.currentFileName.replace(/\.md$/i, "");
        var ext = fmt === "pdf" ? ".pdf" : (fmt === "html" ? ".html" : ".md");
        var filt = fmt === "pdf" ? "PDF (*.pdf)" : (fmt === "html" ? "HTML (*.html)" : "Markdown (*.md)");
        var path = Dialogs.saveFile("导出", base + ext, filt);
        if (path.length === 0) return;
        if (fmt === "md") {
            Dialogs.notify("导出", Export.writeText(path, Views.currentText) ? "已导出 Markdown" : "导出失败");
        } else {
            exportView.exportTo(fmt, Views.currentText, Views.currentFileDir, path, function(ok) {
                Dialogs.notify("导出", ok ? ("已导出 " + fmt.toUpperCase()) : "导出失败");
            });
        }
    }

    function matchedCommands() {
        var kw = field.text.trim().toLowerCase();
        if (kw.length === 0) return root.commands;
        var out = [];
        for (var i = 0; i < root.commands.length; ++i)
            if (root.commands[i].label.toLowerCase().indexOf(kw) !== -1) out.push(root.commands[i]);
        return out;
    }
    // Combined model: command rows then note rows.
    function rows() {
        var out = [];
        var cmds = matchedCommands();
        for (var i = 0; i < cmds.length; ++i) out.push({ type: "cmd", label: cmds[i].label, run: cmds[i].run });
        if (field.text.trim().length > 0 && typeof Search !== "undefined") {
            var notes = Search.results;
            for (var j = 0; j < notes.length; ++j)
                out.push({ type: "note", label: notes[j].name, path: notes[j].path, idx: j });
        }
        return out;
    }

    function exec(row) {
        if (!row) return;
        if (row.type === "cmd") row.run();
        else if (typeof Search !== "undefined") Search.openResult(row.idx);
        root.hide();
    }

    Rectangle {
        anchors.fill: parent; color: "#80000000"
        MouseArea { anchors.fill: parent; onClicked: root.hide() }
    }

    Rectangle {
        id: card
        anchors.horizontalCenter: parent.horizontalCenter
        y: 90
        width: 560
        height: Math.min(440, 60 + list.contentHeight + 12)
        radius: 14
        color: Theme.window; border.color: Theme.border; border.width: 1
        MouseArea { anchors.fill: parent }

        Column {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 8

            Rectangle {
                width: parent.width; height: 38; radius: 9; color: Theme.card
                border.color: Theme.accent; border.width: 1
                TextField {
                    id: field
                    anchors.fill: parent; anchors.leftMargin: 12; anchors.rightMargin: 12
                    verticalAlignment: TextInput.AlignVCenter
                    placeholderText: "跳转、搜索或运行命令…"
                    color: Theme.text; placeholderTextColor: Theme.faint
                    font.pixelSize: 14; font.family: Theme.fontUi; background: Item {}
                    onTextChanged: {
                        root.currentIndex = 0;
                        if (text.trim().length > 0 && typeof Search !== "undefined")
                            Search.search(text, 1, 0x1, ""); // AllNotebooks, ObjName
                        else if (typeof Search !== "undefined") Search.clear();
                    }
                    Keys.onDownPressed: root.currentIndex = Math.min(root.currentIndex + 1, root.rows().length - 1)
                    Keys.onUpPressed: root.currentIndex = Math.max(root.currentIndex - 1, 0)
                    Keys.onReturnPressed: root.exec(root.rows()[root.currentIndex])
                    Keys.onEscapePressed: root.hide()
                }
            }

            ListView {
                id: list
                width: parent.width
                height: parent.height - 46
                clip: true
                spacing: 2
                model: root.rows()
                delegate: Rectangle {
                    width: list.width
                    height: 36
                    radius: 8
                    color: index === root.currentIndex ? Theme.accentSoft : (hov.hovered ? Theme.hover : "transparent")
                    Row {
                        anchors.left: parent.left; anchors.leftMargin: 12; anchors.verticalCenter: parent.verticalCenter
                        spacing: 10
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 34; height: 18; radius: 5
                            color: modelData.type === "cmd" ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.16) : Theme.hover
                            Text { anchors.centerIn: parent; text: modelData.type === "cmd" ? "命令" : "笔记"
                                   color: modelData.type === "cmd" ? Theme.accent : Theme.dim
                                   font.pixelSize: 9; font.family: Theme.fontUi }
                        }
                        Text { anchors.verticalCenter: parent.verticalCenter; text: modelData.label
                               color: index === root.currentIndex ? Theme.accent : Theme.text
                               font.pixelSize: 13; font.family: Theme.fontUi
                               width: list.width - 70; elide: Text.ElideRight }
                    }
                    HoverHandler { id: hov }
                    MouseArea { anchors.fill: parent; onClicked: root.exec(modelData) }
                }
            }
        }
    }
}
