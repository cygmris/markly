#ifndef THEMEMGR_H
#define THEMEMGR_H

#include <QColor>
#include <QObject>
#include <QString>

#include "theme/tokens.h"

namespace markly {
class Appearance;

// Theme bridge between core (ThemeModel/Appearance) and the UI (QML + QWidget QSS).
// Holds the Appearance prefs, caches the current ThemeTokens, exposes every token
// as a QML-bindable Q_PROPERTY, and generates the widget stylesheet.
// Replaces the foundation-spec placeholder (same fetchQtStyleSheet/refreshCurrentTheme
// /themeChanged surface, so Application/main.cpp need no changes).
class ThemeMgr : public QObject {
  Q_OBJECT

  // Surfaces.
  Q_PROPERTY(QColor window READ window NOTIFY themeChanged)
  Q_PROPERTY(QColor rail READ rail NOTIFY themeChanged)
  Q_PROPERTY(QColor sidebar READ sidebar NOTIFY themeChanged)
  Q_PROPERTY(QColor canvas READ canvas NOTIFY themeChanged)
  Q_PROPERTY(QColor card READ card NOTIFY themeChanged)
  Q_PROPERTY(QColor bar READ bar NOTIFY themeChanged)
  // Lines + text.
  Q_PROPERTY(QColor border READ border NOTIFY themeChanged)
  Q_PROPERTY(QColor borderStrong READ borderStrong NOTIFY themeChanged)
  Q_PROPERTY(QColor text READ text NOTIFY themeChanged)
  Q_PROPERTY(QColor dim READ dim NOTIFY themeChanged)
  Q_PROPERTY(QColor faint READ faint NOTIFY themeChanged)
  Q_PROPERTY(QColor gutter READ gutter NOTIFY themeChanged)
  Q_PROPERTY(QColor railText READ railText NOTIFY themeChanged)
  Q_PROPERTY(QColor railDim READ railDim NOTIFY themeChanged)
  Q_PROPERTY(QColor railActive READ railActive NOTIFY themeChanged)
  // Accent + interactive.
  Q_PROPERTY(QColor accent READ accent NOTIFY themeChanged)
  Q_PROPERTY(QColor accentText READ accentText NOTIFY themeChanged)
  Q_PROPERTY(QColor accentSoft READ accentSoft NOTIFY themeChanged)
  Q_PROPERTY(QColor selection READ selection NOTIFY themeChanged)
  Q_PROPERTY(QColor hover READ hover NOTIFY themeChanged)
  // Editor semantics.
  Q_PROPERTY(QColor heading READ heading NOTIFY themeChanged)
  Q_PROPERTY(QColor emphasis READ emphasis NOTIFY themeChanged)
  Q_PROPERTY(QColor codeInline READ codeInline NOTIFY themeChanged)
  Q_PROPERTY(QColor link READ link NOTIFY themeChanged)
  Q_PROPERTY(QColor codeBg READ codeBg NOTIFY themeChanged)
  Q_PROPERTY(QColor codeInk READ codeInk NOTIFY themeChanged)
  Q_PROPERTY(QColor windowButton READ windowButton NOTIFY themeChanged)
  // Extras.
  Q_PROPERTY(QColor lime READ lime NOTIFY themeChanged)
  Q_PROPERTY(QColor amber READ amber NOTIFY themeChanged)
  Q_PROPERTY(QColor pink READ pink NOTIFY themeChanged)
  Q_PROPERTY(QColor sage READ sage NOTIFY themeChanged)
  // Fonts + metrics.
  Q_PROPERTY(QString fontUi READ fontUi NOTIFY themeChanged)
  Q_PROPERTY(QString fontSerif READ fontSerif NOTIFY themeChanged)
  Q_PROPERTY(QString fontDisplay READ fontDisplay NOTIFY themeChanged)
  Q_PROPERTY(QString fontMono READ fontMono NOTIFY themeChanged)
  Q_PROPERTY(bool isDark READ isDark NOTIFY themeChanged)
  Q_PROPERTY(qreal contentZoom READ contentZoom NOTIFY themeChanged)
  Q_PROPERTY(int radiusSmall READ radiusSmall NOTIFY themeChanged)
  Q_PROPERTY(int radiusMedium READ radiusMedium NOTIFY themeChanged)
  Q_PROPERTY(int radiusLarge READ radiusLarge NOTIFY themeChanged)

public:
  explicit ThemeMgr(QObject *p_parent = nullptr);

  Appearance *getAppearance() const { return m_appearance; }

  const ThemeTokens &getTokens() const { return m_tokens; }

  // Qt widget stylesheet for the current theme.
  QString fetchQtStyleSheet() const;

  // Recompute the current tokens from the current appearance prefs.
  void refreshCurrentTheme();

  // --- token getters (used by Q_PROPERTY) ---
  QColor window() const { return m_tokens.window; }
  QColor rail() const { return m_tokens.rail; }
  QColor sidebar() const { return m_tokens.sidebar; }
  QColor canvas() const { return m_tokens.canvas; }
  QColor card() const { return m_tokens.card; }
  QColor bar() const { return m_tokens.bar; }
  QColor border() const { return m_tokens.border; }
  QColor borderStrong() const { return m_tokens.borderStrong; }
  QColor text() const { return m_tokens.text; }
  QColor dim() const { return m_tokens.dim; }
  QColor faint() const { return m_tokens.faint; }
  QColor gutter() const { return m_tokens.gutter; }
  QColor railText() const { return m_tokens.railText; }
  QColor railDim() const { return m_tokens.railDim; }
  QColor railActive() const { return m_tokens.railActive; }
  QColor accent() const { return m_tokens.accent; }
  QColor accentText() const { return m_tokens.accentText; }
  QColor accentSoft() const { return m_tokens.accentSoft; }
  QColor selection() const { return m_tokens.selection; }
  QColor hover() const { return m_tokens.hover; }
  QColor heading() const { return m_tokens.heading; }
  QColor emphasis() const { return m_tokens.emphasis; }
  QColor codeInline() const { return m_tokens.codeInline; }
  QColor link() const { return m_tokens.link; }
  QColor codeBg() const { return m_tokens.codeBg; }
  QColor codeInk() const { return m_tokens.codeInk; }
  QColor windowButton() const { return m_tokens.windowButton; }
  QColor lime() const { return m_tokens.lime; }
  QColor amber() const { return m_tokens.amber; }
  QColor pink() const { return m_tokens.pink; }
  QColor sage() const { return m_tokens.sage; }
  QString fontUi() const { return m_tokens.fontUi; }
  QString fontSerif() const { return m_tokens.fontSerif; }
  QString fontDisplay() const { return m_tokens.fontDisplay; }
  QString fontMono() const { return m_tokens.fontMono; }
  bool isDark() const { return m_tokens.isDark; }
  qreal contentZoom() const { return m_tokens.contentZoom; }
  int radiusSmall() const { return m_tokens.radiusSmall; }
  int radiusMedium() const { return m_tokens.radiusMedium; }
  int radiusLarge() const { return m_tokens.radiusLarge; }

signals:
  void themeChanged();

private:
  Appearance *m_appearance = nullptr;

  ThemeTokens m_tokens;
};
} // namespace markly

#endif // THEMEMGR_H
