#include "framelessmainwindow.h"

#include <QEvent>
#include <QWindow>

using namespace markly;

FramelessMainWindow::FramelessMainWindow(QWidget *p_parent) : QMainWindow(p_parent) {
  setWindowFlag(Qt::FramelessWindowHint, true);
  // Keep a normal window with translucent-free background; the QML shell paints
  // the whole surface including the title bar.
}

void FramelessMainWindow::startMove() {
  if (auto handle = windowHandle()) {
    handle->startSystemMove();
  }
}

void FramelessMainWindow::startResize(int p_edges) {
  if (auto handle = windowHandle()) {
    handle->startSystemResize(static_cast<Qt::Edges>(p_edges));
  }
}

void FramelessMainWindow::toggleMaximize() {
  if (isMaximized()) {
    showNormal();
  } else {
    showMaximized();
  }
}

void FramelessMainWindow::minimizeWindow() { showMinimized(); }

void FramelessMainWindow::closeWindow() { close(); }

bool FramelessMainWindow::isMaximizedWindow() const { return isMaximized(); }

bool FramelessMainWindow::event(QEvent *p_event) {
  if (p_event->type() == QEvent::WindowStateChange) {
    emit maximizedChanged(isMaximized());
  }
  return QMainWindow::event(p_event);
}
