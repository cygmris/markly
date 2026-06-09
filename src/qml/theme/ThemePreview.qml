// Theme validation harness — proves live switching + persistence before the
// full shell (spec #3). Shows swatches colored by Theme.* and a floating "外观"
// button that opens the AppearancePanel drawer.
import QtQuick

Rectangle {
    id: root
    color: Theme.window

    readonly property var swatches: [
        { name: "window", c: Theme.window },
        { name: "rail", c: Theme.rail },
        { name: "sidebar", c: Theme.sidebar },
        { name: "canvas", c: Theme.canvas },
        { name: "card", c: Theme.card },
        { name: "bar", c: Theme.bar },
        { name: "accent", c: Theme.accent },
        { name: "accentSoft", c: Theme.accentSoft },
        { name: "selection", c: Theme.selection },
        { name: "border", c: Theme.borderStrong },
        { name: "heading", c: Theme.heading },
        { name: "emphasis", c: Theme.emphasis },
        { name: "codeInline", c: Theme.codeInline },
        { name: "link", c: Theme.link },
        { name: "codeBg", c: Theme.codeBg },
        { name: "lime", c: Theme.lime },
        { name: "amber", c: Theme.amber },
        { name: "pink", c: Theme.pink }
    ]

    Flickable {
        anchors.fill: parent
        contentHeight: content.height
        clip: true

        Column {
            id: content
            width: root.width
            padding: 28
            spacing: 20

            Text {
                text: "Markly · 主题预览"
                color: Theme.text
                font.pixelSize: 28
                font.bold: true
                font.family: Theme.fontDisplay
            }
            Text {
                text: "风格 / 主题 / 强调色 / 布局 实时切换 — 右下角「外观」"
                color: Theme.dim
                font.pixelSize: 14
                font.family: Theme.fontUi
            }

            // Typography sample on a canvas card.
            Rectangle {
                width: content.width - 56
                radius: Theme.radiusLarge
                color: Theme.canvas
                border.color: Theme.border
                border.width: 1
                height: sampleCol.height + 36
                Column {
                    id: sampleCol
                    x: 22; y: 18
                    width: parent.width - 44
                    spacing: 8
                    Text { text: "二叉树的最大路径和"; color: Theme.heading; font.pixelSize: 22; font.bold: true; font.family: Theme.fontUi }
                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: "给定一个二叉树的根节点 root，返回其最大路径和。"
                        color: Theme.text; font.pixelSize: 15; font.family: Theme.fontSerif
                    }
                    Rectangle {
                        width: codeText.width + 24; height: codeText.height + 16
                        radius: 8; color: Theme.codeBg
                        Text {
                            id: codeText
                            x: 12; y: 8
                            text: "int dfs(TreeNode* node) { ... }"
                            color: Theme.codeInk; font.pixelSize: 13; font.family: Theme.fontMono
                        }
                    }
                    Text { text: "#算法  #DFS  #二叉树"; color: Theme.accent; font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi }
                }
            }

            // Token swatch grid.
            Grid {
                columns: 6
                spacing: 12
                Repeater {
                    model: root.swatches
                    delegate: Column {
                        required property var modelData
                        spacing: 5
                        Rectangle {
                            width: 110; height: 48; radius: 8
                            color: modelData.c
                            border.color: Theme.border
                            border.width: 1
                        }
                        Text { text: modelData.name; color: Theme.dim; font.pixelSize: 11; font.family: Theme.fontMono }
                    }
                }
            }
        }
    }

    // Dim overlay + sliding panel.
    Rectangle {
        id: scrim
        anchors.fill: parent
        color: "#60000000"
        visible: panelDrawer.open
        MouseArea { anchors.fill: parent; onClicked: panelDrawer.open = false }
    }

    Item {
        id: panelDrawer
        property bool open: false
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: panelLoader.width
        x: open ? parent.width - width : parent.width
        Behavior on x { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }

        AppearancePanel {
            id: panelLoader
            height: parent.height
        }
    }

    // Floating "外观" button.
    Rectangle {
        width: btnRow.width + 30
        height: 42
        radius: 21
        color: Theme.accent
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 18
        Row {
            id: btnRow
            anchors.centerIn: parent
            spacing: 8
            Text { text: "⚙"; color: Theme.accentText; font.pixelSize: 16; anchors.verticalCenter: parent.verticalCenter }
            Text { text: "外观"; color: Theme.accentText; font.pixelSize: 14; font.bold: true; font.family: Theme.fontUi; anchors.verticalCenter: parent.verticalCenter }
        }
        MouseArea { anchors.fill: parent; onClicked: panelDrawer.open = !panelDrawer.open }
    }
}
