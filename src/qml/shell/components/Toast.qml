// Bottom-center toast: show(text) then auto-hide.
import QtQuick
import "../../icons" as Icons

Rectangle {
    id: root
    function show(msg) { label.text = msg; opacity = 1; hideTimer.restart() }

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 46
    width: row.implicitWidth + 30
    height: 36
    radius: 10
    color: Theme.window
    border.color: Theme.border; border.width: 1
    opacity: 0
    visible: opacity > 0
    z: 200
    Behavior on opacity { NumberAnimation { duration: 160 } }

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 8
        Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "check"; size: 15; color: Theme.accent }
        Text { id: label; anchors.verticalCenter: parent.verticalCenter; color: Theme.text; font.pixelSize: 13; font.family: Theme.fontUi }
    }
    Timer { id: hideTimer; interval: 1800; onTriggered: root.opacity = 0 }
}
