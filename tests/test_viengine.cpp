#include <QtTest>

#include <core/editor/viengine.h>

using namespace markly;

// GUILESS unit tests for the Vi state machine (#8b). Each test drives a key sequence
// and asserts the resulting text / cursor / mode, applying edits as the editor would.
class TestViEngine : public QObject {
  Q_OBJECT

  // Apply one key; mutate text/cursor; return the result.
  ViResult feed(ViEngine &e, QString &text, int &cursor, int &selStart, int &selEnd,
                const QString &keyText, int qkey = 0, int mods = 0) {
    ViResult r = e.handleKey(text, cursor, selStart, selEnd, keyText, qkey, mods);
    if (!r.handled) return r;
    // Apply edits in descending start order so earlier offsets stay valid.
    QVector<ViEdit> edits = r.edits;
    std::sort(edits.begin(), edits.end(),
              [](const ViEdit &a, const ViEdit &b) { return a.start > b.start; });
    for (const ViEdit &ed : edits) {
      text.replace(ed.start, ed.end - ed.start, ed.text);
    }
    cursor = r.cursor;
    if (r.anchor >= 0) { selStart = qMin(r.anchor, cursor); selEnd = qMax(r.anchor, cursor); }
    else { selStart = selEnd = cursor; }
    return r;
  }

  // Feed a string of single-char keys (printable). Returns last result.
  ViResult type(ViEngine &e, QString &text, int &cursor, int &selStart, int &selEnd,
                const QString &keys) {
    ViResult r;
    for (const QChar &c : keys) {
      r = feed(e, text, cursor, selStart, selEnd, QString(c));
    }
    return r;
  }

private slots:
  void motions();
  void countMotions();
  void deleteAndChange();
  void yankPaste();
  void modesAndInsert();
  void visualDelete();
  void wordEndMotion();
  void findInLine();
  void replaceChar();
  void toggleCase();
  void indentLines();
  void wordCapMotions();
  void substitute();
  void matchBracket();
  void repeatChange();
  void macroRecordReplay();
};

void TestViEngine::motions() {
  ViEngine e;
  QString t = QStringLiteral("hello\nworld");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("l")); QCOMPARE(c, 1);
  type(e, t, c, s, ee, QStringLiteral("$")); QCOMPARE(c, 5);   // end of "hello"
  type(e, t, c, s, ee, QStringLiteral("0")); QCOMPARE(c, 0);
  type(e, t, c, s, ee, QStringLiteral("w")); QCOMPARE(c, 6);   // start of "world"
  type(e, t, c, s, ee, QStringLiteral("b")); QCOMPARE(c, 0);   // back to "hello"
  type(e, t, c, s, ee, QStringLiteral("G")); QCOMPARE(c, 6);   // start of last line
  type(e, t, c, s, ee, QStringLiteral("gg")); QCOMPARE(c, 0);  // first line
}

void TestViEngine::countMotions() {
  ViEngine e;
  QString t = QStringLiteral("abcdef");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("3l")); QCOMPARE(c, 3);
  // 3x deletes 3 chars from cursor.
  type(e, t, c, s, ee, QStringLiteral("3x"));
  QCOMPARE(t, QStringLiteral("abc"));
}

void TestViEngine::deleteAndChange() {
  ViEngine e;
  QString t = QStringLiteral("one two\nthree");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("dw"));             // delete "one "
  QCOMPARE(t, QStringLiteral("two\nthree"));
  QCOMPARE(c, 0);
  type(e, t, c, s, ee, QStringLiteral("D"));              // delete to end of line
  QCOMPARE(t, QStringLiteral("\nthree"));

  QString t2 = QStringLiteral("aaa\nbbb\nccc");
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t2, c2, s2, e2, QStringLiteral("dd"));          // delete first line
  QCOMPARE(t2, QStringLiteral("bbb\nccc"));
  type(e, t2, c2, s2, e2, QStringLiteral("2dd"));         // delete remaining 2 lines
  QCOMPARE(t2, QString());
}

void TestViEngine::yankPaste() {
  ViEngine e;
  QString t = QStringLiteral("line1\nline2");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("yy"));             // yank "line1" (linewise)
  type(e, t, c, s, ee, QStringLiteral("p"));              // paste below line 1
  QCOMPARE(t, QStringLiteral("line1\nline1\nline2"));
}

void TestViEngine::modesAndInsert() {
  ViEngine e;
  QString t = QStringLiteral("abc");
  int c = 1, s = 1, ee = 1;
  QCOMPARE(static_cast<int>(e.mode()), 0);
  type(e, t, c, s, ee, QStringLiteral("i"));
  QCOMPARE(static_cast<int>(e.mode()), 1);               // Insert
  // Esc back to Normal moves left within the line.
  ViResult r = e.handleKey(t, c, s, ee, QString(), Qt::Key_Escape, 0);
  QVERIFY(r.handled);
  QCOMPARE(static_cast<int>(r.mode), 0);
}

void TestViEngine::visualDelete() {
  ViEngine e;
  QString t = QStringLiteral("abcdef");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("v"));             // enter Visual, anchor 0
  QCOMPARE(static_cast<int>(e.mode()), 2);
  type(e, t, c, s, ee, QStringLiteral("lll"));           // extend to pos 3
  type(e, t, c, s, ee, QStringLiteral("d"));             // delete [0,3)
  QCOMPARE(t, QStringLiteral("def"));
  QCOMPARE(static_cast<int>(e.mode()), 0);               // back to Normal
}

void TestViEngine::wordEndMotion() {
  ViEngine e;
  QString t = QStringLiteral("foo bar");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("e")); QCOMPARE(c, 2);   // end of "foo"
  type(e, t, c, s, ee, QStringLiteral("e")); QCOMPARE(c, 6);   // end of "bar"
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t, c2, s2, e2, QStringLiteral("2e")); QCOMPARE(c2, 6);
}

void TestViEngine::findInLine() {
  ViEngine e;
  QString t = QStringLiteral("a.b.c");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("f.")); QCOMPARE(c, 1);  // first '.'
  type(e, t, c, s, ee, QStringLiteral(";"));  QCOMPARE(c, 3);  // repeat -> second '.'
  type(e, t, c, s, ee, QStringLiteral(","));  QCOMPARE(c, 1);  // reverse -> first '.'
  // t. from 0 stops one before the '.'
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t, c2, s2, e2, QStringLiteral("t.")); QCOMPARE(c2, 0);
}

void TestViEngine::replaceChar() {
  ViEngine e;
  QString t = QStringLiteral("abc");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("rX"));
  QCOMPARE(t, QStringLiteral("Xbc"));
  QCOMPARE(c, 0);
  QString t2 = QStringLiteral("abc");
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t2, c2, s2, e2, QStringLiteral("2rY"));
  QCOMPARE(t2, QStringLiteral("YYc"));
  QCOMPARE(c2, 1);
}

void TestViEngine::toggleCase() {
  ViEngine e;
  QString t = QStringLiteral("abc");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("~")); QCOMPARE(t, QStringLiteral("Abc")); QCOMPARE(c, 1);
  QString t2 = QStringLiteral("abc");
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t2, c2, s2, e2, QStringLiteral("3~")); QCOMPARE(t2, QStringLiteral("ABC")); QCOMPARE(c2, 3);
}

void TestViEngine::indentLines() {
  ViEngine e;
  QString t = QStringLiteral("a\nb");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral(">>")); QCOMPARE(t, QStringLiteral("\ta\nb"));
  type(e, t, c, s, ee, QStringLiteral("<<")); QCOMPARE(t, QStringLiteral("a\nb"));
  // 2>> indents both lines.
  QString t2 = QStringLiteral("a\nb");
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t2, c2, s2, e2, QStringLiteral("2>>")); QCOMPARE(t2, QStringLiteral("\ta\n\tb"));
}

void TestViEngine::wordCapMotions() {
  ViEngine e;
  QString t = QStringLiteral("a.b cd"); // "a.b" is one WORD, "cd" the next
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("W")); QCOMPARE(c, 4);   // start of "cd"
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t, c2, s2, e2, QStringLiteral("E")); QCOMPARE(c2, 2); // end of "a.b"
  int c3 = 4, s3 = 4, e3 = 4;
  type(e, t, c3, s3, e3, QStringLiteral("B")); QCOMPARE(c3, 0); // back to "a.b"
}

void TestViEngine::substitute() {
  ViEngine e;
  QString t = QStringLiteral("abc");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("s"));
  QCOMPARE(t, QStringLiteral("bc"));
  QCOMPARE(static_cast<int>(e.mode()), 1); // Insert
  // S clears the line content and enters insert.
  ViEngine e2;
  QString t2 = QStringLiteral("ab\ncd");
  int c2 = 0, s2 = 0, e2v = 0;
  type(e2, t2, c2, s2, e2v, QStringLiteral("S"));
  QCOMPARE(t2, QStringLiteral("\ncd"));
  QCOMPARE(static_cast<int>(e2.mode()), 1);
}

void TestViEngine::matchBracket() {
  ViEngine e;
  QString t = QStringLiteral("(ab)");
  int c = 0, s = 0, ee = 0;
  type(e, t, c, s, ee, QStringLiteral("%")); QCOMPARE(c, 3);   // ( -> )
  type(e, t, c, s, ee, QStringLiteral("%")); QCOMPARE(c, 0);   // ) -> (
  QString t2 = QStringLiteral("a(b)c");
  int c2 = 0, s2 = 0, e2 = 0;
  type(e, t2, c2, s2, e2, QStringLiteral("%")); QCOMPARE(c2, 3); // first bracket ( @1 -> ) @3
}

void TestViEngine::repeatChange() {
  { // x then .
    ViEngine e; QString t = QStringLiteral("abcdef"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("x")); QCOMPARE(t, QStringLiteral("bcdef"));
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("cdef"));
  }
  { // dd then .
    ViEngine e; QString t = QStringLiteral("a\nb\nc"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("dd")); QCOMPARE(t, QStringLiteral("b\nc"));
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("c"));
  }
  { // ~ then .
    ViEngine e; QString t = QStringLiteral("abc"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("~")); QCOMPARE(t, QStringLiteral("Abc")); QCOMPARE(c, 1);
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("ABc")); QCOMPARE(c, 2);
  }
  { // rX, move, then . repeats the replace at the new cursor
    ViEngine e; QString t = QStringLiteral("abc"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("rX")); QCOMPARE(t, QStringLiteral("Xbc")); QCOMPARE(c, 0);
    type(e, t, c, s, ee, QStringLiteral("l")); QCOMPARE(c, 1);
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("XXc"));
  }
  { // 2x then . repeats with the recorded count
    ViEngine e; QString t = QStringLiteral("abcdef"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("2x")); QCOMPARE(t, QStringLiteral("cdef"));
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("ef"));
  }
  { // . with no recorded change is a no-op
    ViEngine e; QString t = QStringLiteral("abc"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral(".")); QCOMPARE(t, QStringLiteral("abc"));
  }
}

void TestViEngine::macroRecordReplay() {
  { // qa x x q records [x,x] (and executes live), then @a repeats it
    ViEngine e; QString t = QStringLiteral("abcdef"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("qa")); // start recording into 'a'
    type(e, t, c, s, ee, QStringLiteral("xx")); // delete 'a','b' live -> "cdef"
    type(e, t, c, s, ee, QStringLiteral("q"));  // stop
    QCOMPARE(t, QStringLiteral("cdef"));
    type(e, t, c, s, ee, QStringLiteral("@a")); QCOMPARE(t, QStringLiteral("ef"));
    type(e, t, c, s, ee, QStringLiteral("@a")); QCOMPARE(t, QStringLiteral(""));
  }
  { // multi-command macro: 0 x j (line-start, delete char, down) applied per line
    ViEngine e; QString t = QStringLiteral("x1\nx2\nx3"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("qa")); // record into 'a'
    type(e, t, c, s, ee, QStringLiteral("0xj")); // line1: "1", cursor on line 2
    type(e, t, c, s, ee, QStringLiteral("q"));
    QCOMPARE(t, QStringLiteral("1\nx2\nx3"));
    type(e, t, c, s, ee, QStringLiteral("@a")); QCOMPARE(t, QStringLiteral("1\n2\nx3"));
    type(e, t, c, s, ee, QStringLiteral("@a")); QCOMPARE(t, QStringLiteral("1\n2\n3"));
  }
  { // empty register @z is a no-op
    ViEngine e; QString t = QStringLiteral("abc"); int c = 0, s = 0, ee = 0;
    type(e, t, c, s, ee, QStringLiteral("@z")); QCOMPARE(t, QStringLiteral("abc"));
  }
}

QTEST_APPLESS_MAIN(TestViEngine)
#include "test_viengine.moc"
