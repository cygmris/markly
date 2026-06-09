#include "editinputqml.h"

#include <core/editor/editinputhelper.h>

using namespace markly;

EditInputQml::EditInputQml(QObject *p_parent) : QObject(p_parent) {}

QVariantMap EditInputQml::continueOnEnter(const QString &p_curLine, bool p_autoIndent,
                                          bool p_continueList) const {
  const auto r = EditInputHelper::continueOnEnter(p_curLine, p_autoIndent, p_continueList);
  QVariantMap m;
  m[QStringLiteral("clearMarker")] = r.clearMarker;
  m[QStringLiteral("lineIndent")] = r.lineIndent;
  m[QStringLiteral("insert")] = r.insert;
  return m;
}

QString EditInputQml::indentLines(const QString &p_block, const QString &p_tabUnit) const {
  return EditInputHelper::indentLines(p_block, p_tabUnit);
}

QString EditInputQml::outdentLines(const QString &p_block, const QString &p_tabUnit) const {
  return EditInputHelper::outdentLines(p_block, p_tabUnit);
}

QString EditInputQml::pairFor(const QString &p_open) const {
  if (p_open.isEmpty()) {
    return QString();
  }
  const QChar c = EditInputHelper::pairFor(p_open.at(0));
  return c.unicode() == 0 ? QString() : QString(c);
}
