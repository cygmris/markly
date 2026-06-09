// Snippet page (#14): list snippets; insert into the editor; add/delete. Bound to the
// "Snippets" bridge. Insertion goes through Views.requestInsert (active editor).
import QtQuick

Rectangle {
    id: root
    color: Theme.sidebar

    function insert(name) {
        var note = (typeof Views !== "undefined") ? Views.currentFileName : "";
        var r = Snippets.apply(name, "", note);
        if (typeof Views !== "undefined") Views.requestInsert(r.text, r.cursorOffset);
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Item {
            width: parent.width; height: 24
            Text { anchors.verticalCenter: parent.verticalCenter; text: "片段"; color: Theme.heading
                   font.pixelSize: 15; font.bold: true; font.family: Theme.fontUi }
            Rectangle {
                anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                width: addLbl.implicitWidth + 18; height: 22; radius: 7
                color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.14)
                Text { id: addLbl; anchors.centerIn: parent; text: "新增"; color: Theme.accent
                       font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var name = Dialogs.promptText("新增片段", "片段名称", "");
                        if (name.length > 0) {
                            var content = Dialogs.promptText("片段内容", "支持 @@光标 $$选区 %date%", "");
                            Snippets.add(name, content);
                        }
                    }
                }
            }
        }

        Text {
            visible: Snippets.list.length === 0
            text: "暂无片段"; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi
        }

        ListView {
            id: list
            width: parent.width
            height: parent.height - y
            clip: true
            spacing: 4
            model: Snippets.list
            delegate: Rectangle {
                width: list.width
                height: 44
                radius: 8
                color: hover.hovered ? Theme.hover : "transparent"
                Column {
                    anchors.left: parent.left; anchors.leftMargin: 10; anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - 110
                    spacing: 1
                    Text { text: modelData.name; color: Theme.text; font.pixelSize: 13; font.bold: true
                           font.family: Theme.fontUi; elide: Text.ElideRight; width: parent.width }
                    Text { visible: modelData.description.length > 0; text: modelData.description
                           color: Theme.faint; font.pixelSize: 10; font.family: Theme.fontUi
                           elide: Text.ElideRight; width: parent.width }
                }
                Row {
                    anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter
                    spacing: 6
                    Rectangle {
                        width: 44; height: 24; radius: 7; color: Theme.accent
                        Text { anchors.centerIn: parent; text: "插入"; color: Theme.accentText
                               font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
                        MouseArea { anchors.fill: parent; onClicked: root.insert(modelData.name) }
                    }
                    Rectangle {
                        width: 24; height: 24; radius: 7; color: Theme.card; border.color: Theme.border; border.width: 1
                        Text { anchors.centerIn: parent; text: "✕"; color: Theme.dim; font.pixelSize: 11 }
                        MouseArea { anchors.fill: parent; onClicked: Snippets.remove(modelData.name) }
                    }
                }
                HoverHandler { id: hover }
            }
        }
    }
}
