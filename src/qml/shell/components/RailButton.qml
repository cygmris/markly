// Activity-bar icon button. Active state uses accent + selection tint + a left bar.
import QtQuick
import "../../icons" as Icons

Item {
    id: root
    property string icon: ""
    property bool active: false
    property int iconSize: 21
    property color activeColor: Theme.accent
    property color inactiveColor: Theme.dim
    signal clicked()

    width: 44
    height: 44

    Rectangle {
        anchors.fill: parent
        radius: 9
        color: root.active ? Theme.selection : (hover.containsMouse ? Theme.hover : "transparent")
    }
    // Active left indicator bar (VNote-style edge bar). Kept inside the button so it
    // is never clipped by the rail, whatever the rail's horizontal inset is.
    Rectangle {
        visible: root.active
        x: 0
        y: 10
        width: 3
        height: parent.height - 20
        radius: 3
        color: Theme.accent
    }
    Icons.Icon {
        anchors.centerIn: parent
        name: root.icon
        size: root.iconSize
        color: root.active ? root.activeColor : root.inactiveColor
    }
    MouseArea {
        id: hover
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
