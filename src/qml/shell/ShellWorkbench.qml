// Style C — Bold Workbench (ink nav, omnibar, property header). Ports bold.jsx.
import QtQuick
import "../icons" as Icons
import "components" as C
import "DemoData.js" as Demo

Rectangle {
    id: shell
    color: Theme.window

    function tok(role) { var c = Theme[role]; return c !== undefined ? c : Theme.accent; }

    // Left dock page: "explorer" (ink nav) or "search".
    property string leftPage: "explorer"

    SettingsDialog { id: settingsDialog; z: 100 }
    function openSettings() { settingsDialog.show() }

    function newRootNote() {
        var name = Dialogs.promptText("新建笔记", "笔记名称（含 .md）", "新笔记.md");
        if (name.length > 0) { var e = Explorer.newNote(0, name); if (e.length > 0) Dialogs.notify("操作失败", e); }
    }
    UnitedEntry {
        id: unitedEntry; z: 110
        actions: ({
            openSettings: function(){ settingsDialog.show() },
            setPage: function(p){ shell.leftPage = p },
            newNote: function(){ shell.newRootNote() }
        })
    }
    function openEntry() { unitedEntry.show() }
    Shortcut { sequences: ["Ctrl+P"]; onActivated: unitedEntry.show() }
    // VNote parity: Ctrl+T toggles edit/read (EditRead).
    Shortcut { sequences: ["Ctrl+T"]; onActivated: if (typeof Views !== "undefined") Views.setViewMode(Views.viewMode === "edit" ? "read" : "edit") }
    // 分屏预览（左编辑右渲染）专属快捷键。
    Shortcut { sequences: ["Ctrl+E"]; onActivated: if (typeof Views !== "undefined") Views.setViewMode("split") }
    // VNote parity: Ctrl+S saves the active note regardless of focus.
    // Disabled in mindmap mode, where MindmapPane owns Ctrl+S (avoids ambiguity).
    Shortcut {
        sequences: ["Ctrl+S"]
        enabled: (typeof Views !== "undefined") && Views.viewMode !== "mindmap"
        onActivated: if (Views.splits.length > Views.activeSplitIndex)
                         Views.saveTab(Views.splits[Views.activeSplitIndex].currentBufferId)
    }

    Column {
        anchors.fill: parent

        // ---- Title bar with omnibar ----
        Rectangle {
            width: parent.width; height: 40; color: Theme.rail
            MouseArea { anchors.fill: parent; onPressed: Win.startMove(); onDoubleClicked: Win.toggleMaximize() }
            Rectangle {
                anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter
                width: 19; height: 19; radius: 6; color: Theme.accent
                Text { anchors.centerIn: parent; text: "M"; color: "#ffffff"; font.pixelSize: 12; font.bold: true; font.family: Theme.fontDisplay }
            }
            Item {
                anchors.centerIn: parent; width: Math.min(380, parent.width * 0.5); height: 30
                C.Omnibar { anchors.fill: parent }
                MouseArea { anchors.fill: parent; onClicked: unitedEntry.show() }
            }
            Icons.Icon {
                anchors.right: winCtl.left; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
                name: "settings"; size: 16; color: Theme.railDim
                MouseArea { anchors.fill: parent; anchors.margins: -4; onClicked: settingsDialog.show() }
            }
            C.WinControls { id: winCtl; anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter }
        }

        // ---- Body ----
        Row {
            width: parent.width
            height: parent.height - 40 - 26

            // Ink nav
            Rectangle {
                visible: Appearance.showLeft
                width: 244; height: parent.height; color: Theme.rail
                SearchPanel { anchors.fill: parent; visible: shell.leftPage === "search" }
                TagsPanel { anchors.fill: parent; visible: shell.leftPage === "tags" }
                QuickAccessPanel { anchors.fill: parent; visible: shell.leftPage === "quick" }
                SnippetPanel { anchors.fill: parent; visible: shell.leftPage === "snippet" }
                Column {
                    visible: shell.leftPage === "explorer"
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
                            MouseArea { anchors.fill: parent; onClicked: shell.leftPage = (modelData.label === "最近") ? "quick" : "explorer" }
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
                    Text {
                        text: "标签"; color: Theme.railDim; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi; leftPadding: 8; topPadding: 10
                        MouseArea { anchors.fill: parent; onClicked: shell.leftPage = "tags" }
                    }
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
                            Text {
                                visible: (typeof Views === "undefined") || Views.outline.length === 0
                                text: "（无标题）"; color: Theme.faint; font.pixelSize: 12; font.family: Theme.fontUi
                            }
                            Repeater {
                                model: (typeof Views !== "undefined") ? Views.outline : []
                                delegate: Row {
                                    required property var modelData
                                    height: 26; spacing: 8
                                    Rectangle { anchors.verticalCenter: parent.verticalCenter; width: 3; height: 14; radius: 2; color: "transparent" }
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        leftPadding: Math.max(0, modelData.level - 1) * 12
                                        text: modelData.text; color: Theme.dim; font.pixelSize: 13; font.weight: modelData.level === 1 ? Font.DemiBold : Font.Medium; font.family: Theme.fontUi
                                        elide: Text.ElideRight; width: oc.width - 20
                                    }
                                    MouseArea { anchors.fill: parent; onClicked: Views.gotoOutlineLine(modelData.line) }
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
