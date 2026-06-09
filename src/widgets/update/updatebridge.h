#ifndef MARKLY_UPDATEBRIDGE_H
#define MARKLY_UPDATEBRIDGE_H

#include <QObject>
#include <QString>

#include <core/update/updatechecker.h>

namespace markly {
// QML bridge (context property "Update", #20b). The "检查更新" command calls check();
// the result is relayed to QML which notifies the user. The owner/repo below are
// placeholders — point them at the real release repository before shipping.
class UpdateBridge : public QObject {
  Q_OBJECT
public:
  explicit UpdateBridge(QObject *p_parent = nullptr);

  Q_INVOKABLE void check();

  // Check only if auto-update is enabled and the 7-day interval has elapsed (#20c).
  Q_INVOKABLE void autoCheckIfDue();

signals:
  void checked(bool p_hasUpdate, const QString &p_latest, const QString &p_error);

private:
  UpdateChecker m_checker;
};
} // namespace markly

#endif // MARKLY_UPDATEBRIDGE_H
