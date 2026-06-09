// Editor tab: md icon + title + dirty dot / close, active underline.
import QtQuick
import "../../icons" as Icons

Item {
    id: root
    property string label: ""
    property bool active: false
    property bool dirty: false

    implicitWidth: Math.min(200, row.implicitWidth + 26)
    height: 38

    Rectangle {
        anchors.fill: parent
        color: root.active ? Theme.canvas : "transparent"
    }
    Rectangle {
        visible: root.active
        anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
        height: 2; color: Theme.accent
    }
    Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }

    Row {
        id: row
        anchors.verticalCenter: parent.verticalCenter
        x: 14
        spacing: 8
        Icons.Icon {
            anchors.verticalCenter: parent.verticalCenter
            name: "md"; size: 15
            color: root.active ? Theme.accent : Theme.faint
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.label
            color: root.active ? Theme.text : Theme.dim
            font.pixelSize: 13
            font.weight: root.active ? Font.DemiBold : Font.Medium
            font.family: Theme.fontUi
            elide: Text.ElideRight
        }
        Item {
            anchors.verticalCenter: parent.verticalCenter
            width: 12; height: 12
            Rectangle {
                visible: root.dirty
                anchors.centerIn: parent
                width: 8; height: 8; radius: 4; color: Theme.dim
            }
            Icons.Icon {
                visible: !root.dirty
                anchors.centerIn: parent
                name: "close"; size: 13; color: Theme.faint
            }
        }
    }
}
