// Style B — Focus Writing (warm paper, serif reading column). Ports focus.jsx.
import QtQuick
import "../icons" as Icons
import "components" as C
import "DemoData.js" as Demo

Rectangle {
    id: shell
    color: Theme.window

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
                    C.RailButton { icon: "notebook"; active: true; iconSize: 20 }
                    C.RailButton { icon: "star"; iconSize: 20 }
                    C.RailButton { icon: "tag"; iconSize: 20 }
                    C.RailButton { icon: "history"; iconSize: 20 }
                }
                C.RailButton { icon: "settings"; iconSize: 20; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 10 }
            }

            // Notes list
            Rectangle {
                visible: Appearance.showLeft
                width: 312; height: parent.height; color: Theme.sidebar
                Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }
                Column {
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
                Flickable {
                    anchors.fill: parent
                    contentHeight: paper.implicitHeight * Theme.contentZoom
                    Column {
                        id: paper
                        width: Math.min(680, parent.width / Theme.contentZoom - 64)
                        scale: Theme.contentZoom
                        transformOrigin: Item.TopLeft
                        x: Math.max(32, (parent.width / Theme.contentZoom - width) / 2)
                        y: 60
                        spacing: 20
                        Row {
                            spacing: 10
                            Text { text: "算法"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontUi }
                            Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "chevR"; size: 12; color: Theme.faint }
                            Text { text: "二叉树"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontUi }
                        }
                        Text { text: "二叉树的最大路径和"; color: Theme.text; font.pixelSize: 38; font.weight: Font.DemiBold; font.family: Theme.fontSerif }
                        Text { text: "更新于 6 月 9 日 · 612 字 · 约 3 分钟"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontMono }
                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            text: "给定一个二叉树的根节点 root，返回其最大路径和。同一个节点在一条路径序列中至多出现一次，且路径至少包含一个节点，不一定经过根节点。"
                            color: Theme.text; font.pixelSize: 19; font.family: Theme.fontSerif; lineHeight: 1.6
                        }
                        Rectangle {
                            width: parent.width; implicitHeight: quote.implicitHeight + 16
                            color: Theme.accentSoft; radius: 8
                            Rectangle { width: 3; height: parent.height; color: Theme.accent }
                            Text {
                                id: quote
                                x: 22; width: parent.width - 40; y: 8
                                wrapMode: Text.WordWrap
                                text: "路径 被定义为一条从树中任意节点出发，沿父—子连接走过的节点序列。"
                                color: Theme.dim; font.pixelSize: 18; font.italic: true; font.family: Theme.fontSerif; lineHeight: 1.5
                            }
                        }
                        Text { text: "思路"; color: Theme.text; font.pixelSize: 25; font.weight: Font.DemiBold; font.family: Theme.fontSerif }
                        Text {
                            width: parent.width; wrapMode: Text.WordWrap
                            text: "对每个节点，递归计算它对父节点的「最大贡献值」——即以该节点为端点、向下延伸的最大路径和。负贡献直接舍弃。"
                            color: Theme.text; font.pixelSize: 19; font.family: Theme.fontSerif; lineHeight: 1.6
                        }
                        Rectangle {
                            width: parent.width; implicitHeight: code.implicitHeight + 36; radius: 12
                            color: Theme.codeBg; border.color: Theme.border; border.width: 1
                            Text {
                                id: code
                                x: 20; y: 18; width: parent.width - 40
                                text: "int dfs(TreeNode* node) {\n  if (!node) return 0;\n  int l = max(0, dfs(node->left));\n  int r = max(0, dfs(node->right));\n  ans = max(ans, node->val + l + r);\n  return node->val + max(l, r);\n}"
                                color: Theme.codeInk; font.pixelSize: 14; font.family: Theme.fontMono; lineHeight: 1.6
                            }
                        }
                    }
                }
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
