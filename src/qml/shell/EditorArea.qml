// Shared editor region for all three shells. Renders Views.splits: each split has a
// tab bar + an editable text placeholder. The editor core (#7) replaces the TextArea.
import QtQuick
import QtQuick.Controls
import "../icons" as Icons

Rectangle {
    id: root
    color: Theme.canvas

    readonly property bool hasOpen: (typeof Views !== "undefined") && Views.hasOpenFile
    readonly property string editFont: Appearance.style === 1 ? Theme.fontSerif
                                     : Appearance.style === 0 ? Theme.fontMono
                                                              : Theme.fontUi

    // Empty state.
    Text {
        anchors.centerIn: parent
        visible: !root.hasOpen
        text: "选择一篇笔记开始"
        color: Theme.faint
        font.pixelSize: 15
        font.family: Theme.fontUi
    }

    Row {
        anchors.fill: parent
        visible: root.hasOpen
        Repeater {
            model: (typeof Views !== "undefined") ? Views.splitCount : 0
            delegate: SplitPane {
                required property int index
                splitIndex: index
                width: root.width / Math.max(1, Views.splitCount)
                height: root.height
            }
        }
    }

    component SplitPane: Column {
        id: pane
        property int splitIndex: 0
        readonly property var split: (typeof Views !== "undefined" && Views.splits.length > splitIndex)
                                     ? Views.splits[splitIndex]
                                     : ({ tabs: [], currentBufferId: 0, currentText: "" })

        // Tab bar + save.
        Rectangle {
            width: pane.width; height: 38; color: Theme.bar
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }
            Row {
                anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                Repeater {
                    model: pane.split.tabs
                    delegate: Rectangle {
                        required property var modelData
                        height: 38
                        width: tlabel.implicitWidth + 44
                        color: modelData.active ? Theme.canvas : "transparent"
                        Rectangle { visible: modelData.active; anchors.bottom: parent.bottom; width: parent.width; height: 2; color: Theme.accent }
                        Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }
                        Row {
                            anchors.verticalCenter: parent.verticalCenter; x: 12; spacing: 7
                            Icons.Icon { anchors.verticalCenter: parent.verticalCenter; name: "md"; size: 14; color: modelData.active ? Theme.accent : Theme.faint }
                            Text {
                                id: tlabel
                                anchors.verticalCenter: parent.verticalCenter
                                text: modelData.name
                                color: modelData.active ? Theme.text : Theme.dim
                                font.pixelSize: 13; font.family: Theme.fontUi
                            }
                            Item {
                                anchors.verticalCenter: parent.verticalCenter; width: 12; height: 12
                                Rectangle { visible: modelData.modified; anchors.centerIn: parent; width: 8; height: 8; radius: 4; color: Theme.dim }
                                Icons.Icon {
                                    visible: !modelData.modified; anchors.centerIn: parent; name: "close"; size: 12; color: Theme.faint
                                    MouseArea {
                                        anchors.fill: parent; anchors.margins: -3
                                        onClicked: Views.closeTab(pane.splitIndex, modelData.bufferId)
                                    }
                                }
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton
                            onClicked: {
                                if (modelData.modified
                                    && !Dialogs.confirm("切换", "切换标签，未保存的修改仍保留在内存。"))
                                    return;
                                Views.activateTab(pane.splitIndex, modelData.bufferId);
                            }
                        }
                    }
                }
            }
            // Save button.
            Icons.Icon {
                anchors.right: parent.right; anchors.rightMargin: 10; anchors.verticalCenter: parent.verticalCenter
                name: "save"; size: 16; color: Theme.dim
                MouseArea { anchors.fill: parent; anchors.margins: -4; onClicked: Views.saveTab(pane.split.currentBufferId) }
            }
        }

        // Editor / preview / split by Views.viewMode, or a viewer for pdf/html (#18).
        Item {
            width: pane.width
            height: pane.height - 38
            readonly property string mode: (typeof Views !== "undefined") ? Views.viewMode : "edit"
            // Non-markdown viewer type by the active tab's extension.
            readonly property string viewerType: {
                var n = pane.split.currentName || "";
                if (/\.pdf$/i.test(n)) return "pdf";
                if (/\.html?$/i.test(n)) return "html";
                return "";
            }

            MarkdownEditor {
                visible: parent.viewerType === "" && parent.mode !== "read" && parent.mode !== "mindmap"
                x: 0
                width: parent.mode === "split" ? parent.width / 2 : parent.width
                height: parent.height
                bufferId: pane.split.currentBufferId
                content: pane.split.currentText
            }
            Rectangle {
                visible: parent.viewerType === "" && parent.mode === "split"
                x: parent.width / 2; width: 1; height: parent.height; color: Theme.border
            }
            PreviewPane {
                visible: parent.viewerType === "" && parent.mode !== "edit" && parent.mode !== "mindmap"
                x: parent.mode === "split" ? parent.width / 2 : 0
                width: parent.mode === "split" ? parent.width / 2 : parent.width
                height: parent.height
                content: pane.split.currentText
                baseDir: (typeof Views !== "undefined") ? Views.currentFileDir : ""
            }
            ViewerPane {
                visible: parent.viewerType !== "" && parent.mode !== "mindmap"
                anchors.fill: parent
                kind: parent.viewerType
                path: pane.split.currentPath || ""
            }
            // Editable mindmap of the note's heading outline (#18b view / #18c edit).
            // #18c: editable; Ctrl+S / the "保存思维导图到笔记" command (via
            // Views.saveMindmapRequested) serialize the tree back to the buffer.
            MindmapPane {
                visible: parent.mode === "mindmap" && parent.viewerType === ""
                anchors.fill: parent
                text: pane.split.currentText
                editable: true
                bufferId: pane.split.currentBufferId
            }
        }
    }
}
