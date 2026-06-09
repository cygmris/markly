// Self-drawn window controls (minimize / maximize-restore / close).
// Calls the injected Win.* methods (FramelessMainWindow).
import QtQuick

Row {
    id: root
    spacing: 2
    property color color: Theme.windowButton

    component Btn: Rectangle {
        property string glyph
        property var act
        width: 28
        height: 26
        radius: 6
        color: hover.containsMouse ? Theme.hover : "transparent"
        Text {
            anchors.centerIn: parent
            text: glyph
            color: root.color
            font.pixelSize: 13
            font.family: Theme.fontMono
        }
        MouseArea {
            id: hover
            anchors.fill: parent
            hoverEnabled: true
            onClicked: if (act) act()
        }
    }

    Btn { glyph: "—"; act: function() { Win.minimizeWindow() } }      // em dash
    Btn { glyph: "□"; act: function() { Win.toggleMaximize() } }      // square
    Btn { glyph: "✕"; act: function() { Win.closeWindow() } }         // x
}
