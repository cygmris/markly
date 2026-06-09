// Style C — Bold Workbench (ink nav, omnibar, property header). Ports bold.jsx.
import QtQuick
import "../icons" as Icons
import "components" as C
import "DemoData.js" as Demo

Rectangle {
    id: shell
    color: Theme.window

    function tok(role) { var c = Theme[role]; return c !== undefined ? c : Theme.accent; }

    Column {
        anchors.fill: parent

        // ---- Title bar with omnibar ----
        Rectangle {
            width: parent.width; height: 40; color: Theme.rail
            MouseArea { anchors.fill: parent; onPressed: Win.startMove(); onDoubleClicked: Win.toggleMaximize() }
            Rectangle {
                anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter
                width: 19; height: 19; radius: 6; color: Theme.accent
                Text { anchors.centerIn: parent; text: "V"; color: "#ffffff"; font.pixelSize: 12; font.bold: true; font.family: Theme.fontDisplay }
            }
            C.Omnibar { anchors.centerIn: parent; width: Math.min(380, parent.width * 0.5) }
            C.WinControls { anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter }
        }

        // ---- Body ----
        Row {
            width: parent.width
            height: parent.height - 40 - 26

            // Ink nav
            Rectangle {
                visible: Appearance.showLeft
                width: 244; height: parent.height; color: Theme.rail
                Column {
                    anchors.fill: parent; anchors.margins: 10; spacing: 2
                    Repeater {
                        model: Demo.NAV_ITEMS
                        delegate: Rectangle {
                            required property var modelData
                            width: parent.width; height: 32; radius: 8
                            color: modelData.label === "收件箱" ? Theme.selection : "transparent"
                            Row {
                                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10; spacing: 10
                                Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: modelData.icon; size: 16; color: modelData.label === "收件箱" ? Theme.accent : Theme.railDim }
                                Text { anchors.verticalCenter: parent.verticalCenter; width: parent.width - 50; text: modelData.label; color: modelData.label === "收件箱" ? Theme.railActive : Theme.railDim; font.pixelSize: 13; font.family: Theme.fontUi }
                                Text { anchors.verticalCenter: parent.verticalCenter; visible: modelData.count !== ""; text: modelData.count; color: Theme.railDim; font.pixelSize: 11; font.family: Theme.fontMono }
                            }
                        }
                    }
                    Rectangle { width: parent.width; height: 1; color: Theme.border; opacity: 0.6 }
                    Text { text: "笔记本"; color: Theme.railDim; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi; leftPadding: 8; topPadding: 6 }
                    Repeater {
                        model: (typeof Explorer !== "undefined") ? Explorer.notebooks : []
                        delegate: Rectangle {
                            required property var modelData
                            width: parent.width; height: 30; radius: 8
                            color: modelData.current ? Theme.hover : "transparent"
                            Row {
                                anchors.fill: parent; anchors.leftMargin: 10; spacing: 10
                                Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 9; height: 9; radius: 3; color: Theme.accent }
                                Text { anchors.verticalCenter: parent.verticalCenter; text: modelData.name; color: modelData.current ? Theme.railActive : Theme.railText; font.pixelSize: 13; font.family: Theme.fontUi }
                            }
                            MouseArea { anchors.fill: parent; onClicked: Explorer.switchNotebook(modelData.id) }
                        }
                    }
                    Text { text: "标签"; color: Theme.railDim; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi; leftPadding: 8; topPadding: 10 }
                    Flow {
                        width: parent.width; spacing: 6; leftPadding: 6
                        Repeater {
                            model: Demo.NAV_TAGS
                            delegate: Rectangle {
                                required property string modelData
                                height: 22; radius: 7; width: ttg.implicitWidth + 18; color: Theme.hover
                                Text { id: ttg; anchors.centerIn: parent; text: modelData; color: Theme.railText; font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi }
                            }
                        }
                    }
                }
            }

            // Content
            Rectangle {
                width: parent.width - (Appearance.showLeft ? 244 : 0) - (Appearance.showRight ? 270 : 0)
                height: parent.height; color: Theme.canvas; clip: true
                Column {
                    anchors.fill: parent
                    // Header band
                    Rectangle {
                        width: parent.width; implicitHeight: hdr.implicitHeight + 36; color: Theme.canvas
                        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }
                        Column {
                            id: hdr
                            x: 40; y: 20; width: parent.width - 80; spacing: 12
                            Row {
                                width: parent.width; spacing: 8
                                Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 8; height: 8; radius: 3; color: Theme.accent }
                                Text { anchors.verticalCenter: parent.verticalCenter; text: "算法"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontUi }
                                Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "chevR"; size: 12; color: Theme.faint }
                                Text { anchors.verticalCenter: parent.verticalCenter; text: "二叉树"; color: Theme.faint; font.pixelSize: 13; font.family: Theme.fontUi }
                            }
                            Text { text: "二叉树的最大路径和"; color: Theme.text; font.pixelSize: 32; font.weight: Font.DemiBold; font.family: Theme.fontDisplay }
                            Row {
                                spacing: 10
                                Text { anchors.verticalCenter: parent.verticalCenter; width: 50; text: "状态"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontUi }
                                C.Chip { text: Demo.PROPS.status; chipColor: Theme.lime; solid: true }
                            }
                            Row {
                                spacing: 6
                                Text { anchors.verticalCenter: parent.verticalCenter; width: 50; text: "标签"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontUi }
                                Repeater { model: Demo.PROPS.tags; delegate: C.Chip { required property string modelData; text: modelData; chipColor: Theme.accent } }
                            }
                            Row {
                                spacing: 10
                                Text { anchors.verticalCenter: parent.verticalCenter; width: 50; text: "更新"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontUi }
                                Text { anchors.verticalCenter: parent.verticalCenter; text: Demo.PROPS.updated; color: Theme.dim; font.pixelSize: 13; font.family: Theme.fontMono }
                            }
                        }
                    }
                    // Editor
                    Flickable {
                        width: parent.width; height: parent.height - (hdr.implicitHeight + 36)
                        contentHeight: ed.implicitHeight * Theme.contentZoom; clip: true
                        Column {
                            id: ed
                            width: Math.min(720, parent.width / Theme.contentZoom - 80)
                            scale: Theme.contentZoom; transformOrigin: Item.TopLeft
                            x: 40; y: 28; spacing: 18
                            Text {
                                width: parent.width; wrapMode: Text.WordWrap
                                text: "给定一个二叉树的根节点 root，返回其最大路径和。路径至少包含一个节点，且不一定经过根节点。"
                                color: Theme.text; font.pixelSize: 16; font.family: Theme.fontUi; lineHeight: 1.6
                            }
                            Rectangle {
                                width: parent.width; implicitHeight: callout.implicitHeight + 28; radius: 12; color: Theme.accentSoft
                                Row {
                                    x: 16; y: 14; width: parent.width - 32; spacing: 12
                                    Icons.Icon { name: "flash"; size: 18; color: Theme.accent }
                                    Text { id: callout; width: parent.width - 30; wrapMode: Text.WordWrap; text: "关键洞察 — 对每个节点计算「最大贡献值」，负贡献取 0 舍弃，再用 val + left + right 更新全局答案。"; color: Theme.text; font.pixelSize: 14; font.family: Theme.fontUi; lineHeight: 1.5 }
                                }
                            }
                            Text { text: "解法"; color: Theme.text; font.pixelSize: 22; font.weight: Font.DemiBold; font.family: Theme.fontDisplay }
                            Rectangle {
                                width: parent.width; implicitHeight: cc.implicitHeight + 50; radius: 12; color: Theme.codeBg; border.color: Theme.border; border.width: 1
                                Row {
                                    x: 14; y: 9; spacing: 5
                                    Rectangle { width: 9; height: 9; radius: 5; color: Theme.pink }
                                    Rectangle { width: 9; height: 9; radius: 5; color: Theme.amber }
                                    Rectangle { width: 9; height: 9; radius: 5; color: Theme.lime }
                                    Text { text: "maxPathSum.cpp"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontMono; leftPadding: 4 }
                                }
                                Text {
                                    id: cc
                                    x: 18; y: 34; width: parent.width - 36
                                    text: "int dfs(TreeNode* node) {\n  if (!node) return 0;\n  int l = max(0, dfs(node->left));\n  int r = max(0, dfs(node->right));\n  ans = max(ans, node->val + l + r);\n  return node->val + max(l, r);\n}"
                                    color: Theme.codeInk; font.pixelSize: 14; font.family: Theme.fontMono; lineHeight: 1.7
                                }
                            }
                        }
                    }
                }
            }

            // Workbench (right)
            Rectangle {
                visible: Appearance.showRight
                width: 270; height: parent.height; color: Theme.window
                Rectangle { anchors.left: parent.left; width: 1; height: parent.height; color: Theme.border }
                Column {
                    anchors.fill: parent; anchors.margins: 12; spacing: 12
                    // Outline card
                    Rectangle {
                        width: parent.width; implicitHeight: oc.implicitHeight + 24; radius: 12; color: Theme.card; border.color: Theme.border; border.width: 1
                        Column {
                            id: oc
                            x: 14; y: 12; width: parent.width - 28; spacing: 4
                            Text { text: "大纲"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
                            Repeater {
                                model: ["解法", "复杂度", "相关题目"]
                                delegate: Row {
                                    required property string modelData
                                    required property int index
                                    height: 26; spacing: 8
                                    Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 3; height: 14; radius: 2; color: index === 0 ? Theme.accent : "transparent" }
                                    Text { anchors.verticalCenter: parent.verticalCenter; text: modelData; color: index === 0 ? Theme.accent : Theme.dim; font.pixelSize: 13; font.weight: index === 0 ? Font.DemiBold : Font.Medium; font.family: Theme.fontUi }
                                }
                            }
                        }
                    }
                    // Backlinks card
                    Rectangle {
                        width: parent.width; implicitHeight: bc.implicitHeight + 24; radius: 12; color: Theme.card; border.color: Theme.border; border.width: 1
                        Column {
                            id: bc
                            x: 14; y: 12; width: parent.width - 28; spacing: 0
                            Text { text: "反向链接 · 2"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi; bottomPadding: 6 }
                            Repeater {
                                model: Demo.BACKLINKS
                                delegate: Row {
                                    required property var modelData
                                    height: 30; spacing: 9
                                    Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 7; height: 7; radius: 2; color: shell.tok(modelData.colorRole) }
                                    Text { anchors.verticalCenter: parent.verticalCenter; text: modelData.label; color: Theme.text; font.pixelSize: 13; font.family: Theme.fontUi }
                                }
                            }
                        }
                    }
                }
            }
        }

        // ---- Status bar ----
        C.StatusBar {
            width: parent.width; barColor: Theme.rail
            Text { anchors.verticalCenter: parent.verticalCenter; text: "● 已保存"; color: Theme.lime; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "行 12 · 列 8"; color: Theme.railDim; font.pixelSize: 12; font.family: Theme.fontMono }
            Item { width: 1; height: 1 }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "Markdown"; color: Theme.railDim; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "UTF-8"; color: Theme.railDim; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "⌘K 命令"; color: Theme.accent; font.pixelSize: 12; font.family: Theme.fontMono }
        }
    }
}
