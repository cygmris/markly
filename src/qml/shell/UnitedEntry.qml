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
    property string lang: (typeof Locale !== "undefined") ? Locale.language : "auto"
    readonly property var commands: [
        { label: qsTr("新建笔记"), run: function(){ if (root.actions.newNote) root.actions.newNote() } },
        { label: qsTr("快速记录（闪念）"), run: function(){ if (typeof Quick !== "undefined") Quick.openFlash() } },
        { label: qsTr("打开设置"), run: function(){ if (root.actions.openSettings) root.actions.openSettings() } },
        { label: qsTr("切换 深色/浅色"), run: function(){ if (typeof Appearance !== "undefined") Appearance.theme = Appearance.resolvedDark ? 0 : 1 } },
        { label: qsTr("聚焦：搜索"), run: function(){ if (root.actions.setPage) root.actions.setPage("search") } },
        { label: qsTr("聚焦：标签"), run: function(){ if (root.actions.setPage) root.actions.setPage("tags") } },
        { label: qsTr("聚焦：快速访问"), run: function(){ if (root.actions.setPage) root.actions.setPage("quick") } },
        { label: qsTr("聚焦：片段"), run: function(){ if (root.actions.setPage) root.actions.setPage("snippet") } },
        { label: qsTr("聚焦：编辑器"), run: function(){ if (typeof Views !== "undefined") Views.setViewMode("edit") } },
        { label: qsTr("导出 PDF"), run: function(){ root.doExport("pdf") } },
        { label: qsTr("导出 HTML"), run: function(){ root.doExport("html") } },
        { label: qsTr("导出 Markdown"), run: function(){ root.doExport("md") } },
        { label: qsTr("上传图片到图床"), run: function(){ root.uploadImages() } },
        { label: qsTr("检查更新"), run: function(){ if (typeof Update !== "undefined") Update.check() } },
        { label: qsTr("分屏预览（左编辑右渲染）"), run: function(){ if (typeof Views !== "undefined") Views.setViewMode("split") } },
        { label: qsTr("阅读模式"), run: function(){ if (typeof Views !== "undefined") Views.setViewMode("read") } },
        { label: qsTr("查看思维导图"), run: function(){ if (typeof Views !== "undefined") Views.setViewMode("mindmap") } },
        { label: qsTr("保存思维导图到笔记"), run: function(){ if (typeof Views !== "undefined") Views.requestSaveMindmap() } },
        { label: qsTr("编辑模式"), run: function(){ if (typeof Views !== "undefined") Views.setViewMode("edit") } }
    ]

    // Pandoc export commands (#15b), only when pandoc is available.
    readonly property var pandocCommands: [
        { label: qsTr("导出 Word (Pandoc)"), fmt: "docx", ext: ".docx", filt: "Word (*.docx)" },
        { label: qsTr("导出 EPUB (Pandoc)"), fmt: "epub", ext: ".epub", filt: "EPUB (*.epub)" },
        { label: qsTr("导出 LaTeX (Pandoc)"), fmt: "latex", ext: ".tex", filt: "LaTeX (*.tex)" }
    ]
    function pandocExportToc() {
        if (typeof Views === "undefined" || Views.currentFileName.length === 0) return;
        var base = Views.currentFileName.replace(/\.md$/i, "");
        var path = Dialogs.saveFile(qsTr("导出"), base + ".html", "HTML (*.html)");
        if (path.length === 0) return;
        var ok = Pandoc.exportNoteOpts(Views.currentText, path, "html", true, true, "");
        Dialogs.notify(qsTr("导出"), ok ? qsTr("已导出 含目录 HTML") : qsTr("Pandoc 导出失败"));
    }
    function pandocExport(fmt, ext, filt) {
        if (typeof Views === "undefined" || Views.currentFileName.length === 0) return;
        var base = Views.currentFileName.replace(/\.md$/i, "");
        var path = Dialogs.saveFile(qsTr("导出"), base + ext, filt);
        if (path.length === 0) return;
        var ok = Pandoc.exportNote(Views.currentText, path, fmt);
        Dialogs.notify(qsTr("导出"), ok ? (qsTr("已导出 ") + fmt) : qsTr("Pandoc 导出失败"));
    }

    // Merge all notes in the current folder and export (#15c).
    readonly property var mergeCommands: [
        { label: qsTr("合并导出 Markdown"), fmt: "md", ext: ".md", filt: "Markdown (*.md)", pandoc: false },
        { label: qsTr("合并导出 HTML"), fmt: "html", ext: ".html", filt: "HTML (*.html)", pandoc: true },
        { label: qsTr("合并导出 Word"), fmt: "docx", ext: ".docx", filt: "Word (*.docx)", pandoc: true }
    ]
    function mergeExport(fmt, ext, filt) {
        if (typeof Views === "undefined" || Views.currentFileDir.length === 0) return;
        var path = Dialogs.saveFile(qsTr("合并导出"), "merged" + ext, filt);
        if (path.length === 0) return;
        var ok = Merge.exportDir(Views.currentFileDir, path, fmt);
        Dialogs.notify(qsTr("合并导出"), ok ? (qsTr("已导出 ") + fmt) : qsTr("合并导出失败"));
    }

    // Update check result -> notify (#20b).
    Connections {
        target: (typeof Update !== "undefined") ? Update : null
        function onChecked(hasUpdate, latest, error) {
            if (typeof Dialogs === "undefined") return;
            if (error.length > 0) Dialogs.notify(qsTr("检查更新"), qsTr("检查失败：") + error);
            else if (hasUpdate) Dialogs.notify(qsTr("检查更新"), qsTr("有新版本：") + latest);
            else Dialogs.notify(qsTr("检查更新"), qsTr("已是最新版本"));
        }
    }

    // Upload every local image in the current note to the image host (#10b); the
    // editor swaps each local path for the returned URL via ImageHost.uploaded.
    function uploadImages() {
        if (typeof ImageHost === "undefined" || !ImageHost.configured()) {
            if (typeof Dialogs !== "undefined") Dialogs.notify(qsTr("图床"), qsTr("请先在设置中配置图床"));
            return;
        }
        if (typeof Views === "undefined") return;
        var text = Views.currentText, dir = Views.currentFileDir;
        var re = /!\[[^\]]*\]\(([^)\s]+)\)/g, m, count = 0, seen = {};
        while ((m = re.exec(text)) !== null) {
            var p = m[1];
            if (/^(https?:|data:)/.test(p) || seen[p]) continue;
            seen[p] = true; count++;
            ImageHost.upload(p, dir);
        }
        if (typeof Dialogs !== "undefined")
            Dialogs.notify(qsTr("图床"), count > 0 ? (qsTr("上传中：") + count) : qsTr("没有可上传的本地图片"));
    }

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

    function allCommands() {
        var list = root.commands.slice();
        var pandocOk = (typeof Pandoc !== "undefined") && Pandoc.available();
        if (pandocOk) {
            for (var i = 0; i < root.pandocCommands.length; ++i) {
                var pc = root.pandocCommands[i];
                list.push({ label: pc.label,
                            run: (function(c){ return function(){ root.pandocExport(c.fmt, c.ext, c.filt); }; })(pc) });
            }
            list.push({ label: qsTr("导出 HTML（含目录）"), run: function(){ root.pandocExportToc(); } });
        }
        // Merged folder export (#15c): md always; html/docx need pandoc.
        for (var j = 0; j < root.mergeCommands.length; ++j) {
            var mc = root.mergeCommands[j];
            if (mc.pandoc && !pandocOk) continue;
            list.push({ label: mc.label,
                        run: (function(c){ return function(){ root.mergeExport(c.fmt, c.ext, c.filt); }; })(mc) });
        }
        return list;
    }
    function matchedCommands() {
        var cmds = allCommands();
        var kw = field.text.trim().toLowerCase();
        if (kw.length === 0) return cmds;
        var out = [];
        for (var i = 0; i < cmds.length; ++i)
            if (cmds[i].label.toLowerCase().indexOf(kw) !== -1) out.push(cmds[i]);
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
                    placeholderText: qsTr("跳转、搜索或运行命令…")
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
