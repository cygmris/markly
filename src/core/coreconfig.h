#ifndef CORECONFIG_H
#define CORECONFIG_H

#include "iconfig.h"

#include <QString>

namespace markly {
// Core configuration: locale, theme name, history policy, ...
// NOTE: foundation subset. Later specs (theme/i18n/notebook) extend the fields here.
class CoreConfig : public IConfig {
public:
  CoreConfig(ConfigMgr *p_mgr, IConfig *p_topConfig);

  void init(const QJsonObject &p_app, const QJsonObject &p_user) Q_DECL_OVERRIDE;

  QJsonObject toJson() const Q_DECL_OVERRIDE;

  const QString &getLocale() const;
  void setLocale(const QString &p_locale);

  const QString &getTheme() const;
  void setTheme(const QString &p_theme);

  bool isPerNotebookHistoryEnabled() const;
  void setPerNotebookHistoryEnabled(bool p_enabled);

private:
  // Locale to use. Empty means follow system.
  QString m_locale;

  // Current theme name.
  QString m_theme;

  // Whether to store history per notebook.
  bool m_perNotebookHistoryEnabled = false;

  // TODO(later-spec): port remaining CoreConfig fields (shortcuts, recent notebooks,
  // toolbar icon size, recover last session, etc.) as the corresponding specs land.
};
} // namespace markly

#endif // CORECONFIG_H
