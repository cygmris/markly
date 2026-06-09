#ifndef WIDGETCONFIG_H
#define WIDGETCONFIG_H

#include "iconfig.h"

#include "theme/appearanceconfig.h"

namespace markly {
// Widget configuration framework.
// NOTE: foundation scaffolding only; UI specs (#3/#5/#20) add concrete fields.
// Unknown keys are round-tripped to avoid data loss.
class WidgetConfig : public IConfig {
public:
  WidgetConfig(ConfigMgr *p_mgr, IConfig *p_topConfig);

  void init(const QJsonObject &p_app, const QJsonObject &p_user) Q_DECL_OVERRIDE;

  QJsonObject toJson() const Q_DECL_OVERRIDE;

  // Appearance preferences (theme-appearance spec).
  AppearanceConfig getAppearance() const;
  void setAppearance(const AppearanceConfig &p_appearance);

private:
  QJsonObject m_jobj;
};
} // namespace markly

#endif // WIDGETCONFIG_H
