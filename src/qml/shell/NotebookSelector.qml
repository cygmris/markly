// Notebook selector dropdown: lists open notebooks (switch) + new / open actions.
import QtQuick
import QtQuick.Controls

Menu {
    id: menu

    function doNew() {
        var dir = Dialogs.chooseDirectory("选择新笔记本的根目录");
        if (dir.length === 0) return;
        var name = Dialogs.promptText("新建笔记本", "笔记本名称", "我的笔记本");
        if (name.length === 0) return;
        if (!Explorer.newNotebookAt(dir, name, ""))
            Dialogs.notify("失败", "无法在该目录创建笔记本。");
    }
    function doOpen() {
        var dir = Dialogs.chooseDirectory("打开笔记本目录");
        if (dir.length === 0) return;
        if (!Explorer.openNotebookAt(dir))
            Dialogs.notify("失败", "该目录不是有效的笔记本（缺少 vx_notebook.json）。");
    }

    Repeater {
        model: (typeof Explorer !== "undefined") ? Explorer.notebooks : []
        delegate: MenuItem {
            required property var modelData
            text: (modelData.current ? "✓  " : "    ") + modelData.name
            onTriggered: Explorer.switchNotebook(modelData.id)
        }
    }
    MenuSeparator {}
    MenuItem { text: "新建笔记本…"; onTriggered: menu.doNew() }
    MenuItem { text: "打开笔记本…"; onTriggered: menu.doOpen() }
}
