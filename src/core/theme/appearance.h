#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <QObject>

#include "appearanceconfig.h"
#include "tokens.h"

namespace markly {
// Reactive appearance preferences exposed to QML. Each setter persists to
// WidgetConfig.appearance immediately and emits changed().
class Appearance : public QObject {
  Q_OBJECT
  Q_PROPERTY(int style READ styleInt WRITE setStyleInt NOTIFY changed)
  Q_PROPERTY(int theme READ themeInt WRITE setThemeInt NOTIFY changed)
  Q_PROPERTY(QString accent READ accentId WRITE setAccentId NOTIFY changed)
  Q_PROPERTY(bool showLeft READ showLeft WRITE setShowLeft NOTIFY changed)
  Q_PROPERTY(bool showRight READ showRight WRITE setShowRight NOTIFY changed)
  Q_PROPERTY(QString density READ densityId WRITE setDensityId NOTIFY changed)
  Q_PROPERTY(bool resolvedDark READ resolvedDark NOTIFY changed)
  Q_PROPERTY(qreal contentZoom READ contentZoom NOTIFY changed)
public:
  explicit Appearance(QObject *p_parent = nullptr);

  StyleId getStyle() const { return m_cfg.style; }
  ThemeMode getThemeMode() const { return m_cfg.theme; }

  int styleInt() const { return static_cast<int>(m_cfg.style); }
  void setStyleInt(int p_style);

  int themeInt() const { return static_cast<int>(m_cfg.theme); }
  void setThemeInt(int p_theme);

  QString accentId() const { return m_cfg.accentId; }
  void setAccentId(const QString &p_accentId);

  bool showLeft() const { return m_cfg.showLeft; }
  void setShowLeft(bool p_on);

  bool showRight() const { return m_cfg.showRight; }
  void setShowRight(bool p_on);

  QString densityId() const { return m_cfg.densityId; }
  void setDensityId(const QString &p_densityId);

  // Resolve theme=auto against the system color scheme.
  bool resolvedDark() const;

  qreal contentZoom() const;

  // Accent hex resolved from accentId (invalid => style default applies in ThemeModel).
  QColor accentOverride() const;

  Q_INVOKABLE void resetDefaults();

signals:
  void changed();

private:
  void persist();

  AppearanceConfig m_cfg;
};
} // namespace markly

#endif // APPEARANCE_H
