// Tags panel (#11): list the current notebook's tags with note counts; clicking a
// tag shows the notes carrying it. Bound to the "Tags" bridge (C++). Left-dock page.
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    color: Theme.sidebar

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        Text {
            text: "标签"
            color: Theme.heading
            font.pixelSize: 15
            font.bold: true
            font.family: Theme.fontUi
        }

        // Empty state.
        Text {
            visible: Tags.tags.length === 0
            width: parent.width
            text: "暂无标签。右键笔记选「标签…」添加。"
            wrapMode: Text.Wrap
            color: Theme.faint
            font.pixelSize: 12
            font.family: Theme.fontUi
        }

        // Tag chips with counts.
        Flow {
            width: parent.width
            spacing: 6
            Repeater {
                model: Tags.tags
                Rectangle {
                    height: 24
                    width: lbl.implicitWidth + 22
                    radius: 8
                    property bool sel: Tags.selectedTag === modelData.name
                    color: sel ? Theme.accent
                               : Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.12)
                    Row {
                        id: lbl
                        anchors.centerIn: parent
                        spacing: 5
                        Text {
                            text: "#" + modelData.name
                            color: parent.parent.sel ? Theme.accentText : Theme.accent
                            font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi
                        }
                        Text {
                            text: modelData.count
                            color: parent.parent.sel ? Theme.accentText : Theme.dim
                            font.pixelSize: 11; font.family: Theme.fontMono
                        }
                    }
                    MouseArea { anchors.fill: parent; onClicked: Tags.selectTag(modelData.name) }
                }
            }
        }

        // Divider.
        Rectangle {
            visible: Tags.selectedTag.length > 0
            width: parent.width; height: 1; color: Theme.border
        }

        // Notes carrying the selected tag.
        ListView {
            id: list
            width: parent.width
            height: parent.height - y
            clip: true
            spacing: 4
            model: Tags.taggedNodes
            delegate: Rectangle {
                width: list.width
                height: col.implicitHeight + 12
                radius: 8
                color: hover.hovered ? Theme.hover : "transparent"
                Column {
                    id: col
                    x: 10; y: 6
                    width: parent.width - 20
                    spacing: 2
                    Text {
                        text: modelData.name
                        color: Theme.accent
                        font.pixelSize: 13; font.bold: true; font.family: Theme.fontUi
                        elide: Text.ElideRight; width: parent.width
                    }
                    Text {
                        text: modelData.path
                        color: Theme.faint
                        font.pixelSize: 10; font.family: Theme.fontMono
                        elide: Text.ElideMiddle; width: parent.width
                    }
                }
                HoverHandler { id: hover }
                MouseArea { anchors.fill: parent; onClicked: Tags.openTaggedNode(index) }
            }
        }
    }
}
