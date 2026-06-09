#include <QTranslator>
#include <QtTest>

#include <core/configmgr.h>
#include <core/widgetconfig.h>

using namespace markly;

class TestLocale : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testLanguageRoundTrip() {
    auto &cfg = ConfigMgr::getInst().getWidgetConfig();
    QCOMPARE(cfg.getLanguage(), QStringLiteral("auto")); // default
    cfg.setLanguage(QStringLiteral("en_US"));
    QCOMPARE(cfg.getLanguage(), QStringLiteral("en_US"));
    QCOMPARE(cfg.toJson().value("language").toString(), QStringLiteral("en_US"));
  }

  void testTranslatorContent() {
    QTranslator tr;
    QVERIFY(tr.load(QStringLiteral(":/i18n/markly_en_US.qm")));
    QCOMPARE(tr.translate("SettingsDialog", "设置"), QStringLiteral("Settings"));
    QCOMPARE(tr.translate("SettingsDialog", "编辑器"), QStringLiteral("Editor"));
    QCOMPARE(tr.translate("UnitedEntry", "新建笔记"), QStringLiteral("New note"));
  }
};

QTEST_GUILESS_MAIN(TestLocale)
#include "test_locale.moc"
