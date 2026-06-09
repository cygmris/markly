#ifndef MARKLY_HOTKEYPARSER_H
#define MARKLY_HOTKEYPARSER_H

#include <QString>

namespace markly {
struct ParsedHotkey {
  bool valid = false;
  int modifiers = 0; // Qt::KeyboardModifiers
  int key = 0;       // Qt::Key
};

// Parses a portable hotkey string like "Ctrl+Alt+M" (#17b). Pure / unit-testable.
class HotkeyParser {
public:
  static ParsedHotkey parse(const QString &p_seq);
};
} // namespace markly

#endif // MARKLY_HOTKEYPARSER_H
