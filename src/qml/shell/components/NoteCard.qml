// Note list card (Focus style): serif title + 2-line snippet + mono meta.
import QtQuick
import "../../icons" as Icons

Item {
    id: root
    property string title: ""
    property string snippet: ""
    property string meta: ""
    property bool active: false
    property bool fav: false

    width: parent ? parent.width : 280
    implicitHeight: col.implicitHeight + 27

    Rectangle {
        anchors.fill: parent
        radius: 11
        color: root.active ? Theme.canvas : (hover.containsMouse ? Theme.hover : "transparent")
        border.width: root.active ? 1 : 0
        border.color: Theme.border
    }
    Rectangle {
        visible: root.active
        x: 0; y: 14; width: 3; height: parent.height - 28; radius: 3; color: Theme.accent
    }
    Column {
        id: col
        x: 16; y: 13
        width: parent.width - 32
        spacing: 5
        Row {
            width: parent.width
            spacing: 7
            Icons.Icon {
                anchors.verticalCenter: parent.verticalCenter
                visible: root.fav; name: "star"; size: 13; color: Theme.accent
            }
            Text {
                width: parent.width - (root.fav ? 20 : 0)
                text: root.title
                color: Theme.text
                font.pixelSize: 16
                font.weight: Font.DemiBold
                font.family: Theme.fontSerif
                elide: Text.ElideRight
            }
        }
        Text {
            width: parent.width
            text: root.snippet
            color: Theme.dim
            font.pixelSize: 13
            font.family: Theme.fontUi
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            lineHeight: 1.4
        }
        Text {
            text: root.meta
            color: Theme.faint
            font.pixelSize: 12
            font.family: Theme.fontMono
        }
    }
    MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true }
}
