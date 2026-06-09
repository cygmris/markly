#include <QSignalSpy>
#include <QtTest>

#include <core/singleinstanceguard.h>

using namespace markly;

class TestSingleInstanceGuard : public QObject {
  Q_OBJECT
private slots:
  void testPrimaryAndSecondaryIpc() {
    SingleInstanceGuard primary;
    if (!primary.tryRun()) {
      QSKIP("Another Markly instance appears to be running; skipping IPC test.");
    }

    QSignalSpy showSpy(&primary, &SingleInstanceGuard::showRequested);
    QSignalSpy openSpy(&primary, &SingleInstanceGuard::openFilesRequested);

    SingleInstanceGuard secondary;
    // Secondary must detect the primary and refuse to run.
    QCOMPARE(secondary.tryRun(), false);

    secondary.requestOpenFiles(QStringList() << "x.md" << "y.md");
    secondary.requestShow();

    QVERIFY(openSpy.wait(3000) || openSpy.count() > 0);
    QVERIFY(showSpy.count() > 0 || showSpy.wait(3000));

    QCOMPARE(openSpy.count(), 1);
    const auto files = openSpy.at(0).at(0).toStringList();
    QCOMPARE(files.size(), 2);
    QCOMPARE(files.at(0), QStringLiteral("x.md"));
    QCOMPARE(files.at(1), QStringLiteral("y.md"));

    QCOMPARE(showSpy.count(), 1);
  }
};

QTEST_GUILESS_MAIN(TestSingleInstanceGuard)
#include "test_singleinstanceguard.moc"
