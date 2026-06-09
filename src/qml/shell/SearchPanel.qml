// Full-text search panel (#12). Keyword + scope + object filters + result list.
// Bound to the "Search" bridge (C++). Lives in the left dock as a switchable page.
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    color: Theme.sidebar

    // Object bitmask flags must match SearchObject in searchdata.h.
    readonly property int objName: 0x1
    readonly property int objContent: 0x2
    readonly property int objPath: 0x4
    readonly property int objTag: 0x8

    property int scope: 0          // SearchScope: 0 notebook,1 all,2 folder,3 buffers
    property int objects: objName | objContent

    function runSearch() {
        Search.search(field.text, root.scope, root.objects, "")
    }

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        Text {
            text: "搜索"
            color: Theme.heading
            font.pixelSize: 15
            font.bold: true
            font.family: Theme.fontUi
        }

        // Keyword input.
        Rectangle {
            width: parent.width
            height: 34
            radius: 9
            color: Theme.card
            border.color: field.activeFocus ? Theme.accent : Theme.border
            border.width: 1

            TextField {
                id: field
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                verticalAlignment: TextInput.AlignVCenter
                placeholderText: "输入关键词，回车搜索…"
                color: Theme.text
                placeholderTextColor: Theme.faint
                font.pixelSize: 13
                font.family: Theme.fontUi
                background: Item {}
                onAccepted: root.runSearch()
            }
        }

        // Scope segmented buttons.
        Row {
            spacing: 6
            Repeater {
                model: ["笔记本", "全部", "文件夹", "已打开"]
                Rectangle {
                    width: 64
                    height: 24
                    radius: 7
                    property bool sel: root.scope === index
                    color: sel ? Theme.accent : Theme.card
                    border.color: sel ? Theme.accent : Theme.border
                    border.width: 1
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: parent.sel ? Theme.accentText : Theme.dim
                        font.pixelSize: 11
                        font.family: Theme.fontUi
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: { root.scope = index; root.runSearch() }
                    }
                }
            }
        }

        // Object toggles (bitmask).
        Row {
            spacing: 6
            Repeater {
                model: [
                    { label: "名称", flag: root.objName },
                    { label: "内容", flag: root.objContent },
                    { label: "路径", flag: root.objPath },
                    { label: "标签", flag: root.objTag }
                ]
                Rectangle {
                    width: 48
                    height: 22
                    radius: 7
                    property bool on: (root.objects & modelData.flag) !== 0
                    color: on ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.15)
                              : Theme.card
                    border.color: on ? Theme.accent : Theme.border
                    border.width: 1
                    Text {
                        anchors.centerIn: parent
                        text: modelData.label
                        color: parent.on ? Theme.accent : Theme.dim
                        font.pixelSize: 11
                        font.family: Theme.fontUi
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.objects = root.objects ^ modelData.flag
                            if (root.objects !== 0 && field.text.length > 0)
                                root.runSearch()
                        }
                    }
                }
            }
        }

        // Status line.
        Text {
            text: Search.state === "busy" ? "搜索中…"
                : (Search.keyword.length === 0 ? "" : (Search.count + " 条结果"))
            color: Theme.faint
            font.pixelSize: 11
            font.family: Theme.fontUi
        }

        // Results.
        ListView {
            id: list
            width: parent.width
            height: parent.height - y
            clip: true
            spacing: 4
            model: Search.results
            delegate: Rectangle {
                width: list.width
                height: col.implicitHeight + 14
                radius: 8
                color: hover.hovered ? Theme.hover : "transparent"
                Column {
                    id: col
                    x: 10
                    y: 7
                    width: parent.width - 20
                    spacing: 2
                    Text {
                        text: modelData.name
                        color: Theme.accent
                        font.pixelSize: 13
                        font.bold: true
                        font.family: Theme.fontUi
                        elide: Text.ElideRight
                        width: parent.width
                    }
                    Text {
                        // Qt RichText has no <mark>; map it to bold + accent color.
                        text: modelData.snippet
                              .replace(/<mark>/g, '<b><font color="' + Theme.accent + '">')
                              .replace(/<\/mark>/g, '</font></b>')
                        visible: modelData.snippet.length > 0
                        textFormat: Text.RichText
                        color: Theme.dim
                        font.pixelSize: 11
                        font.family: Theme.fontUi
                        wrapMode: Text.Wrap
                        width: parent.width
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }
                    Text {
                        text: modelData.path
                        color: Theme.faint
                        font.pixelSize: 10
                        font.family: Theme.fontMono
                        elide: Text.ElideMiddle
                        width: parent.width
                    }
                }
                HoverHandler { id: hover }
                MouseArea {
                    anchors.fill: parent
                    onClicked: Search.openResult(index)
                }
            }
        }
    }
}
