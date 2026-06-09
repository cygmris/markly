#ifndef MARKLYAPP_H
#define MARKLYAPP_H

#include <QObject>

#include "global.h"
#include "noncopyable.h"

class QTranslator;

namespace markly {
class MainWindow;
class ThemeMgr;
class NotebookMgr;
class BufferMgr;
class HistoryMgr;
class SnippetMgr;
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

  NotebookMgr *getNotebookMgr() const;

  BufferMgr *getBufferMgr() const;

  HistoryMgr *getHistoryMgr() const;

  SnippetMgr *getSnippetMgr() const;

  // Install the UI translator for the given language ("auto"/"zh_CN"/"en_US") (#20).
  void applyLanguage(const QString &p_language);

  // Placeholder until the owning spec lands.
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

  // Emitted after a note buffer is successfully saved to disk (path = absolute file path).
  // Used by the search index to incrementally re-index the note's content.
  void noteSaved(const QString &p_filePath);

  // Request that the next opened editor jump to this 1-based line (e.g. a search hit).
  void gotoLineRequested(int p_line);

  // Emitted after a node's tags change; the tags panel refreshes on this.
  void tagsChanged();

  void exportRequested();

  void pinToQuickAccessRequested(const QStringList &p_files);

private:
  explicit MarklyApp(QObject *p_parent = nullptr);

  void initThemeMgr();

  void initNotebookMgr();

  void initBufferMgr();

  MainWindow *m_mainWindow = nullptr;

  // QObject managed.
  ThemeMgr *m_themeMgr = nullptr;

  // QObject managed.
  NotebookMgr *m_notebookMgr = nullptr;
  HistoryMgr *m_historyMgr = nullptr;
  SnippetMgr *m_snippetMgr = nullptr;
  TaskMgr *m_taskMgr = nullptr;
  QTranslator *m_translator = nullptr;

  // QObject managed.
  BufferMgr *m_bufferMgr = nullptr;

  ID m_instanceId = 0;
};
} // namespace markly

#endif // MARKLYAPP_H
