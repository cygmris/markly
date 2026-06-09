#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>

class QQuickWidget;

namespace markly {
// Placeholder main window. Currently hosts the theme-preview QML harness
// (theme-appearance spec) via a QQuickWidget to prove live theming + the
// QML<->QWidget bridge.
// NOTE: spec #3 (app-shell-ui) replaces the content with the frameless self-drawn
// window hosting the full QML shell (title bar / activity rail / docks / status bar).
class MainWindow : public QMainWindow {
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

  QQuickWidget *m_quick = nullptr;
};
} // namespace markly

#endif // MAINWINDOW_H
