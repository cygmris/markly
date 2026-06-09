#include <QtTest>

#include <core/configmgr.h>
#include <core/snippet/snippetmgr.h>

using namespace markly;

class TestSnippet : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testMagicWords() {
    const auto out = SnippetMgr::expandMagicWords(
        QStringLiteral("note=%note% unknown=%foo% date=%date%"), QStringLiteral("MyNote"));
    QVERIFY(out.contains(QStringLiteral("note=MyNote")));
    QVERIFY(out.contains(QStringLiteral("unknown=%foo%"))); // unknown preserved
    // date looks like YYYY-MM-DD
    const int di = out.indexOf(QStringLiteral("date=")) + 5;
    const auto date = out.mid(di, 10);
    QCOMPARE(date.count(QLatin1Char('-')), 2);
    QCOMPARE(date.size(), 10);
  }

  void testApplyMarks() {
    SnippetMgr mgr;
    mgr.add(QStringLiteral("t"), QStringLiteral("pre$$mid@@post"));
    int offset = -1;
    const auto text = mgr.apply(QStringLiteral("t"), QStringLiteral("X"), QString(), offset);
    QCOMPARE(text, QStringLiteral("preXmidpost"));
    QCOMPARE(offset, QStringLiteral("preXmid").size()); // cursor where @@ was
  }

  void testApplyNoCursorMark() {
    SnippetMgr mgr;
    mgr.add(QStringLiteral("t2"), QStringLiteral("hello"));
    int offset = -1;
    const auto text = mgr.apply(QStringLiteral("t2"), QString(), QString(), offset);
    QCOMPARE(text, QStringLiteral("hello"));
    QCOMPARE(offset, 5); // end of text
  }

  void testCrudRoundTrip() {
    SnippetMgr mgr;
    mgr.add(QStringLiteral("foo"), QStringLiteral("bar"));
    QCOMPARE(mgr.get(QStringLiteral("foo")).m_content, QStringLiteral("bar"));
    // A fresh manager loads it from disk.
    SnippetMgr mgr2;
    mgr2.load();
    QCOMPARE(mgr2.get(QStringLiteral("foo")).m_content, QStringLiteral("bar"));
    mgr2.remove(QStringLiteral("foo"));
    QVERIFY(!mgr2.get(QStringLiteral("foo")).isValid());
  }
};

QTEST_GUILESS_MAIN(TestSnippet)
#include "test_snippet.moc"
