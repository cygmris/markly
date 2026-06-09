#include "editinputhelper.h"

#include <QRegularExpression>

using namespace markly;

EditInputHelper::EnterResult EditInputHelper::continueOnEnter(const QString &p_curLine,
                                                              bool p_autoIndent,
                                                              bool p_continueList) {
  EnterResult res;

  // Leading whitespace (indent).
  static const QRegularExpression indentRe(QStringLiteral("^([ \\t]*)"));
  const QString indent = indentRe.match(p_curLine).captured(1);

  if (p_continueList) {
    // Unordered / task list: indent, bullet, optional checkbox, content.
    static const QRegularExpression ulRe(
        QStringLiteral("^([ \\t]*)([-*+])[ \\t]+(\\[[ xX]\\][ \\t]+)?(.*)$"));
    // Ordered list: indent, number, separator (. or )), content.
    static const QRegularExpression olRe(
        QStringLiteral("^([ \\t]*)(\\d+)([.)])[ \\t]+(.*)$"));

    auto ul = ulRe.match(p_curLine);
    if (ul.hasMatch()) {
      const QString bullet = ul.captured(2);
      const bool isTask = !ul.captured(3).isEmpty();
      const QString content = ul.captured(4);
      if (content.trimmed().isEmpty()) {
        res.clearMarker = true;
        res.lineIndent = indent;
        return res;
      }
      res.insert = QStringLiteral("\n") + indent + bullet + QStringLiteral(" ") +
                   (isTask ? QStringLiteral("[ ] ") : QString());
      return res;
    }

    auto ol = olRe.match(p_curLine);
    if (ol.hasMatch()) {
      const QString content = ol.captured(4);
      if (content.trimmed().isEmpty()) {
        res.clearMarker = true;
        res.lineIndent = indent;
        return res;
      }
      const int next = ol.captured(2).toInt() + 1;
      res.insert = QStringLiteral("\n") + indent + QString::number(next) + ol.captured(3) +
                   QStringLiteral(" ");
      return res;
    }
  }

  // Plain line: keep indent when auto-indent is on.
  res.insert = QStringLiteral("\n") + (p_autoIndent ? indent : QString());
  return res;
}

QString EditInputHelper::indentLines(const QString &p_block, const QString &p_tabUnit) {
  const auto lines = p_block.split(QLatin1Char('\n'));
  QStringList out;
  out.reserve(lines.size());
  for (const auto &line : lines) {
    out.append(p_tabUnit + line);
  }
  return out.join(QLatin1Char('\n'));
}

QString EditInputHelper::outdentLines(const QString &p_block, const QString &p_tabUnit) {
  const int width = p_tabUnit.isEmpty() ? 4 : p_tabUnit.size();
  const auto lines = p_block.split(QLatin1Char('\n'));
  QStringList out;
  out.reserve(lines.size());
  for (auto line : lines) {
    if (line.startsWith(QLatin1Char('\t'))) {
      line.remove(0, 1);
    } else {
      int n = 0;
      while (n < width && n < line.size() && line.at(n) == QLatin1Char(' ')) {
        ++n;
      }
      line.remove(0, n);
    }
    out.append(line);
  }
  return out.join(QLatin1Char('\n'));
}

QChar EditInputHelper::pairFor(QChar p_open) {
  switch (p_open.unicode()) {
  case '(':
    return QLatin1Char(')');
  case '[':
    return QLatin1Char(']');
  case '{':
    return QLatin1Char('}');
  case '"':
    return QLatin1Char('"');
  case '`':
    return QLatin1Char('`');
  default:
    return QChar(u'\0');
  }
}
