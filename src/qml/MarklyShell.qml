// Root shell: swaps the active layout by Appearance.style (Loader) and mounts the
// floating "外观" button + AppearancePanel drawer (reused from spec #2).
import QtQuick
import "theme" as ThemeUI
import "shell"

Item {
    id: root

    readonly property bool hasNotebook: (typeof Explorer !== "undefined") && Explorer.hasNotebook

    // Dev/test hook: switch the active shell to the search page and run a query.
    function showSearch(keyword) {
        if (loader.item) loader.item.leftPage = "search";
        Search.search(keyword, 0, 0x1 | 0x2, "");
    }

    Loader {
        id: loader
        anchors.fill: parent
        visible: root.hasNotebook
        source: Appearance.style === 0 ? "shell/ShellRefined.qml"
              : Appearance.style === 1 ? "shell/ShellFocus.qml"
                                       : "shell/ShellWorkbench.qml"
    }

    EmptyState {
        anchors.fill: parent
        visible: !root.hasNotebook
    }

    // Dim overlay + sliding appearance panel.
    Rectangle {
        anchors.fill: parent
        color: "#60000000"
        visible: drawer.open
        MouseArea { anchors.fill: parent; onClicked: drawer.open = false }
    }
    Item {
        id: drawer
        property bool open: false
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: panel.width
        x: open ? parent.width - width : parent.width
        Behavior on x { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }
        ThemeUI.AppearancePanel { id: panel; height: parent.height }
    }

    // Floating "外观" button.
    Rectangle {
        width: btnRow.width + 30; height: 42; radius: 21; color: Theme.accent
        anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 18
        Row {
            id: btnRow
            anchors.centerIn: parent; spacing: 8
            Text { anchors.verticalCenter: parent.verticalCenter; text: "⚙"; color: Theme.accentText; font.pixelSize: 16 }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "外观"; color: Theme.accentText; font.pixelSize: 14; font.bold: true; font.family: Theme.fontUi }
        }
        MouseArea { anchors.fill: parent; onClicked: drawer.open = !drawer.open }
    }
}
