#ifndef MARKLYAPP_H
#define MARKLYAPP_H

#include <QObject>

#include "global.h"
#include "noncopyable.h"

namespace markly {
class MainWindow;
class ThemeMgr;
class NotebookMgr;
class BufferMgr;
class TaskMgr;

// Central singleton coordinator (counterpart of VNote's VNoteX).
// Owns the managers and serves as the global signal hub for the app.
// NOTE: foundation wires only ThemeMgr (placeholder). NotebookMgr/BufferMgr/TaskMgr
// accessors return nullptr until specs #4/#6/#16 install the real managers, and the
// Node/Event-based signals are added in those specs (kept out here to avoid depending
// on types that do not exist yet).
class MarklyApp : public QObject, private Noncopyable {
  Q_OBJECT
public:
  static MarklyApp &getInst() {
    static MarklyApp inst;
    return inst;
  }

  // MUST be called to load heavy data. Good to call after MainWindow is shown.
  void initLoad();

  ThemeMgr &getThemeMgr() const;

  // Placeholders until the owning specs land.
  NotebookMgr *getNotebookMgr() const;
  BufferMgr *getBufferMgr() const;
  TaskMgr *getTaskMgr() const;

  void setMainWindow(MainWindow *p_mainWindow);
  MainWindow *getMainWindow() const;

  ID getInstanceId() const;

public slots:
  void showStatusMessage(const QString &p_message, int p_timeoutMilliseconds = 0);

  void showStatusMessageShort(const QString &p_message);

  void showTips(const QString &p_message, int p_timeoutMilliseconds = 3000);

signals:
  void newNotebookRequested();

  void newNotebookFromFolderRequested();

  void importNotebookRequested();

  void manageNotebooksRequested();

  void importFileRequested();

  void importFolderRequested();

  void newNoteRequested();

  void newQuickNoteRequested();

  void newFolderRequested();

  void showOutputRequested(const QString &p_text);

  void statusMessageRequested(const QString &p_message, int p_timeoutMilliseconds);

  void tipsRequested(const QString &p_message, int p_timeoutMilliseconds);

  // Simplified open-file signal (FileOpenParameters added in the buffer spec #6).
  void openFileRequested(const QString &p_filePath);

  void exportRequested();

  void pinToQuickAccessRequested(const QStringList &p_files);

private:
  explicit MarklyApp(QObject *p_parent = nullptr);

  void initThemeMgr();

  MainWindow *m_mainWindow = nullptr;

  // QObject managed.
  ThemeMgr *m_themeMgr = nullptr;

  ID m_instanceId = 0;
};
} // namespace markly

#endif // MARKLYAPP_H
