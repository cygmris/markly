#include "marklyapp.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QTranslator>

#include "configmgr.h"
#include "spell/spellchecker.h"
#include "widgetconfig.h"

#include "buffer/buffermgr.h"
#include "historymgr.h"
#include "notebookmgr.h"
#include "snippet/snippetmgr.h"
#include "task/taskmgr.h"
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
  m_snippetMgr = new SnippetMgr(this);
  m_taskMgr = new TaskMgr(this);
}

void MarklyApp::initThemeMgr() { m_themeMgr = new ThemeMgr(this); }

void MarklyApp::initNotebookMgr() { m_notebookMgr = new NotebookMgr(this); }

void MarklyApp::initBufferMgr() { m_bufferMgr = new BufferMgr(this); }

void MarklyApp::initLoad() {
  // Restore previously open notebooks (good to call after MainWindow is shown).
  m_notebookMgr->loadNotebooks();
  m_historyMgr->load();
  m_snippetMgr->load();
  m_taskMgr->load();
}

HistoryMgr *MarklyApp::getHistoryMgr() const { return m_historyMgr; }

SnippetMgr *MarklyApp::getSnippetMgr() const { return m_snippetMgr; }

void MarklyApp::applyLanguage(const QString &p_language) {
  // Resolve "auto" to the system locale name.
  QString resolved = p_language;
  if (resolved.isEmpty() || resolved == QStringLiteral("auto")) {
    resolved = QLocale::system().name(); // e.g. "en_US", "zh_CN"
  }

  if (m_translator) {
    QCoreApplication::removeTranslator(m_translator);
    delete m_translator;
    m_translator = nullptr;
  }

  // Chinese is the source language — no .qm needed; fall back to source strings.
  if (resolved.startsWith(QStringLiteral("zh"))) {
    return;
  }

  // Map to a packaged catalog (only en_US shipped for now).
  QString catalog = resolved.startsWith(QStringLiteral("en")) ? QStringLiteral("en_US")
                                                              : resolved;
  auto *tr = new QTranslator(this);
  if (tr->load(QStringLiteral(":/i18n/markly_%1.qm").arg(catalog))) {
    QCoreApplication::installTranslator(tr);
    m_translator = tr;
  } else {
    delete tr;
  }
}

ThemeMgr &MarklyApp::getThemeMgr() const { return *m_themeMgr; }

NotebookMgr *MarklyApp::getNotebookMgr() const { return m_notebookMgr; }

BufferMgr *MarklyApp::getBufferMgr() const { return m_bufferMgr; }

TaskMgr *MarklyApp::getTaskMgr() const { return m_taskMgr; }

SpellChecker *MarklyApp::getSpellChecker() {
  if (m_spellTried) {
    return m_spell;
  }
  m_spellTried = true;

  // Dictionary directory resolution: MARKLY_DICT_DIR env > user <config>/dicts >
  // app data/dicts. The first directory containing en_US.dic wins.
  QStringList candidates;
  const QByteArray envDir = qgetenv("MARKLY_DICT_DIR");
  if (!envDir.isEmpty()) {
    candidates << QString::fromLocal8Bit(envDir);
  }
  candidates << ConfigMgr::getInst().getUserDictsFolder();
  candidates << ConfigMgr::getInst().getAppDictsFolder();

  for (const QString &dir : candidates) {
    if (dir.isEmpty()) {
      continue;
    }
    if (QFileInfo::exists(dir + QStringLiteral("/en_US.dic"))) {
      m_spell = new SpellChecker(dir, QStringLiteral("en_US"));
      if (m_spell->ready()) {
        return m_spell;
      }
      delete m_spell;
      m_spell = nullptr;
    }
  }
  return m_spell; // null -> highlighter degrades gracefully
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
