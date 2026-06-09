#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStringList>

#include "framelessmainwindow/framelessmainwindow.h"

class QQuickWidget;
class QSystemTrayIcon;

namespace markly {
class ViewArea;
class GlobalHotkey;

// Frameless main window hosting the full QML shell (MarklyShell.qml): title bar /
// activity rail / docks / status bar, with live A/B/C style + theme switching.
class MainWindow : public FramelessMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *p_parent = nullptr);

  // Handle the initial paths passed on the command line.
  void kickOffOnStart(const QStringList &p_paths);

public slots:
  // Bring the window to the front (single-instance "show" request).
  void showMainWindow();

  // Open the given files (single-instance / Finder request).
  void openFiles(const QStringList &p_files);

protected:
  void closeEvent(QCloseEvent *p_event) Q_DECL_OVERRIDE;

private:
  void loadStateAndGeometry();

  void saveStateAndGeometry();

  void setupContent();

  void setupTray();

  QQuickWidget *m_quick = nullptr;
  ViewArea *m_views = nullptr;
  QSystemTrayIcon *m_trayIcon = nullptr;
  GlobalHotkey *m_hotkey = nullptr;
};
} // namespace markly

#endif // MAINWINDOW_H
