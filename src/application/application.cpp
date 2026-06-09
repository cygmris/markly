#include "application.h"

#include <QDebug>
#include <QDir>
#include <QFileOpenEvent>
#include <QFileSystemWatcher>
#include <QStyle>
#include <QTimer>

#include <core/marklyapp.h>
#include <core/thememgr.h>

using namespace markly;

Application::Application(int &p_argc, char **p_argv) : QApplication(p_argc, p_argv) {}

void Application::watchThemeFolder(const QString &p_themeFolderPath) {
  if (p_themeFolderPath.isEmpty()) {
    return;
  }

  if (!m_styleWatcher) {
    m_styleWatcher = new QFileSystemWatcher(this);
  }
  if (!m_reloadTimer) {
    m_reloadTimer = new QTimer(this);
    m_reloadTimer->setSingleShot(true);
    m_reloadTimer->setInterval(500); // 500ms debounce.
    connect(m_reloadTimer, &QTimer::timeout, this, &Application::reloadThemeResources);

    connect(m_styleWatcher, &QFileSystemWatcher::directoryChanged, m_reloadTimer,
            qOverload<>(&QTimer::start));
    connect(m_styleWatcher, &QFileSystemWatcher::fileChanged, m_reloadTimer,
            qOverload<>(&QTimer::start));
  }

  m_styleWatcher->addPath(p_themeFolderPath);

  QDir themeDir(p_themeFolderPath);
  const QStringList files = themeDir.entryList(QDir::Files);
  for (const QString &file : files) {
    m_styleWatcher->addPath(themeDir.filePath(file));
  }
}

void Application::reloadThemeResources() {
  auto &themeMgr = MarklyApp::getInst().getThemeMgr();
  themeMgr.refreshCurrentTheme();

  auto stylesheet = themeMgr.fetchQtStyleSheet();
  if (!stylesheet.isEmpty()) {
    setStyleSheet(stylesheet);
    style()->unpolish(this);
    style()->polish(this);
  }
}

bool Application::event(QEvent *p_event) {
  // On macOS, this is how files opened from Finder arrive.
  if (p_event->type() == QEvent::FileOpen) {
    QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(p_event);
    qDebug() << "request to open file" << openEvent->file();
    emit openFileRequested(openEvent->file());
  }

  return QApplication::event(p_event);
}
