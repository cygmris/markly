#include "stylesheetgenerator.h"

using namespace markly;

QString StyleSheetGenerator::cssColor(const QColor &p_color) {
  if (p_color.alpha() == 255) {
    return p_color.name(QColor::HexRgb);
  }
  return QStringLiteral("rgba(%1,%2,%3,%4)")
      .arg(p_color.red())
      .arg(p_color.green())
      .arg(p_color.blue())
      .arg(QString::number(p_color.alphaF(), 'f', 3));
}

QString StyleSheetGenerator::generate(const ThemeTokens &p_t) {
  const QString window = cssColor(p_t.window);
  const QString canvas = cssColor(p_t.canvas);
  const QString text = cssColor(p_t.text);
  const QString border = cssColor(p_t.borderStrong);
  const QString accent = cssColor(p_t.accent);
  const QString accentText = cssColor(p_t.accentText);
  const QString selection = cssColor(p_t.selection);
  const QString hover = cssColor(p_t.hover);
  const QString card = cssColor(p_t.card);
  const QString faint = cssColor(p_t.faint);

  return QStringLiteral(R"(
QWidget {
  background-color: %1;
  color: %2;
  font-family: %3;
}
QMainWindow, QDialog {
  background-color: %1;
}
QToolTip {
  background-color: %4;
  color: %2;
  border: 1px solid %5;
  padding: 4px 6px;
}
QMenu {
  background-color: %4;
  color: %2;
  border: 1px solid %5;
  padding: 4px;
}
QMenu::item {
  padding: 5px 22px;
  border-radius: 6px;
}
QMenu::item:selected {
  background-color: %6;
  color: %2;
}
QLineEdit, QPlainTextEdit, QTextEdit {
  background-color: %7;
  color: %2;
  border: 1px solid %5;
  border-radius: 6px;
  selection-background-color: %8;
  selection-color: %2;
}
QPushButton {
  background-color: %9;
  color: %10;
  border: none;
  border-radius: 7px;
  padding: 6px 14px;
}
QPushButton:hover {
  background-color: %9;
}
QPushButton:flat, QPushButton[flat="true"] {
  background-color: transparent;
  color: %2;
}
QLabel {
  background-color: transparent;
}
QScrollBar:vertical {
  background: transparent;
  width: 10px;
  margin: 0;
}
QScrollBar::handle:vertical {
  background: %11;
  border-radius: 5px;
  min-height: 24px;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
  height: 0;
}
QScrollBar:horizontal {
  background: transparent;
  height: 10px;
  margin: 0;
}
QScrollBar::handle:horizontal {
  background: %11;
  border-radius: 5px;
  min-width: 24px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
  width: 0;
}
)")
      .arg(window)       // %1
      .arg(text)         // %2
      .arg(p_t.fontUi)   // %3
      .arg(card)         // %4
      .arg(border)       // %5
      .arg(hover)        // %6
      .arg(canvas)       // %7
      .arg(selection)    // %8
      .arg(accent)       // %9
      .arg(accentText)   // %10
      .arg(faint);       // %11
}
