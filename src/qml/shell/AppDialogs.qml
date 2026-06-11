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

    // ---- 插入链接（对齐 refined-editor-dialogs.InsertLink, 460）----
    function openInsertLink() { linkText.text = "官方题解"; linkUrl.text = ""; linkTitle.text = ""; insertLinkDlg.open(); linkText.takeFocus() }
    C.MklModal {
        id: insertLinkDlg
        dlgWidth: 460; title: "插入链接"; iconName: "link"
        C.MklField { label: "显示文本"; C.MklTextField { id: linkText } }
        C.MklField { label: "链接地址"; C.MklTextField { id: linkUrl; mono: true; prefix: "link"; placeholder: "https://… 或 笔记路径"; onAccepted: insertLinkDlg.confirm() } }
        C.MklField { label: "提示标题"; optional: true; C.MklTextField { id: linkTitle; placeholder: "悬停提示" } }
        // 预览行
        Rectangle {
            width: parent.width; height: 36; radius: 9; color: Theme.hover
            Row {
                anchors.left: parent.left; anchors.leftMargin: 12; anchors.verticalCenter: parent.verticalCenter
                Text { text: "预览  "; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontMono }
                Text { text: "[" + (linkText.text || "文本") + "]"; color: Theme.link; font.pixelSize: 12; font.family: Theme.fontMono }
                Text { text: "(" + (linkUrl.text || "url") + ")"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontMono }
            }
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertLinkDlg.close() },
            C.MklButton { label: "插入"; iconName: "link"; primary: true; onClicked: insertLinkDlg.confirm() }
        ]
        function confirm() {
            var u = linkUrl.text.trim(); if (u.length === 0) return;
            var t = linkText.text.trim() || u;
            var ti = linkTitle.text.trim();
            var md = "[" + t + "](" + u + (ti.length > 0 ? ' "' + ti + '"' : "") + ")";
            Views.requestInsert(md, md.length);
            insertLinkDlg.close();
        }
    }

    // ---- 插入图片（对齐 refined-editor-dialogs.InsertImage, 500）----
    property string imgSource: "local"
    function openInsertImage() { imgSource = "local"; imgPath.text = ""; imgUrl.text = ""; imgTitle.text = ""; imgWidth.text = ""; insertImageDlg.open() }
    C.MklModal {
        id: insertImageDlg
        dlgWidth: 500; title: "插入图片"; iconName: "image"
        C.MklField {
            label: "来源"
            C.MklSegmented {
                value: root.imgSource
                options: [{ id: "local", name: "本地文件" }, { id: "url", name: "网络链接" }, { id: "clip", name: "剪贴板" }]
                onPicked: function(id) { root.imgSource = id }
            }
        }
        C.MklField {
            label: root.imgSource === "clip" ? "剪贴板预览" : (root.imgSource === "url" ? "图片地址" : "选择文件")
            // 网络链接：地址输入框
            C.MklTextField {
                visible: root.imgSource === "url"
                id: imgUrl; mono: true; prefix: "link"; placeholder: "https://…/image.png"
            }
            // 本地 / 剪贴板：拖拽区
            C.MklDropzone {
                visible: root.imgSource !== "url"
                hintText: root.imgSource === "clip" ? "从剪贴板粘贴的图片将显示在这里" : "拖入图片，或点击选择"
                showButton: root.imgSource !== "clip"
                onPick: { var f = Dialogs.chooseFile("选择图片", "图片 (*.png *.jpg *.jpeg *.gif *.svg *.webp)"); if (f.length > 0) imgPath.text = f }
            }
            // 已选本地路径回显
            Text {
                visible: root.imgSource === "local" && imgPath.text.length > 0
                width: parent.width; topPadding: 6; elide: Text.ElideMiddle
                text: imgPath.text; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontMono
            }
            TextInput { id: imgPath; visible: false }
        }
        Row {
            width: parent.width; spacing: 12
            C.MklField { width: (parent.width - 12) * 2 / 3; label: "标题"; optional: true; C.MklTextField { id: imgTitle; placeholder: "图片说明" } }
            C.MklField { width: (parent.width - 12) / 3; label: "宽度"; optional: true; C.MklTextField { id: imgWidth; mono: true; placeholder: "auto"; suffix: "px" } }
        }
        // 图床开关
        Rectangle {
            width: parent.width; height: 52; radius: 10; color: Theme.hover
            Row {
                anchors.left: parent.left; anchors.leftMargin: 13; anchors.verticalCenter: parent.verticalCenter
                Column {
                    spacing: 1
                    Text { text: "上传到图床"; color: Theme.text; font.pixelSize: 13; font.weight: Font.DemiBold; font.family: Theme.fontUi }
                    Text { text: "默认图床：GitHub · vnote-images"; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi }
                }
            }
            C.MklToggle { id: imgHost; on: true; anchors.right: parent.right; anchors.rightMargin: 13; anchors.verticalCenter: parent.verticalCenter }
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertImageDlg.close() },
            C.MklButton { label: "插入"; iconName: "image"; primary: true; onClicked: insertImageDlg.confirm() }
        ]
        function confirm() {
            var src = root.imgSource === "url" ? imgUrl.text.trim() : imgPath.text.trim();
            if (src.length === 0) { insertImageDlg.close(); return }
            var md = "![" + imgTitle.text.trim() + "](" + src + ")";
            Views.requestInsert(md, md.length);
            insertImageDlg.close();
        }
    }

    // ---- 插入表格（对齐 refined-editor-dialogs.InsertTable, 480）----
    property string tblAlign: "left"
    C.MklModal {
        id: insertTableDlg
        dlgWidth: 480; title: "插入表格"; iconName: "table"
        Row {
            width: parent.width; spacing: 12
            C.MklField { width: (parent.width - 12) / 2; label: "行数"; C.MklStepper { id: tblRows; value: 3; minValue: 1; maxValue: 12 } }
            C.MklField { width: (parent.width - 12) / 2; label: "列数"; C.MklStepper { id: tblCols; value: 3; minValue: 1; maxValue: 8 } }
        }
        C.MklField {
            label: "对齐"
            C.MklSegmented {
                value: root.tblAlign
                options: [{ id: "left", name: "左" }, { id: "center", name: "中" }, { id: "right", name: "右" }]
                onPicked: function(id) { root.tblAlign = id }
            }
        }
        C.MklCheckbox { id: tblHeader; label: "包含表头"; checked: true; bottomPadding: 14 }
        Text { text: "预览"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.5; bottomPadding: 8; font.family: Theme.fontUi }
        Column {
            width: parent.width; spacing: 4
            Repeater {
                model: Math.min(tblRows.value, 6)
                delegate: Row {
                    id: prow
                    required property int index
                    spacing: 4
                    x: root.tblAlign === "center" ? (parent.width - width) / 2 : (root.tblAlign === "right" ? parent.width - width : 0)
                    Repeater {
                        model: Math.min(tblCols.value, 8)
                        delegate: Rectangle {
                            readonly property bool head: tblHeader.checked && prow.index === 0
                            width: 38; height: 22; radius: 4
                            color: head ? Theme.selection : Theme.hover
                            border.width: head ? 1 : 0; border.color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.33)
                        }
                    }
                }
            }
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertTableDlg.close() },
            C.MklButton { label: "插入"; iconName: "table"; primary: true; onClicked: insertTableDlg.confirm() }
        ]
        function confirm() {
            var r = tblRows.value, c = tblCols.value;
            var sep = root.tblAlign === "center" ? ":---:" : (root.tblAlign === "right" ? "---:" : ":---");
            var line = function(fill) { var s = "|"; for (var i = 0; i < c; ++i) s += " " + fill + " |"; return s + "\n" }
            var md = line("列");
            md += "|"; for (var i = 0; i < c; ++i) md += " " + sep + " |"; md += "\n";
            for (var j = 0; j < r; ++j) md += line(" ");
            Views.requestInsert(md, 2);
            insertTableDlg.close();
        }
    }

    // ---- 插入数学公式（对齐 refined-editor-dialogs.InsertMath, 500）----
    property string mathMode: "block"
    function openInsertMath() { mathMode = "block"; mathTex.text = "\\sum_{i=1}^{n} a_i"; insertMathDlg.open() }
    C.MklModal {
        id: insertMathDlg
        dlgWidth: 500; title: "插入数学公式"; iconName: "sigma"
        C.MklField {
            label: "类型"
            C.MklSegmented {
                value: root.mathMode
                options: [{ id: "inline", name: "行内 $…$" }, { id: "block", name: "独立块 $$…$$" }]
                onPicked: function(id) { root.mathMode = id }
            }
        }
        C.MklField { label: "LaTeX"; C.MklTextArea { id: mathTex; mono: true; rows: 3 } }
        Flow {
            width: parent.width; spacing: 6; bottomPadding: 16
            Repeater {
                model: ["\\frac{a}{b}", "\\sqrt{x}", "\\sum", "\\int", "\\alpha", "\\beta", "\\pi", "\\infty", "\\leq", "\\geq", "\\times", "\\partial"]
                delegate: Rectangle {
                    required property var modelData
                    width: symTxt.implicitWidth + 18; height: 28; radius: 7
                    color: "transparent"; border.width: 1; border.color: Theme.border
                    Text { id: symTxt; anchors.centerIn: parent; text: modelData; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
                    MouseArea { anchors.fill: parent; onClicked: mathTex.text = mathTex.text + " " + modelData }
                }
            }
        }
        footer: [
            C.MklButton { label: "取消"; onClicked: insertMathDlg.close() },
            C.MklButton { label: "插入"; iconName: "sigma"; primary: true; onClicked: insertMathDlg.confirm() }
        ]
        function confirm() {
            var tex = mathTex.text.trim(); if (tex.length === 0) { insertMathDlg.close(); return }
            var md = root.mathMode === "inline" ? ("$" + tex + "$") : ("$$\n" + tex + "\n$$");
            Views.requestInsert(md, md.length);
            insertMathDlg.close();
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
            else if (kind === "math") root.openInsertMath();
            else if (kind === "wordcount") root.openWordCount();
        }
    }

    function openExportDialog() { toast("导出对话框待实现（task 7）") }
    C.Toast { id: toastBox }
}
