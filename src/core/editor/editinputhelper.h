#ifndef MARKLY_EDITINPUTHELPER_H
#define MARKLY_EDITINPUTHELPER_H

#include <QString>

namespace markly {
// Pure-text editing helpers used by the markdown editor (no UI dependency, so they
// are unit-testable). The QML editor calls these via EditInputQml and applies the
// results to the TextEdit.
class EditInputHelper {
public:
  // Result of pressing Enter on a line.
  struct EnterResult {
    // True when the current line is an empty list marker that should be cleared:
    // the caller replaces the current line with `lineIndent` and inserts a plain "\n".
    bool clearMarker = false;
    QString lineIndent; // indent to keep when clearing the marker
    QString insert;     // text to insert at the cursor (newline + new prefix) otherwise
  };

  // Compute what to insert when Enter is pressed at the end of `curLine`.
  // autoIndent: keep leading whitespace; continueList: continue list markers.
  static EnterResult continueOnEnter(const QString &p_curLine, bool p_autoIndent,
                                     bool p_continueList);

  // Add one indent unit to the start of every line in the block.
  static QString indentLines(const QString &p_block, const QString &p_tabUnit);
  // Remove up to one indent unit from the start of every line (a leading tab, or up
  // to `p_tabUnit` worth of leading spaces).
  static QString outdentLines(const QString &p_block, const QString &p_tabUnit);

  // Matching closing character for an auto-pair opener, or '\0' if none.
  static QChar pairFor(QChar p_open);
};
} // namespace markly

#endif // MARKLY_EDITINPUTHELPER_H
