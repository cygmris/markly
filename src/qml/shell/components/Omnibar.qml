// Command omnibar (Workbench style): search glyph + placeholder + Cmd-K badge.
import QtQuick
import "../../icons" as Icons

Rectangle {
    id: root
    property string placeholder: "跳转、搜索或运行命令…"
    implicitWidth: 380
    height: 28
    radius: 8
    color: Qt.rgba(1, 1, 1, 0.07)
    border.color: Qt.rgba(1, 1, 1, 0.09)
    border.width: 1

    Row {
        anchors.fill: parent
        anchors.leftMargin: 11
        anchors.rightMargin: 8
        spacing: 8
        Icons.Icon {
            anchors.verticalCenter: parent.verticalCenter
            name: "search"; size: 14; color: Theme.railDim
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width - 70
            text: root.placeholder
            color: Theme.railDim
            font.pixelSize: 13
            font.family: Theme.fontUi
            elide: Text.ElideRight
        }
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            width: badge.implicitWidth + 12
            height: 18
            radius: 5
            color: Qt.rgba(1, 1, 1, 0.08)
            Text {
                id: badge
                anchors.centerIn: parent
                text: "⌘K"
                color: Theme.railText
                font.pixelSize: 11
                font.family: Theme.fontMono
            }
        }
    }
}
