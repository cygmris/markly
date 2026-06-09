#ifndef MARKLY_EDITINPUTQML_H
#define MARKLY_EDITINPUTQML_H

#include <QObject>
#include <QVariantMap>

namespace markly {
// QML bridge over EditInputHelper (context property "EditInput"). Exposes the
// pure-text editing helpers so the QML markdown editor can apply them to its TextEdit.
class EditInputQml : public QObject {
  Q_OBJECT
public:
  explicit EditInputQml(QObject *p_parent = nullptr);

  // Returns { clearMarker: bool, lineIndent: string, insert: string }.
  Q_INVOKABLE QVariantMap continueOnEnter(const QString &p_curLine, bool p_autoIndent,
                                          bool p_continueList) const;
  Q_INVOKABLE QString indentLines(const QString &p_block, const QString &p_tabUnit) const;
  Q_INVOKABLE QString outdentLines(const QString &p_block, const QString &p_tabUnit) const;
  // Matching close char for an auto-pair opener, or "" if none.
  Q_INVOKABLE QString pairFor(const QString &p_open) const;
};
} // namespace markly

#endif // MARKLY_EDITINPUTQML_H
