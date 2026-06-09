#include "quickbridge.h"

#include <QFileInfo>
#include <QVariantMap>

#include <core/historymgr.h>
#include <core/marklyapp.h>

using namespace markly;

QuickBridge::QuickBridge(HistoryMgr *p_mgr, QObject *p_parent)
    : QObject(p_parent), m_mgr(p_mgr) {
  if (m_mgr) {
    connect(m_mgr, &HistoryMgr::changed, this, &QuickBridge::changed);
  }
}

static QVariantList toItems(const QStringList &p_paths, bool p_existingOnly) {
  QVariantList list;
  for (const auto &path : p_paths) {
    if (p_existingOnly && !QFileInfo::exists(path)) {
      continue;
    }
    QVariantMap m;
    m[QStringLiteral("name")] = QFileInfo(path).fileName();
    m[QStringLiteral("path")] = path;
    list.append(m);
  }
  return list;
}

QVariantList QuickBridge::history() const {
  return m_mgr ? toItems(m_mgr->history(), true) : QVariantList();
}

QVariantList QuickBridge::quickAccess() const {
  return m_mgr ? toItems(m_mgr->quickAccess(), false) : QVariantList();
}

void QuickBridge::open(const QString &p_path) {
  if (!p_path.isEmpty() && QFileInfo::exists(p_path)) {
    emit MarklyApp::getInst().openFileRequested(p_path);
  }
}

void QuickBridge::pin(const QString &p_path) {
  if (m_mgr) {
    m_mgr->pin(p_path);
  }
}

void QuickBridge::unpin(const QString &p_path) {
  if (m_mgr) {
    m_mgr->unpin(p_path);
  }
}

bool QuickBridge::isPinned(const QString &p_path) const {
  return m_mgr && m_mgr->isPinned(p_path);
}

void QuickBridge::openFlash() {
  if (m_mgr) {
    emit MarklyApp::getInst().openFileRequested(m_mgr->ensureFlashFile());
  }
}
