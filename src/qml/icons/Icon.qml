// Single-color stroke icon. Renders the joined 24x24 SVG paths via Qt Quick
// Shapes, scaled to `size`. Color binds to a Theme role by default.
import QtQuick
import QtQuick.Shapes
import "IconPaths.js" as IconPaths

Item {
    id: root
    property string name: ""
    property int size: 18
    property color color: Theme.dim
    property real strokeWidth: 1.75

    implicitWidth: size
    implicitHeight: size
    width: size
    height: size

    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        // Scale the 24-unit path space down to `size`.
        transform: Scale { xScale: root.size / 24; yScale: root.size / 24 }
        ShapePath {
            strokeColor: root.color
            fillColor: "transparent"
            strokeWidth: root.strokeWidth
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            PathSvg { path: IconPaths.joined(root.name) }
        }
    }
}
