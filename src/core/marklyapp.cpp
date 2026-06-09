#include "marklyapp.h"

#include <QCoreApplication>
#include <QDateTime>

#include "thememgr.h"

using namespace markly;

MarklyApp::MarklyApp(QObject *p_parent) : QObject(p_parent) {
  m_instanceId = static_cast<ID>(QDateTime::currentMSecsSinceEpoch())
                 ^ static_cast<ID>(QCoreApplication::applicationPid());

  initThemeMgr();
}

void MarklyApp::initThemeMgr() { m_themeMgr = new ThemeMgr(this); }

void MarklyApp::initLoad() {
  // Foundation phase: nothing heavy to load yet. Later specs load notebooks here.
}

ThemeMgr &MarklyApp::getThemeMgr() const { return *m_themeMgr; }

NotebookMgr *MarklyApp::getNotebookMgr() const {
  // Installed by spec #4 (notebook-core).
  return nullptr;
}

BufferMgr *MarklyApp::getBufferMgr() const {
  // Installed by spec #6 (buffer-view-area).
  return nullptr;
}

TaskMgr *MarklyApp::getTaskMgr() const {
  // Installed by spec #16 (task-system).
  return nullptr;
}

void MarklyApp::setMainWindow(MainWindow *p_mainWindow) { m_mainWindow = p_mainWindow; }

MainWindow *MarklyApp::getMainWindow() const { return m_mainWindow; }

ID MarklyApp::getInstanceId() const { return m_instanceId; }

void MarklyApp::showStatusMessage(const QString &p_message, int p_timeoutMilliseconds) {
  emit statusMessageRequested(p_message, p_timeoutMilliseconds);
}

void MarklyApp::showStatusMessageShort(const QString &p_message) {
  showStatusMessage(p_message, 3000);
}

void MarklyApp::showTips(const QString &p_message, int p_timeoutMilliseconds) {
  emit tipsRequested(p_message, p_timeoutMilliseconds);
}
