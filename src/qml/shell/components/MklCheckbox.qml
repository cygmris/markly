// Design-system checkbox (refined-kit InsertTable checkbox) + label.
import QtQuick
import "../../icons" as Icons
Row {
    id: root
    property string label: ""
    property bool checked: false
    signal toggled(bool value)
    spacing: 9
    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: 18; height: 18; radius: 5
        color: root.checked ? Theme.accent : "transparent"
        border.width: 1.5; border.color: root.checked ? Theme.accent : Theme.faint
        Icons.Icon { visible: root.checked; anchors.centerIn: parent; name: "check"; size: 12; color: "white" }
    }
    Text {
        anchors.verticalCenter: parent.verticalCenter
        text: root.label; color: Theme.text; font.pixelSize: 13; font.family: Theme.fontUi
    }
    MouseArea { anchors.fill: parent; onClicked: { root.checked = !root.checked; root.toggled(root.checked) } }
}
