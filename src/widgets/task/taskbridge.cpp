#include "taskbridge.h"

#include <QVariantMap>

#include <core/notebook/notebook.h>
#include <core/notebookmgr.h>
#include <core/task/taskmgr.h>

using namespace markly;

TaskBridge::TaskBridge(TaskMgr *p_taskMgr, NotebookMgr *p_notebookMgr, QObject *p_parent)
    : QObject(p_parent), m_taskMgr(p_taskMgr), m_notebookMgr(p_notebookMgr) {
  if (m_taskMgr) {
    connect(m_taskMgr, &TaskMgr::changed, this, &TaskBridge::changed);
  }
}

QVariantList TaskBridge::list() const {
  QVariantList out;
  if (!m_taskMgr) {
    return out;
  }
  for (const auto &t : m_taskMgr->tasks()) {
    QVariantMap m;
    m[QStringLiteral("name")] = t.m_name;
    out.append(m);
  }
  return out;
}

QString TaskBridge::run(const QString &p_name, const QString &p_bufferDir,
                        const QString &p_bufferName) {
  if (!m_taskMgr) {
    return QString();
  }
  TaskContext ctx;
  ctx.m_bufferDir = p_bufferDir;
  ctx.m_bufferName = p_bufferName;
  if (!p_bufferDir.isEmpty() && !p_bufferName.isEmpty()) {
    ctx.m_bufferPath = p_bufferDir + QLatin1Char('/') + p_bufferName;
  }
  if (m_notebookMgr) {
    if (auto nb = m_notebookMgr->getCurrentNotebook()) {
      ctx.m_notebookFolder = nb->getRootPath();
    }
  }
  int exitCode = -1;
  const auto output = m_taskMgr->run(p_name, ctx, exitCode);
  return QStringLiteral("[退出码 %1]\n%2").arg(exitCode).arg(output);
}
