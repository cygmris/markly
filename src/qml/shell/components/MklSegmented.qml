// Design-system segmented control (refined-kit Segmented). model: [{id,name,icon}].
import QtQuick
import "../../icons" as Icons
Rectangle {
    id: root
    property var options: []          // [{id,name,icon}]
    property string value: ""
    signal picked(string id)
    width: parent ? parent.width : 200
    height: 34
    radius: 9
    color: Theme.hover
    Row {
        anchors.fill: parent; anchors.margins: 3; spacing: 2
        Repeater {
            model: root.options
            delegate: Rectangle {
                required property var modelData
                readonly property bool active: modelData.id === root.value
                width: (root.width - 6 - (root.options.length - 1) * 2) / root.options.length
                height: parent.height
                radius: 7
                color: active ? (Theme.isDark ? Qt.rgba(1, 1, 1, 0.1) : Theme.canvas) : "transparent"
                Row {
                    anchors.centerIn: parent; spacing: 6
                    Icons.Icon {
                        visible: modelData.icon !== undefined && modelData.icon.length > 0
                        anchors.verticalCenter: parent.verticalCenter
                        name: modelData.icon || ""; size: 14
                        color: active ? Theme.text : Theme.dim
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.name; color: active ? Theme.text : Theme.dim
                        font.pixelSize: 12; font.weight: Font.DemiBold; font.family: Theme.fontUi
                    }
                }
                MouseArea { anchors.fill: parent; onClicked: root.picked(modelData.id) }
            }
        }
    }
}
