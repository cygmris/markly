// Notebook tree row: indent + chevron + icon + label, with selected state.
import QtQuick
import "../../icons" as Icons

Item {
    id: root
    property int depth: 0
    property string icon: ""
    property string label: ""
    property var open: undefined   // undefined = leaf (no chevron)
    property bool selected: false
    property bool muted: false

    width: parent ? parent.width : 200
    height: 28

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 0
        radius: 6
        color: root.selected ? Theme.selection : (hover.containsMouse ? Theme.hover : "transparent")
    }
    Rectangle {
        visible: root.selected
        x: 0; y: 5; width: 2.5; height: parent.height - 10; radius: 2; color: Theme.accent
    }
    Row {
        anchors.verticalCenter: parent.verticalCenter
        x: 8 + root.depth * 15
        spacing: 6
        Icons.Icon {
            anchors.verticalCenter: parent.verticalCenter
            visible: root.open !== undefined
            name: root.open ? "chevD" : "chevR"
            size: 13
            color: Theme.faint
        }
        Item { visible: root.open === undefined; width: 13; height: 1 }
        Icons.Icon {
            anchors.verticalCenter: parent.verticalCenter
            visible: root.icon !== ""
            name: root.icon
            size: 15
            color: root.selected ? Theme.accent : Theme.faint
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.label
            color: root.selected ? Theme.text : (root.muted ? Theme.faint : Theme.dim)
            font.pixelSize: 13
            font.weight: root.selected ? Font.DemiBold : Font.Medium
            font.family: Theme.fontUi
        }
    }
    MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true }
}
