// Markdown editor toolbar: tool button groups with separators + trailing actions.
import QtQuick
import QtQuick.Controls
import "../../icons" as Icons
import ".." as Shell

Rectangle {
    id: root
    property color barColor: Theme.canvas
    implicitHeight: 42
    color: barColor

    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }

    // Hidden render surface for HTML/PDF export (#15).
    Shell.ExportView { id: exportView }

    // Export the current note in the given format.
    function doExport(fmt) {
        if (typeof Views === "undefined" || Views.currentFileName.length === 0) return;
        var base = Views.currentFileName.replace(/\.md$/i, "");
        var ext = fmt === "pdf" ? ".pdf" : (fmt === "html" ? ".html" : ".md");
        var filt = fmt === "pdf" ? "PDF (*.pdf)" : (fmt === "html" ? "HTML (*.html)" : "Markdown (*.md)");
        var path = Dialogs.saveFile("导出", base + ext, filt);
        if (path.length === 0) return;
        if (fmt === "md") {
            Dialogs.notify("导出", Export.writeText(path, Views.currentText) ? "已导出 Markdown" : "导出失败");
        } else {
            exportView.exportTo(fmt, Views.currentText, Views.currentFileDir, path, function(ok) {
                Dialogs.notify("导出", ok ? ("已导出 " + fmt.toUpperCase()) : "导出失败");
            });
        }
    }

    Menu {
        id: exportMenu
        MenuItem { text: "导出 Markdown"; onTriggered: root.doExport("md") }
        MenuItem { text: "导出 HTML"; onTriggered: root.doExport("html") }
        MenuItem { text: "导出 PDF"; onTriggered: root.doExport("pdf") }
    }

    // Task menu (#16): list tasks, run the chosen one on the current note.
    Menu {
        id: taskMenu
        MenuItem { text: "（无任务）"; enabled: false; visible: (typeof Tasks === "undefined") || Tasks.list.length === 0 }
        Instantiator {
            model: (typeof Tasks !== "undefined") ? Tasks.list : []
            delegate: MenuItem {
                text: modelData.name
                onTriggered: {
                    var out = Tasks.run(modelData.name, Views.currentFileDir, Views.currentFileName);
                    Dialogs.notify("任务: " + modelData.name, out);
                }
            }
            onObjectAdded: function(index, object) { taskMenu.insertItem(index + 1, object) }
            onObjectRemoved: function(index, object) { taskMenu.removeItem(object) }
        }
    }

    component ToolBtn: Item {
        property string icon
        property bool active: false
        property var act: undefined
        width: 30; height: 28
        Rectangle {
            anchors.fill: parent; radius: 6
            color: active ? Theme.selection : (h.containsMouse ? Theme.hover : "transparent")
        }
        Icons.Icon {
            anchors.centerIn: parent; name: parent.icon; size: 17
            color: parent.active ? Theme.accent : Theme.dim
        }
        MouseArea { id: h; anchors.fill: parent; hoverEnabled: true; onClicked: if (parent.act) parent.act() }
    }
    component Sep: Rectangle { width: 1; height: 18; color: Theme.border; anchors.verticalCenter: parent.verticalCenter }

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1
        ToolBtn { icon: "save"; act: function() { if (typeof Views !== "undefined") Views.saveTab(Views.splits.length > 0 ? Views.splits[0].currentBufferId : 0) } }
        ToolBtn {
            icon: "eye"
            active: (typeof Views !== "undefined") && Views.viewMode !== "edit"
            act: function() { if (typeof Views !== "undefined") Views.cycleViewMode() }
        }
        ToolBtn { icon: "image" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "heading" }
        ToolBtn { icon: "bold" }
        ToolBtn { icon: "italic" }
        ToolBtn { icon: "strike" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "listU" }
        ToolBtn { icon: "listO" }
        ToolBtn { icon: "check" }
        ToolBtn { icon: "quote" }
        Item { width: 8; height: 1 }
        Sep {}
        Item { width: 8; height: 1 }
        ToolBtn { icon: "code" }
        ToolBtn { icon: "sigma" }
        ToolBtn { icon: "link" }
        ToolBtn { icon: "table" }
    }
    Row {
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1
        ToolBtn { icon: "cmd"; act: function() { taskMenu.popup() } }
        ToolBtn { icon: "export"; act: function() { exportMenu.popup() } }
        ToolBtn { icon: "search" }
        ToolBtn { icon: "moreV" }
    }
}
