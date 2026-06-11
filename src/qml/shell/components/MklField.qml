// Design-system field wrapper (refined-kit Field): uppercase label + optional + hint.
import QtQuick
Column {
    property string label: ""
    property bool optional: false
    property string hint: ""
    default property alias content: slot.data
    width: parent ? parent.width : 200
    spacing: 0
    bottomPadding: 15
    Item {
        visible: label.length > 0
        width: parent.width; height: 18
        Text {
            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
            text: label.toUpperCase(); color: Theme.faint
            font.pixelSize: 11; font.bold: true; font.letterSpacing: 0.5; font.family: Theme.fontUi
        }
        Text {
            visible: optional
            anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
            text: "可选"; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi
        }
    }
    Item { visible: label.length > 0; width: 1; height: 7 }
    Column { id: slot; width: parent.width; spacing: 0 }
    Text {
        visible: hint.length > 0
        width: parent.width; topPadding: 6
        text: hint; color: Theme.faint; wrapMode: Text.Wrap
        font.pixelSize: 11; font.family: Theme.fontUi; lineHeight: 1.4
    }
}
