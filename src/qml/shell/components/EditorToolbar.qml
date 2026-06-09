// Markdown editor toolbar: tool button groups with separators + trailing actions.
import QtQuick
import "../../icons" as Icons

Rectangle {
    id: root
    property color barColor: Theme.canvas
    implicitHeight: 42
    color: barColor

    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }

    component ToolBtn: Item {
        property string icon
        property bool active: false
        width: 30; height: 28
        Rectangle {
            anchors.fill: parent; radius: 6
            color: active ? Theme.selection : (h.containsMouse ? Theme.hover : "transparent")
        }
        Icons.Icon {
            anchors.centerIn: parent; name: parent.icon; size: 17
            color: parent.active ? Theme.accent : Theme.dim
        }
        MouseArea { id: h; anchors.fill: parent; hoverEnabled: true }
    }
    component Sep: Rectangle { width: 1; height: 18; color: Theme.border; anchors.verticalCenter: parent.verticalCenter }

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1
        ToolBtn { icon: "save" }
        ToolBtn { icon: "eye"; active: true }
        ToolBtn { icon: "image" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "heading" }
        ToolBtn { icon: "bold" }
        ToolBtn { icon: "italic" }
        ToolBtn { icon: "strike" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "listU" }
        ToolBtn { icon: "listO" }
        ToolBtn { icon: "check" }
        ToolBtn { icon: "quote" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "code" }
        ToolBtn { icon: "sigma" }
        ToolBtn { icon: "link" }
        ToolBtn { icon: "table" }
    }
    Row {
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1
        ToolBtn { icon: "search" }
        ToolBtn { icon: "moreV" }
    }
}
