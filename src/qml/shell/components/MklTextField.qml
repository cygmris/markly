// Design-system text input (refined-kit TextField): 38h, accent focus ring, suffix text.
import QtQuick
import QtQuick.Controls
import "../../icons" as Icons

Rectangle {
    id: root
    property alias text: input.text
    property alias placeholder: input.placeholderText
    property string prefix: ""   // icon name
    property string suffix: ""
    property bool mono: false
    signal accepted()
    function takeFocus() { input.forceActiveFocus(); input.selectAll() }

    width: parent ? parent.width : 200
    height: 38
    radius: 9
    color: Theme.canvas
    border.width: input.activeFocus ? 2 : 1
    border.color: input.activeFocus ? Theme.accent : Theme.border

    TextField {
        id: input
        anchors.fill: parent
        anchors.leftMargin: prefixIcon.visible ? 34 : 0
        anchors.rightMargin: suffixLabel.visible ? suffixLabel.width + 16 : 0
        leftPadding: 11
        verticalAlignment: TextInput.AlignVCenter
        color: Theme.text
        placeholderTextColor: Theme.faint
        font.pixelSize: root.mono ? 13 : 13.5
        font.family: root.mono ? Theme.fontMono : Theme.fontUi
        background: Item {}
        onAccepted: root.accepted()
    }
    Icons.Icon {
        id: prefixIcon
        visible: root.prefix.length > 0
        anchors.left: parent.left; anchors.leftMargin: 11; anchors.verticalCenter: parent.verticalCenter
        name: root.prefix; size: 15
        color: input.activeFocus ? Theme.accent : Theme.faint
    }
    Text {
        id: suffixLabel
        visible: root.suffix.length > 0
        anchors.right: parent.right; anchors.rightMargin: 11; anchors.verticalCenter: parent.verticalCenter
        text: root.suffix; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontMono
    }
}
