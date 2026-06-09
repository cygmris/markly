#include <QtTest>

#include <core/hotkey/hotkeyparser.h>

using namespace markly;

// GUILESS tests for hotkey-string parsing (#17b). The X11 grab itself cannot run on a
// Wayland session and is not exercised here.
class TestHotkey : public QObject {
  Q_OBJECT
private slots:
  void parseCtrlAltM();
  void parseMetaShiftP();
  void parseInvalid();
};

void TestHotkey::parseCtrlAltM() {
  const ParsedHotkey h = HotkeyParser::parse(QStringLiteral("Ctrl+Alt+M"));
  QVERIFY(h.valid);
  QVERIFY(h.modifiers & Qt::ControlModifier);
  QVERIFY(h.modifiers & Qt::AltModifier);
  QCOMPARE(h.key, static_cast<int>(Qt::Key_M));
}

void TestHotkey::parseMetaShiftP() {
  const ParsedHotkey h = HotkeyParser::parse(QStringLiteral("Meta+Shift+P"));
  QVERIFY(h.valid);
  QVERIFY(h.modifiers & Qt::MetaModifier);
  QVERIFY(h.modifiers & Qt::ShiftModifier);
  QCOMPARE(h.key, static_cast<int>(Qt::Key_P));
}

void TestHotkey::parseInvalid() {
  QVERIFY(!HotkeyParser::parse(QString()).valid);
  QVERIFY(!HotkeyParser::parse(QStringLiteral("   ")).valid);
}

QTEST_APPLESS_MAIN(TestHotkey)
#include "test_hotkey.moc"
