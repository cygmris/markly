#ifndef MARKLY_VIENGINE_H
#define MARKLY_VIENGINE_H

#include <QChar>
#include <QHash>
#include <QString>
#include <QVector>

namespace markly {
enum class ViMode { Normal = 0, Insert = 1, Visual = 2 };

// A single text replacement: replace [start, end) with `text`.
struct ViEdit {
  int start = 0;
  int end = 0;
  QString text;
};

// Outcome of handling one key. When `handled` is false the QML editor performs
// its default handling (e.g. inserting a character in Insert mode).
struct ViResult {
  bool handled = false;
  ViMode mode = ViMode::Normal;
  int cursor = 0;
  int anchor = -1; // selection anchor (-1 = no selection)
  QVector<ViEdit> edits;
};

// Pure-logic Vi input state machine (no Qt UI dependency, so it is unit-testable).
// Holds mode / count / pending operator / unnamed register. The QML editor feeds keys
// via ViBridge and applies the returned edits/cursor/selection. Implements a practical
// core subset (see docs/vi-mode); advanced commands are spec #8e.
class ViEngine {
public:
  ViMode mode() const { return m_mode; }
  void reset();

  // Current document text + cursor + selection (half-open [selStart, selEnd)).
  // keyText: the event's printable text; key: Qt::Key; mods: Qt::KeyboardModifiers.
  ViResult handleKey(const QString &p_text, int p_cursor, int p_selStart, int p_selEnd,
                     const QString &p_keyText, int p_key, int p_mods);

private:
  // Line helpers (positions are between-character indices into the text).
  static int lineStartOf(const QString &t, int pos);
  static int lineEndOf(const QString &t, int pos);
  static int columnOf(const QString &t, int pos);
  static int wordForward(const QString &t, int pos, int count);
  static int wordBackward(const QString &t, int pos, int count);
  static int wordEnd(const QString &t, int pos, int count);
  // In-line find for f/F/t/T; returns new cursor (unchanged if not found).
  static int findInLine(const QString &t, int cursor, QChar cmd, QChar target);
  // WORD motions (whitespace-delimited only) for W/B/E (#8f).
  static int WORDForward(const QString &t, int pos, int count);
  static int WORDBackward(const QString &t, int pos, int count);
  static int WORDEnd(const QString &t, int pos, int count);
  // %: jump to the bracket matching the first ()[]{} at/after cursor on the line.
  static int matchBracket(const QString &t, int cursor);
  // Motion target for an operator (w/b/0/$/h/l), or -1 if `key` is not such a motion.
  int motionTarget(const QString &t, int cursor, const QString &key, int count) const;

  // One recorded keystroke, for `.` repeat (#8g) and macros (#8h).
  struct ViKey { QString text; int key; int mods; };
  ViResult repeatLastChange(const QString &t, int cursor, int selStart, int selEnd);
  // Replay macro register `reg` over the text; returns a single whole-document edit (#8h).
  ViResult replayMacro(QChar reg, const QString &t, int cursor, int selStart, int selEnd);

  ViResult normalKey(const QString &t, int cursor, const QString &key, int qkey);
  ViResult visualKey(const QString &t, int cursor, int selStart, int selEnd,
                     const QString &key, int qkey);

  ViMode m_mode = ViMode::Normal;
  int m_count = 0;            // 0 = no count entered yet
  QChar m_pendingOp = QChar(0); // pending operator d/y/c
  bool m_gPending = false;   // saw first 'g' of gg
  int m_visualAnchor = 0;    // anchor while in Visual mode
  QString m_register;        // unnamed register
  bool m_registerLinewise = false;

  // vi-advanced (#8e): pending argument command (f/F/t/T/r) + last find for ; and ,.
  QChar m_pendingChar = QChar(0);
  QChar m_lastFindCmd = QChar(0);
  QChar m_lastFindTarget = QChar(0);

  // `.` repeat (#8g): record the keys of the current/last change command.
  QVector<ViKey> m_curKeys;
  QVector<ViKey> m_lastChange;
  bool m_replaying = false;

  // Macros (#8h): q<reg> records, @<reg> replays.
  QHash<QChar, QVector<ViKey>> m_macros;
  bool m_recording = false;
  QChar m_recordReg = QChar(0);
  bool m_pendingQ = false;  // q awaiting register letter
  bool m_pendingAt = false; // @ awaiting register letter
};
} // namespace markly

#endif // MARKLY_VIENGINE_H
