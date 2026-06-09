// Markdown rendered preview via QtWebEngine + markdown-it (preview.html).
import QtQuick
import QtWebEngine

Item {
    id: root
    property string content: ""
    property bool ready: false

    onContentChanged: debounce.restart()

    function hex(c) {
        function h(v) { var s = Math.round(v * 255).toString(16); return s.length < 2 ? "0" + s : s; }
        return "#" + h(c.r) + h(c.g) + h(c.b);
    }

    function pushContent() {
        if (root.ready)
            web.runJavaScript("mdRender(" + JSON.stringify(root.content) + ")");
    }
    function pushTheme() {
        if (!root.ready || typeof Theme === "undefined") return;
        var t = {
            bg: hex(Theme.canvas), text: hex(Theme.text), dim: hex(Theme.dim),
            heading: hex(Theme.heading), codeBg: hex(Theme.codeBg), codeInk: hex(Theme.codeInk),
            inlineCode: hex(Theme.codeInline), link: hex(Theme.link), quote: hex(Theme.dim),
            border: hex(Theme.border), accent: hex(Theme.accent),
            fontUi: Theme.fontUi, fontMono: Theme.fontMono, fontSerif: Theme.fontSerif,
            // Prism code-token colors, mapped from theme accent roles (#9b).
            tokKeyword: hex(Theme.pink), tokString: hex(Theme.sage), tokComment: hex(Theme.dim),
            tokNumber: hex(Theme.amber), tokFunc: hex(Theme.link), tokOperator: hex(Theme.text),
            tokTag: hex(Theme.accent)
        };
        web.runJavaScript("mdSetTheme(" + JSON.stringify(t) + ")");
    }

    Timer { id: debounce; interval: 150; onTriggered: root.pushContent() }

    WebEngineView {
        id: web
        anchors.fill: parent
        backgroundColor: (typeof Theme !== "undefined") ? Theme.canvas : "#ffffff"
        url: "qrc:/data/web/preview.html"
        onLoadingChanged: function(info) {
            if (info.status === WebEngineView.LoadSucceededStatus) {
                root.ready = true;
                root.pushTheme();
                root.pushContent();
            }
        }
    }

    Connections {
        target: (typeof Theme !== "undefined") ? Theme : null
        function onThemeChanged() { root.pushTheme(); }
    }
}
