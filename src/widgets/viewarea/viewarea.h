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
  Q_PROPERTY(bool hasOpenFile READ hasOpenFile NOTIFY changed)
public:
  explicit ViewArea(BufferMgr *p_bufferMgr, QObject *p_parent = nullptr);

  QVariantList splits() const;
  int splitCount() const { return m_splits.size(); }
  bool hasOpenFile() const;

  Q_INVOKABLE void openFile(const QString &p_path);
  Q_INVOKABLE void activateTab(int p_splitIndex, double p_bufferId);
  Q_INVOKABLE void closeTab(int p_splitIndex, double p_bufferId);
  Q_INVOKABLE void saveTab(double p_bufferId);
  Q_INVOKABLE void updateText(double p_bufferId, const QString &p_text);
  Q_INVOKABLE bool isTabDirty(double p_bufferId) const;
  Q_INVOKABLE void setActiveSplit(int p_index);
  Q_INVOKABLE void splitView();
  Q_INVOKABLE void unsplit(int p_index);

  // Restore opened tabs from the session config.
  void restoreSession();

signals:
  void changed();

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
};
} // namespace markly

#endif // VIEWAREA_H
