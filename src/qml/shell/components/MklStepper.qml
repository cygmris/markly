// Design-system number stepper (refined-kit InsertTable Stepper).
import QtQuick
import "../../icons" as Icons
Rectangle {
    id: root
    property int value: 3
    property int minValue: 1
    property int maxValue: 12
    width: parent ? parent.width : 140
    height: 38
    radius: 9
    color: "transparent"
    border.width: 1.5; border.color: Theme.border
    Row {
        anchors.fill: parent
        Rectangle {
            width: 38; height: parent.height; color: dec.containsMouse ? Theme.hover : "transparent"
            radius: 8
            Icons.Icon { anchors.centerIn: parent; name: "minus"; size: 15; color: Theme.dim }
            MouseArea { id: dec; anchors.fill: parent; hoverEnabled: true; onClicked: if (root.value > root.minValue) root.value-- }
        }
        Text {
            width: root.width - 76; height: parent.height
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            text: String(root.value); color: Theme.text
            font.pixelSize: 14; font.bold: true; font.family: Theme.fontMono
        }
        Rectangle {
            width: 38; height: parent.height; color: inc.containsMouse ? Theme.hover : "transparent"
            radius: 8
            Icons.Icon { anchors.centerIn: parent; name: "plus"; size: 15; color: Theme.dim }
            MouseArea { id: inc; anchors.fill: parent; hoverEnabled: true; onClicked: if (root.value < root.maxValue) root.value++ }
        }
    }
}
