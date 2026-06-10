// Shown when there is no open notebook. Guides the user to create / open one.
import QtQuick

Rectangle {
    id: root
    color: Theme.window

    Column {
        anchors.centerIn: parent
        spacing: 18
        width: 360

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 64; height: 64; radius: 16; color: Theme.accent
            Text { anchors.centerIn: parent; text: "M"; color: Theme.accentText; font.pixelSize: 36; font.bold: true; font.family: Theme.fontDisplay }
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "欢迎使用 Markly"
            color: Theme.text; font.pixelSize: 24; font.bold: true; font.family: Theme.fontUi
        }
        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: "创建一个新笔记本，或打开一个已有的 Markly 笔记本目录开始。"
            color: Theme.dim; font.pixelSize: 14; font.family: Theme.fontUi
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            Rectangle {
                width: 130; height: 40; radius: 10; color: Theme.accent
                Text { anchors.centerIn: parent; text: "新建笔记本"; color: Theme.accentText; font.pixelSize: 14; font.bold: true; font.family: Theme.fontUi }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var dir = Dialogs.chooseDirectory("选择新笔记本的根目录");
                        if (dir.length === 0) return;
                        var name = Dialogs.promptText("新建笔记本", "笔记本名称", "我的笔记本");
                        if (name.length === 0) return;
                        if (!Explorer.newNotebookAt(dir, name, ""))
                            Dialogs.notify("失败", "无法在该目录创建笔记本。");
                    }
                }
            }
            Rectangle {
                width: 130; height: 40; radius: 10; color: "transparent"; border.color: Theme.border; border.width: 1
                Text { anchors.centerIn: parent; text: "打开笔记本"; color: Theme.text; font.pixelSize: 14; font.bold: true; font.family: Theme.fontUi }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var dir = Dialogs.chooseDirectory("打开笔记本目录");
                        if (dir.length === 0) return;
                        if (!Explorer.openNotebookAt(dir))
                            Dialogs.notify("失败", "该目录不是有效的笔记本（缺少 vx_notebook.json）。");
                    }
                }
            }
        }
    }
}
