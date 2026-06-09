#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QColor>

#include "tokens.h"

namespace markly {
// Pure token generator: (style, dark, accentOverride) -> ThemeTokens.
// Ports the design's three tok() functions (refined/focus/bold.jsx).
class ThemeModel {
public:
  ThemeModel() = delete;

  // Build the full token set. @p_accentOverride invalid => style default accent.
  static ThemeTokens buildTokens(StyleId p_style, bool p_dark, const QColor &p_accentOverride);

  // The built-in default accent for a style/mode.
  static QColor defaultAccent(StyleId p_style, bool p_dark);

  // Choose readable text color over an accent (black/white by luminance).
  static QColor accentTextFor(const QColor &p_accent);

private:
  static ThemeTokens buildRefinedA(bool p_dark, const QColor &p_accent);
  static ThemeTokens buildFocusB(bool p_dark, const QColor &p_accent);
  static ThemeTokens buildWorkbenchC(bool p_dark, const QColor &p_accent);
};
} // namespace markly

#endif // THEMEMODEL_H
