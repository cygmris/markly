// Design-system button (refined-kit PrimaryBtn/GhostBtn): primary=accent fill, ghost=outline.
import QtQuick

Rectangle {
    id: root
    property string label: ""
    property bool primary: false
    property bool danger: false
    signal clicked()

    width: t.implicitWidth + 28
    height: 36
    radius: 9
    color: primary ? (danger ? (Theme.isDark ? "#b3503c" : "#c64a32") : Theme.accent)
                   : (mb.containsMouse ? Theme.hover : "transparent")
    border.width: primary ? 0 : 1
    border.color: Theme.border

    Text {
        id: t
        anchors.centerIn: parent
        text: root.label
        color: root.primary ? Theme.accentText : (root.danger ? (Theme.isDark ? "#e8806f" : "#c64a32") : Theme.text)
        font.pixelSize: 13; font.weight: root.primary ? Font.Bold : Font.DemiBold; font.family: Theme.fontUi
    }
    MouseArea { id: mb; anchors.fill: parent; hoverEnabled: true; onClicked: root.clicked() }
}
