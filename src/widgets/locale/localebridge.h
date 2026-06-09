#ifndef MARKLY_LOCALEBRIDGE_H
#define MARKLY_LOCALEBRIDGE_H

#include <QObject>
#include <QString>

class QQmlEngine;

namespace markly {
// Language switch bridge (context property "Locale", #20). Persists the choice,
// reinstalls the translator, and retranslates the live QML so UI updates immediately.
class LocaleBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString language READ language NOTIFY changed)
public:
  explicit LocaleBridge(QQmlEngine *p_engine, QObject *p_parent = nullptr);

  QString language() const;
  Q_INVOKABLE void setLanguage(const QString &p_language);

signals:
  void changed();

private:
  QQmlEngine *m_engine = nullptr;
};
} // namespace markly

#endif // MARKLY_LOCALEBRIDGE_H
