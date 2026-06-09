#ifndef MARKLY_FILEASSOC_H
#define MARKLY_FILEASSOC_H

#include <QStringList>

namespace markly {
// Associates .md (text/markdown) with Markly on Linux desktops (#20c). registerCommands
// is pure (testable); registerNow runs xdg-mime best-effort.
class FileAssoc {
public:
  // The xdg-mime command line that sets markly.desktop as the default for text/markdown.
  static QStringList registerCommands();
  // Best-effort: run xdg-mime now. Returns false if xdg-mime is unavailable or fails.
  static bool registerNow();
};
} // namespace markly

#endif // MARKLY_FILEASSOC_H
