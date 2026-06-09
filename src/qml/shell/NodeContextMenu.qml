// Right-click menu for a tree node. Set the target_* props then popup().
// Actions drive native dialogs (Dialogs) + the Explorer bridge.
import QtQuick
import QtQuick.Controls

Menu {
    id: menu
    property double targetId: 0
    property double targetParentId: 0
    property string targetType: "file"   // file | folder
    property string targetName: ""
    property bool targetIsExternal: false

    // Folder that new children go into.
    readonly property double containerId: targetType === "folder" ? targetId : targetParentId

    function reportError(err) {
        if (err && err.length > 0) {
            Dialogs.notify("操作失败", err);
        }
    }

    MenuItem {
        text: "新建笔记"
        enabled: !menu.targetIsExternal
        onTriggered: {
            var name = Dialogs.promptText("新建笔记", "笔记名称（含 .md）", "新笔记.md");
            if (name.length > 0) menu.reportError(Explorer.newNote(menu.containerId, name));
        }
    }
    MenuItem {
        text: "新建文件夹"
        enabled: !menu.targetIsExternal
        onTriggered: {
            var name = Dialogs.promptText("新建文件夹", "文件夹名称", "新文件夹");
            if (name.length > 0) menu.reportError(Explorer.newFolder(menu.containerId, name));
        }
    }
    MenuSeparator { visible: !menu.targetIsExternal }
    MenuItem {
        text: "重命名"
        visible: !menu.targetIsExternal
        onTriggered: {
            var name = Dialogs.promptText("重命名", "新名称", menu.targetName);
            if (name.length > 0) menu.reportError(Explorer.renameNode(menu.targetId, name));
        }
    }
    MenuItem {
        text: "设置颜色"
        visible: !menu.targetIsExternal
        onTriggered: {
            var c = Dialogs.chooseColor("");
            if (c.length > 0) Explorer.setNodeColor(menu.targetId, "", "", c);
        }
    }
    MenuItem {
        text: "清除颜色"
        visible: !menu.targetIsExternal
        onTriggered: Explorer.clearNodeColor(menu.targetId)
    }
    MenuSeparator { visible: !menu.targetIsExternal }
    MenuItem {
        text: "删除到回收站"
        visible: !menu.targetIsExternal
        onTriggered: {
            if (Dialogs.confirm("删除", "将「" + menu.targetName + "」移到回收站？"))
                Explorer.removeToRecycle(menu.targetId);
        }
    }
    MenuItem {
        text: "永久删除"
        visible: !menu.targetIsExternal
        onTriggered: {
            if (Dialogs.confirm("永久删除", "永久删除「" + menu.targetName + "」？此操作不可恢复。"))
                Explorer.removePermanent(menu.targetId);
        }
    }
    MenuItem {
        text: "导入到笔记本"
        visible: menu.targetIsExternal
        onTriggered: Explorer.importExternal(menu.targetParentId, menu.targetName)
    }
}
