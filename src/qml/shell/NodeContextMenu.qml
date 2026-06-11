// Right-click menu for a tree node, styled per the refined-menus design.
// Set the target_* props then popup(). Rename/new/delete go through AppDialogs.
import QtQuick
import QtQuick.Controls

Menu {
    id: menu
    property double targetId: 0
    property double targetParentId: 0
    property string targetType: "file"   // file | folder
    property string targetName: ""
    property bool targetIsExternal: false
    property var dialogs: null           // AppDialogs instance

    readonly property double containerId: targetType === "folder" ? targetId : targetParentId
    readonly property color dangerColor: Theme.isDark ? "#e8806f" : "#c64a32"

    width: 236
    background: Rectangle {
        implicitWidth: 236
        color: Theme.window; radius: 11
        border.color: Theme.border; border.width: 1
    }
    delegate: MenuItem {
        id: mi
        implicitHeight: 32
        visible: height > 0
        contentItem: Text {
            leftPadding: 9
            verticalAlignment: Text.AlignVCenter
            text: mi.text
            color: mi.text.indexOf("删除") === 0 ? menu.dangerColor
                 : mi.text === "打开" ? Theme.accent
                 : mi.enabled ? Theme.text : Theme.faint
            font.pixelSize: 13; font.weight: Font.Medium; font.family: Theme.fontUi
        }
        background: Rectangle {
            anchors.fill: parent; anchors.margins: 3
            radius: 7
            color: mi.highlighted ? (mi.text.indexOf("删除") === 0 ? Qt.rgba(0.78, 0.29, 0.2, 0.10) : Theme.hover) : "transparent"
        }
    }

    function reportError(err) { if (err && err.length > 0) Dialogs.notify("操作失败", err) }

    MenuItem { text: "打开"; enabled: menu.targetType === "file"; onTriggered: Explorer.selectNode(menu.targetId) }
    MenuSeparator { contentItem: Rectangle { implicitWidth: 228; implicitHeight: 1; color: Theme.border } }
    MenuItem { text: "新建笔记"; enabled: !menu.targetIsExternal; onTriggered: menu.dialogs.openNewNote(menu.containerId) }
    MenuItem {
        text: "从模板新建笔记…"
        enabled: !menu.targetIsExternal
        onTriggered: {
            var tpl = Dialogs.promptText("从模板新建", "模板（片段）名称", "");
            if (tpl.length > 0) {
                var name = Dialogs.promptText("新建笔记", "笔记名称（含 .md）", "新笔记.md");
                if (name.length > 0) menu.reportError(Explorer.newNoteFromTemplate(menu.containerId, name, tpl));
            }
        }
    }
    MenuItem { text: "新建文件夹"; enabled: !menu.targetIsExternal; onTriggered: menu.dialogs.openNewFolder(menu.containerId) }
    MenuSeparator { contentItem: Rectangle { implicitWidth: 228; implicitHeight: 1; color: Theme.border } }
    MenuItem { text: "重命名"; enabled: !menu.targetIsExternal; onTriggered: menu.dialogs.openRename(menu.targetId, menu.targetName, menu.targetType) }
    MenuItem {
        text: "标签…"
        enabled: menu.targetType === "file" && !menu.targetIsExternal
        onTriggered: {
            var cur = Explorer.nodeTagsCsv(menu.targetId);
            var v = Dialogs.promptText("编辑标签", "逗号分隔（留空清除）", cur);
            Explorer.setNodeTags(menu.targetId, v);
        }
    }
    MenuItem {
        property string absPath: menu.targetType === "file" ? Explorer.nodeAbsPath(menu.targetId) : ""
        text: Quick.isPinned(absPath) ? "取消固定" : "固定到快速访问"
        enabled: menu.targetType === "file"
        onTriggered: {
            if (Quick.isPinned(absPath)) Quick.unpin(absPath);
            else { Quick.pin(absPath); menu.dialogs.toast("已固定到快速访问") }
        }
    }
    MenuItem {
        text: "设置颜色…"
        enabled: !menu.targetIsExternal
        onTriggered: {
            var c = Dialogs.chooseColor("");
            if (c.length > 0) Explorer.setNodeColor(menu.targetId, "", "", c);
        }
    }
    MenuItem { text: "清除颜色"; enabled: !menu.targetIsExternal; onTriggered: Explorer.clearNodeColor(menu.targetId) }
    MenuSeparator { contentItem: Rectangle { implicitWidth: 228; implicitHeight: 1; color: Theme.border } }
    MenuItem {
        text: "复制路径"
        onTriggered: { Dialogs.copyText(Explorer.nodeAbsPath(menu.targetId)); menu.dialogs.toast("已复制路径") }
    }
    MenuItem {
        text: "导入到笔记本"
        enabled: menu.targetIsExternal
        height: menu.targetIsExternal ? implicitHeight : 0
        onTriggered: Explorer.importExternal(menu.targetParentId, menu.targetName)
    }
    MenuSeparator { contentItem: Rectangle { implicitWidth: 228; implicitHeight: 1; color: Theme.border } }
    MenuItem { text: "删除到回收站"; enabled: !menu.targetIsExternal; onTriggered: menu.dialogs.openDelete(menu.targetId, menu.targetName, menu.targetType) }
    MenuItem {
        text: "永久删除"
        enabled: !menu.targetIsExternal
        onTriggered: {
            if (Dialogs.confirm("永久删除", "永久删除「" + menu.targetName + "」？此操作不可恢复。"))
                Explorer.removePermanent(menu.targetId);
        }
    }
}
