// Quick access page (#13): pinned notes + recent history. Bound to the "Quick" bridge.
import QtQuick

Rectangle {
    id: root
    color: Theme.sidebar

    // A clickable note row (name + path), with an optional trailing action.
    component NoteRow: Rectangle {
        property string name: ""
        property string path: ""
        property bool pinned: false
        signal activated()
        signal removed()
        width: ListView.view ? ListView.view.width : parent.width
        height: col.implicitHeight + 12
        radius: 8
        color: hover.hovered ? Theme.hover : "transparent"
        Column {
            id: col
            x: 10; y: 6
            width: parent.width - (pinned ? 36 : 20)
            spacing: 2
            Text { text: name; color: Theme.accent; font.pixelSize: 13; font.bold: true
                   font.family: Theme.fontUi; elide: Text.ElideRight; width: parent.width }
            Text { text: path; color: Theme.faint; font.pixelSize: 10; font.family: Theme.fontMono
                   elide: Text.ElideMiddle; width: parent.width }
        }
        Text {
            visible: pinned
            anchors.right: parent.right; anchors.rightMargin: 10; anchors.verticalCenter: parent.verticalCenter
            text: "✕"; color: Theme.dim; font.pixelSize: 12
            MouseArea { anchors.fill: parent; anchors.margins: -6; onClicked: parent.parent.removed() }
        }
        HoverHandler { id: hover }
        MouseArea { anchors.fill: parent; z: -1; onClicked: parent.activated() }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Text { text: "快速访问"; color: Theme.heading; font.pixelSize: 15; font.bold: true; font.family: Theme.fontUi }

        // Pinned.
        Text { text: "已固定"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
        Text {
            visible: Quick.quickAccess.length === 0
            text: "右键笔记「固定到快速访问」"; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi
        }
        Column {
            width: parent.width
            Repeater {
                model: Quick.quickAccess
                NoteRow {
                    name: modelData.name; path: modelData.path; pinned: true
                    onActivated: Quick.open(modelData.path)
                    onRemoved: Quick.unpin(modelData.path)
                }
            }
        }

        Rectangle { width: parent.width; height: 1; color: Theme.border }

        // Recent.
        Text { text: "最近"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
        Text {
            visible: Quick.history.length === 0
            text: "暂无最近记录"; color: Theme.faint; font.pixelSize: 11; font.family: Theme.fontUi
        }
        ListView {
            id: recent
            width: parent.width
            height: parent.height - y
            clip: true
            spacing: 2
            model: Quick.history
            delegate: NoteRow {
                name: modelData.name; path: modelData.path
                onActivated: Quick.open(modelData.path)
            }
        }
    }
}
