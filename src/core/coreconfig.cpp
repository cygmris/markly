#include "coreconfig.h"

using namespace markly;

CoreConfig::CoreConfig(ConfigMgr *p_mgr, IConfig *p_topConfig) : IConfig(p_mgr, p_topConfig) {}

void CoreConfig::init(const QJsonObject &p_app, const QJsonObject &p_user) {
  m_sessionName = QStringLiteral("core");

  m_locale = readString(p_app, p_user, QStringLiteral("locale"));
  m_theme = readString(p_app, p_user, QStringLiteral("theme"));
  if (m_theme.isEmpty()) {
    m_theme = QStringLiteral("markly-light");
  }
  m_perNotebookHistoryEnabled =
      readBool(p_app, p_user, QStringLiteral("per_notebook_history"));
}

QJsonObject CoreConfig::toJson() const {
  QJsonObject obj;
  obj[QStringLiteral("locale")] = m_locale;
  obj[QStringLiteral("theme")] = m_theme;
  obj[QStringLiteral("per_notebook_history")] = m_perNotebookHistoryEnabled;
  return obj;
}

const QString &CoreConfig::getLocale() const { return m_locale; }

void CoreConfig::setLocale(const QString &p_locale) {
  updateConfig(m_locale, p_locale, this);
}

const QString &CoreConfig::getTheme() const { return m_theme; }

void CoreConfig::setTheme(const QString &p_theme) { updateConfig(m_theme, p_theme, this); }

bool CoreConfig::isPerNotebookHistoryEnabled() const { return m_perNotebookHistoryEnabled; }

void CoreConfig::setPerNotebookHistoryEnabled(bool p_enabled) {
  updateConfig(m_perNotebookHistoryEnabled, p_enabled, this);
}
