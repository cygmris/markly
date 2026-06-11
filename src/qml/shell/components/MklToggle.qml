// Design-system toggle switch (refined-kit Toggle).
import QtQuick
Rectangle {
    id: root
    property bool on: false
    signal toggled(bool value)
    width: 38; height: 22; radius: 11
    color: on ? Theme.accent : (Theme.isDark ? Qt.rgba(1,1,1,0.16) : Qt.rgba(0,0,0,0.16))
    Behavior on color { ColorAnimation { duration: 150 } }
    Rectangle {
        width: 18; height: 18; radius: 9; color: "white"
        y: 2; x: root.on ? 18 : 2
        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }
    }
    MouseArea { anchors.fill: parent; onClicked: { root.on = !root.on; root.toggled(root.on) } }
}
