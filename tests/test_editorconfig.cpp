#include <QtTest>

#include <core/configmgr.h>
#include <core/editorconfig.h>

using namespace markly;

class TestEditorConfig : public QObject {
  Q_OBJECT
private slots:
  void initTestCase() { ConfigMgr::initForUnitTest(); }

  void testDefaults() {
    auto &cfg = ConfigMgr::getInst().getEditorConfig();
    QCOMPARE(cfg.getFontSize(), 14);
    QCOMPARE(cfg.getTabWidth(), 4);
    QCOMPARE(cfg.isExpandTab(), true);
    QCOMPARE(cfg.isLineNumberEnabled(), true);
    QCOMPARE(cfg.isHighlightCurrentLineEnabled(), true);
  }

  void testUpdateAndPersist() {
    auto &cfg = ConfigMgr::getInst().getEditorConfig();
    cfg.setFontSize(18);
    cfg.setTabWidth(2);
    cfg.setLineNumberEnabled(false);
    QCOMPARE(cfg.getFontSize(), 18);
    QCOMPARE(cfg.getTabWidth(), 2);
    QCOMPARE(cfg.isLineNumberEnabled(), false);

    // toJson reflects the fields.
    const auto json = cfg.toJson();
    QCOMPARE(json.value("font_size").toInt(), 18);
    QCOMPARE(json.value("tab_width").toInt(), 2);
    QCOMPARE(json.value("line_number").toBool(), false);
  }

  void testInputSettings() {
    auto &cfg = ConfigMgr::getInst().getEditorConfig();
    cfg.setAutoIndentEnabled(false);
    cfg.setContinueListEnabled(false);
    cfg.setAutoPairEnabled(false);
    QCOMPARE(cfg.isAutoIndentEnabled(), false);
    QCOMPARE(cfg.isContinueListEnabled(), false);
    QCOMPARE(cfg.isAutoPairEnabled(), false);

    const auto json = cfg.toJson();
    QCOMPARE(json.value("auto_indent").toBool(), false);
    QCOMPARE(json.value("continue_list").toBool(), false);
    QCOMPARE(json.value("auto_pair").toBool(), false);
  }
};

QTEST_GUILESS_MAIN(TestEditorConfig)
#include "test_editorconfig.moc"
