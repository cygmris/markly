#include "updatebridge.h"

#include <QCoreApplication>
#include <QDateTime>

#include <core/configmgr.h>
#include <core/widgetconfig.h>

using namespace markly;

namespace {
// Placeholder release repository — replace with the real one before shipping.
const char *c_owner = "markly";
const char *c_repo = "markly";
} // namespace

UpdateBridge::UpdateBridge(QObject *p_parent) : QObject(p_parent) {
  connect(&m_checker, &UpdateChecker::result, this,
          [this](bool hasUpdate, const QString &latest, const QString &error) {
            emit checked(hasUpdate, latest, error);
          });
}

void UpdateBridge::check() {
  m_checker.check(QString::fromLatin1(c_owner), QString::fromLatin1(c_repo),
                  QCoreApplication::applicationVersion());
}

void UpdateBridge::autoCheckIfDue() {
  auto &wc = ConfigMgr::getInst().getWidgetConfig();
  if (!wc.getAutoUpdateCheck()) {
    return;
  }
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  if (UpdateChecker::shouldCheck(wc.getLastUpdateCheck(), now, 7)) {
    wc.setLastUpdateCheck(now);
    check();
  }
}
