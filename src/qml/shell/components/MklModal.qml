// Design-system modal (refined-kit Modal): dim backdrop, 460-wide card, icon header,
// footer button row. Content goes in `body`, buttons in `footer`.
import QtQuick
import QtQuick.Controls
import "../../icons" as Icons

Popup {
    id: root
    property string title: ""
    property string subtitle: ""
    property string iconName: ""
    default property alias body: bodyCol.data
    property alias footer: footRow.data

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min(460, parent ? parent.width - 48 : 460)
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    padding: 0
    Overlay.modal: Rectangle { color: "#6b0a0c0e" }

    background: Rectangle {
        color: Theme.window
        radius: 14
        border.color: Theme.border
        border.width: 1
    }

    contentItem: Column {
        width: root.width
        // Header
        Item {
            width: parent.width; height: 62
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }
            Row {
                anchors.verticalCenter: parent.verticalCenter
                x: 18; spacing: 11
                Rectangle {
                    visible: root.iconName.length > 0
                    width: 30; height: 30; radius: 8; color: Theme.selection
                    anchors.verticalCenter: parent.verticalCenter
                    Icons.Icon { anchors.centerIn: parent; name: root.iconName; size: 17; color: Theme.accent }
                }
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2
                    Text { text: root.title; color: Theme.text; font.pixelSize: 15; font.bold: true; font.family: Theme.fontUi }
                    Text { visible: root.subtitle.length > 0; text: root.subtitle; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontUi }
                }
            }
            Icons.Icon {
                anchors.right: parent.right; anchors.rightMargin: 14; anchors.verticalCenter: parent.verticalCenter
                name: "close"; size: 17; color: Theme.dim
                MouseArea { anchors.fill: parent; anchors.margins: -6; onClicked: root.close() }
            }
        }
        // Body
        Column { id: bodyCol; width: parent.width - 36; x: 18; topPadding: 16; bottomPadding: 16; spacing: 0 }
        // Footer
        Item {
            width: parent.width; height: footRow.children.length > 0 ? 62 : 0
            visible: footRow.children.length > 0
            Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: Theme.border }
            Row {
                id: footRow
                anchors.right: parent.right; anchors.rightMargin: 18; anchors.verticalCenter: parent.verticalCenter
                spacing: 9
            }
        }
    }
}
