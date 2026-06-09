#ifndef THEMEMGR_H
#define THEMEMGR_H

#include <QObject>
#include <QString>

namespace markly {
// Placeholder theme manager for the foundation spec.
// NOTE: spec #2 (theme-appearance) replaces this with the full A/B/C style + token
// system (light/dark/accent/layout) and QML Theme singleton + QSS generation.
class ThemeMgr : public QObject {
  Q_OBJECT
public:
  explicit ThemeMgr(const QString &p_currentThemeName, QObject *p_parent = nullptr);

  // Qt widget stylesheet for the current theme. Empty in the foundation phase.
  QString fetchQtStyleSheet() const;

  // Re-read the current theme from disk (no-op placeholder).
  void refreshCurrentTheme();

  const QString &getCurrentThemeName() const;

signals:
  void themeChanged();

private:
  QString m_currentThemeName;
};
} // namespace markly

#endif // THEMEMGR_H
