#include "historymgr.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include <core/configmgr.h>
#include <core/sessionconfig.h>

using namespace markly;

const int HistoryMgr::c_maxHistory = 50;

HistoryMgr::HistoryMgr(QObject *p_parent) : QObject(p_parent) {}

void HistoryMgr::load() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  m_history = session.getHistory();
  m_quickAccess = session.getQuickAccess();
  emit changed();
}

void HistoryMgr::persist() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  session.setHistory(m_history);
  session.setQuickAccess(m_quickAccess);
}

void HistoryMgr::addToHistory(const QString &p_path) {
  if (p_path.isEmpty()) {
    return;
  }
  m_history.removeAll(p_path);
  m_history.prepend(p_path);
  while (m_history.size() > c_maxHistory) {
    m_history.removeLast();
  }
  persist();
  emit changed();
}

void HistoryMgr::pin(const QString &p_path) {
  if (p_path.isEmpty() || m_quickAccess.contains(p_path)) {
    return;
  }
  m_quickAccess.prepend(p_path);
  persist();
  emit changed();
}

void HistoryMgr::unpin(const QString &p_path) {
  if (m_quickAccess.removeAll(p_path) > 0) {
    persist();
    emit changed();
  }
}

QString HistoryMgr::ensureFlashFile() const {
  const auto dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir().mkpath(dir);
  const auto path = dir + QStringLiteral("/flash.md");
  if (!QFile::exists(path)) {
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
      f.write(QByteArray("# \xe9\x97\xaa\xe5\xbf\xb5\n\n")); // "# 闪念\n\n"
      f.close();
    }
  }
  return path;
}
