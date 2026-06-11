#ifndef VIEWAREA_H
#define VIEWAREA_H

#include <QObject>
#include <QVariantList>
#include <QVector>

#include <core/global.h>

namespace markly {
class BufferMgr;

// Bridge for the view/tab/split layer (context property "Views"). Holds splits of
// open-buffer tabs; flattens to a QML-bindable `splits` list; opens files (catching
// MarklyApp.openFileRequested), edits/saves buffers, restores session.
// ViewWindow content is a plain-text placeholder here; the editor spec (#7) replaces
// it with the QWidget editor core.
class ViewArea : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList splits READ splits NOTIFY changed)
  Q_PROPERTY(int splitCount READ splitCount NOTIFY changed)
  Q_PROPERTY(int activeSplitIndex READ activeSplitIndex NOTIFY changed)
  Q_PROPERTY(bool hasOpenFile READ hasOpenFile NOTIFY changed)
  Q_PROPERTY(int statsLine READ statsLine NOTIFY statsChanged)
  Q_PROPERTY(int statsColumn READ statsColumn NOTIFY statsChanged)
  Q_PROPERTY(int statsLineCount READ statsLineCount NOTIFY statsChanged)
  Q_PROPERTY(int statsCharCount READ statsCharCount NOTIFY statsChanged)
  Q_PROPERTY(QString viewMode READ viewMode NOTIFY viewModeChanged)
  Q_PROPERTY(QVariantList outline READ outline NOTIFY outlineChanged)
  Q_PROPERTY(QString currentFileDir READ currentFileDir NOTIFY changed)
  Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY changed)
  Q_PROPERTY(QString currentText READ currentText NOTIFY changed)
public:
  explicit ViewArea(BufferMgr *p_bufferMgr, QObject *p_parent = nullptr);

  QVariantList splits() const;
  int splitCount() const { return m_splits.size(); }
  int activeSplitIndex() const { return m_activeSplit; }
  bool hasOpenFile() const;

  Q_INVOKABLE void openFile(const QString &p_path);
  Q_INVOKABLE void activateTab(int p_splitIndex, double p_bufferId);
  Q_INVOKABLE void closeTab(int p_splitIndex, double p_bufferId);
  Q_INVOKABLE void saveTab(double p_bufferId);
  Q_INVOKABLE void updateText(double p_bufferId, const QString &p_text);
  Q_INVOKABLE QString textForBuffer(double p_bufferId) const;
  Q_INVOKABLE bool isTabDirty(double p_bufferId) const;
  Q_INVOKABLE void setActiveSplit(int p_index);
  Q_INVOKABLE void splitView();
  Q_INVOKABLE void unsplit(int p_index);

  int statsLine() const { return m_statsLine; }
  int statsColumn() const { return m_statsColumn; }
  int statsLineCount() const { return m_statsLineCount; }
  int statsCharCount() const { return m_statsCharCount; }
  Q_INVOKABLE void setStats(int p_line, int p_column, int p_lineCount, int p_charCount);

  QString viewMode() const { return m_viewMode; }
  Q_INVOKABLE void setViewMode(const QString &p_mode);
  Q_INVOKABLE void cycleViewMode();

  // Pending jump-to-line (1-based), consumed by the editor when a buffer loads.
  // -1 means no pending jump. Set via MarklyApp::gotoLineRequested (e.g. search hit).
  Q_INVOKABLE int takePendingGotoLine();

  // Absolute directory of the active note's file ("" if none). For image saving (#10).
  QString currentFileDir() const;
  // File name (basename) of the active note ("" if none). For %note% magic word (#14).
  QString currentFileName() const;
  // Content of the active note ("" if none). For export (#15).
  QString currentText() const;
  // Insert text at the active editor's cursor, then offset the cursor (snippets #14).
  Q_INVOKABLE void requestInsert(const QString &p_text, int p_cursorOffset);
  // Ask the visible mindmap pane to serialize itself back to its buffer (#18c).
  Q_INVOKABLE void requestSaveMindmap();
  // Open the find bar of the active editor (toolbar search button).
  Q_INVOKABLE void requestEditorFind();
  // Open a content-insert dialog (image|link|table|more) hosted by the shell.
  Q_INVOKABLE void requestContentDialog(const QString &p_kind);

  // Outline (headings) of the active buffer; [{level, text, line}] (1-based line).
  QVariantList outline() const;
  // Jump the current editor to a line immediately (outline click) via gotoLineNow.
  Q_INVOKABLE void gotoOutlineLine(int p_line);

  // Restore opened tabs from the session config.
  void restoreSession();

signals:
  void changed();
  void statsChanged();
  void viewModeChanged();
  void outlineChanged();
  // Jump the active editor to a 1-based line right now (outline / already-open hit).
  void gotoLineNow(int p_line);
  // Insert text at the active editor's cursor, then move cursor by p_cursorOffset.
  void insertText(const QString &p_text, int p_cursorOffset);
  // Request the active/visible mindmap pane save its tree back to the note (#18c).
  void saveMindmapRequested();
  // Open the active editor's find bar (toolbar search button).
  void editorFindRequested();
  // Open a shell-hosted content dialog of the given kind (#editor-content-dialogs).
  void contentDialogRequested(const QString &p_kind);

private:
  struct Split {
    QVector<ID> m_tabs;
    ID m_active = 0;
  };

  void persistSession();
  void removeEmptySplits();

  BufferMgr *m_bufferMgr = nullptr;
  QVector<Split> m_splits;
  int m_activeSplit = 0;

  int m_statsLine = 0;
  int m_statsColumn = 0;
  int m_statsLineCount = 0;
  int m_statsCharCount = 0;

  QString m_viewMode = QStringLiteral("edit");
  int m_pendingGotoLine = -1;
};
} // namespace markly

#endif // VIEWAREA_H
