// Design-system button (refined-kit PrimaryBtn/GhostBtn): primary=accent fill + icon, ghost=outline.
import QtQuick
import "../../icons" as Icons

Rectangle {
    id: root
    property string label: ""
    property string iconName: ""
    property bool primary: false
    property bool danger: false
    property bool enabledBtn: true
    signal clicked()

    width: row.implicitWidth + 30
    height: 36
    radius: 9
    opacity: enabledBtn ? 1 : 0.5
    color: primary ? (danger ? (Theme.isDark ? "#b3503c" : "#c64a32") : Theme.accent)
                   : (mb.containsMouse ? Theme.hover : "transparent")
    border.width: primary ? 0 : 1
    border.color: Theme.border

    Row {
        id: row
        anchors.centerIn: parent; spacing: 7
        Icons.Icon {
            visible: root.iconName.length > 0
            anchors.verticalCenter: parent.verticalCenter
            name: root.iconName; size: 16
            color: root.primary ? Theme.accentText : (root.danger ? (Theme.isDark ? "#e8806f" : "#c64a32") : Theme.text)
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.label
            color: root.primary ? Theme.accentText : (root.danger ? (Theme.isDark ? "#e8806f" : "#c64a32") : Theme.text)
            font.pixelSize: 13; font.weight: root.primary ? Font.Bold : Font.DemiBold; font.family: Theme.fontUi
        }
    }
    MouseArea { id: mb; anchors.fill: parent; hoverEnabled: true; onClicked: if (root.enabledBtn) root.clicked() }
}
