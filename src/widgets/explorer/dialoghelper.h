#ifndef DIALOGHELPER_H
#define DIALOGHELPER_H

#include <QObject>
#include <QString>

class QWidget;

namespace markly {
// Exposes native dialogs to QML (context property "Dialogs"). All return empty /
// false on cancel.
class DialogHelper : public QObject {
  Q_OBJECT
public:
  explicit DialogHelper(QWidget *p_parent, QObject *p_objParent = nullptr);

  Q_INVOKABLE QString chooseDirectory(const QString &p_title);
  Q_INVOKABLE QString saveFile(const QString &p_title, const QString &p_defaultName,
                               const QString &p_filter);
  Q_INVOKABLE QString promptText(const QString &p_title, const QString &p_label,
                                 const QString &p_default);
  Q_INVOKABLE QString chooseColor(const QString &p_initial);
  Q_INVOKABLE bool confirm(const QString &p_title, const QString &p_text);
  Q_INVOKABLE void notify(const QString &p_title, const QString &p_text);

private:
  QWidget *m_parent = nullptr;
};
} // namespace markly

#endif // DIALOGHELPER_H
