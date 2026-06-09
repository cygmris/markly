#include "marklyapp.h"

#include <QCoreApplication>
#include <QDateTime>

#include "buffer/buffermgr.h"
#include "historymgr.h"
#include "notebookmgr.h"
#include "thememgr.h"

using namespace markly;

MarklyApp::MarklyApp(QObject *p_parent) : QObject(p_parent) {
  m_instanceId = static_cast<ID>(QDateTime::currentMSecsSinceEpoch())
                 ^ static_cast<ID>(QCoreApplication::applicationPid());

  initThemeMgr();
  initNotebookMgr();
  initBufferMgr();
  m_historyMgr = new HistoryMgr(this);
  // Every open (explorer / search / tags / history) records into history.
  connect(this, &MarklyApp::openFileRequested, m_historyMgr, &HistoryMgr::addToHistory);
}

void MarklyApp::initThemeMgr() { m_themeMgr = new ThemeMgr(this); }

void MarklyApp::initNotebookMgr() { m_notebookMgr = new NotebookMgr(this); }

void MarklyApp::initBufferMgr() { m_bufferMgr = new BufferMgr(this); }

void MarklyApp::initLoad() {
  // Restore previously open notebooks (good to call after MainWindow is shown).
  m_notebookMgr->loadNotebooks();
  m_historyMgr->load();
}

HistoryMgr *MarklyApp::getHistoryMgr() const { return m_historyMgr; }

ThemeMgr &MarklyApp::getThemeMgr() const { return *m_themeMgr; }

NotebookMgr *MarklyApp::getNotebookMgr() const { return m_notebookMgr; }

BufferMgr *MarklyApp::getBufferMgr() const { return m_bufferMgr; }

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
