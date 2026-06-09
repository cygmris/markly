#ifndef STYLESHEETGENERATOR_H
#define STYLESHEETGENERATOR_H

#include <QString>

#include "tokens.h"

namespace markly {
// Generate a base QWidget stylesheet from the current ThemeTokens so the QWidget
// parts (editor, dialogs) follow the active theme.
class StyleSheetGenerator {
public:
  StyleSheetGenerator() = delete;

  static QString generate(const ThemeTokens &p_tokens);

  // QColor -> CSS color string (#rrggbb or rgba(...)).
  static QString cssColor(const QColor &p_color);
};
} // namespace markly

#endif // STYLESHEETGENERATOR_H
