#include "thememgr.h"

using namespace markly;

ThemeMgr::ThemeMgr(const QString &p_currentThemeName, QObject *p_parent)
    : QObject(p_parent), m_currentThemeName(p_currentThemeName) {}

QString ThemeMgr::fetchQtStyleSheet() const {
  // Foundation phase: no themed stylesheet yet.
  return QString();
}

void ThemeMgr::refreshCurrentTheme() {
  // Placeholder: spec #2 reloads theme resources here.
  emit themeChanged();
}

const QString &ThemeMgr::getCurrentThemeName() const { return m_currentThemeName; }
