// Style B — Focus Writing (warm paper, serif reading column). Ports focus.jsx.
import QtQuick
import "../icons" as Icons
import "components" as C
import "DemoData.js" as Demo

Rectangle {
    id: shell
    color: Theme.window

    // Left dock page: "explorer" (notes list) or "search".
    property string leftPage: "explorer"

    Column {
        anchors.fill: parent

        // ---- Top bar ----
        Item {
            width: parent.width; height: 44
            MouseArea { anchors.fill: parent; onPressed: Win.startMove(); onDoubleClicked: Win.toggleMaximize() }
            Row {
                anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; spacing: 10
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 19; height: 19; radius: 6; color: Theme.accent
                    Text { anchors.centerIn: parent; text: "V"; color: "#ffffff"; font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi }
                }
                Row {
                    anchors.verticalCenter: parent.verticalCenter; spacing: 6
                    Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "book"; size: 14; color: Theme.accent }
                    Text { anchors.verticalCenter: parent.verticalCenter; text: (typeof Explorer !== "undefined" && Explorer.currentNotebookName.length > 0) ? Explorer.currentNotebookName : "笔记本"; color: Theme.text; font.pixelSize: 13; font.weight: Font.DemiBold; font.family: Theme.fontUi }
                    Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "chevD"; size: 13; color: Theme.faint }
                }
            }
            Row {
                anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter; spacing: 4
                Repeater {
                    model: ["search", "edit", Theme.isDark ? "sun" : "moon", "more"]
                    delegate: Icons.Icon { required property string modelData; anchors.verticalCenter: parent.verticalCenter; name: modelData; size: 17; color: Theme.dim }
                }
                Item { width: 4; height: 1 }
                C.WinControls { anchors.verticalCenter: parent.verticalCenter }
            }
        }

        // ---- Body ----
        Row {
            width: parent.width
            height: parent.height - 44

            // Rail
            Rectangle {
                width: 52; height: parent.height; color: Theme.rail
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter; y: 6; spacing: 4
                    C.RailButton { icon: "notebook"; active: shell.leftPage === "explorer"; iconSize: 20; onClicked: shell.leftPage = "explorer" }
                    C.RailButton { icon: "star"; iconSize: 20 }
                    C.RailButton { icon: "search"; active: shell.leftPage === "search"; iconSize: 20; onClicked: shell.leftPage = "search" }
                    C.RailButton { icon: "tag"; active: shell.leftPage === "tags"; iconSize: 20; onClicked: shell.leftPage = "tags" }
                    C.RailButton { icon: "snippet"; active: shell.leftPage === "snippet"; iconSize: 20; onClicked: shell.leftPage = "snippet" }
                    C.RailButton { icon: "history"; active: shell.leftPage === "quick"; iconSize: 20; onClicked: shell.leftPage = "quick" }
                }
                C.RailButton { icon: "settings"; iconSize: 20; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 10 }
            }

            // Notes list
            Rectangle {
                visible: Appearance.showLeft
                width: 312; height: parent.height; color: Theme.sidebar
                Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }
                SearchPanel { anchors.fill: parent; visible: shell.leftPage === "search" }
                TagsPanel { anchors.fill: parent; visible: shell.leftPage === "tags" }
                QuickAccessPanel { anchors.fill: parent; visible: shell.leftPage === "quick" }
                SnippetPanel { anchors.fill: parent; visible: shell.leftPage === "snippet" }
                Column {
                    visible: shell.leftPage === "explorer"
                    anchors.fill: parent
                    Column {
                        width: parent.width; topPadding: 14; leftPadding: 16; rightPadding: 16; spacing: 12
                        Item {
                            width: parent.width - 32; height: 26
                            Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter; text: (typeof Explorer !== "undefined" && Explorer.currentNotebookName.length > 0) ? Explorer.currentNotebookName : "笔记本"; color: Theme.text; font.pixelSize: 22; font.weight: Font.DemiBold; font.family: Theme.fontSerif }
                            Row {
                                anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; spacing: 2
                                Repeater { model: ["sort", "plus"]; delegate: Icons.Icon { required property string modelData; name: modelData; size: 16; color: Theme.dim } }
                            }
                        }
                        Rectangle {
                            width: parent.width - 32; height: 34; radius: 9; color: Theme.hover; border.color: Theme.border; border.width: 1
                            Row {
                                anchors.left: parent.left; anchors.leftMargin: 11; anchors.verticalCenter: parent.verticalCenter; spacing: 8
                                Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "search"; size: 15; color: Theme.faint }
                                Text { anchors.verticalCenter: parent.verticalCenter; text: "搜索笔记…"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontUi }
                            }
                            MouseArea { anchors.fill: parent; onClicked: shell.leftPage = "search" }
                        }
                    }
                    Column {
                        width: parent.width - 16; x: 8; topPadding: 4
                        Repeater {
                            model: (typeof Explorer !== "undefined")
                                   ? Explorer.visibleNodes.filter(function(n) { return n.type === "file" && !n.isExternal; })
                                   : []
                            delegate: C.NoteCard {
                                required property var modelData
                                title: modelData.name
                                snippet: ""
                                meta: ""
                                active: modelData.selected === true
                                fav: false
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: Explorer.selectNode(modelData.nodeId)
                                }
                            }
                        }
                    }
                }
            }

            // Paper (reading column)
            Rectangle {
                width: parent.width - 52 - (Appearance.showLeft ? 312 : 0)
                height: parent.height; color: Theme.canvas; clip: true
                EditorArea { anchors.fill: parent }
                // Floating word count
                Rectangle {
                    anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 18
                    width: wc.implicitWidth + 26; height: 32; radius: 16
                    color: Theme.card; border.color: Theme.border; border.width: 1
                    Row {
                        id: wc
                        anchors.centerIn: parent; spacing: 9
                        Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 7; height: 7; radius: 4; color: Theme.sage }
                        Text { anchors.verticalCenter: parent.verticalCenter; text: "已自动保存 · 612 字"; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
                    }
                }
            }
        }
    }
}
