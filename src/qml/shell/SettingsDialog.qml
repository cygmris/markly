// Searchable settings dialog (#19). Categories: 编辑器 / 外观 / 关于. Each setting binds
// to EditorCfg (writable, #19) or Appearance (writable, #2) with immediate effect.
import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent
    visible: false

    property string category: "editor"

    function show() { visible = true; searchField.text = ""; searchField.forceActiveFocus(); }
    function hide() { visible = false; }

    readonly property var accents: [
        { id: "default", hex: "" }, { id: "teal", hex: "#0e8c6f" }, { id: "blue", hex: "#2f6fed" },
        { id: "violet", hex: "#6c4be0" }, { id: "ochre", hex: "#bd6234" },
        { id: "rose", hex: "#d4488a" }, { id: "green", hex: "#3f9142" }
    ]

    // Setting items. get/set are closures; type drives the control. lang ties retranslate.
    // cat is a stable key (not translated); display label comes from catLabel().
    property string lang: (typeof Locale !== "undefined") ? Locale.language : "auto"
    readonly property var items: [
        { cat: "editor", label: qsTr("字号"), type: "stepper", min: 8, max: 32,
          get: function(){ return EditorCfg.fontSize }, set: function(v){ EditorCfg.fontSize = v } },
        { cat: "editor", label: qsTr("Tab 宽度"), type: "stepper", min: 1, max: 8,
          get: function(){ return EditorCfg.tabWidth }, set: function(v){ EditorCfg.tabWidth = v } },
        { cat: "editor", label: qsTr("空格展开 Tab"), type: "toggle",
          get: function(){ return EditorCfg.expandTab }, set: function(v){ EditorCfg.expandTab = v } },
        { cat: "editor", label: qsTr("显示行号"), type: "toggle",
          get: function(){ return EditorCfg.lineNumber }, set: function(v){ EditorCfg.lineNumber = v } },
        { cat: "editor", label: qsTr("高亮当前行"), type: "toggle",
          get: function(){ return EditorCfg.highlightCurrentLine }, set: function(v){ EditorCfg.highlightCurrentLine = v } },
        { cat: "editor", label: qsTr("自动缩进"), type: "toggle",
          get: function(){ return EditorCfg.autoIndent }, set: function(v){ EditorCfg.autoIndent = v } },
        { cat: "editor", label: qsTr("列表续行"), type: "toggle",
          get: function(){ return EditorCfg.continueList }, set: function(v){ EditorCfg.continueList = v } },
        { cat: "editor", label: qsTr("括号自动配对"), type: "toggle",
          get: function(){ return EditorCfg.autoPair }, set: function(v){ EditorCfg.autoPair = v } },
        { cat: "appearance", label: qsTr("界面风格"), type: "seg", options: [{v:0,t:"精炼"},{v:1,t:"沉浸"},{v:2,t:"工作台"}],
          get: function(){ return Appearance.style }, set: function(v){ Appearance.style = v } },
        { cat: "appearance", label: qsTr("主题"), type: "seg", options: [{v:0,t:"浅色"},{v:1,t:"深色"},{v:2,t:"跟随系统"}],
          get: function(){ return Appearance.theme }, set: function(v){ Appearance.theme = v } },
        { cat: "appearance", label: qsTr("强调色"), type: "swatch",
          get: function(){ return Appearance.accent }, set: function(v){ Appearance.accent = v } },
        { cat: "appearance", label: qsTr("语言"), type: "langseg",
          get: function(){ return root.lang }, set: function(v){ if (typeof Locale !== "undefined") Locale.setLanguage(v) } },
        { cat: "appearance", label: qsTr("显示左栏"), type: "toggle",
          get: function(){ return Appearance.showLeft }, set: function(v){ Appearance.showLeft = v } },
        { cat: "appearance", label: qsTr("显示右栏"), type: "toggle",
          get: function(){ return Appearance.showRight }, set: function(v){ Appearance.showRight = v } },
        { cat: "about", label: "Markly", type: "about", text: "Qt 6.8 · QML 外壳 · QWebEngine 预览 · SQLite FTS5 · v0.1" }
    ]
    // Category key -> display label (retranslated via lang).
    readonly property var categories: [
        { key: "editor", label: qsTr("编辑器") },
        { key: "appearance", label: qsTr("外观") },
        { key: "about", label: qsTr("关于") }
    ]

    function visibleItems() {
        var kw = searchField.text.trim().toLowerCase();
        var out = [];
        for (var i = 0; i < items.length; ++i) {
            var it = items[i];
            if (kw.length > 0) { if (it.label.toLowerCase().indexOf(kw) !== -1) out.push(it); }
            else if (it.cat === root.category) out.push(it);
        }
        return out;
    }

    // Dim background.
    Rectangle {
        anchors.fill: parent; color: "#80000000"
        MouseArea { anchors.fill: parent; onClicked: root.hide() }
    }

    // Card.
    Rectangle {
        anchors.centerIn: parent
        width: 640; height: 480; radius: 14
        color: Theme.window; border.color: Theme.border; border.width: 1
        MouseArea { anchors.fill: parent } // swallow clicks (don't close)

        Column {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 14

            // Header
            Item {
                width: parent.width; height: 32
                Text { anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                       text: qsTr("设置"); color: Theme.heading; font.pixelSize: 18; font.bold: true; font.family: Theme.fontUi }
                Rectangle {
                    anchors.centerIn: parent; width: 240; height: 30; radius: 8; color: Theme.card
                    border.color: searchField.activeFocus ? Theme.accent : Theme.border; border.width: 1
                    TextField {
                        id: searchField
                        anchors.fill: parent; anchors.leftMargin: 8; anchors.rightMargin: 8
                        verticalAlignment: TextInput.AlignVCenter
                        placeholderText: qsTr("搜索设置…"); color: Theme.text; placeholderTextColor: Theme.faint
                        font.pixelSize: 13; font.family: Theme.fontUi; background: Item {}
                    }
                }
                Rectangle {
                    anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                    width: 30; height: 30; radius: 8; color: closeHover.hovered ? Theme.hover : "transparent"
                    Text { anchors.centerIn: parent; text: "✕"; color: Theme.dim; font.pixelSize: 14 }
                    HoverHandler { id: closeHover }
                    MouseArea { anchors.fill: parent; onClicked: root.hide() }
                }
            }

            Row {
                width: parent.width
                height: parent.height - 46
                spacing: 14

                // Categories
                Column {
                    width: 120
                    spacing: 4
                    visible: searchField.text.trim().length === 0
                    Repeater {
                        model: root.categories
                        Rectangle {
                            width: parent.width; height: 34; radius: 8
                            property bool sel: root.category === modelData.key
                            color: sel ? Theme.accentSoft : (catHover.hovered ? Theme.hover : "transparent")
                            Text { anchors.left: parent.left; anchors.leftMargin: 12; anchors.verticalCenter: parent.verticalCenter
                                   text: modelData.label; color: parent.sel ? Theme.accent : Theme.text
                                   font.pixelSize: 13; font.family: Theme.fontUi; font.bold: parent.sel }
                            HoverHandler { id: catHover }
                            MouseArea { anchors.fill: parent; onClicked: root.category = modelData.key }
                        }
                    }
                }

                // Settings list
                ListView {
                    id: settingsList
                    width: parent.width - (searchField.text.trim().length === 0 ? 134 : 0)
                    height: parent.height
                    clip: true
                    spacing: 6
                    model: root.visibleItems()
                    delegate: Item {
                        width: settingsList.width
                        height: modelData.type === "about" ? 60 : 40
                        Text {
                            anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                            width: 150; text: modelData.label; color: Theme.text
                            font.pixelSize: 13; font.family: Theme.fontUi
                            visible: modelData.type !== "about"
                        }
                        // Control area on the right.
                        Loader {
                            anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                            sourceComponent: modelData.type === "toggle" ? toggleC
                                           : modelData.type === "stepper" ? stepperC
                                           : modelData.type === "seg" ? segC
                                           : modelData.type === "langseg" ? langsegC
                                           : modelData.type === "swatch" ? swatchC : aboutC
                            property var item: modelData
                        }
                    }
                }
            }
        }
    }

    // ---- control components ----
    Component {
        id: toggleC
        Rectangle {
            width: 44; height: 24; radius: 12
            property bool on: item.get()
            color: on ? Theme.accent : Theme.border
            Rectangle { width: 18; height: 18; radius: 9; color: "#ffffff"; y: 3
                        x: parent.on ? parent.width - width - 3 : 3
                        Behavior on x { NumberAnimation { duration: 120 } } }
            MouseArea { anchors.fill: parent; onClicked: { item.set(!parent.on); parent.on = item.get() } }
        }
    }
    Component {
        id: stepperC
        Row {
            spacing: 6
            property int val: item.get()
            Rectangle { width: 26; height: 26; radius: 7; color: Theme.card; border.color: Theme.border; border.width: 1
                Text { anchors.centerIn: parent; text: "−"; color: Theme.text; font.pixelSize: 15 }
                MouseArea { anchors.fill: parent; onClicked: { if (parent.parent.val > item.min) { item.set(parent.parent.val - 1); parent.parent.val = item.get() } } } }
            Text { width: 28; horizontalAlignment: Text.AlignHCenter; anchors.verticalCenter: parent.verticalCenter
                   text: parent.val; color: Theme.text; font.pixelSize: 13; font.family: Theme.fontMono }
            Rectangle { width: 26; height: 26; radius: 7; color: Theme.card; border.color: Theme.border; border.width: 1
                Text { anchors.centerIn: parent; text: "+"; color: Theme.text; font.pixelSize: 15 }
                MouseArea { anchors.fill: parent; onClicked: { if (parent.parent.val < item.max) { item.set(parent.parent.val + 1); parent.parent.val = item.get() } } } }
        }
    }
    Component {
        id: segC
        Row {
            spacing: 4
            property int val: item.get()
            Repeater {
                model: item.options
                Rectangle {
                    width: 60; height: 26; radius: 7
                    property bool sel: parent.val === modelData.v
                    color: sel ? Theme.accent : Theme.card
                    border.color: sel ? Theme.accent : Theme.border; border.width: 1
                    Text { anchors.centerIn: parent; text: modelData.t; color: parent.sel ? Theme.accentText : Theme.dim
                           font.pixelSize: 11; font.family: Theme.fontUi }
                    MouseArea { anchors.fill: parent; onClicked: { item.set(modelData.v); parent.parent.val = item.get() } }
                }
            }
        }
    }
    // String-valued segmented control for language (#20).
    Component {
        id: langsegC
        Row {
            spacing: 4
            property string val: item.get()
            Repeater {
                model: [{ v: "auto", t: qsTr("跟随系统") }, { v: "zh_CN", t: "中文" }, { v: "en_US", t: "English" }]
                Rectangle {
                    width: 66; height: 26; radius: 7
                    property bool sel: parent.val === modelData.v
                    color: sel ? Theme.accent : Theme.card
                    border.color: sel ? Theme.accent : Theme.border; border.width: 1
                    Text { anchors.centerIn: parent; text: modelData.t; color: parent.sel ? Theme.accentText : Theme.dim
                           font.pixelSize: 11; font.family: Theme.fontUi }
                    MouseArea { anchors.fill: parent; onClicked: { item.set(modelData.v); parent.parent.val = item.get() } }
                }
            }
        }
    }
    Component {
        id: swatchC
        Row {
            spacing: 6
            property string val: item.get()
            Repeater {
                model: root.accents
                Rectangle {
                    width: 22; height: 22; radius: 11
                    color: modelData.hex === "" ? Theme.accent : modelData.hex
                    border.width: parent.val === modelData.id ? 2 : 0; border.color: Theme.text
                    MouseArea { anchors.fill: parent; onClicked: { item.set(modelData.id); parent.parent.val = item.get() } }
                }
            }
        }
    }
    Component {
        id: aboutC
        Text { width: 460; text: item.text; color: Theme.dim; font.pixelSize: 12; font.family: Theme.fontUi; wrapMode: Text.Wrap }
    }
}
