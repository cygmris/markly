#include <QtTest>

#include <core/desktop/fileassoc.h>
#include <core/update/updatechecker.h>

using namespace markly;

// GUILESS tests for the update version parse/compare (#20b). No real network.
class TestUpdate : public QObject {
  Q_OBJECT
private slots:
  void parseVersion();
  void compareVersions();
  void autoCheckInterval();
  void fileAssocCommands();
};

void TestUpdate::parseVersion() {
  QCOMPARE(UpdateChecker::parseLatestVersion(R"({"tag_name":"v1.2.0"})"),
           QStringLiteral("1.2.0"));
  QCOMPARE(UpdateChecker::parseLatestVersion(R"({"tag_name":"2.0.1"})"),
           QStringLiteral("2.0.1")); // no leading v
  QCOMPARE(UpdateChecker::parseLatestVersion(QByteArrayLiteral("{}")), QString());
  QCOMPARE(UpdateChecker::parseLatestVersion(QByteArrayLiteral("not json")), QString());
}

void TestUpdate::compareVersions() {
  QVERIFY(UpdateChecker::isNewer(QStringLiteral("1.2.0"), QStringLiteral("1.0.0")));
  QVERIFY(!UpdateChecker::isNewer(QStringLiteral("1.0.0"), QStringLiteral("1.0.0")));
  QVERIFY(!UpdateChecker::isNewer(QStringLiteral("1.0.0"), QStringLiteral("1.2.0")));
  // Numeric, not lexicographic: 1.10.0 > 1.9.0.
  QVERIFY(UpdateChecker::isNewer(QStringLiteral("1.10.0"), QStringLiteral("1.9.0")));
  // Empty latest (parse failure) is never newer.
  QVERIFY(!UpdateChecker::isNewer(QString(), QStringLiteral("1.0.0")));
}

void TestUpdate::autoCheckInterval() {
  const qint64 now = 1000LL * 86400000LL; // some large "now"
  const qint64 day = 86400000LL;
  QVERIFY(UpdateChecker::shouldCheck(0, now, 7));            // never checked
  QVERIFY(UpdateChecker::shouldCheck(now - 8 * day, now, 7)); // 8 days ago, interval 7
  QVERIFY(!UpdateChecker::shouldCheck(now - 1 * day, now, 7)); // 1 day ago
  QVERIFY(!UpdateChecker::shouldCheck(-1, now, 7));          // invalid last
}

void TestUpdate::fileAssocCommands() {
  const QStringList c = FileAssoc::registerCommands();
  QVERIFY(c.contains(QStringLiteral("xdg-mime")));
  QVERIFY(c.contains(QStringLiteral("markly.desktop")));
  QVERIFY(c.contains(QStringLiteral("text/markdown")));
}

QTEST_APPLESS_MAIN(TestUpdate)
#include "test_update.moc"
