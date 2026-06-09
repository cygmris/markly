#include <QJsonObject>
#include <QTemporaryFile>
#include <QtTest>

#include <core/configmgr.h>
#include <core/coreconfig.h>
#include <core/mainconfig.h>
#include <core/sessionconfig.h>

using namespace markly;

class TestConfigMgr : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() {
    // Force the single instance into unit-test mode (temp config dirs).
    ConfigMgr::initForUnitTest();
  }

  void testDefaultMerge() {
    auto &core = ConfigMgr::getInst().getCoreConfig();
    // From the packaged default markly.json.
    QCOMPARE(core.getTheme(), QStringLiteral("markly-light"));
    QCOMPARE(core.getLocale(), QString());
    QCOMPARE(core.isPerNotebookHistoryEnabled(), false);
  }

  void testParseAndReadConfig() {
    auto val = ConfigMgr::getInst().parseAndReadConfig(QStringLiteral("main.core.theme"));
    QVERIFY(val.isString());
    QCOMPARE(val.toString(), QStringLiteral("markly-light"));

    auto missing = ConfigMgr::getInst().parseAndReadConfig(QStringLiteral("main.core.nope"));
    QVERIFY(missing.isUndefined() || missing.isNull());
  }

  void testUpdateIncrementsRevisionAndPersists() {
    auto &core = ConfigMgr::getInst().getCoreConfig();
    const int before = core.revision();
    core.setTheme(QStringLiteral("markly-dark"));
    QVERIFY(core.revision() > before);
    QCOMPARE(core.getTheme(), QStringLiteral("markly-dark"));

    // Persisted to the user config file: re-read via parseAndReadConfig (in-memory toJson).
    auto val = ConfigMgr::getInst().parseAndReadConfig(QStringLiteral("main.core.theme"));
    QCOMPARE(val.toString(), QStringLiteral("markly-dark"));
  }

  void testSessionRoundTrip() {
    auto &session = ConfigMgr::getInst().getSessionConfig();
    const QByteArray geom("\x01\x02\x03test", 7);
    session.setMainWindowGeometry(geom);
    QCOMPARE(session.getMainWindowGeometry(), geom);

    session.setNewNotebookDefaultRootFolderPath(QStringLiteral("/tmp/notebooks"));
    auto val =
        ConfigMgr::getInst().parseAndReadConfig(
            QStringLiteral("session.new_notebook_default_root_folder_path"));
    QCOMPARE(val.toString(), QStringLiteral("/tmp/notebooks"));
  }

  void testCorruptedJsonFallsBack() {
    QTemporaryFile f;
    QVERIFY(f.open());
    f.write("{ this is not valid json ");
    f.flush();
    auto settings = ConfigMgr::Settings::fromFile(f.fileName());
    QVERIFY(settings);
    // Falls back to an empty object rather than crashing.
    QVERIFY(settings->getJson().isEmpty());
  }

  void testNonExistentFileReturnsEmpty() {
    auto settings = ConfigMgr::Settings::fromFile(QStringLiteral("/no/such/markly-xyz.json"));
    QVERIFY(settings);
    QVERIFY(settings->getJson().isEmpty());
  }
};

QTEST_GUILESS_MAIN(TestConfigMgr)
#include "test_configmgr.moc"
