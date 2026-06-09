#include <QtTest>

#include <core/configmgr.h>
#include <core/theme/appearance.h>
#include <core/theme/appearanceconfig.h>
#include <core/theme/thememodel.h>
#include <core/theme/tokens.h>
#include <core/widgetconfig.h>
#include <utils/colorutils.h>

using namespace markly;

static QString hexName(const QColor &c) { return c.name(QColor::HexRgb); }

class TestThemeModel : public QObject {
  Q_OBJECT
private slots:
  void testRefinedADefaults() {
    auto light = ThemeModel::buildTokens(StyleId::Refined, false, QColor());
    QCOMPARE(hexName(light.accent), QStringLiteral("#0e8c6f"));
    QCOMPARE(hexName(light.canvas), QStringLiteral("#ffffff"));
    QCOMPARE(hexName(light.text), QStringLiteral("#1d2528"));

    auto dark = ThemeModel::buildTokens(StyleId::Refined, true, QColor());
    QCOMPARE(hexName(dark.window), QStringLiteral("#14171a"));
    QCOMPARE(hexName(dark.accent), QStringLiteral("#2fbf9b"));
  }

  void testFocusBDefaults() {
    auto light = ThemeModel::buildTokens(StyleId::Focus, false, QColor());
    QCOMPARE(hexName(light.canvas), QStringLiteral("#fbf8f1")); // paper
    QCOMPARE(hexName(light.accent), QStringLiteral("#bd6234"));
    QCOMPARE(hexName(light.text), QStringLiteral("#2e2a23")); // ink
    QVERIFY(light.fontSerif.contains(QStringLiteral("Newsreader")));
  }

  void testWorkbenchCDefaults() {
    auto dark = ThemeModel::buildTokens(StyleId::Workbench, true, QColor());
    QCOMPARE(hexName(dark.rail), QStringLiteral("#15161d")); // nav
    QCOMPARE(hexName(dark.accent), QStringLiteral("#8b6cf0"));

    auto light = ThemeModel::buildTokens(StyleId::Workbench, false, QColor());
    QCOMPARE(hexName(light.lime), QStringLiteral("#5f8f15"));
    QCOMPARE(hexName(light.rail), QStringLiteral("#1a1c24")); // dark nav even in light
    QVERIFY(light.fontDisplay.contains(QStringLiteral("Space Grotesk")));
  }

  void testAccentOverrideAndDerivation() {
    QColor blue(QStringLiteral("#2f6fed"));
    auto t = ThemeModel::buildTokens(StyleId::Refined, false, blue);
    QCOMPARE(hexName(t.accent), QStringLiteral("#2f6fed"));
    // selection = alpha(accent, 0.12)
    QCOMPARE(hexName(t.selection), QStringLiteral("#2f6fed"));
    QVERIFY(qAbs(t.selection.alphaF() - 0.12) < 0.02);
  }

  void testColorUtilsAlpha() {
    auto c = ColorUtils::alpha(QColor(QStringLiteral("#6c4be0")), 0.2);
    QCOMPARE(hexName(c), QStringLiteral("#6c4be0"));
    QVERIFY(qAbs(c.alphaF() - 0.2) < 0.02);
  }

  void testAccentTextContrast() {
    // White text over a deep violet, black text over a light color.
    QVERIFY(ThemeModel::accentTextFor(QColor(QStringLiteral("#6c4be0"))) == QColor("#ffffff"));
    QVERIFY(ThemeModel::accentTextFor(QColor(QStringLiteral("#f0e9db"))) == QColor("#000000"));
  }

  void testAppearanceConfigRoundTrip() {
    AppearanceConfig cfg;
    cfg.style = StyleId::Workbench;
    cfg.theme = ThemeMode::Dark;
    cfg.accentId = QStringLiteral("violet");
    cfg.showLeft = false;
    cfg.densityId = QStringLiteral("roomy");

    auto restored = AppearanceConfig::fromJson(cfg.toJson());
    QCOMPARE(restored.style, StyleId::Workbench);
    QCOMPARE(restored.theme, ThemeMode::Dark);
    QCOMPARE(restored.accentId, QStringLiteral("violet"));
    QCOMPARE(restored.showLeft, false);
    QCOMPARE(restored.densityId, QStringLiteral("roomy"));
  }

  void testAppearancePersistence() {
    ConfigMgr::initForUnitTest();
    Appearance appearance;
    appearance.setStyleInt(static_cast<int>(StyleId::Workbench));
    appearance.setAccentId(QStringLiteral("violet"));
    appearance.setShowRight(false);

    // Re-read straight from the persisted widget config.
    auto stored = ConfigMgr::getInst().getWidgetConfig().getAppearance();
    QCOMPARE(stored.style, StyleId::Workbench);
    QCOMPARE(stored.accentId, QStringLiteral("violet"));
    QCOMPARE(stored.showRight, false);
  }

  void testAppearanceConfigInvalidFallback() {
    QJsonObject bad;
    bad[QStringLiteral("style")] = QStringLiteral("nope");
    bad[QStringLiteral("accent")] = QStringLiteral("ultraviolet");
    bad[QStringLiteral("density")] = QStringLiteral("huge");
    auto cfg = AppearanceConfig::fromJson(bad);
    QCOMPARE(cfg.style, StyleId::Refined);              // unknown -> default
    QCOMPARE(cfg.accentId, QStringLiteral("default"));  // invalid -> default
    QCOMPARE(cfg.densityId, QStringLiteral("normal"));  // invalid -> default
  }
};

QTEST_GUILESS_MAIN(TestThemeModel)
#include "test_thememodel.moc"
