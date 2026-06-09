// Outline row: level indent + label, active highlight.
import QtQuick

Item {
    id: root
    property string label: ""
    property int level: 0
    property bool active: false
    signal clicked()

    width: parent ? parent.width : 200
    height: 26

    Rectangle {
        anchors.fill: parent
        radius: 6
        color: root.active ? Theme.selection : (hover.containsMouse ? Theme.hover : "transparent")
    }
    MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true; onClicked: root.clicked() }
    Text {
        anchors.verticalCenter: parent.verticalCenter
        x: 12 + root.level * 14
        width: parent.width - x - 10
        elide: Text.ElideRight
        text: root.label
        color: root.active ? Theme.accent : (root.level === 0 ? Theme.text : Theme.dim)
        font.pixelSize: 13
        font.weight: root.level === 0 ? Font.DemiBold : Font.Medium
        font.family: Theme.fontUi
    }
}
