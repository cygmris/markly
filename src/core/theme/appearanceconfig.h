#ifndef APPEARANCECONFIG_H
#define APPEARANCECONFIG_H

#include <QJsonObject>
#include <QString>

#include "tokens.h"

namespace markly {
// Plain data + JSON for the appearance preferences (stored in WidgetConfig.appearance).
struct AppearanceConfig {
  StyleId style = StyleId::Refined;
  ThemeMode theme = ThemeMode::Light;
  QString accentId = QStringLiteral("default");
  bool showLeft = true;
  bool showRight = true;
  QString densityId = QStringLiteral("normal");

  // Parse from JSON, falling back to defaults for missing/invalid values.
  static AppearanceConfig fromJson(const QJsonObject &p_obj);

  QJsonObject toJson() const;
};
} // namespace markly

#endif // APPEARANCECONFIG_H
