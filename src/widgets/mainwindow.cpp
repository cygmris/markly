#include "mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QLabel>

#include <core/configmgr.h>
#include <core/sessionconfig.h>

using namespace markly;

MainWindow::MainWindow(QWidget *p_parent) : QMainWindow(p_parent) {
  setWindowTitle(QStringLiteral("Markly"));

  auto label = new QLabel(QStringLiteral("Markly — foundation"), this);
  label->setAlignment(Qt::AlignCenter);
  setCentralWidget(label);

  resize(1100, 720);
  loadStateAndGeometry();
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
