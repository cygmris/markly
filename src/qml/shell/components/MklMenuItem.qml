// Design-system menu row (refined-kit MenuItem): icon/label/sub/shortcut/danger/checked.
import QtQuick
import "../../icons" as Icons

Rectangle {
    id: root
    property string icon: ""
    property string label: ""
    property string sub: ""
    property string shortcut: ""
    property bool danger: false
    property bool accent: false
    property bool showCheck: false      // reserve the check column
    property bool checked: false
    signal clicked()

    width: parent ? parent.width : 200
    height: sub.length > 0 ? 40 : 30
    radius: 7
    color: ma.containsMouse ? (danger ? Qt.rgba(0.78, 0.29, 0.2, 0.10) : Theme.hover) : "transparent"

    readonly property color fg: danger ? (Theme.isDark ? "#e8806f" : "#c64a32") : Theme.text

    Row {
        anchors.verticalCenter: parent.verticalCenter
        x: 9; spacing: 10
        width: parent.width - 18
        Item {
            visible: root.showCheck
            width: 16; height: 16; anchors.verticalCenter: parent.verticalCenter
            Icons.Icon { visible: root.checked; anchors.centerIn: parent; name: "check"; size: 15; color: Theme.accent }
        }
        Icons.Icon {
            visible: !root.showCheck && root.icon.length > 0
            anchors.verticalCenter: parent.verticalCenter
            name: root.icon; size: 16
            color: root.danger ? root.fg : (root.accent ? Theme.accent : (ma.containsMouse ? Theme.text : Theme.dim))
        }
        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 1
            Text { text: root.label; color: root.fg; font.pixelSize: 13; font.weight: Font.Medium; font.family: Theme.fontUi }
            Text { visible: root.sub.length > 0; text: root.sub; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi }
        }
    }
    Text {
        visible: root.shortcut.length > 0
        anchors.right: parent.right; anchors.rightMargin: 9; anchors.verticalCenter: parent.verticalCenter
        text: root.shortcut; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontMono
    }
    MouseArea { id: ma; anchors.fill: parent; hoverEnabled: true; onClicked: root.clicked() }
}
