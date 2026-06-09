// Appearance settings panel — ports appshell.jsx SettingsPanel.
// Style cards (A/B/C) / theme segmented / accent dots / layout toggles + density.
// Binds Theme.* for chrome colors; calls Appearance.* to mutate (which persists).
import QtQuick

Rectangle {
    id: panel
    width: 344
    color: Theme.window
    border.color: Theme.border
    border.width: 1

    readonly property var accentList: [
        { id: "default", name: "默认", hex: "" },
        { id: "teal",    name: "石墨青", hex: "#0e8c6f" },
        { id: "blue",    name: "海蓝",  hex: "#2f6fed" },
        { id: "violet",  name: "电光紫", hex: "#6c4be0" },
        { id: "ochre",   name: "暖赭",  hex: "#bd6234" },
        { id: "rose",    name: "玫瑰",  hex: "#d4488a" },
        { id: "green",   name: "森绿",  hex: "#3f9142" }
    ]
    readonly property var styleList: [
        { id: 0, name: "精炼经典", desc: "三栏 IDE",  sw: ["#f5f6f7", "#0e8c6f", "#16191c"] },
        { id: 1, name: "沉浸写作", desc: "衬线阅读", sw: ["#fbf8f1", "#bd6234", "#2e2a23"] },
        { id: 2, name: "工作台",   desc: "命令栏",   sw: ["#1a1c24", "#6c4be0", "#b6e04b"] }
    ]

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 22

        // Header
        Row {
            width: parent.width
            spacing: 9
            Text {
                text: "外观"
                color: Theme.text
                font.pixelSize: 16
                font.bold: true
                font.family: Theme.fontUi
            }
        }

        // STYLE
        Column {
            width: parent.width
            spacing: 11
            Text { text: "风格"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
            Row {
                width: parent.width
                spacing: 9
                Repeater {
                    model: panel.styleList
                    delegate: Rectangle {
                        required property var modelData
                        width: (panel.width - 40 - 18) / 3
                        height: 70
                        radius: 12
                        color: Theme.card
                        border.width: 1.5
                        border.color: Appearance.style === modelData.id ? Theme.accent : Theme.border
                        Column {
                            anchors.centerIn: parent
                            spacing: 6
                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 3
                                Repeater {
                                    model: modelData.sw
                                    delegate: Rectangle {
                                        required property string modelData
                                        width: 11; height: 18; radius: 3; color: modelData
                                        border.width: 1; border.color: "#40808080"
                                    }
                                }
                            }
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.name
                                font.pixelSize: 12; font.bold: true; font.family: Theme.fontUi
                                color: Appearance.style === modelData.id ? Theme.accent : Theme.text
                            }
                        }
                        MouseArea { anchors.fill: parent; onClicked: Appearance.style = modelData.id }
                    }
                }
            }
        }

        // THEME
        Column {
            width: parent.width
            spacing: 11
            Text { text: "主题"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
            Row {
                width: parent.width
                spacing: 3
                Repeater {
                    model: [ { id: 0, name: "浅色" }, { id: 1, name: "深色" }, { id: 2, name: "跟随系统" } ]
                    delegate: Rectangle {
                        required property var modelData
                        width: (panel.width - 40 - 6) / 3
                        height: 32
                        radius: 8
                        color: Appearance.theme === modelData.id ? Theme.accentSoft : Theme.hover
                        Text {
                            anchors.centerIn: parent
                            text: modelData.name
                            font.pixelSize: 13; font.family: Theme.fontUi
                            font.bold: Appearance.theme === modelData.id
                            color: Appearance.theme === modelData.id ? Theme.accent : Theme.dim
                        }
                        MouseArea { anchors.fill: parent; onClicked: Appearance.theme = modelData.id }
                    }
                }
            }
        }

        // ACCENT
        Column {
            width: parent.width
            spacing: 11
            Text { text: "强调色"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
            Row {
                width: parent.width
                spacing: 10
                Repeater {
                    model: panel.accentList
                    delegate: Rectangle {
                        required property var modelData
                        width: 28; height: 28; radius: 9
                        color: modelData.hex === "" ? Theme.accent : modelData.hex
                        border.width: Appearance.accent === modelData.id ? 2 : 0
                        border.color: Theme.text
                        Text {
                            anchors.centerIn: parent
                            visible: Appearance.accent === modelData.id
                            text: "✓"; color: "#ffffff"; font.pixelSize: 14; font.bold: true
                        }
                        MouseArea { anchors.fill: parent; onClicked: Appearance.accent = modelData.id }
                    }
                }
            }
        }

        // LAYOUT
        Column {
            width: parent.width
            spacing: 12
            Text { text: "布局"; color: Theme.faint; font.pixelSize: 11; font.bold: true; font.family: Theme.fontUi }
            Row {
                width: parent.width
                Text { text: "左侧栏"; color: Theme.text; font.pixelSize: 14; font.family: Theme.fontUi; width: parent.width - 44 }
                Rectangle {
                    width: 38; height: 22; radius: 11
                    color: Appearance.showLeft ? Theme.accent : Theme.border
                    Rectangle {
                        width: 18; height: 18; radius: 9; color: "#ffffff"; y: 2
                        x: Appearance.showLeft ? 18 : 2
                        Behavior on x { NumberAnimation { duration: 150 } }
                    }
                    MouseArea { anchors.fill: parent; onClicked: Appearance.showLeft = !Appearance.showLeft }
                }
            }
            Row {
                width: parent.width
                Text { text: "右侧面板"; color: Theme.text; font.pixelSize: 14; font.family: Theme.fontUi; width: parent.width - 44 }
                Rectangle {
                    width: 38; height: 22; radius: 11
                    color: Appearance.showRight ? Theme.accent : Theme.border
                    Rectangle {
                        width: 18; height: 18; radius: 9; color: "#ffffff"; y: 2
                        x: Appearance.showRight ? 18 : 2
                        Behavior on x { NumberAnimation { duration: 150 } }
                    }
                    MouseArea { anchors.fill: parent; onClicked: Appearance.showRight = !Appearance.showRight }
                }
            }
            Text { text: "正文密度"; color: Theme.text; font.pixelSize: 13; font.family: Theme.fontUi }
            Row {
                width: parent.width
                spacing: 3
                Repeater {
                    model: [ { id: "compact", name: "紧凑" }, { id: "normal", name: "适中" }, { id: "roomy", name: "宽松" } ]
                    delegate: Rectangle {
                        required property var modelData
                        width: (panel.width - 40 - 6) / 3
                        height: 30; radius: 8
                        color: Appearance.density === modelData.id ? Theme.accentSoft : Theme.hover
                        Text {
                            anchors.centerIn: parent
                            text: modelData.name
                            font.pixelSize: 13; font.family: Theme.fontUi
                            color: Appearance.density === modelData.id ? Theme.accent : Theme.dim
                        }
                        MouseArea { anchors.fill: parent; onClicked: Appearance.density = modelData.id }
                    }
                }
            }
        }

        // Reset
        Text {
            text: "恢复默认"
            color: Theme.dim
            font.pixelSize: 13
            font.family: Theme.fontUi
            MouseArea { anchors.fill: parent; onClicked: Appearance.resetDefaults() }
        }
    }
}
