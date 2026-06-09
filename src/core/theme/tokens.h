#ifndef THEME_TOKENS_H
#define THEME_TOKENS_H

#include <QColor>
#include <QString>
#include <QVector>

namespace markly {

// The three appearance styles (design directions A/B/C).
enum class StyleId { Refined = 0, Focus, Workbench };

// Theme mode preference. Auto follows the system color scheme.
enum class ThemeMode { Light = 0, Dark, Auto };

// Semantic, style-agnostic palette + fonts + metrics.
// Each StyleId fills these roles (see ThemeModel). UI binds to roles, never to
// a specific style's color, so styles can hot-swap.
struct ThemeTokens {
  bool isDark = false;

  // Surfaces.
  QColor window;   // app/window background
  QColor rail;     // activity bar background
  QColor sidebar;  // notebook tree / notes list surface
  QColor canvas;   // editor/content background
  QColor card;     // card surface (falls back to canvas where a style has none)
  QColor bar;      // title/status bar background

  // Lines + text.
  QColor border;
  QColor borderStrong;
  QColor text;
  QColor dim;
  QColor faint;
  QColor gutter; // line-number / muted gutter

  // Rail text roles (C has a dark rail with its own text colors; A/B fall back).
  QColor railText;
  QColor railDim;
  QColor railActive;

  // Accent + interactive.
  QColor accent;
  QColor accentText; // text drawn on top of accent
  QColor accentSoft; // low-alpha accent fill (callouts/soft chips)
  QColor selection;  // selected-row tint
  QColor hover;

  // Editor / markdown semantic colors.
  QColor heading;
  QColor emphasis;
  QColor codeInline;
  QColor link;
  QColor codeBg;
  QColor codeInk;

  // Misc.
  QColor windowButton; // min/max/close glyphs

  // Style-specific extras (fall back to accent/dim when a style omits them).
  QColor lime;
  QColor amber;
  QColor pink;
  QColor sage;

  // Font roles (family strings with CJK fallbacks).
  QString fontUi;
  QString fontSerif;
  QString fontDisplay;
  QString fontMono;

  // Metrics.
  qreal contentZoom = 1.0;
  int radiusSmall = 6;
  int radiusMedium = 8;
  int radiusLarge = 10;
};

// Accent preset (id + display name + hex). hex empty => use the style default.
struct AccentPreset {
  QString id;
  QString name;
  QColor hex; // invalid => style default
};

// Density level (id + content zoom factor).
struct DensityLevel {
  QString id;
  QString name;
  qreal zoom;
};

namespace themedefs {

// Font role strings (match the design's font system).
inline QString fontUi() {
  return QStringLiteral("\"Hanken Grotesk\", \"Noto Sans SC\", system-ui, sans-serif");
}
inline QString fontSerif() {
  return QStringLiteral("\"Newsreader\", \"Noto Serif SC\", \"Songti SC\", Georgia, serif");
}
inline QString fontDisplay() {
  return QStringLiteral("\"Space Grotesk\", \"Hanken Grotesk\", \"Noto Sans SC\", sans-serif");
}
inline QString fontMono() {
  return QStringLiteral("\"JetBrains Mono\", \"ui-monospace\", monospace");
}

// Accent presets (match appshell.jsx ACCENTS). "default" => style default.
inline QVector<AccentPreset> accentPresets() {
  return {
      {QStringLiteral("default"), QStringLiteral("默认"), QColor()},
      {QStringLiteral("teal"), QStringLiteral("石墨青"), QColor(QStringLiteral("#0e8c6f"))},
      {QStringLiteral("blue"), QStringLiteral("海蓝"), QColor(QStringLiteral("#2f6fed"))},
      {QStringLiteral("violet"), QStringLiteral("电光紫"), QColor(QStringLiteral("#6c4be0"))},
      {QStringLiteral("ochre"), QStringLiteral("暖赭"), QColor(QStringLiteral("#bd6234"))},
      {QStringLiteral("rose"), QStringLiteral("玫瑰"), QColor(QStringLiteral("#d4488a"))},
      {QStringLiteral("green"), QStringLiteral("森绿"), QColor(QStringLiteral("#3f9142"))},
  };
}

// Density levels (match appshell.jsx DENSITY).
inline QVector<DensityLevel> densityLevels() {
  return {
      {QStringLiteral("compact"), QStringLiteral("紧凑"), 0.9},
      {QStringLiteral("normal"), QStringLiteral("适中"), 1.0},
      {QStringLiteral("roomy"), QStringLiteral("宽松"), 1.12},
  };
}

// Resolve an accent id to its hex (invalid QColor for "default").
inline QColor accentHexForId(const QString &p_id) {
  for (const auto &preset : accentPresets()) {
    if (preset.id == p_id) {
      return preset.hex;
    }
  }
  return QColor();
}

// Resolve a density id to its zoom factor (1.0 fallback).
inline qreal zoomForDensityId(const QString &p_id) {
  for (const auto &level : densityLevels()) {
    if (level.id == p_id) {
      return level.zoom;
    }
  }
  return 1.0;
}

// String <-> enum helpers.
inline QString styleIdToString(StyleId p_style) {
  switch (p_style) {
  case StyleId::Refined:
    return QStringLiteral("refined");
  case StyleId::Focus:
    return QStringLiteral("focus");
  case StyleId::Workbench:
    return QStringLiteral("workbench");
  }
  return QStringLiteral("refined");
}

inline StyleId stringToStyleId(const QString &p_str) {
  if (p_str == QStringLiteral("focus")) {
    return StyleId::Focus;
  } else if (p_str == QStringLiteral("workbench")) {
    return StyleId::Workbench;
  }
  return StyleId::Refined;
}

inline QString themeModeToString(ThemeMode p_mode) {
  switch (p_mode) {
  case ThemeMode::Light:
    return QStringLiteral("light");
  case ThemeMode::Dark:
    return QStringLiteral("dark");
  case ThemeMode::Auto:
    return QStringLiteral("auto");
  }
  return QStringLiteral("light");
}

inline ThemeMode stringToThemeMode(const QString &p_str) {
  if (p_str == QStringLiteral("dark")) {
    return ThemeMode::Dark;
  } else if (p_str == QStringLiteral("auto")) {
    return ThemeMode::Auto;
  }
  return ThemeMode::Light;
}

} // namespace themedefs
} // namespace markly

#endif // THEME_TOKENS_H
