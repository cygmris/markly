// Design-system dialogs + toast for the Refined shell (spec refined-dialogs-menus).
// Replaces native QInputDialog prompts for new note/folder/notebook, rename, delete.
import QtQuick
import "components" as C

Item {
    id: root
    anchors.fill: parent
    z: 150

    function reportError(err) { if (err && err.length > 0) Dialogs.notify("操作失败", err) }
    function toast(msg) { toastBox.show(msg) }

    // Target container for new note/folder (0 = notebook root).
    property double containerId: 0
    // Rename/delete target.
    property double targetId: 0
    property string targetName: ""
    property string targetType: "file"

    function openNewNote(container) { containerId = container || 0; noteField.text = "新笔记"; newNoteDlg.open(); noteField.takeFocus() }
    function openNewFolder(container) { containerId = container || 0; folderField.text = "新文件夹"; newFolderDlg.open(); folderField.takeFocus() }
    function openNewNotebook() { nbDir.text = ""; nbName.text = "我的笔记本"; newNotebookDlg.open(); nbName.takeFocus() }
    function openRename(id, name, type) { targetId = id; targetName = name; targetType = type; renameField.text = name; renameDlg.open(); renameField.takeFocus() }
    function openDelete(id, name, type) { targetId = id; targetName = name; targetType = type; deleteDlg.open() }

    // ---- 新建笔记 ----
    C.MklModal {
        id: newNoteDlg
        title: "新建笔记"; subtitle: "Markdown 文档"; iconName: "filePlus"
        C.MklMenuLabel { label: "名称" }
        C.MklTextField { id: noteField; suffix: ".md"; onAccepted: newNoteDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: newNoteDlg.close() },
            C.MklButton { label: "创建"; primary: true; onClicked: newNoteDlg.confirm() }
        ]
        function confirm() {
            var n = noteField.text.trim();
            if (n.length === 0) return;
            if (!/\.md$/i.test(n)) n += ".md";
            root.reportError(Explorer.newNote(root.containerId, n));
            newNoteDlg.close(); root.toast("已创建 " + n);
        }
    }

    // ---- 新建文件夹 ----
    C.MklModal {
        id: newFolderDlg
        title: "新建文件夹"; iconName: "folderPlus"
        C.MklMenuLabel { label: "名称" }
        C.MklTextField { id: folderField; onAccepted: newFolderDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: newFolderDlg.close() },
            C.MklButton { label: "创建"; primary: true; onClicked: newFolderDlg.confirm() }
        ]
        function confirm() {
            var n = folderField.text.trim();
            if (n.length === 0) return;
            root.reportError(Explorer.newFolder(root.containerId, n));
            newFolderDlg.close(); root.toast("已创建文件夹 " + n);
        }
    }

    // ---- 新建笔记本 ----
    C.MklModal {
        id: newNotebookDlg
        title: "新建笔记本"; subtitle: "选择一个空目录作为根目录"; iconName: "notebook"
        C.MklMenuLabel { label: "根目录" }
        Row {
            width: parent.width; spacing: 8
            C.MklTextField { id: nbDir; width: parent.width - 84; mono: true; placeholder: "~/Notebooks/my-notebook" }
            C.MklButton { label: "浏览…"; onClicked: { var d = Dialogs.chooseDirectory("选择笔记本根目录"); if (d.length > 0) nbDir.text = d } }
        }
        Item { width: 1; height: 12 }
        C.MklMenuLabel { label: "名称" }
        C.MklTextField { id: nbName; onAccepted: newNotebookDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: newNotebookDlg.close() },
            C.MklButton { label: "创建"; primary: true; onClicked: newNotebookDlg.confirm() }
        ]
        function confirm() {
            var d = nbDir.text.trim(), n = nbName.text.trim();
            if (d.length === 0 || n.length === 0) return;
            if (!Explorer.newNotebookAt(d, n, "")) Dialogs.notify("失败", "无法在该目录创建笔记本。");
            else root.toast("已创建笔记本 " + n);
            newNotebookDlg.close();
        }
    }

    // ---- 重命名 ----
    C.MklModal {
        id: renameDlg
        title: "重命名"; iconName: "edit"
        C.MklMenuLabel { label: "名称" }
        C.MklTextField { id: renameField; onAccepted: renameDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: renameDlg.close() },
            C.MklButton { label: "重命名"; primary: true; onClicked: renameDlg.confirm() }
        ]
        function confirm() {
            var n = renameField.text.trim();
            if (n.length === 0 || n === root.targetName) { renameDlg.close(); return }
            root.reportError(Explorer.renameNode(root.targetId, n));
            renameDlg.close(); root.toast("已重命名");
        }
    }

    // ---- 删除确认 ----
    C.MklModal {
        id: deleteDlg
        title: "删除" + (root.targetType === "folder" ? "文件夹" : "笔记")
        subtitle: root.targetName
        iconName: "trash"
        Text {
            width: parent.width
            wrapMode: Text.Wrap
            text: "将从磁盘删除「" + root.targetName + "」" +
                  (root.targetType === "folder" ? " 及其全部内容" : "") + "，此操作不可恢复。"
            color: Theme.dim; font.pixelSize: 13; font.family: Theme.fontUi
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: deleteDlg.close() },
            C.MklButton { label: "删除"; primary: true; danger: true; onClicked: deleteDlg.confirm() }
        ]
        function confirm() {
            Explorer.removeToRecycle(root.targetId);
            deleteDlg.close(); root.toast("已删除");
        }
    }

    // ---- 插入链接 ----
    function openInsertLink() { linkText.text = ""; linkUrl.text = ""; insertLinkDlg.open(); linkText.takeFocus() }
    C.MklModal {
        id: insertLinkDlg
        title: "插入链接"; iconName: "link"
        C.MklMenuLabel { label: "文字" }
        C.MklTextField { id: linkText; placeholder: "链接文字" }
        Item { width: 1; height: 12 }
        C.MklMenuLabel { label: "URL" }
        C.MklTextField { id: linkUrl; mono: true; placeholder: "https://"; onAccepted: insertLinkDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertLinkDlg.close() },
            C.MklButton { label: "插入"; primary: true; onClicked: insertLinkDlg.confirm() }
        ]
        function confirm() {
            var u = linkUrl.text.trim(); if (u.length === 0) return;
            var t = linkText.text.trim() || u;
            var md = "[" + t + "](" + u + ")";
            Views.requestInsert(md, md.length);
            insertLinkDlg.close();
        }
    }

    // ---- 插入图片 ----
    function openInsertImage() { imgSrc.text = ""; imgAlt.text = ""; insertImageDlg.open(); imgSrc.takeFocus() }
    C.MklModal {
        id: insertImageDlg
        title: "插入图片"; iconName: "image"
        C.MklMenuLabel { label: "路径或 URL" }
        Row {
            width: parent.width; spacing: 8
            C.MklTextField { id: imgSrc; width: parent.width - 84; mono: true; placeholder: "https:// 或本地路径" }
            C.MklButton { label: "浏览…"; onClicked: { var f = Dialogs.chooseFile("选择图片", "图片 (*.png *.jpg *.jpeg *.gif *.svg *.webp)"); if (f.length > 0) imgSrc.text = f } }
        }
        Item { width: 1; height: 12 }
        C.MklMenuLabel { label: "替代文字（可选）" }
        C.MklTextField { id: imgAlt; onAccepted: insertImageDlg.confirm() }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertImageDlg.close() },
            C.MklButton { label: "插入"; primary: true; onClicked: insertImageDlg.confirm() }
        ]
        function confirm() {
            var src = imgSrc.text.trim(); if (src.length === 0) return;
            var md = "![" + imgAlt.text.trim() + "](" + src + ")";
            Views.requestInsert(md, md.length);
            insertImageDlg.close();
        }
    }

    // ---- 插入表格 ----
    function openInsertTable() { tblRows.text = "3"; tblCols.text = "3"; insertTableDlg.open(); tblRows.takeFocus() }
    C.MklModal {
        id: insertTableDlg
        title: "插入表格"; iconName: "table"
        Row {
            width: parent.width; spacing: 12
            Column {
                width: (parent.width - 12) / 2
                C.MklMenuLabel { label: "行数" }
                C.MklTextField { id: tblRows; mono: true }
            }
            Column {
                width: (parent.width - 12) / 2
                C.MklMenuLabel { label: "列数" }
                C.MklTextField { id: tblCols; mono: true; onAccepted: insertTableDlg.confirm() }
            }
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertTableDlg.close() },
            C.MklButton { label: "插入"; primary: true; onClicked: insertTableDlg.confirm() }
        ]
        function confirm() {
            var r = Math.max(1, Math.min(20, parseInt(tblRows.text) || 3));
            var c = Math.max(1, Math.min(20, parseInt(tblCols.text) || 3));
            var line = function(fill) { var s = "|"; for (var i = 0; i < c; ++i) s += " " + fill + " |"; return s + "\n" }
            var md = line("列");
            md += line("---");
            for (var i = 0; i < r; ++i) md += line(" ");
            Views.requestInsert(md, 2);
            insertTableDlg.close();
        }
    }

    // ---- 字数统计 ----
    function openWordCount() { wordCountDlg.open() }
    C.MklModal {
        id: wordCountDlg
        title: "字数统计"; subtitle: (typeof Views !== "undefined") ? Views.currentFileName : ""; iconName: "file"
        Column {
            width: parent.width; spacing: 10
            Repeater {
                model: {
                    var t = (typeof Views !== "undefined") ? Views.currentText : "";
                    return [
                        { k: "字符数", v: t.length },
                        { k: "非空白字符", v: t.replace(/\s/g, "").length },
                        { k: "行数", v: t.length === 0 ? 0 : t.split("\n").length }
                    ];
                }
                delegate: Row {
                    required property var modelData
                    width: parent.width
                    Text { width: parent.width - 90; text: modelData.k; color: Theme.dim; font.pixelSize: 13; font.family: Theme.fontUi }
                    Text { text: String(modelData.v); color: Theme.text; font.pixelSize: 13; font.bold: true; font.family: Theme.fontMono }
                }
            }
        }
        footer: [ C.MklButton { label: "关闭"; primary: true; onClicked: wordCountDlg.close() } ]
    }

    // 工具栏 image/link/table/moreV 经 Views 信号到达（仅 Refined 实例化生效）。
    Connections {
        target: (typeof Views !== "undefined") ? Views : null
        function onContentDialogRequested(kind) {
            if (kind === "image") root.openInsertImage();
            else if (kind === "link") root.openInsertLink();
            else if (kind === "table") root.openInsertTable();
            else if (kind === "wordcount") root.openWordCount();
        }
    }

    C.Toast { id: toastBox }
}
