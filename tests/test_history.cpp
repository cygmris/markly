#include <QFile>
#include <QtTest>

#include <core/configmgr.h>
#include <core/historymgr.h>

using namespace markly;

class TestHistory : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testMruAndDedup() {
    HistoryMgr mgr;
    mgr.addToHistory(QStringLiteral("/a.md"));
    mgr.addToHistory(QStringLiteral("/b.md"));
    mgr.addToHistory(QStringLiteral("/c.md"));
    QCOMPARE(mgr.history(), QStringList({"/c.md", "/b.md", "/a.md"}));
    // Re-adding moves to front, deduped.
    mgr.addToHistory(QStringLiteral("/a.md"));
    QCOMPARE(mgr.history(), QStringList({"/a.md", "/c.md", "/b.md"}));
  }

  void testCap() {
    HistoryMgr mgr;
    for (int i = 0; i < HistoryMgr::c_maxHistory + 10; ++i) {
      mgr.addToHistory(QStringLiteral("/n%1.md").arg(i));
    }
    QCOMPARE(mgr.history().size(), HistoryMgr::c_maxHistory);
    // Most recent is at the front.
    QCOMPARE(mgr.history().first(),
             QStringLiteral("/n%1.md").arg(HistoryMgr::c_maxHistory + 9));
  }

  void testPinUnpin() {
    HistoryMgr mgr;
    QVERIFY(!mgr.isPinned(QStringLiteral("/x.md")));
    mgr.pin(QStringLiteral("/x.md"));
    mgr.pin(QStringLiteral("/y.md"));
    mgr.pin(QStringLiteral("/x.md")); // dedup
    QVERIFY(mgr.isPinned(QStringLiteral("/x.md")));
    QCOMPARE(mgr.quickAccess().size(), 2);
    mgr.unpin(QStringLiteral("/x.md"));
    QVERIFY(!mgr.isPinned(QStringLiteral("/x.md")));
    QCOMPARE(mgr.quickAccess().size(), 1);
  }

  void testPersistRoundTrip() {
    {
      HistoryMgr mgr;
      mgr.addToHistory(QStringLiteral("/p.md"));
      mgr.pin(QStringLiteral("/q.md"));
    }
    // A fresh manager loads the same data from the session config.
    HistoryMgr mgr2;
    mgr2.load();
    QVERIFY(mgr2.history().contains(QStringLiteral("/p.md")));
    QVERIFY(mgr2.isPinned(QStringLiteral("/q.md")));
  }

  void testFlashFile() {
    HistoryMgr mgr;
    const auto path = mgr.ensureFlashFile();
    QVERIFY(path.endsWith(QStringLiteral("flash.md")));
    QVERIFY(QFile::exists(path));
  }
};

QTEST_GUILESS_MAIN(TestHistory)
#include "test_history.moc"
