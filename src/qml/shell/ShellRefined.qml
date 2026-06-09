// Style A — Refined Classic (3-pane IDE). Ports refined.jsx RefinedClassic.
import QtQuick
import "../icons" as Icons
import "components" as C
import "DemoData.js" as Demo

Rectangle {
    id: shell
    color: Theme.window

    // Left dock page: "explorer" (notebook tree) or "search".
    property string leftPage: "explorer"

    function roleColor(role) {
        if (!role || role === "text") return Theme.text;
        var c = Theme[role];
        return c !== undefined ? c : Theme.text;
    }

    NodeContextMenu { id: nodeMenu }
    NotebookSelector { id: nbSelector }

    function newRootNote() {
        var name = Dialogs.promptText("新建笔记", "笔记名称（含 .md）", "新笔记.md");
        if (name.length > 0) {
            var err = Explorer.newNote(0, name);
            if (err.length > 0) Dialogs.notify("操作失败", err);
        }
    }

    Column {
        anchors.fill: parent

        // ---- Title bar ----
        Item {
            width: parent.width
            height: 38
            Rectangle { anchors.fill: parent; color: Theme.window }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }
            // Drag region behind the controls.
            MouseArea {
                anchors.fill: parent
                onPressed: Win.startMove()
                onDoubleClicked: Win.toggleMaximize()
            }
            Row {
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 18; height: 18; radius: 5; color: Theme.accent
                    Text { anchors.centerIn: parent; text: "V"; color: Theme.accentText; font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi }
                }
                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    height: 24; radius: 7; color: Theme.hover
                    width: nbRow.implicitWidth + 18
                    MouseArea { anchors.fill: parent; onClicked: nbSelector.popup() }
                    Row {
                        id: nbRow
                        anchors.centerIn: parent
                        spacing: 6
                        Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "book"; size: 14; color: Theme.accent }
                        Text { anchors.verticalCenter: parent.verticalCenter; text: (typeof Explorer !== "undefined" && Explorer.currentNotebookName.length > 0) ? Explorer.currentNotebookName : "笔记本"; color: Theme.text; font.pixelSize: 13; font.weight: Font.DemiBold; font.family: Theme.fontUi }
                        Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "chevD"; size: 13; color: Theme.faint }
                    }
                }
                Repeater {
                    model: [ { ic: "plus", t: "新建" }, { ic: "import", t: "导入" }, { ic: "flash", t: "快速记录" } ]
                    delegate: Row {
                        required property var modelData
                        spacing: 5
                        Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: modelData.ic; size: 14; color: Theme.dim }
                        Text { anchors.verticalCenter: parent.verticalCenter; text: modelData.t; color: Theme.dim; font.pixelSize: 13; font.family: Theme.fontUi }
                    }
                }
            }
            Row {
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2
                Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: Theme.isDark ? "sun" : "moon"; size: 15; color: Theme.dim }
                Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "settings"; size: 15; color: Theme.dim }
                Item { width: 6; height: 1 }
                C.WinControls { anchors.verticalCenter: parent.verticalCenter }
            }
        }

        // ---- Body ----
        Row {
            width: parent.width
            height: parent.height - 38 - 26

            // Rail
            Rectangle {
                width: 56
                height: parent.height
                color: Theme.rail
                Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }
                Column {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 8
                    spacing: 4
                    C.RailButton { icon: "notebook"; active: shell.leftPage === "explorer"; onClicked: shell.leftPage = "explorer" }
                    C.RailButton { icon: "tag"; active: shell.leftPage === "tags"; onClicked: shell.leftPage = "tags" }
                    C.RailButton { icon: "search"; active: shell.leftPage === "search"; onClicked: shell.leftPage = "search" }
                    C.RailButton { icon: "snippet" }
                    C.RailButton { icon: "history" }
                }
                C.RailButton { icon: "settings"; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: 8 }
            }

            // Sidebar (notebook tree)
            Rectangle {
                visible: Appearance.showLeft
                width: 256
                height: parent.height
                color: Theme.sidebar
                Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }
                SearchPanel { anchors.fill: parent; visible: shell.leftPage === "search" }
                TagsPanel { anchors.fill: parent; visible: shell.leftPage === "tags" }
                Column {
                    visible: shell.leftPage === "explorer"
                    anchors.fill: parent
                    Item {
                        width: parent.width; height: 42
                        Text { anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; text: "笔记本"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
                        Row {
                            anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter; spacing: 1
                            Icons.Icon {
                                name: "plus"; size: 15; color: Theme.dim
                                MouseArea { anchors.fill: parent; anchors.margins: -4; onClicked: shell.newRootNote() }
                            }
                            Icons.Icon { name: "sort"; size: 15; color: Theme.dim }
                            Icons.Icon { name: "more"; size: 15; color: Theme.dim }
                        }
                    }
                    Column {
                        width: parent.width - 16
                        x: 8
                        Repeater {
                            model: (typeof Explorer !== "undefined") ? Explorer.visibleNodes : []
                            delegate: C.TreeRow {
                                required property var modelData
                                depth: modelData.depth
                                icon: modelData.type === "folder" ? "folder" : "md"
                                label: modelData.name
                                open: modelData.type === "folder" ? modelData.expanded : undefined
                                selected: modelData.selected === true
                                muted: modelData.isExternal === true
                                customNameColor: modelData.nameColor
                                onClicked: {
                                    if (modelData.type === "folder") Explorer.toggleExpand(modelData.nodeId);
                                    else Explorer.selectNode(modelData.nodeId);
                                }
                                onRightClicked: function(px, py) {
                                    nodeMenu.targetId = modelData.nodeId;
                                    nodeMenu.targetParentId = modelData.parentId;
                                    nodeMenu.targetType = modelData.type;
                                    nodeMenu.targetName = modelData.name;
                                    nodeMenu.targetIsExternal = modelData.isExternal === true;
                                    nodeMenu.popup();
                                }
                            }
                        }
                    }
                }
            }

            // Editor column
            Rectangle {
                width: parent.width - 56 - (Appearance.showLeft ? 256 : 0) - (Appearance.showRight ? 236 : 0)
                height: parent.height
                color: Theme.canvas
                clip: true
                Column {
                    anchors.fill: parent
                    C.EditorToolbar { width: parent.width }
                    EditorArea {
                        width: parent.width
                        height: parent.height - 42
                    }
                }
            }

            // Outline
            Rectangle {
                visible: Appearance.showRight
                width: 236
                height: parent.height
                color: Theme.sidebar
                Rectangle { anchors.left: parent.left; width: 1; height: parent.height; color: Theme.border }
                Column {
                    anchors.fill: parent
                    Item {
                        width: parent.width; height: 42
                        Text { anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; text: "大纲"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
                        Icons.Icon { anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter; name: "sort"; size: 15; color: Theme.dim }
                    }
                    Column {
                        width: parent.width - 16
                        x: 8
                        Repeater {
                            model: Demo.OUTLINE
                            delegate: C.OutlineRow {
                                required property var modelData
                                label: modelData.label; level: modelData.level; active: modelData.active === true
                            }
                        }
                    }
                    Item { width: parent.width; height: 12 }
                    Flow {
                        width: parent.width - 24
                        x: 12
                        spacing: 6
                        Repeater {
                            model: Demo.TAGS
                            delegate: Rectangle {
                                required property string modelData
                                height: 22; radius: 11; width: tg.implicitWidth + 18
                                color: Theme.accentSoft
                                Text { id: tg; anchors.centerIn: parent; text: modelData; color: Theme.accent; font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi }
                            }
                        }
                    }
                }
            }
        }

        // ---- Status bar ----
        C.StatusBar {
            width: parent.width
            Text { anchors.verticalCenter: parent.verticalCenter; text: "● 已保存"; color: Theme.accent; font.pixelSize: 12; font.weight: Font.DemiBold; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: (typeof Views !== "undefined") ? ("行 " + Views.statsLine + ", 列 " + Views.statsColumn) : "行 1, 列 1"; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: (typeof Views !== "undefined") ? (Views.statsLineCount + " 行 · " + Views.statsCharCount + " 字") : "0 行"; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
            Item { width: 1; height: 1 }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "UTF-8"; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "Markdown"; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontMono }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "NORMAL"; color: Theme.emphasis; font.pixelSize: 12; font.family: Theme.fontMono }
        }
    }
}
