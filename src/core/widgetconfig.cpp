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

QString WidgetConfig::getLanguage() const {
  return m_jobj.value(QStringLiteral("language")).toString(QStringLiteral("auto"));
}

void WidgetConfig::setLanguage(const QString &p_language) {
  m_jobj[QStringLiteral("language")] = p_language;
  ++m_revision;
  writeToSettings();
}

QJsonObject WidgetConfig::getImageHostConfig() const {
  return m_jobj.value(QStringLiteral("image_host")).toObject();
}

void WidgetConfig::setImageHostConfig(const QJsonObject &p_config) {
  m_jobj[QStringLiteral("image_host")] = p_config;
  ++m_revision;
  writeToSettings();
}

bool WidgetConfig::getMinimizeToTray() const {
  return m_jobj.value(QStringLiteral("minimize_to_tray")).toBool(false);
}

void WidgetConfig::setMinimizeToTray(bool p_on) {
  m_jobj[QStringLiteral("minimize_to_tray")] = p_on;
  ++m_revision;
  writeToSettings();
}

bool WidgetConfig::getAutoUpdateCheck() const {
  return m_jobj.value(QStringLiteral("auto_update_check")).toBool(false);
}

void WidgetConfig::setAutoUpdateCheck(bool p_on) {
  m_jobj[QStringLiteral("auto_update_check")] = p_on;
  ++m_revision;
  writeToSettings();
}

qint64 WidgetConfig::getLastUpdateCheck() const {
  return static_cast<qint64>(m_jobj.value(QStringLiteral("last_update_check")).toDouble(0));
}

void WidgetConfig::setLastUpdateCheck(qint64 p_ms) {
  m_jobj[QStringLiteral("last_update_check")] = static_cast<double>(p_ms);
  ++m_revision;
  writeToSettings();
}

QString WidgetConfig::getGlobalHotkey() const {
  return m_jobj.value(QStringLiteral("global_hotkey")).toString(QStringLiteral("Ctrl+Alt+M"));
}

void WidgetConfig::setGlobalHotkey(const QString &p_seq) {
  m_jobj[QStringLiteral("global_hotkey")] = p_seq;
  ++m_revision;
  writeToSettings();
}
