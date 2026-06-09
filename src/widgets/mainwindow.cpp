#include "mainwindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QQmlContext>
#include <QQuickWidget>
#include <QUrl>

#include <core/configmgr.h>
#include <core/marklyapp.h>
#include <core/sessionconfig.h>
#include <core/theme/appearance.h>
#include <core/thememgr.h>

using namespace markly;

MainWindow::MainWindow(QWidget *p_parent) : QMainWindow(p_parent) {
  setWindowTitle(QStringLiteral("Markly"));

  setupContent();

  resize(1100, 720);
  loadStateAndGeometry();
}

void MainWindow::setupContent() {
  auto &themeMgr = MarklyApp::getInst().getThemeMgr();

  m_quick = new QQuickWidget(this);
  m_quick->setResizeMode(QQuickWidget::SizeRootObjectToView);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Theme"), &themeMgr);
  m_quick->rootContext()->setContextProperty(QStringLiteral("Appearance"),
                                             themeMgr.getAppearance());
  m_quick->setSource(QUrl(QStringLiteral("qrc:/qml/theme/ThemePreview.qml")));
  if (m_quick->status() == QQuickWidget::Error) {
    qCritical() << "failed to load ThemePreview.qml:" << m_quick->errors();
  }
  setCentralWidget(m_quick);

  // Keep the QWidget chrome in sync with the active theme.
  connect(&themeMgr, &ThemeMgr::themeChanged, this, []() {
    qApp->setStyleSheet(MarklyApp::getInst().getThemeMgr().fetchQtStyleSheet());
  });
}

void MainWindow::loadStateAndGeometry() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  const auto geometry = session.getMainWindowGeometry();
  if (!geometry.isEmpty()) {
    restoreGeometry(geometry);
  }
  const auto state = session.getMainWindowState();
  if (!state.isEmpty()) {
    restoreState(state);
  }
}

void MainWindow::saveStateAndGeometry() {
  auto &session = ConfigMgr::getInst().getSessionConfig();
  session.setMainWindowGeometry(saveGeometry());
  session.setMainWindowState(saveState());
}

void MainWindow::kickOffOnStart(const QStringList &p_paths) {
  if (!p_paths.isEmpty()) {
    openFiles(p_paths);
  }
}

void MainWindow::showMainWindow() {
  setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
  show();
  raise();
  activateWindow();
}

void MainWindow::openFiles(const QStringList &p_files) {
  // Foundation phase: real opening lands in spec #6 (buffer-view-area).
  for (const auto &file : p_files) {
    qInfo() << "request to open file" << file;
  }
}

void MainWindow::closeEvent(QCloseEvent *p_event) {
  saveStateAndGeometry();
  QMainWindow::closeEvent(p_event);
}
