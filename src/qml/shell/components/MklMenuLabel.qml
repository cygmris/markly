import QtQuick
Text {
    property string label: ""
    text: label.toUpperCase()
    leftPadding: 9; topPadding: 7; bottomPadding: 4
    color: Theme.faint
    font.pixelSize: 10; font.bold: true; font.letterSpacing: 0.6; font.family: Theme.fontUi
}
