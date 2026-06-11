// Bottom status bar. Shells place Text/items as default children into the row.
import QtQuick

Rectangle {
    id: root
    default property alias content: row.data
    property color barColor: Theme.bar
    property real spacingValue: 16

    implicitHeight: 26
    color: barColor

    Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: Theme.border }

    Row {
        id: row
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 110  // clear of the floating 外观 button (design chat2)
        spacing: root.spacingValue
    }
}
