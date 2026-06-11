// Design-system multiline input (refined-kit TextArea).
import QtQuick
import QtQuick.Controls
Rectangle {
    id: root
    property alias text: input.text
    property alias placeholder: input.placeholderText
    property bool mono: false
    property int rows: 4
    width: parent ? parent.width : 200
    height: rows * (root.mono ? 19 : 20) + 20
    radius: 9
    color: Theme.canvas
    border.width: input.activeFocus ? 2 : 1
    border.color: input.activeFocus ? Theme.accent : Theme.border
    ScrollView {
        anchors.fill: parent; anchors.margins: 1; clip: true
        TextArea {
            id: input
            leftPadding: 11; topPadding: 10; rightPadding: 11
            wrapMode: TextArea.Wrap
            color: Theme.text; placeholderTextColor: Theme.faint
            font.pixelSize: root.mono ? 13 : 13.5
            font.family: root.mono ? Theme.fontMono : Theme.fontUi
            background: Item {}
        }
    }
}
