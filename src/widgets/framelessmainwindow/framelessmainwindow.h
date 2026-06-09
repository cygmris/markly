#ifndef FRAMELESSMAINWINDOW_H
#define FRAMELESSMAINWINDOW_H

#include <QMainWindow>

namespace markly {
// Frameless main window with a self-drawn title bar. Window move/resize are
// delegated to the windowing system via startSystemMove/Resize, driven from QML
// (the title bar and edge strips call these Q_INVOKABLE methods through the
// injected "Win" context object). Linux (X11/Wayland) first; degrades gracefully.
class FramelessMainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit FramelessMainWindow(QWidget *p_parent = nullptr);

  // Called from QML (Win.*).
  Q_INVOKABLE void startMove();

  // @p_edges is a Qt::Edges flag value (passed as int from QML).
  Q_INVOKABLE void startResize(int p_edges);

  Q_INVOKABLE void toggleMaximize();

  Q_INVOKABLE void minimizeWindow();

  Q_INVOKABLE void closeWindow();

  Q_INVOKABLE bool isMaximizedWindow() const;

signals:
  void maximizedChanged(bool p_maximized);

protected:
  bool event(QEvent *p_event) Q_DECL_OVERRIDE;
};
} // namespace markly

#endif // FRAMELESSMAINWINDOW_H
