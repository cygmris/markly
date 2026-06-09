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
                    EditorArea {
                        width: parent.width; height: parent.height - (hdr.implicitHeight + 36)
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
