#include "appearanceconfig.h"

using namespace markly;
using namespace markly::themedefs;

static bool isValidAccentId(const QString &p_id) {
  for (const auto &preset : accentPresets()) {
    if (preset.id == p_id) {
      return true;
    }
  }
  return false;
}

static bool isValidDensityId(const QString &p_id) {
  for (const auto &level : densityLevels()) {
    if (level.id == p_id) {
      return true;
    }
  }
  return false;
}

AppearanceConfig AppearanceConfig::fromJson(const QJsonObject &p_obj) {
  AppearanceConfig cfg;

  if (p_obj.contains(QStringLiteral("style"))) {
    cfg.style = stringToStyleId(p_obj.value(QStringLiteral("style")).toString());
  }
  if (p_obj.contains(QStringLiteral("theme"))) {
    cfg.theme = stringToThemeMode(p_obj.value(QStringLiteral("theme")).toString());
  }

  const auto accent = p_obj.value(QStringLiteral("accent")).toString();
  cfg.accentId = isValidAccentId(accent) ? accent : QStringLiteral("default");

  cfg.showLeft = p_obj.value(QStringLiteral("show_left")).toBool(true);
  cfg.showRight = p_obj.value(QStringLiteral("show_right")).toBool(true);

  const auto density = p_obj.value(QStringLiteral("density")).toString();
  cfg.densityId = isValidDensityId(density) ? density : QStringLiteral("normal");

  return cfg;
}

QJsonObject AppearanceConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("style")] = styleIdToString(style);
  obj[QStringLiteral("theme")] = themeModeToString(theme);
  obj[QStringLiteral("accent")] = accentId;
  obj[QStringLiteral("show_left")] = showLeft;
  obj[QStringLiteral("show_right")] = showRight;
  obj[QStringLiteral("density")] = densityId;
  return obj;
}
