#include "appearance.h"

#include <QGuiApplication>
#include <QStyleHints>

#include <core/configmgr.h>
#include <core/widgetconfig.h>

using namespace markly;
using namespace markly::themedefs;

Appearance::Appearance(QObject *p_parent) : QObject(p_parent) {
  m_cfg = ConfigMgr::getInst().getWidgetConfig().getAppearance();

  // Follow the system color scheme when theme == Auto.
  if (auto hints = QGuiApplication::styleHints()) {
    connect(hints, &QStyleHints::colorSchemeChanged, this, [this]() {
      if (m_cfg.theme == ThemeMode::Auto) {
        emit changed();
      }
    });
  }
}

void Appearance::persist() {
  ConfigMgr::getInst().getWidgetConfig().setAppearance(m_cfg);
}

void Appearance::setStyleInt(int p_style) {
  auto style = static_cast<StyleId>(p_style);
  if (style == m_cfg.style) {
    return;
  }
  m_cfg.style = style;
  persist();
  emit changed();
}

void Appearance::setThemeInt(int p_theme) {
  auto theme = static_cast<ThemeMode>(p_theme);
  if (theme == m_cfg.theme) {
    return;
  }
  m_cfg.theme = theme;
  persist();
  emit changed();
}

void Appearance::setAccentId(const QString &p_accentId) {
  if (p_accentId == m_cfg.accentId) {
    return;
  }
  m_cfg.accentId = p_accentId;
  persist();
  emit changed();
}

void Appearance::setShowLeft(bool p_on) {
  if (p_on == m_cfg.showLeft) {
    return;
  }
  m_cfg.showLeft = p_on;
  persist();
  emit changed();
}

void Appearance::setShowRight(bool p_on) {
  if (p_on == m_cfg.showRight) {
    return;
  }
  m_cfg.showRight = p_on;
  persist();
  emit changed();
}

void Appearance::setDensityId(const QString &p_densityId) {
  if (p_densityId == m_cfg.densityId) {
    return;
  }
  m_cfg.densityId = p_densityId;
  persist();
  emit changed();
}

bool Appearance::resolvedDark() const {
  switch (m_cfg.theme) {
  case ThemeMode::Light:
    return false;
  case ThemeMode::Dark:
    return true;
  case ThemeMode::Auto:
    if (auto hints = QGuiApplication::styleHints()) {
      return hints->colorScheme() == Qt::ColorScheme::Dark;
    }
    return false;
  }
  return false;
}

qreal Appearance::contentZoom() const { return zoomForDensityId(m_cfg.densityId); }

QColor Appearance::accentOverride() const { return accentHexForId(m_cfg.accentId); }

void Appearance::resetDefaults() {
  m_cfg = AppearanceConfig();
  persist();
  emit changed();
}
