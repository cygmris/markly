#include "viengine.h"

#include <Qt>

using namespace markly;

namespace {
int classOf(QChar c) {
  if (c.isSpace()) return 0;            // whitespace
  if (c.isLetterOrNumber() || c == QLatin1Char('_')) return 1; // word
  return 2;                             // punctuation
}
} // namespace

void ViEngine::reset() {
  m_mode = ViMode::Normal;
  m_count = 0;
  m_pendingOp = QChar(0);
  m_gPending = false;
  m_pendingChar = QChar(0);
  m_curKeys.clear();   // keep m_lastChange so `.` survives mode changes
  m_pendingQ = false;  // cancel a pending q/@ (keep recording + macros, #8h)
  m_pendingAt = false;
}

int ViEngine::lineStartOf(const QString &t, int pos) {
  pos = qBound(0, pos, t.length());
  int i = pos;
  while (i > 0 && t.at(i - 1) != QLatin1Char('\n')) --i;
  return i;
}

int ViEngine::lineEndOf(const QString &t, int pos) {
  pos = qBound(0, pos, t.length());
  int i = pos;
  while (i < t.length() && t.at(i) != QLatin1Char('\n')) ++i;
  return i;
}

int ViEngine::columnOf(const QString &t, int pos) { return pos - lineStartOf(t, pos); }

int ViEngine::wordForward(const QString &t, int pos, int count) {
  const int n = t.length();
  for (int c = 0; c < count && pos < n; ++c) {
    int c0 = classOf(t.at(pos));
    if (c0 != 0) {
      while (pos < n && classOf(t.at(pos)) == c0) ++pos;
    }
    while (pos < n && classOf(t.at(pos)) == 0) ++pos;
  }
  return pos;
}

int ViEngine::wordBackward(const QString &t, int pos, int count) {
  for (int c = 0; c < count && pos > 0; ++c) {
    --pos;
    while (pos > 0 && classOf(t.at(pos)) == 0) --pos;
    if (pos == 0) break;
    int c0 = classOf(t.at(pos));
    while (pos > 0 && classOf(t.at(pos - 1)) == c0) --pos;
  }
  return pos;
}

int ViEngine::wordEnd(const QString &t, int pos, int count) {
  const int n = t.length();
  for (int c = 0; c < count && pos < n; ++c) {
    ++pos;
    while (pos < n && classOf(t.at(pos)) == 0) ++pos; // skip whitespace
    if (pos >= n) { pos = n; break; }
    int c0 = classOf(t.at(pos));
    while (pos + 1 < n && classOf(t.at(pos + 1)) == c0) ++pos; // to end of this word
  }
  return pos;
}

int ViEngine::findInLine(const QString &t, int cursor, QChar cmd, QChar target) {
  const int ls = lineStartOf(t, cursor);
  const int le = lineEndOf(t, cursor);
  if (cmd == QLatin1Char('f') || cmd == QLatin1Char('t')) {
    for (int i = cursor + 1; i < le; ++i) {
      if (t.at(i) == target) {
        return cmd == QLatin1Char('t') ? i - 1 : i;
      }
    }
  } else { // F / T
    for (int i = cursor - 1; i >= ls; --i) {
      if (t.at(i) == target) {
        return cmd == QLatin1Char('T') ? i + 1 : i;
      }
    }
  }
  return cursor; // not found: no move
}

int ViEngine::WORDForward(const QString &t, int pos, int count) {
  const int n = t.length();
  for (int c = 0; c < count && pos < n; ++c) {
    while (pos < n && !t.at(pos).isSpace()) ++pos; // skip current WORD
    while (pos < n && t.at(pos).isSpace()) ++pos;   // skip whitespace
  }
  return pos;
}

int ViEngine::WORDBackward(const QString &t, int pos, int count) {
  for (int c = 0; c < count && pos > 0; ++c) {
    --pos;
    while (pos > 0 && t.at(pos).isSpace()) --pos;        // skip whitespace
    while (pos > 0 && !t.at(pos - 1).isSpace()) --pos;   // to start of WORD
  }
  return pos;
}

int ViEngine::WORDEnd(const QString &t, int pos, int count) {
  const int n = t.length();
  for (int c = 0; c < count && pos < n; ++c) {
    ++pos;
    while (pos < n && t.at(pos).isSpace()) ++pos;             // skip whitespace
    if (pos >= n) { pos = n; break; }
    while (pos + 1 < n && !t.at(pos + 1).isSpace()) ++pos;    // to end of WORD
  }
  return pos;
}

int ViEngine::matchBracket(const QString &t, int cursor) {
  static const QString opens = QStringLiteral("([{");
  static const QString closes = QStringLiteral(")]}");
  const int n = t.length();
  const int le = lineEndOf(t, cursor);
  // Find the first bracket at/after cursor on this line.
  int i = -1;
  QChar bc;
  for (int p = cursor; p < le; ++p) {
    if (opens.contains(t.at(p)) || closes.contains(t.at(p))) { i = p; bc = t.at(p); break; }
  }
  if (i < 0) return cursor;
  int oi = opens.indexOf(bc);
  if (oi >= 0) { // open -> scan forward
    QChar close = closes.at(oi);
    int depth = 0;
    for (int p = i; p < n; ++p) {
      if (t.at(p) == bc) ++depth;
      else if (t.at(p) == close && --depth == 0) return p;
    }
  } else { // close -> scan backward
    int ci = closes.indexOf(bc);
    QChar open = opens.at(ci);
    int depth = 0;
    for (int p = i; p >= 0; --p) {
      if (t.at(p) == bc) ++depth;
      else if (t.at(p) == open && --depth == 0) return p;
    }
  }
  return cursor; // no match
}

int ViEngine::motionTarget(const QString &t, int cursor, const QString &key, int count) const {
  if (key == QLatin1String("w")) return wordForward(t, cursor, count);
  if (key == QLatin1String("b")) return wordBackward(t, cursor, count);
  if (key == QLatin1String("0")) return lineStartOf(t, cursor);
  if (key == QLatin1String("$")) return lineEndOf(t, cursor);
  if (key == QLatin1String("h")) return qMax(lineStartOf(t, cursor), cursor - count);
  if (key == QLatin1String("l")) return qMin(lineEndOf(t, cursor), cursor + count);
  return -1;
}

ViResult ViEngine::handleKey(const QString &p_text, int p_cursor, int p_selStart, int p_selEnd,
                             const QString &p_keyText, int p_key, int p_mods) {
  ViResult r;
  r.mode = m_mode;
  r.cursor = qBound(0, p_cursor, p_text.length());

  // Let the editor/shortcuts handle Ctrl/Alt/Meta chords.
  const int chord = Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier;
  if (p_mods & chord) {
    return r; // handled = false
  }

  const bool isEsc = (p_key == Qt::Key_Escape);

  if (m_mode == ViMode::Insert) {
    if (isEsc) {
      reset();
      int ls = lineStartOf(p_text, r.cursor);
      r.cursor = (r.cursor > ls) ? r.cursor - 1 : r.cursor;
      r.mode = ViMode::Normal;
      r.handled = true;
    }
    return r; // other keys: editor inserts normally
  }

  if (isEsc) {
    reset();
    r.mode = ViMode::Normal;
    r.anchor = -1;
    r.handled = true;
    return r;
  }

  if (m_mode == ViMode::Visual) {
    return visualKey(p_text, r.cursor, p_selStart, p_selEnd, p_keyText, p_key);
  }

  // Macro control keys (#8h) — handled before recording so they are not recorded.
  if (!m_replaying && m_pendingChar == QChar(0) && m_pendingOp == QChar(0)) {
    if (m_pendingQ) {
      m_pendingQ = false;
      if (!p_keyText.isEmpty() && p_keyText.at(0).isLetter()) {
        m_recording = true;
        m_recordReg = p_keyText.at(0);
        m_macros[m_recordReg].clear();
      }
      r.handled = true;
      return r;
    }
    if (m_pendingAt) {
      m_pendingAt = false;
      if (!p_keyText.isEmpty()) {
        return replayMacro(p_keyText.at(0), p_text, r.cursor, p_selStart, p_selEnd);
      }
      r.handled = true;
      return r;
    }
    if (p_keyText == QLatin1String("q")) {
      if (m_recording) m_recording = false; // stop
      else m_pendingQ = true;               // await register
      r.handled = true;
      return r;
    }
    if (p_keyText == QLatin1String("@")) {
      m_pendingAt = true;
      r.handled = true;
      return r;
    }
  }

  // `.` repeats the last change (#8g) when no multi-key command is pending.
  if (!m_replaying && p_keyText == QLatin1String(".") && m_pendingChar == QChar(0) &&
      m_pendingOp == QChar(0)) {
    return repeatLastChange(p_text, r.cursor, p_selStart, p_selEnd);
  }

  // Record into the active macro register (#8h).
  if (m_recording && !m_replaying) {
    m_macros[m_recordReg].append({p_keyText, p_key, p_mods});
  }

  const bool record = !m_replaying;
  if (record) {
    m_curKeys.append({p_keyText, p_key, p_mods});
  }
  ViResult res = normalKey(p_text, r.cursor, p_keyText, p_key);
  if (record) {
    const bool resting = m_pendingOp == QChar(0) && m_pendingChar == QChar(0) && !m_gPending &&
                         m_count == 0;
    if (resting) {
      // A completed command that edited and stayed in Normal is a repeatable change.
      if (!res.edits.isEmpty() && res.mode == ViMode::Normal) {
        m_lastChange = m_curKeys;
      }
      m_curKeys.clear();
    }
  }
  return res;
}

ViResult ViEngine::repeatLastChange(const QString &t, int cursor, int selStart, int selEnd) {
  ViResult acc;
  acc.handled = true;
  acc.cursor = cursor;
  acc.mode = m_mode;
  acc.anchor = -1;
  if (m_lastChange.isEmpty()) {
    return acc;
  }
  m_replaying = true;
  // Setup keys (d/r/counts) don't move the cursor or edit, so every sub-call sees the
  // same original text/cursor and the editing key's edits are valid against it.
  const QVector<ViKey> keys = m_lastChange;
  for (const ViKey &k : keys) {
    ViResult r = handleKey(t, cursor, selStart, selEnd, k.text, k.key, k.mods);
    acc.edits += r.edits;
    acc.cursor = r.cursor;
    acc.mode = r.mode;
  }
  m_replaying = false;
  return acc;
}

ViResult ViEngine::replayMacro(QChar reg, const QString &t, int cursor, int selStart, int selEnd) {
  ViResult acc;
  acc.handled = true;
  acc.cursor = cursor;
  acc.mode = m_mode;
  acc.anchor = -1;
  auto it = m_macros.constFind(reg);
  if (it == m_macros.constEnd() || it->isEmpty()) {
    return acc; // empty register: no-op
  }
  m_replaying = true;
  // Replay over a local copy, advancing the text/cursor across multiple edits.
  QString cur = t;
  int curPos = cursor, ss = selStart, se = selEnd;
  const QVector<ViKey> keys = *it;
  for (const ViKey &k : keys) {
    ViResult r = handleKey(cur, curPos, ss, se, k.text, k.key, k.mods);
    QVector<ViEdit> edits = r.edits;
    std::sort(edits.begin(), edits.end(),
              [](const ViEdit &a, const ViEdit &b) { return a.start > b.start; });
    for (const ViEdit &e : edits) {
      cur.replace(e.start, e.end - e.start, e.text);
    }
    curPos = qBound(0, r.cursor, cur.length());
    if (r.anchor >= 0) {
      ss = qMin(r.anchor, curPos);
      se = qMax(r.anchor, curPos);
    } else {
      ss = se = curPos;
    }
  }
  m_replaying = false;
  // Return the whole-document change as one edit (multi-step edits already applied locally).
  acc.edits.append({0, static_cast<int>(t.length()), cur});
  acc.cursor = curPos;
  acc.mode = m_mode;
  return acc;
}

ViResult ViEngine::normalKey(const QString &t, int cursor, const QString &key, int qkey) {
  Q_UNUSED(qkey);
  ViResult r;
  r.mode = ViMode::Normal;
  r.cursor = cursor;
  r.handled = true;

  if (key.isEmpty()) {
    r.handled = false;
    return r;
  }
  const QChar ch = key.at(0);

  // Pending-argument command (f/F/t/T/r): this key is the argument (#8e).
  if (m_pendingChar != QChar(0)) {
    const QChar cmd = m_pendingChar;
    m_pendingChar = QChar(0);
    const int n2 = m_count > 0 ? m_count : 1;
    m_count = 0;
    const int le2 = lineEndOf(t, cursor);
    if (cmd == QLatin1Char('r')) {
      int end = qMin(cursor + n2, le2);
      if (end > cursor) {
        r.edits.append({cursor, end, QString(end - cursor, ch)});
        r.cursor = end - 1;
      }
      return r;
    }
    // f / F / t / T
    m_lastFindCmd = cmd;
    m_lastFindTarget = ch;
    int pos = cursor;
    for (int i = 0; i < n2; ++i) pos = findInLine(t, pos, cmd, ch);
    r.cursor = pos;
    return r;
  }

  // Count accumulation (0 only counts when a count is already in progress).
  if (ch.isDigit() && !(ch == QLatin1Char('0') && m_count == 0)) {
    m_count = m_count * 10 + (ch.unicode() - '0');
    m_gPending = false;
    return r; // wait for more
  }
  const int count = m_count > 0 ? m_count : 1;
  const int n = t.length();

  // gg (two g's).
  if (key == QLatin1String("g")) {
    if (m_gPending) { m_gPending = false; m_count = 0; r.cursor = 0; return r; }
    m_gPending = true;
    return r;
  }
  m_gPending = false;

  // Pending operator: a doubled operator is linewise; otherwise apply to a motion.
  if (m_pendingOp != QChar(0)) {
    const QChar op = m_pendingOp;
    m_pendingOp = QChar(0);
    m_count = 0;
    const int ls = lineStartOf(t, cursor);
    // Line indent / outdent: >> and << over `count` lines (#8e).
    if (op == QLatin1Char('>') || op == QLatin1Char('<')) {
      QVector<int> starts;
      int p = ls;
      for (int i = 0; i < count; ++i) {
        starts.append(p);
        int e = lineEndOf(t, p);
        if (e < n) p = e + 1; else break;
      }
      for (int i = starts.size() - 1; i >= 0; --i) {
        int s = starts.at(i);
        if (op == QLatin1Char('>')) {
          r.edits.append({s, s, QStringLiteral("\t")});
        } else if (s < n && t.at(s) == QLatin1Char('\t')) {
          r.edits.append({s, s + 1, QString()});
        } else {
          int sp = 0;
          while (sp < 2 && s + sp < n && t.at(s + sp) == QLatin1Char(' ')) ++sp;
          if (sp > 0) r.edits.append({s, s + sp, QString()});
        }
      }
      r.cursor = (op == QLatin1Char('>')) ? ls + 1 : ls;
      return r;
    }
    if (ch == op) { // dd / yy / cc
      int le = ls;
      for (int i = 0; i < count; ++i) {
        le = lineEndOf(t, le);
        if (le < n) ++le; else break;
      }
      QString slice = t.mid(ls, le - ls);
      if (slice.endsWith(QLatin1Char('\n'))) slice.chop(1);
      m_register = slice;
      m_registerLinewise = true;
      if (op == QLatin1Char('y')) {
        r.cursor = cursor; // yank: no edit
      } else if (op == QLatin1Char('d')) {
        r.edits.append({ls, le, QString()});
        r.cursor = lineStartOf(t, ls); // first line of remainder
      } else { // cc: clear line content, enter insert
        int contentEnd = lineEndOf(t, ls);
        r.edits.append({ls, contentEnd, QString()});
        r.cursor = ls;
        r.mode = ViMode::Insert;
        m_mode = ViMode::Insert;
      }
      return r;
    }
    int to = motionTarget(t, cursor, key, count);
    if (to < 0) { return r; } // unknown motion cancels the operator
    int lo = qMin(cursor, to), hi = qMax(cursor, to);
    m_register = t.mid(lo, hi - lo);
    m_registerLinewise = false;
    if (op == QLatin1Char('y')) {
      r.cursor = lo;
    } else {
      r.edits.append({lo, hi, QString()});
      r.cursor = lo;
      if (op == QLatin1Char('c')) { r.mode = ViMode::Insert; m_mode = ViMode::Insert; }
    }
    return r;
  }

  // Reset count after we use it below (for plain commands).
  auto done = [&]() { m_count = 0; };

  const int ls = lineStartOf(t, cursor);
  const int le = lineEndOf(t, cursor);

  // Mode switches.
  if (key == QLatin1String("i")) { m_mode = ViMode::Insert; r.mode = m_mode; done(); return r; }
  if (key == QLatin1String("a")) {
    r.cursor = qMin(le, cursor + 1); m_mode = ViMode::Insert; r.mode = m_mode; done(); return r;
  }
  if (key == QLatin1String("I")) { r.cursor = ls; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r; }
  if (key == QLatin1String("A")) { r.cursor = le; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r; }
  if (key == QLatin1String("o")) {
    r.edits.append({le, le, QStringLiteral("\n")});
    r.cursor = le + 1; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r;
  }
  if (key == QLatin1String("O")) {
    r.edits.append({ls, ls, QStringLiteral("\n")});
    r.cursor = ls; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r;
  }
  if (key == QLatin1String("v")) {
    m_mode = ViMode::Visual; m_visualAnchor = cursor; r.mode = m_mode;
    r.anchor = cursor; r.cursor = cursor; done(); return r;
  }

  // Operators (await a motion / doubling). >> / << are indent operators (#8e).
  if (key == QLatin1String("d") || key == QLatin1String("y") || key == QLatin1String("c") ||
      key == QLatin1String(">") || key == QLatin1String("<")) {
    m_pendingOp = ch; return r; // keep count
  }

  // vi-advanced (#8e): pending-argument commands wait for the next key.
  if (key == QLatin1String("f") || key == QLatin1String("F") || key == QLatin1String("t") ||
      key == QLatin1String("T") || key == QLatin1String("r")) {
    m_pendingChar = ch; return r; // keep count
  }
  // Repeat last in-line find.
  if (key == QLatin1String(";") || key == QLatin1String(",")) {
    if (!m_lastFindCmd.isNull()) {
      QChar cmd = m_lastFindCmd;
      if (key == QLatin1String(",")) { // reverse direction
        if (cmd == QLatin1Char('f')) cmd = QLatin1Char('F');
        else if (cmd == QLatin1Char('F')) cmd = QLatin1Char('f');
        else if (cmd == QLatin1Char('t')) cmd = QLatin1Char('T');
        else if (cmd == QLatin1Char('T')) cmd = QLatin1Char('t');
      }
      int pos = cursor;
      for (int i = 0; i < count; ++i) pos = findInLine(t, pos, cmd, m_lastFindTarget);
      r.cursor = pos;
    }
    done(); return r;
  }
  // Toggle case of `count` chars and advance.
  if (key == QLatin1String("~")) {
    int end = qMin(cursor + count, le);
    if (end > cursor) {
      QString seg = t.mid(cursor, end - cursor);
      for (int i = 0; i < seg.length(); ++i) {
        QChar c = seg.at(i);
        seg[i] = c.isLower() ? c.toUpper() : c.toLower();
      }
      r.edits.append({cursor, end, seg});
      r.cursor = end;
    }
    done(); return r;
  }
  if (key == QLatin1String("D")) { m_register = t.mid(cursor, le - cursor); m_registerLinewise = false;
    r.edits.append({cursor, le, QString()}); r.cursor = qMax(ls, cursor); done(); return r; }
  if (key == QLatin1String("C")) { m_register = t.mid(cursor, le - cursor); m_registerLinewise = false;
    r.edits.append({cursor, le, QString()}); r.cursor = cursor; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r; }
  if (key == QLatin1String("Y")) { QString s = t.mid(ls, le - ls); m_register = s; m_registerLinewise = true; done(); return r; }

  // Motions.
  if (key == QLatin1String("h")) { r.cursor = qMax(ls, cursor - count); done(); return r; }
  if (key == QLatin1String("l")) { r.cursor = qMin(le, cursor + count); done(); return r; }
  if (key == QLatin1String("0")) { r.cursor = ls; done(); return r; }
  if (key == QLatin1String("$")) { r.cursor = le; done(); return r; }
  if (key == QLatin1String("w")) { r.cursor = wordForward(t, cursor, count); done(); return r; }
  if (key == QLatin1String("b")) { r.cursor = wordBackward(t, cursor, count); done(); return r; }
  if (key == QLatin1String("e")) { r.cursor = wordEnd(t, cursor, count); done(); return r; }
  if (key == QLatin1String("W")) { r.cursor = WORDForward(t, cursor, count); done(); return r; }
  if (key == QLatin1String("B")) { r.cursor = WORDBackward(t, cursor, count); done(); return r; }
  if (key == QLatin1String("E")) { r.cursor = WORDEnd(t, cursor, count); done(); return r; }
  if (key == QLatin1String("%")) { r.cursor = matchBracket(t, cursor); done(); return r; }
  if (key == QLatin1String("s")) { // substitute char(s): delete then insert
    int end = qMin(cursor + count, le);
    if (end > cursor) {
      m_register = t.mid(cursor, end - cursor);
      m_registerLinewise = false;
      r.edits.append({cursor, end, QString()});
    }
    r.cursor = cursor; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r;
  }
  if (key == QLatin1String("S")) { // substitute line: clear content, enter insert
    m_register = t.mid(ls, le - ls);
    m_registerLinewise = true;
    r.edits.append({ls, le, QString()});
    r.cursor = ls; m_mode = ViMode::Insert; r.mode = m_mode; done(); return r;
  }
  if (key == QLatin1String("G")) { r.cursor = lineStartOf(t, n); done(); return r; }
  if (key == QLatin1String("j") || key == QLatin1String("k")) {
    const int col = columnOf(t, cursor);
    int p = cursor;
    for (int i = 0; i < count; ++i) {
      if (key == QLatin1String("j")) {
        int e = lineEndOf(t, p);
        if (e >= n) break;
        p = e + 1;
      } else {
        int s = lineStartOf(t, p);
        if (s == 0) break;
        p = lineStartOf(t, s - 1);
      }
    }
    int targetLs = lineStartOf(t, p), targetLe = lineEndOf(t, p);
    r.cursor = qMin(targetLs + col, targetLe);
    done(); return r;
  }
  if (key == QLatin1String("x")) {
    int end = qMin(cursor + count, le);
    if (end > cursor) {
      m_register = t.mid(cursor, end - cursor); m_registerLinewise = false;
      r.edits.append({cursor, end, QString()});
      int newLe = le - (end - cursor);
      if (cursor > ls && cursor >= newLe) r.cursor = qMax(ls, newLe - 1);
    }
    done(); return r;
  }
  if (key == QLatin1String("p") || key == QLatin1String("P")) {
    if (!m_register.isEmpty()) {
      if (m_registerLinewise) {
        if (key == QLatin1String("p")) {
          r.edits.append({le, le, QStringLiteral("\n") + m_register});
          r.cursor = le + 1;
        } else {
          r.edits.append({ls, ls, m_register + QStringLiteral("\n")});
          r.cursor = ls;
        }
      } else {
        int ip = (key == QLatin1String("p")) ? qMin(le, cursor + 1) : cursor;
        r.edits.append({ip, ip, m_register});
        r.cursor = ip + m_register.length();
      }
    }
    done(); return r;
  }

  // Unrecognized: swallow in Normal mode (do not insert the char).
  done();
  return r;
}

ViResult ViEngine::visualKey(const QString &t, int cursor, int selStart, int selEnd,
                             const QString &key, int qkey) {
  Q_UNUSED(qkey);
  ViResult r;
  r.mode = ViMode::Visual;
  r.handled = true;
  // Keep the engine anchor in sync with the editor's selection if it drifted.
  int anchor = m_visualAnchor;
  if (selStart != selEnd) {
    anchor = (cursor == selEnd) ? selStart : (cursor == selStart ? selEnd : m_visualAnchor);
  }
  m_visualAnchor = anchor;
  r.anchor = anchor;
  r.cursor = cursor;

  if (key.isEmpty()) { r.handled = false; return r; }
  const QChar ch = key.at(0);
  if (ch.isDigit() && !(ch == QLatin1Char('0') && m_count == 0)) {
    m_count = m_count * 10 + (ch.unicode() - '0');
    return r;
  }
  const int count = m_count > 0 ? m_count : 1;
  m_count = 0;

  const int ls = lineStartOf(t, cursor);
  const int le = lineEndOf(t, cursor);
  int nc = cursor;
  if (key == QLatin1String("h")) nc = qMax(ls, cursor - count);
  else if (key == QLatin1String("l")) nc = qMin(le, cursor + count);
  else if (key == QLatin1String("0")) nc = ls;
  else if (key == QLatin1String("$")) nc = le;
  else if (key == QLatin1String("w")) nc = wordForward(t, cursor, count);
  else if (key == QLatin1String("b")) nc = wordBackward(t, cursor, count);
  else if (key == QLatin1String("j") || key == QLatin1String("k")) {
    const int col = columnOf(t, cursor);
    int p = cursor;
    if (key == QLatin1String("j")) { int e = lineEndOf(t, p); if (e < t.length()) p = e + 1; }
    else { int s = lineStartOf(t, p); if (s > 0) p = lineStartOf(t, s - 1); }
    nc = qMin(lineStartOf(t, p) + col, lineEndOf(t, p));
  } else if (key == QLatin1String("d") || key == QLatin1String("x") || key == QLatin1String("y")
             || key == QLatin1String("c")) {
    int lo = qMin(anchor, cursor), hi = qMax(anchor, cursor);
    m_register = t.mid(lo, hi - lo);
    m_registerLinewise = false;
    if (key == QLatin1String("y")) {
      r.cursor = lo;
    } else {
      r.edits.append({lo, hi, QString()});
      r.cursor = lo;
    }
    r.anchor = -1;
    if (key == QLatin1String("c")) { m_mode = ViMode::Insert; r.mode = ViMode::Insert; }
    else { m_mode = ViMode::Normal; r.mode = ViMode::Normal; }
    return r;
  } else {
    return r; // ignore other keys in visual
  }

  r.cursor = nc;
  r.anchor = anchor;
  return r;
}
