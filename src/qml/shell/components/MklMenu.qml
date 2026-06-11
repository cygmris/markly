// Design-system anchored popup menu (refined-kit Menu). Fill with MklMenuItem/Sep/Label.
import QtQuick
import QtQuick.Controls

Popup {
    id: root
    property int menuWidth: 224
    default property alias items: col.data

    width: menuWidth
    padding: 6
    modal: true
    dim: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle {
        color: Theme.window; radius: 11
        border.color: Theme.border; border.width: 1
    }
    contentItem: Column { id: col; spacing: 2 }
}
