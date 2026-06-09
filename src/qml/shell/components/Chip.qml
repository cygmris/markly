// Property chip (solid or soft). Used by the workbench property rows.
import QtQuick

Rectangle {
    id: root
    property string text: ""
    property color chipColor: Theme.accent
    property bool solid: false

    implicitWidth: label.implicitWidth + 18
    implicitHeight: 22
    radius: 7
    color: solid ? chipColor : Qt.rgba(chipColor.r, chipColor.g, chipColor.b, 0.12)

    Text {
        id: label
        anchors.centerIn: parent
        text: root.text
        color: root.solid ? "#ffffff" : root.chipColor
        font.pixelSize: 12
        font.bold: true
        font.family: Theme.fontUi
    }
}
