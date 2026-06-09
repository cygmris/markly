#include "widgetconfig.h"

using namespace markly;

WidgetConfig::WidgetConfig(ConfigMgr *p_mgr, IConfig *p_topConfig)
    : IConfig(p_mgr, p_topConfig) {}

void WidgetConfig::init(const QJsonObject &p_app, const QJsonObject &p_user) {
  m_sessionName = QStringLiteral("widget");

  m_jobj = p_app;
  for (auto it = p_user.begin(); it != p_user.end(); ++it) {
    m_jobj[it.key()] = it.value();
  }
}

QJsonObject WidgetConfig::toJson() const { return m_jobj; }

AppearanceConfig WidgetConfig::getAppearance() const {
  return AppearanceConfig::fromJson(m_jobj.value(QStringLiteral("appearance")).toObject());
}

void WidgetConfig::setAppearance(const AppearanceConfig &p_appearance) {
  m_jobj[QStringLiteral("appearance")] = p_appearance.toJson();
  ++m_revision;
  writeToSettings();
}
