// Read-only viewer for non-markdown files (#18): PDF (QWebEngine built-in PDF viewer)
// and HTML, loaded directly as file:// (page origin is the file itself, no cross-origin).
import QtQuick
import QtWebEngine

Item {
    id: root
    property string path: ""   // absolute file path
    property string kind: ""   // "pdf" / "html"

    Rectangle { anchors.fill: parent; color: (typeof Theme !== "undefined") ? Theme.canvas : "#ffffff" }

    WebEngineView {
        id: web
        anchors.fill: parent
        backgroundColor: (typeof Theme !== "undefined") ? Theme.canvas : "#ffffff"
        settings.pdfViewerEnabled: true
        settings.localContentCanAccessFileUrls: true
        url: root.path.length > 0 ? "file://" + root.path : "about:blank"
    }
}
