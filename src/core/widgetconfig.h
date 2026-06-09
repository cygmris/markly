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

  // UI language: "auto" / "zh_CN" / "en_US" (i18n spec #20).
  QString getLanguage() const;
  void setLanguage(const QString &p_language);

  // Image host config { type, user, repo, token, branch } (#10b).
  QJsonObject getImageHostConfig() const;
  void setImageHostConfig(const QJsonObject &p_config);

  // Minimize to system tray on window close (#20b, default false).
  bool getMinimizeToTray() const;
  void setMinimizeToTray(bool p_on);

  // Auto-update-check toggle + last-check timestamp (ms) (#20c).
  bool getAutoUpdateCheck() const;
  void setAutoUpdateCheck(bool p_on);
  qint64 getLastUpdateCheck() const;
  void setLastUpdateCheck(qint64 p_ms);

  // Global hotkey to summon the window (#17b, X11 only). Default "Ctrl+Alt+M".
  QString getGlobalHotkey() const;
  void setGlobalHotkey(const QString &p_seq);

private:
  QJsonObject m_jobj;
};
} // namespace markly

#endif // WIDGETCONFIG_H
