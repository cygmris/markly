#include "hotkeyparser.h"

#include <QKeySequence>

using namespace markly;

ParsedHotkey HotkeyParser::parse(const QString &p_seq) {
  ParsedHotkey r;
  if (p_seq.trimmed().isEmpty()) {
    return r;
  }
  const QKeySequence seq(p_seq, QKeySequence::PortableText);
  if (seq.count() != 1) {
    return r; // empty / multi-chord -> invalid
  }
  const QKeyCombination combo = seq[0];
  const int key = combo.key();
  if (key == Qt::Key_unknown || key == 0) {
    return r;
  }
  r.valid = true;
  r.modifiers = static_cast<int>(combo.keyboardModifiers());
  r.key = key;
  return r;
}
