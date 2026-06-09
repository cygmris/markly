// Hidden WebEngine used to render a note for HTML / PDF export (#15). Reuses the
// preview.html pipeline (markdown-it + theme CSS + Prism) and #10 image data URIs.
import QtQuick
import QtWebEngine

Item {
    id: root
    width: 794; height: 1123 // A4 at ~96 DPI, so PDF layout is sensible
    visible: false

    property bool ready: false
    property var _pending: null // queued export until ready

    function hex(c) {
        function h(v) { var s = Math.round(v * 255).toString(16); return s.length < 2 ? "0" + s : s; }
        return "#" + h(c.r) + h(c.g) + h(c.b);
    }

    function resolveContent(content, baseDir) {
        if (typeof Images === "undefined" || !baseDir || baseDir.length === 0) return content;
        return content.replace(/(!\[[^\]]*\]\()([^)\s]+)/g, function(m, pre, path) {
            var uri = Images.toDataUri(path, baseDir);
            return uri.length > 0 ? (pre + uri) : m;
        });
    }

    function pushTheme() {
        if (typeof Theme === "undefined") return;
        var t = {
            bg: hex(Theme.canvas), text: hex(Theme.text), dim: hex(Theme.dim),
            heading: hex(Theme.heading), codeBg: hex(Theme.codeBg), codeInk: hex(Theme.codeInk),
            inlineCode: hex(Theme.codeInline), link: hex(Theme.link), quote: hex(Theme.dim),
            border: hex(Theme.border), accent: hex(Theme.accent),
            fontUi: Theme.fontUi, fontMono: Theme.fontMono, fontSerif: Theme.fontSerif,
            tokKeyword: hex(Theme.pink), tokString: hex(Theme.sage), tokComment: hex(Theme.dim),
            tokNumber: hex(Theme.amber), tokFunc: hex(Theme.link), tokOperator: hex(Theme.text),
            tokTag: hex(Theme.accent)
        };
        web.runJavaScript("mdSetTheme(" + JSON.stringify(t) + ")");
    }

    // op: { fmt, content, baseDir, path, cb }
    function _run(op) {
        pushTheme();
        var resolved = resolveContent(op.content, op.baseDir);
        web.runJavaScript("mdRender(" + JSON.stringify(resolved) + ")", function() {
            // Wait for async Mermaid diagrams (#9d) to finish before reading output.
            _waitGraphs(0, function() { _emit(op); });
        });
    }

    // Poll mermaidPending() until 0 (or ~3.2s timeout) so exported output captures SVGs.
    function _waitGraphs(tries, done) {
        web.runJavaScript("(window.mermaidPending ? window.mermaidPending() : 0)", function(n) {
            if (n > 0 && tries < 40) {
                root._graphRetry = function() { _waitGraphs(tries + 1, done); };
                graphTimer.restart();
            } else {
                done();
            }
        });
    }

    function _emit(op) {
        if (op.fmt === "pdf") {
            root._pdfCb = op.cb;
            web.printToPdf(op.path);
        } else { // html — make self-contained: inline CSS, drop now-unneeded scripts
            web.runJavaScript("document.documentElement.outerHTML", function(html) {
                var css = Export.readResource(":/data/web/preview.css");
                html = html.replace(/<link[^>]*preview\.css[^>]*>/i,
                                    "<style>\n" + css + "\n</style>");
                html = html.replace(/<script[\s\S]*?<\/script>/gi, "");
                var ok = Export.writeText(op.path, "<!DOCTYPE html>\n" + html);
                if (op.cb) op.cb(ok);
            });
        }
    }

    property var _pdfCb: null
    property var _graphRetry: null

    Timer {
        id: graphTimer
        interval: 80; repeat: false
        onTriggered: { if (root._graphRetry) { var f = root._graphRetry; root._graphRetry = null; f(); } }
    }

    function exportTo(fmt, content, baseDir, path, cb) {
        var op = { fmt: fmt, content: content, baseDir: baseDir, path: path, cb: cb };
        if (root.ready) _run(op); else root._pending = op;
    }

    WebEngineView {
        id: web
        anchors.fill: parent
        url: "qrc:/data/web/preview.html"
        onLoadingChanged: function(info) {
            if (info.status === WebEngineView.LoadSucceededStatus) {
                root.ready = true;
                if (root._pending) { var op = root._pending; root._pending = null; root._run(op); }
            }
        }
        onPdfPrintingFinished: function(filePath, success) {
            if (root._pdfCb) { root._pdfCb(success); root._pdfCb = null; }
        }
    }
}
