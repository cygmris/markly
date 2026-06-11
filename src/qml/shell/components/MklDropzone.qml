// Design-system file dropzone (refined-kit InsertImage local/clip area).
import QtQuick
import QtQuick.Shapes
import "../../icons" as Icons
Rectangle {
    id: root
    property string hintText: "拖入图片，或点击选择"
    property bool showButton: true
    signal pick()
    width: parent ? parent.width : 300
    height: 132
    radius: 11
    color: Theme.hover
    // dashed border
    Shape {
        anchors.fill: parent
        ShapePath {
            strokeColor: Theme.border; strokeWidth: 1.5; fillColor: "transparent"
            strokeStyle: ShapePath.DashLine; dashPattern: [4, 3]
            startX: 11; startY: 1
            PathLine { x: root.width - 1; y: 1 }
            PathLine { x: root.width - 1; y: root.height - 1 }
            PathLine { x: 1; y: root.height - 1 }
            PathLine { x: 1; y: 11 }
            PathArc { x: 11; y: 1; radiusX: 10; radiusY: 10 }
        }
    }
    Column {
        anchors.centerIn: parent; spacing: 8
        Icons.Icon { anchors.horizontalCenter: parent.horizontalCenter; name: "image"; size: 26; color: Theme.accent }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.hintText; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontUi
        }
        MklButton {
            visible: root.showButton
            anchors.horizontalCenter: parent.horizontalCenter
            label: "选择文件"; iconName: "folder"; onClicked: root.pick()
        }
    }
    MouseArea { anchors.fill: parent; z: -1; onClicked: root.pick() }
}
